/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/FgLuminanceScheme.cpp#6 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/FgLuminanceScheme.hpp"
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
FgLuminanceScheme::FgLuminanceScheme(int id, int vis, WatermarkGenerator* watermarkGenerator):
		WatermarkScheme(id, watermarkGenerator), vis(vis) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	openLog("FgLum");
#endif
	styleNames.push_back(OUSTRING("FG--"));
	styleNames.push_back(OUSTRING("FG00"));
	styleNames.push_back(OUSTRING("FG01"));
	styleNames.push_back(OUSTRING("FG10"));
	styleNames.push_back(OUSTRING("FG11"));
	reset();
}

std::string FgLuminanceScheme::getAbbrev() {
	return std::string("FG");
}

void FgLuminanceScheme::reset() {
	currentBit = Watermark::SIZE;
}

void FgLuminanceScheme::setColorBit(Color* color, unsigned char value, int n) {
	if (value != 0)
		*color |= (1 << n); //set the Nth bit 
	else
		*color &= ~(1 << n); // clear the Nth bit
}

Color FgLuminanceScheme::makeColor(unsigned char bit1, unsigned char bit2) {
	// Add these two bits so that further away from edges
	//Color color = 0x00040400; // This could be the current color?  Black is assumed.
	Color color;
	
	switch (vis) {
		case SCHEME_VIS_DB: // debug
			if (bit1 == 0 && bit2 == 0)
				color = 0x00000000;
			else if (bit1 == 0 && bit2 == 1)
				color = 0x00ff0000;
			else if (bit1 == 1 && bit2 == 0)
				color = 0x0000ff00;
			else if (bit1 == 1 && bit2 == 1)
				color = 0x000000ff;
			else
				throw std::runtime_error("Invalid bit combination");
			break;
		case SCHEME_VIS_HI:
			color = 0x00080800; // results in X 1000
			setColorBit(&color, bit1, 20); // Red component
			setColorBit(&color, bit2, 12); // Green component
			break;
		case SCHEME_VIS_MD:
			color = 0x00040400; // results in X100
			setColorBit(&color, bit1, 19); // Red component
			setColorBit(&color, bit2, 11); // Green component
			break;
		case SCHEME_VIS_LO:
			color = 0x00020200; // results in 0X10
			setColorBit(&color, bit1, 18); // Red component
			setColorBit(&color, bit2, 10); // Green component
			break;
		default:
			throw std::runtime_error("Invalid visibility setting.");
			break;
	}
	return color;
}

std::vector<WatermarkStyle> FgLuminanceScheme::getWatermarkStyles() {
	NamesValuesHelper emptyNamesValuesHelper;
	std::vector<WatermarkStyle> watermarkStyles;
	int index = 0;

	// FgLum--
	watermarkStyles.push_back(WatermarkStyle(styleNames[index++], emptyNamesValuesHelper));
	for (unsigned char bit1 = 0; bit1 < 2 ; bit1++)
		for (unsigned char bit2 = 0; bit2 < 2; bit2++) {
			Color color = makeColor(bit1, bit2);
			NamesValuesHelper namesValuesHelper(1);
			
			namesValuesHelper.add(OUSTRING("CharColor"), Any(color));
			watermarkStyles.push_back(WatermarkStyle(styleNames[index++], namesValuesHelper));
		}
	return watermarkStyles;
}

unsigned char FgLuminanceScheme::nextWatermarkBit() {
	if (Watermark::SIZE <= currentBit) {
		watermark = watermarkGenerator->generate();
		currentBit = 0;
	}
	return watermark[currentBit++];
}

int FgLuminanceScheme::getWatermarkStyleIndex(SmartCursor& smartCursor) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
		std::vector<int> bits;
#endif

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

bool FgLuminanceScheme::verifyIndex(SmartCursor& smartCursor,
		 Reference<XStyle>& xStyle, int index) {
// TODO: actual do some checking
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	writeLogOut(smartCursor, getWatermarkBits(smartCursor, xStyle, index));
#endif
	return true;
}

void FgLuminanceScheme::addWatermarkBits(std::vector<int>& watermarkBits, int index) {
	if (index > 0) {
		watermarkBits.push_back((index - 1) / 2);
		watermarkBits.push_back((index - 1) % 2);
	}
	else {
		watermarkBits.push_back(-1);
		watermarkBits.push_back(-1);
	}
}

bool FgLuminanceScheme::isWatermarkable(SmartCursor& smartCursor) {
	sal_Unicode c = smartCursor.getCurrentCharacter();

	return WatermarkScheme::isWatermarkable(c) && !isSpace(c);
}

Range FgLuminanceScheme::getSubrange(SmartCursor& smartCursor, Range range) {
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
