/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/CharWidthScheme.cpp#6 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/CharWidthScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/SchemeConfig.hpp"

#include "sdkBeg.hpp"
#include "sdkEnd.hpp"

#include <cstring>
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::utils;

using com::haathi::watermark::generator::WatermarkGenerator;
using com::haathi::watermark::utils::Watermark;

using namespace com::sun::star::style;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace scheme {
/******************************************************************************
Class
******************************************************************************/
struct Config {
	sal_Int32 bit;
	sal_Int16 charScaleWidth;
	const char* letters;
};

// TimesNewRoman 12
//static Config configs[] = {
//	{ 0,  95, "fijrt" }, // probably want to avoid these letters
//	{ 0,  96, "s" },
//	{ 0,  97, "abcdeghknopquvxyz" },
//	{ 0,  98, "w" },
//
//	{ 1, 103, "w" },
//	{ 1, 104, "bdghknopquvxy" },
//	{ 1, 105, "acesz" },
//	{ 1, 106, "f" },
//	{ 1, 107, "" },
//	{ 1, 108, "ijlt" }, // Could also have gone on next one up
//	{-1,   0, NULL }
//};

// Arial 12 (14 units higher and lower)
//static OUString usableSymbols = OUSTRING("abcdeghknopqsuvxyz"); // Ariel 12
static Config configs_hi[] = {
	{ 0,  94, "cksvxyz" },
	{ 0,  95, "abdeghnopqu" },
	{ 1, 107, "abcdeghknopqsuvxyz" },
	{-1,   0, NULL }
};

// There are no more reasonable choices after this one
// Arial 12 (7 units higher and lower)
//static OUString usableSymbols = OUSTRING("abcdeghknopqsuvxyz"); // Ariel 12
static Config configs_md[] = {
	{ 0,  94, "cksvxyz" },
	{ 0,  95, "abdeghnopqu" },
	{ 1, 107, "abcdeghknopqsuvxyz" },
	{-1,   0, NULL }
};

// There are no more reasonable choices after this one
// Arial 12 (7 units higher and lower)
//static OUString usableSymbols = OUSTRING("cfkmrstvwxyz"); // Ariel 12
static Config configs_lo[] = {
	{ 0,  95, "ft" },
	{ 0,  96, "r" },
	{ 0,  97, "cksvxyz" },
	{ 0,  98, "w" },
	{ 0,  99, "m" },
	{ 1, 102, "mw" },
	{ 1, 103, "cksvxyz" },
	{ 1, 105, "r" },
	{ 1, 106, "ft" },
	{-1,   0, NULL }
};

static Config* getConfigs(int vis) {
	Config* configs;

	switch (vis) {
		case SCHEME_VIS_HI:
			configs = configs_hi;
			break;
		case SCHEME_VIS_MD:
			configs = configs_md;
			break;
		case SCHEME_VIS_LO:
			configs = configs_lo;
			break;
		default:
			throw std::runtime_error("Invalid visibility setting.");
			break;
	}
	return configs;
}

CharWidthScheme::CharWidthScheme(int id, int vis, WatermarkGenerator* watermarkGenerator):
		WatermarkScheme(id, watermarkGenerator), vis(vis) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	openLog("WS");
#endif
	styleNames.push_back(OUSTRING("CW-"));

	Config* configs = getConfigs(vis);
	for (int i = 0; configs[i].letters != NULL; i++) {
		rtl::OUStringBuffer buffer;

		buffer.appendAscii("CW");
		buffer.append(configs[i].bit);
		buffer.appendAscii("-");
		buffer.append((sal_Int32) (configs[i].charScaleWidth));
		styleNames.push_back(buffer.makeStringAndClear());
	}
	reset();
	hasReserveBit = false;
}

std::string CharWidthScheme::getAbbrev() {
	return std::string("CW");
}

void CharWidthScheme::reset() {
	currentBit = Watermark::SIZE;
}

std::vector<WatermarkStyle> CharWidthScheme::getWatermarkStyles() {
	NamesValuesHelper emptyNamesValuesHelper;
	std::vector<WatermarkStyle> watermarkStyles;
	Config* configs = getConfigs(vis);
	int index = 0;

	// CW-
	watermarkStyles.push_back(WatermarkStyle(styleNames[index++], emptyNamesValuesHelper));
	for (int i = 0; configs[i].letters != NULL; i++) {
		sal_Int16 width = configs[i].charScaleWidth;
		NamesValuesHelper namesValuesHelper(1);
		
		namesValuesHelper.add(OUSTRING("CharScaleWidth"), Any(width));
		watermarkStyles.push_back(WatermarkStyle(styleNames[index++], namesValuesHelper));

		styleToBitMap[i] = configs[i].bit;

		int bit = configs[i].bit;
		int len = strlen(configs[i].letters);

		if (bit == 0)
			usableSymbols += OUString::createFromAscii(configs[i].letters);

		for (int j = 0; j < len; j++)
			symbolToStyleMaps[bit][(sal_Unicode) (configs[i].letters[j])] = i;
	}
	return watermarkStyles;
}

unsigned char CharWidthScheme::nextWatermarkBit() {
	if (Watermark::SIZE <= currentBit) {
		watermark = watermarkGenerator->generate();
		currentBit = 0;
	}
	return watermark[currentBit++];
}

bool CharWidthScheme::usableSymbolFollows(SmartCursor& smartCursor) {
	sal_Int32 oldPos = smartCursor.getLeft();
	sal_Int32 pos = oldPos + 1;
	bool result;

	while (true) {
		if (!smartCursor.selectPos(pos)) {
			result = false;
			break;
		}

		sal_Unicode c = smartCursor.getCurrentCharacter();

		if (isSpace(c) || isEol(c)) {
			result = false;
			break;
		}
		if (usableSymbols.indexOf(c) != -1) {
			result = true;
			break;
		}
		pos++;
	}
	smartCursor.selectPos(oldPos);
	return result;
}

int CharWidthScheme::getWatermarkStyleIndex(SmartCursor& smartCursor) {
	sal_Unicode c = smartCursor.getCurrentCharacter();

	if (isEol(c)) {
		int i = 10;
	}

	if (isHyphen(c)) {
		int j = 20;
	}

	// Do we even get eol?
	if (isSpace(c) || isEol(c)) {
		// Should never get here, because don't reserve unless good one follows
		hasReserveBit = false;
		return 0;
	}

	if (usableSymbols.indexOf(c) == -1)
		// Unusable symbols are not watermarked with a bit
		return 0;

	if (hasReserveBit) {
		// If have a reserved bit, use it
		hasReserveBit = false;
		return symbolToStyleMaps[reserveBit][c] + 1;
	}

	// If there is another suitable character that follows
	// Need to get next ones up to space or eol
	if (usableSymbolFollows(smartCursor)) {
		unsigned char bit = nextWatermarkBit();

		hasReserveBit = true;
		reserveBit = 1 - bit;

		return symbolToStyleMaps[bit][c] + 1;
	}
	return 0;
}

bool CharWidthScheme::verifyIndex(SmartCursor& smartCursor,
		 Reference<XStyle>& xStyle, int index) {
// TODO: actual do some checking
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	writeLogOut(smartCursor, getWatermarkBits(smartCursor, xStyle, index));
#endif
	return true;
}

void CharWidthScheme::addWatermarkBits(std::vector<int>& watermarkBits, int index) {
	if (index > 0)
		watermarkBits.push_back(styleToBitMap[index - 1]);
	else
		watermarkBits.push_back(-1);
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
