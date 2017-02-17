/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/embedder/AttributeEmbedder.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/embedder/AttributeEmbedder.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/utils/WatermarkPropertySetHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/XPropertySet.hpp>
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

using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::lang;
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
AttributeEmbedder::AttributeEmbedder(Reference<XComponent> xComponent,
		std::vector<WatermarkStyle>& watermarkStyles):
		watermarkStyles(watermarkStyles) {
}

void AttributeEmbedder::preWatermark() {
}

void AttributeEmbedder::postWatermark() {
}

int AttributeEmbedder::extract(SmartCursor& smartCursor) {
	return -1;
}

void AttributeEmbedder::embed(SmartCursor& smartCursor, int watermark) {
	// Plain PropertySetHelper might be sufficient
	WatermarkPropertySetHelper propertySetHelper(smartCursor.asXTextRange());

	try {
		propertySetHelper.set(watermarkStyles[watermark].style);
	}
	catch (IllegalArgumentException& ex) {
		std::cerr << "Position :" << ex.ArgumentPosition << std::endl;
	}
}

void AttributeEmbedder::unembed(SmartCursor& smartCursor) {
	// Need to remove attributes, but have to have an
	// idea of what the attributes are, either by knowing
	// the watermark already there or know the entire list
	// of attributes that could be affected and remove them.
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
