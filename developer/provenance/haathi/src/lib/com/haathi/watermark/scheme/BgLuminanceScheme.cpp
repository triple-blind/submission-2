/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/BgLuminanceScheme.cpp#4 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/BgLuminanceScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/SchemeConfig.hpp"
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::utils;

using com::haathi::text::Range;
using com::haathi::watermark::generator::WatermarkGenerator;
using com::haathi::watermark::utils::Watermark;

using namespace com::sun::star::style;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
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
BgLuminanceScheme::BgLuminanceScheme(int id, int vis, WatermarkGenerator* watermarkGenerator):
		WatermarkScheme(id, watermarkGenerator), vis(vis) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	openLog("BgLum");
#endif
	styleNames.push_back(OUSTRING("BG--"));
	styleNames.push_back(OUSTRING("BG00"));
	styleNames.push_back(OUSTRING("BG01"));
	styleNames.push_back(OUSTRING("BG10"));
	styleNames.push_back(OUSTRING("BG11"));
	reset();
}

std::string BgLuminanceScheme::getAbbrev() {
	return std::string("BG");
}

void BgLuminanceScheme::reset() {
	currentBit = Watermark::SIZE;
}

void BgLuminanceScheme::setColorBit(Color* color, unsigned char value, int n) {
	if (value != 0)
		*color |= (1 << n); //set the Nth bit 
	else
		*color &= ~(1 << n); // clear the Nth bit
}

Color BgLuminanceScheme::makeColor(unsigned char bit1, unsigned char bit2) {
	// Add these two bits so that further away from edges
	//Color color = 0x00fbfbff; // This could be the current color?  Black is assumed.

	Color color;

	switch (vis) {
		case SCHEME_VIS_DB:
			color = 0x00fbfbff; // results in X011
			setColorBit(&color, bit1, 23); // Red component
			setColorBit(&color, bit2, 14); // Green component
			break;
		case SCHEME_VIS_HI:
			color = 0x00fbfbff;
			setColorBit(&color, bit1, 19); // Red component
			setColorBit(&color, bit2, 11); // Green component
			break;
		case SCHEME_VIS_MD:
			color = 0x00fdfdff; // results in 1X01
			setColorBit(&color, bit1, 18); // Red component
			setColorBit(&color, bit2, 10); // Green component
			break;
		case SCHEME_VIS_LO:
			color = 0x00fdfdff; // results in 11X0
			setColorBit(&color, bit1, 17); // Red component
			setColorBit(&color, bit2,  9); // Green component
			break;
		default:
			throw std::runtime_error("Invalid visibility setting.");
			break;
	}
	return color;
}

std::vector<WatermarkStyle> BgLuminanceScheme::getWatermarkStyles() {
	NamesValuesHelper emptyNamesValuesHelper;
	std::vector<WatermarkStyle> watermarkStyles;
	int index = 0;

	// BgLum--
	watermarkStyles.push_back(WatermarkStyle(styleNames[index++], emptyNamesValuesHelper));
	for (unsigned char bit1 = 0; bit1 < 2 ; bit1++)
		for (unsigned char bit2 = 0; bit2 < 2; bit2++) {
			Color color = makeColor(bit1, bit2);
			NamesValuesHelper namesValuesHelper(1);
			
			namesValuesHelper.add(OUSTRING("CharBackColor"), Any(color));
			watermarkStyles.push_back(WatermarkStyle(styleNames[index++], namesValuesHelper));
		}
	return watermarkStyles;
}

unsigned char BgLuminanceScheme::nextWatermarkBit() {
	if (Watermark::SIZE <= currentBit) {
		watermark = watermarkGenerator->generate();
		currentBit = 0;
	}
	return watermark[currentBit++];
}

int BgLuminanceScheme::getWatermarkStyleIndex(SmartCursor& smartCursor) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
		std::vector<int> bits;
#endif

	// Skip space because although it could be colored, it probably won't be found
	// by the extractor, especially if it is at the end of a line.
	if (!isWatermarkable(smartCursor)) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
		writeLogIn(smartCursor, bits);
#endif
		return 0;
	}

	unsigned char bit1 = nextWatermarkBit();
	unsigned char bit2 = nextWatermarkBit();

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	bits.push_back(bit1);
	bits.push_back(bit2);
	writeLogIn(smartCursor, bits);
#endif
	return bit1 * 2 + bit2 + 1;
}

bool BgLuminanceScheme::verifyIndex(SmartCursor& smartCursor,
		 Reference<XStyle>& xStyle, int index) {
// TODO: actual do some checking
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	writeLogOut(smartCursor, getWatermarkBits(smartCursor, xStyle, index));
#endif
	return true;
}

void BgLuminanceScheme::addWatermarkBits(std::vector<int>& watermarkBits, int index) {
	if (index > 0) {
		watermarkBits.push_back((index - 1) / 2);
		watermarkBits.push_back((index - 1) % 2);
	}
	else {
		watermarkBits.push_back(-1);
		watermarkBits.push_back(-1);
	}
}

bool BgLuminanceScheme::isWatermarkable(SmartCursor& smartCursor) {
	sal_Unicode c = smartCursor.getCurrentCharacter();

	return WatermarkScheme::isWatermarkable(c) && !isSpace(c);
}

Range BgLuminanceScheme::getSubrange(SmartCursor& smartCursor, Range range) {
	static int minLength = 64 / 2;
	sal_Int32 left = range.getLeft();
	sal_Int32 right = range.getRight();
	sal_Int32 width = range.getWidth();

	if (width < minLength)
		return Range();

	int count = 0;

	for (int pos = right - 1; pos >= left; pos--) {
		smartCursor.selectPos(pos);

		if (isWatermarkable(smartCursor)) {
			count++;
			if (count == minLength)
				return Range(left, pos + 1);
		}
	};
	return Range();
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
