/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/watermark/scheme/DualWatermarkScheme.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/DualWatermarkScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/document/ProvenanceDocument.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;

using namespace com::sun::star::beans;
using namespace com::sun::star::container;
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
/******************************************************************************
Class
******************************************************************************/
DualWatermarkScheme::DualWatermarkScheme(ProvenanceDocument* provenanceDocument):
		WatermarkScheme(provenanceDocument) {
	resetState();
}

void DualWatermarkScheme::resetState() {
	_currentBit = DEFAULT_BLOCK_SIZE;
	_previousBit = 0;
	newLineTracker.reset();
}

bool DualWatermarkScheme::isWatermarkEmbedded() {
	return _currentBit <= 0;
}

void DualWatermarkScheme::setBlockToEmbed(std::bitset<DEFAULT_BLOCK_SIZE> blockToEmbed) {
	WatermarkScheme::setBlockToEmbed(blockToEmbed);
	_currentBit = DEFAULT_BLOCK_SIZE;
}

void DualWatermarkScheme::addCharacterStyles(OUString styleName, OUString propertyName1, Any value1,
		 OUString propertyName2, Any value2) {
	Reference<XNameContainer> xNameContainer = getXNameContainer(OUSTRING("CharacterStyles"));

	if (xNameContainer->hasByName(styleName) == sal_True)
		return;

	Reference<XStyle> xStyle = newXStyle();

	addProperty(xStyle, propertyName1, value1);
	addProperty(xStyle, propertyName2, value2);
	xNameContainer->insertByName(styleName, Any(xStyle));
}

short DualWatermarkScheme::generateWatermark(Reference<XTextViewCursor>& xTextViewCursor) {
	if (isWatermarkEmbedded())
		return 0;
	if (!isText(xTextViewCursor))
		return 0;

	// Will apply to spaces, either 0 or 1 bits depending on isBitEmbedded
	bool isBitEmbedded;
	short kerning = makeKerning(xTextViewCursor, newLineTracker, &isBitEmbedded, &_previousBit);
	// Will apply to non-spaces, 2 bits
	unsigned char bit1, bit2;
	Color color = makeColor(xTextViewCursor, &bit1, &bit2);
	OUString dualKey = convertToOUString(color) + OUSTRING("-") + convertToOUString(kerning);
	OUString styleName;

	std::map<OUString, OUString>* documentStyleMap = provenanceDocument->getStyle();
	std::map<OUString,OUString>::iterator it = documentStyleMap->find(dualKey);
	if (it == documentStyleMap->end()) {
		styleName = OUSTRING("dual") + dualKey;
		documentStyleMap->insert(std::pair<OUString,OUString>(dualKey, styleName));				
		addCharacterStyles(styleName, OUSTRING("CharColor"), Any(color), OUSTRING("CharKerning"), Any(kerning));
	}
	else
		styleName = it->second;

	applyWatermark(xTextViewCursor, styleName);
	return isSpace(xTextViewCursor) ? (isBitEmbedded ? 1 : 0) : 2;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
