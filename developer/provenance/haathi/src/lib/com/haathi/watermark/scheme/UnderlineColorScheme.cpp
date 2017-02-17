/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/UnderlineColorScheme.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/UnderlineColorScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/PropertySetHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/FontUnderline.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::utils;

using com::haathi::watermark::generator::WatermarkGenerator;
using com::haathi::watermark::utils::Watermark;

using namespace com::sun::star::awt;
using namespace com::sun::star::beans;
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
UnderlineColorScheme::UnderlineColorScheme(int id, WatermarkGenerator* watermarkGenerator):
		WatermarkScheme(id, watermarkGenerator) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	openLog("UlCol");
#endif
	for (int red = 0; red < 4; red++)
		for (int green = 0; green < 4; green++)
			for (int blue = 0; blue < 4; blue++)
				styleNames.push_back(OUSTRING("UlCol") +
					convertToOUString(red) + 
					convertToOUString(green) +
					convertToOUString(blue));
	reset();
}

std::string UnderlineColorScheme::getAbbrev() {
	return std::string("UlCol");
}

void UnderlineColorScheme::reset() {
	currentBit = Watermark::SIZE;
}

void UnderlineColorScheme::setColorBit(Color* color, unsigned char value, int n) {
	if (value != 0)
		*color |= (1 << n); //set the Nth bit 
	else
		*color &= ~(1 << n); // clear the Nth bit
}

Color UnderlineColorScheme::makeColor(int red, int green, int blue) {
	Color color = 0xff000000; // Make it transparent

	setColorBit(&color, red   & 2, 17);
	setColorBit(&color, red   & 1, 16);
	setColorBit(&color, green & 2,  9);
	setColorBit(&color, green & 1,  8);
	setColorBit(&color, blue  & 2,  1);
	setColorBit(&color, blue  & 1,  0);
	return color;
}

/* This seems to work as script, but why not here?

Sub Main
	' Recupere le premier element de la selection courante
	oTxt = ThisComponent.CurrentSelection.getByIndex(0)
	' Defini le style de soulignement
	oTxt.CharUnderline = com.sun.star.awt.FontUnderline.DOUBLE
	
	' Indique que le soulignement a une couleur et defini la couleur
	oTxt.CharUnderlineHasColor = True
	oTxt.CharUnderlineColor = RGB( 10, 45, 210 )

End Sub
*/

std::vector<WatermarkStyle> UnderlineColorScheme::getWatermarkStyles() {
	std::vector<WatermarkStyle> watermarkStyles;
	int index = 0;

	for (int red = 0; red < 4 ; red++)
		for (int green = 0; green < 4; green++)
			for (int blue = 0; blue < 4; blue++) {
				Color color = makeColor(red, green, blue);
				NamesValuesHelper namesValuesHelper(3);
			
				namesValuesHelper.add(OUSTRING("CharUnderline"), Any(FontUnderline::SINGLE));
				namesValuesHelper.add(OUSTRING("CharUnderlineHasColor"), Any(sal_True));
				namesValuesHelper.add(OUSTRING("CharUnderlineColor"), Any((sal_Int32) color));
				watermarkStyles.push_back(WatermarkStyle(styleNames[index++], namesValuesHelper));
			}
	return watermarkStyles;
}

unsigned char UnderlineColorScheme::nextWatermarkBit() {
	if (Watermark::SIZE <= currentBit) {
		watermark = watermarkGenerator->generate();
		currentBit = 0;
	}
	return watermark[currentBit++];
}

int UnderlineColorScheme::getWatermarkStyleIndex(SmartCursor& smartCursor) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
		std::vector<int> bits;
#endif

	unsigned char red = 2 * nextWatermarkBit() + nextWatermarkBit();
	unsigned char green = 2 * nextWatermarkBit() + nextWatermarkBit();
	unsigned char blue = 2 * nextWatermarkBit() + nextWatermarkBit();

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	bits.push_back(red);
	bits.push_back(green);
	bits.push_back(blue);
	writeLogIn(smartCursor, bits);
#endif
	return 16 * red + 4 * green + blue;
}

bool UnderlineColorScheme::verifyIndex(SmartCursor& smartCursor,
		Reference<XStyle>& xStyle, int index) {
	PropertySetHelper propertySetHelper(xStyle);

	sal_Int16 fontUnderline;
	propertySetHelper.get("CharUnderline") >>= fontUnderline;

	sal_Bool hasColor;
	propertySetHelper.get("CharUnderlineHasColor") >>= hasColor;

	Color color;
	propertySetHelper.get("CharUnderlineColor") >>= color;

	int red = (color & 0x00ff0000) >> 16;
	int green = (color & 0x0000ff00) >> 8;
	int blue = (color & 0x000000ff) >> 0;
	int tmpIndex = red * 16 + green * 4 + blue;

	if (fontUnderline != FontUnderline::SINGLE || isFalse(hasColor) || index != tmpIndex)
		std::cerr << "Could not verify index." << std::endl;

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	writeLogOut(smartCursor, getWatermarkBits(smartCursor, xStyle, tmpIndex));
#endif

	return fontUnderline == FontUnderline::DONTKNOW && isTrue(hasColor) && index == tmpIndex;
}

void UnderlineColorScheme::addWatermarkBits(std::vector<int>& watermarkBits, int index) {
	if (index > 0) {
		watermarkBits.push_back((index & 0x30) / 16);
		watermarkBits.push_back((index & 0x0c) / 4);
		watermarkBits.push_back((index & 0x03));
	}
	else {
		watermarkBits.push_back(-1);
		watermarkBits.push_back(-1);
		watermarkBits.push_back(-1);
	}
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
