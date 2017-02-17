/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/BaselineScheme.cpp#6 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/BaselineScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/SchemeConfig.hpp"

#include <exception>
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
BaselineScheme::BaselineScheme(int id, int vis, WatermarkGenerator* watermarkGenerator):
		WatermarkScheme(id, watermarkGenerator) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	openLog("FgLum");
#endif
	styleNames.push_back(OUSTRING("BL-"));
	styleNames.push_back(OUSTRING("BL0"));
	styleNames.push_back(OUSTRING("BL1"));

	sal_Int16 escapement = 0;
	switch (vis) {
		case SCHEME_VIS_HI:
			escapement = 3;
			break;
		case SCHEME_VIS_MD:
			escapement = 2;
			break;
		case SCHEME_VIS_LO:
			escapement = 1;
			break;
		default:
			throw std::runtime_error("Invalid visibility setting.");
			break;
	}
	escapements[0] = (sal_Int16) 0;
	escapements[1] = escapement;
	reset();
}

std::string BaselineScheme::getAbbrev() {
	return std::string("BL");
}

void BaselineScheme::reset() {
	currentBit = Watermark::SIZE;
}

std::vector<WatermarkStyle> BaselineScheme::getWatermarkStyles() {
	NamesValuesHelper emptyNamesValuesHelper;
	std::vector<WatermarkStyle> watermarkStyles;
	int index = 0;

	// Base-
	watermarkStyles.push_back(WatermarkStyle(styleNames[index++], emptyNamesValuesHelper));
	for (unsigned char bit = 0; bit < 2 ; bit++) {
		NamesValuesHelper namesValuesHelper(1);
		sal_Int16 height = escapements[bit];
			
		namesValuesHelper.add(OUSTRING("CharEscapement"), Any(height));
		watermarkStyles.push_back(WatermarkStyle(styleNames[index++], namesValuesHelper));
	}
	return watermarkStyles;
}

unsigned char BaselineScheme::nextWatermarkBit() {
	if (Watermark::SIZE <= currentBit) {
		watermark = watermarkGenerator->generate();
		currentBit = 0;
	}
	return watermark[currentBit++];
}

int BaselineScheme::getWatermarkStyleIndex(SmartCursor& smartCursor) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
		std::vector<int> bits;
#endif

		if (!isWatermarkable(smartCursor)) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
		writeLogIn(smartCursor, bits);
#endif
		return 0;
	}

	unsigned char bit = nextWatermarkBit();

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	bits.push_back(bit);
	writeLogIn(smartCursor, bits);
#endif
	return bit + 1;
}

bool BaselineScheme::verifyIndex(SmartCursor& smartCursor,
		 Reference<XStyle>& xStyle, int index) {
// TODO: actual do some checking
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	writeLogOut(smartCursor, getWatermarkBits(smartCursor, xStyle, index));
#endif
	return true;
}

void BaselineScheme::addWatermarkBits(std::vector<int>& watermarkBits, int index) {
	if (index > 0)
		watermarkBits.push_back((index - 1) % 2);
	else
		watermarkBits.push_back(-1);
}

bool BaselineScheme::isWatermarkable(SmartCursor& smartCursor) {
	sal_Unicode c = smartCursor.getCurrentCharacter();

	return WatermarkScheme::isWatermarkable(c) && !isSpace(c);
}

Range BaselineScheme::getSubrange(SmartCursor& smartCursor, Range range) {
	static int minLength = 64 / 1;
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
