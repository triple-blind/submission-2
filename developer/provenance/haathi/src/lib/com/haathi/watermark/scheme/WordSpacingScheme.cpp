/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/WordSpacingScheme.cpp#6 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/WordSpacingScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/SchemeConfig.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/view/XLineCursor.hpp>
#include "sdkEnd.hpp"
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
using namespace com::sun::star::view;
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
// A kerning value of 421 == 0.1661in == 12 pt, 35 == 0.0138in == 1pt
// So in short, now i'm increasing the kerning space of the character by 1 pt.
// This value can also be decided by randomness or some other metric

// The units appear to be 1/100mm 421 == 0.1661in => 2535 == 1.0in == 25.4mm => 1 = 0.01mm				
WordSpacingScheme::WordSpacingScheme(int id, int vis, WatermarkGenerator* watermarkGenerator):
		WatermarkScheme(id, watermarkGenerator) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	openLog("WS");
#endif
	styleNames.push_back(OUSTRING("WS-"));
	styleNames.push_back(OUSTRING("WS0"));
	styleNames.push_back(OUSTRING("WS1"));

	sal_Int16 kerning = 0;
	switch (vis) {
		case SCHEME_VIS_HI:
			kerning = 96;
			break;
		case SCHEME_VIS_MD:
			kerning = 80;
			break;
		case SCHEME_VIS_LO:
			kerning = 64;
			break;
		default:
			throw std::runtime_error("Invalid visibility setting.");
			break;
	}
	kernings[0] = (sal_Int16) 0;
	kernings[1] = kerning;
	reset();
}

std::string WordSpacingScheme::getAbbrev() {
	return std::string("WS");
}

void WordSpacingScheme::reset() {
	currentBit = Watermark::SIZE;
}

std::vector<WatermarkStyle> WordSpacingScheme::getWatermarkStyles() {
	NamesValuesHelper emptyNamesValuesHelper;
	std::vector<WatermarkStyle> watermarkStyles;
	int index = 0;

	// WS-
	watermarkStyles.push_back(WatermarkStyle(styleNames[index++], emptyNamesValuesHelper));
	for (unsigned char bit = 0; bit < 2 ; bit++) {
		sal_Int16 kerning = kernings[bit];
		NamesValuesHelper namesValuesHelper(1);
		
		namesValuesHelper.add(OUSTRING("CharKerning"), Any(kerning));
		watermarkStyles.push_back(WatermarkStyle(styleNames[index++], namesValuesHelper));
	}
	return watermarkStyles;
}

unsigned char WordSpacingScheme::nextWatermarkBit() {
	if (Watermark::SIZE <= currentBit) {
		watermark = watermarkGenerator->generate();
		currentBit = 0;
	}
	return watermark[currentBit++];
}

int WordSpacingScheme::getWatermarkStyleIndex(SmartCursor& smartCursor) {
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

/*
	// This is being supressed higher up.
	if (WatermarkScheme::isEol(smartCursor.getCurrentCharacter()))
		// Having consumed the bit, but not watermarked it.
		return 0;
*/

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	bits.push_back(bit);
	writeLogIn(smartCursor, bits);
#endif
	return bit + 1;
}

bool WordSpacingScheme::verifyIndex(SmartCursor& smartCursor,
		 Reference<XStyle>& xStyle, int index) {
// TODO: actual do some checking
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	writeLogOut(smartCursor, getWatermarkBits(smartCursor, xStyle, index));
#endif
	return true;
}

void WordSpacingScheme::addWatermarkBits(std::vector<int>& watermarkBits, int index) {
	if (index > 0)
		watermarkBits.push_back((index - 1) % 2);
	else
		watermarkBits.push_back(-1);
}

bool WordSpacingScheme::isEol(Reference<XTextViewCursor> xTextViewCursor) {
	Reference<XLineCursor> xLineCursor = Reference<XLineCursor>(xTextViewCursor, UNO_QUERY);

	return isTrue(xLineCursor->isAtEndOfLine());
}


bool WordSpacingScheme::isWatermarkable(SmartCursor& smartCursor) {
	sal_Unicode c = smartCursor.getCurrentCharacter();

	return isSpace(c) || WatermarkScheme::isEol(c);
}

Range WordSpacingScheme::getSubrange(SmartCursor& smartCursor, Range range) {
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
