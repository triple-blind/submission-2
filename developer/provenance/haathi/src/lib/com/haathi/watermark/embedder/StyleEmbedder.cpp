/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/embedder/StyleEmbedder.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/embedder/StyleEmbedder.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/utils/WatermarkPropertySetHelper.hpp"
#include "com/haathi/watermark/utils/Watermarks.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::document::SmartCursor;
using com::haathi::utils::NamesValuesHelper;
using com::haathi::utils::convertToOUString;
using com::haathi::watermark::scheme::WatermarkStyle;
using com::haathi::watermark::utils::WatermarkPropertySetHelper;
using com::haathi::watermark::utils::Watermarks;

using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::lang;
using namespace com::sun::star::style;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace embedder {
/******************************************************************************
Class
******************************************************************************/
StyleEmbedder::StyleEmbedder(Reference<XComponent> xComponent,
		std::vector<WatermarkStyle>& watermarkStyles):
		xComponent(xComponent), watermarkStyles(watermarkStyles) {
}

Reference<XNameContainer> StyleEmbedder::getXNameContainer(OUString name) {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XStyleFamiliesSupplier> xStyleFamiliesSupplier(xTextDocument, UNO_QUERY);
	Reference<XNameAccess> xNameAccess = xStyleFamiliesSupplier->getStyleFamilies();
	Reference<XNameContainer> xNameContainer(xNameAccess->getByName(name), UNO_QUERY);

	return xNameContainer;
}

Reference<XStyle> StyleEmbedder::newXStyle() {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XMultiServiceFactory> xMultiServiceFactory(xTextDocument, UNO_QUERY);
	Reference<XInterface> characterStyle = xMultiServiceFactory->createInstance(OUSTRING("com.sun.star.style.CharacterStyle"));
	Reference<XStyle> xStyle(characterStyle, UNO_QUERY);

	return xStyle;
}

void StyleEmbedder::addCharacterStyle(OUString styleName, OUString stylePrettyName, NamesValuesHelper& namesValuesHelper) {
	// Can this name container be cached?
	Reference<XNameContainer> xNameContainer = getXNameContainer(OUSTRING("CharacterStyles"));

	// Just in case and it could have been different
	if (isTrue(xNameContainer->hasByName(styleName)))
		xNameContainer->removeByName(styleName);

	Reference<XStyle> xStyle = newXStyle();
	WatermarkPropertySetHelper propertySetHelper(xStyle);

	propertySetHelper.set(namesValuesHelper);
	// This does not work!  Can't set the name like this.
	//propertySetHelper.set("UserInterfaceName", Any(stylePrettyName));
	xNameContainer->insertByName(styleName, Any(xStyle));
}

void StyleEmbedder::addIntCharacterStyle(int index, OUString name, NamesValuesHelper namesValuesHelper) {
	addCharacterStyle(convertToOUString(index), name, namesValuesHelper);
}

void StyleEmbedder::preWatermark() {
	int index = 0;

	addIntCharacterStyle(index++, OUSTRING("haathi"), NamesValuesHelper()); // default, empty style
	for (std::vector<WatermarkStyle>::iterator it = watermarkStyles.begin(); it != watermarkStyles.end(); ++it)
		addIntCharacterStyle(index++, it->name, it->style);
}

void StyleEmbedder::subCharacterStyle(OUString styleName, OUString stylePrettyName, NamesValuesHelper& namesValuesHelper) {
	Reference<XNameContainer> xNameContainer = getXNameContainer(OUSTRING("CharacterStyles"));

	if (isTrue(xNameContainer->hasByName(styleName)))
		xNameContainer->removeByName(styleName);
}

void StyleEmbedder::subIntCharacterStyle(int index, OUString name, NamesValuesHelper namesValuesHelper) {
	subCharacterStyle(convertToOUString(index), name, namesValuesHelper);
}

void StyleEmbedder::postWatermark() {
	int index = 0;

	subIntCharacterStyle(index++, OUSTRING("haathi"), NamesValuesHelper()); // default, empty style
	for (std::vector<WatermarkStyle>::iterator it = watermarkStyles.begin(); it != watermarkStyles.end(); ++it)
		subIntCharacterStyle(index++, it->name, it->style);
}

int StyleEmbedder::extract(SmartCursor& smartCursor) {
	return -1;
}

void StyleEmbedder::embed(SmartCursor& smartCursor, int watermark) {
	OUString styleName = convertToOUString(watermark);

	try {
		smartCursor.getXPropertySet()->setPropertyValue(OUSTRING("CharStyleName"), Any(styleName));
	}
	catch (IllegalArgumentException& ex) {
		std::cerr << "Position :" << ex.ArgumentPosition << std::endl;
	}
}

void StyleEmbedder::unembed(SmartCursor& smartCursor) {
	OUString charStyleName = OUSTRING("CharStyleName");
	// Does this work, setting it to zero?
	Any defaultCharStyleName = Any(convertToOUString(0));
	Reference<XPropertySet> xPropertySet(smartCursor.getXPropertySet());
	Any anyPropertyValue;
	OUString styleName;

	try {
		anyPropertyValue = xPropertySet->getPropertyValue(charStyleName);
		if (isFalse(anyPropertyValue.hasValue()))
			return;
		anyPropertyValue >>= styleName;

		int index = styleName.toInt32();
		if (index <= 0)
			return;
		xPropertySet->setPropertyValue(charStyleName, defaultCharStyleName);
	}
	catch (...) {
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
