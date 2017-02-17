/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/UserAttributesScheme.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/UserAttributesScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/utils/WatermarkPropertySetHelper.hpp"
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::utils;

using com::haathi::watermark::generator::WatermarkGenerator;
using com::haathi::watermark::utils::Watermark;
using com::haathi::watermark::utils::WatermarkPropertySetHelper;

using namespace com::sun::star::beans;
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
UserAttributesScheme::UserAttributesScheme(int id, WatermarkGenerator* watermarkGenerator):
		WatermarkScheme(id, watermarkGenerator) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	openLog("UserAttr");
#endif
	for (int i = 0; i < 8; i++)
		styleNames.push_back(OUSTRING("UserAttr") +	convertToOUString(i));
	reset();
}

std::string UserAttributesScheme::getAbbrev() {
	return std::string("UserAttr");
}

void UserAttributesScheme::reset() {
	currentBit = Watermark::SIZE;
}

std::vector<WatermarkStyle> UserAttributesScheme::getWatermarkStyles() {
	std::vector<WatermarkStyle> watermarkStyles;
	int index = 0;

	for (sal_Int32 i = 0; i < 8; i++) {
		NamesValuesHelper namesValuesHelper(1);
		OUString value = convertToOUString(i);
			
		namesValuesHelper.add(OUSTRING("watermark"), Any(value)); // big hack!
		watermarkStyles.push_back(WatermarkStyle(styleNames[index++], namesValuesHelper));
	}
	return watermarkStyles;
}

unsigned char UserAttributesScheme::nextWatermarkBit() {
	if (Watermark::SIZE <= currentBit) {
		watermark = watermarkGenerator->generate();
		currentBit = 0;
	}
	return watermark[currentBit++];
}

int UserAttributesScheme::getWatermarkStyleIndex(SmartCursor& smartCursor) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
		std::vector<int> bits;
#endif
	int index = 0;

	// User 3 bits to get 8 values
	for (int i = 0; i < 3; i++)
		index = 2 * index + nextWatermarkBit();

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	bits.push_back(index);
	writeLogIn(smartCursor, bits);
#endif
	return index;
}

bool UserAttributesScheme::verifyIndex(SmartCursor& smartCursor,
		Reference<XStyle>& xStyle, int index) {
	WatermarkPropertySetHelper propertySetHelper(xStyle);
	
	OUString tmpOUIndex;
	propertySetHelper.get("watermark") >>= tmpOUIndex;

	sal_Int32 tmpIndex = tmpOUIndex.toInt32();

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	writeLogOut(smartCursor, getWatermarkBits(smartCursor, xStyle, tmpIndex));
#endif
	return index == tmpIndex;
}

void UserAttributesScheme::addWatermarkBits(std::vector<int>& watermarkBits, int index) {
	if (index > 0)
		watermarkBits.push_back(index);
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
