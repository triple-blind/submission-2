/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/utils/WatermarkPropertySetHelper.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/utils/WatermarkPropertySetHelper.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/xml/AttributeData.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::NamesValuesHelper;
using com::haathi::utils::PropertySetHelper;

using com::sun::star::beans::UnknownPropertyException;
using com::sun::star::container::XNameContainer;
using com::sun::star::uno::Any;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::XInterface;
using com::sun::star::xml::AttributeData;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace utils {
/******************************************************************************
Class
******************************************************************************/
WatermarkPropertySetHelper::WatermarkPropertySetHelper(Reference<XInterface> xInterface):
		PropertySetHelper(xInterface) {
}

WatermarkPropertySetHelper::~WatermarkPropertySetHelper() {
}

void WatermarkPropertySetHelper::setUserDefinedAttribute(OUString name, Any anyValue) {
	OUString userDefinedAttributes = OUSTRING("UserDefinedAttributes");

	Reference<XNameContainer> xNameContainer;
	Any anyXNameContainer = xPropertySet->getPropertyValue(userDefinedAttributes);
	anyXNameContainer >>= xNameContainer;
	OUString value;
	anyValue >>= value;

	AttributeData attributeData(OUString(), OUSTRING("CDATA"), value);
	xNameContainer->insertByName(name, Any(attributeData));

	// Make sure this is anyXNameContainer and not Any(xNameContainer)!
	xPropertySet->setPropertyValue(userDefinedAttributes, anyXNameContainer);

	// test
	anyXNameContainer = xPropertySet->getPropertyValue(userDefinedAttributes);
	anyXNameContainer >>= xNameContainer;

	Sequence<OUString> names = xNameContainer->getElementNames();
	for (int i = 0; i < names.getLength(); i++) {
		OUString name = names[i];
		std::cout << name << std::endl;
	}
}

Any WatermarkPropertySetHelper::getUserDefinedAttribute(OUString name) {
	OUString userDefinedAttributes = OUSTRING("UserDefinedAttributes");

	Reference<XNameContainer> xNameContainer;
	Any anyXNameContainer = xPropertySet->getPropertyValue(userDefinedAttributes);
	anyXNameContainer >>= xNameContainer;

	AttributeData attributeData;
	try {
		xNameContainer->getByName(name) >>= attributeData;
		return Any(attributeData.Value);

	}
	catch (...) {
	}
	return Any();
}

PropertySetHelper& WatermarkPropertySetHelper::set(const char* name, Any value) {
	return PropertySetHelper::set(name, value);
}

PropertySetHelper& WatermarkPropertySetHelper::set(Sequence<OUString>& names, Sequence<Any>& values) {
	if (xPropertySet.is())
		for (sal_Int32 i = 0; i < names.getLength(); i++) {
			try {
				xPropertySet->setPropertyValue(names[i], values[i]);
			}
			catch (UnknownPropertyException&) {
				setUserDefinedAttribute(names[i], values[i]);
			}
		}
	return *this;
}

PropertySetHelper& WatermarkPropertySetHelper::set(NamesValuesHelper& namesValuesHelper) {
	return PropertySetHelper::set(namesValuesHelper);
}

Any WatermarkPropertySetHelper::get(const char *name) {
	if (xPropertySet.is()) {
		try {
			return xPropertySet->getPropertyValue(OUString::createFromAscii(name));
		}
		catch (UnknownPropertyException&) {
			return getUserDefinedAttribute(OUString::createFromAscii(name));
		}
	}
	return Any();
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
