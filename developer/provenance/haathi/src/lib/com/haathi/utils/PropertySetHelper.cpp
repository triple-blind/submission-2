/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/PropertySetHelper.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/PropertySetHelper.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/HaathiException.hpp"

#include <fstream>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::sun::star::beans::Property;
using com::sun::star::beans::XPropertySetInfo;
using com::sun::star::uno::Any;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::XInterface;
using com::sun::star::uno::UNO_QUERY;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
PropertySetHelper::PropertySetHelper(Reference<XInterface> xInterface):
		xPropertySet(xInterface, UNO_QUERY) {
}

PropertySetHelper::~PropertySetHelper() {
}

PropertySetHelper& PropertySetHelper::set(const char* name, Any value) {
	if (xPropertySet.is())
		xPropertySet->setPropertyValue(OUString::createFromAscii(name), value);
	return *this;
}

PropertySetHelper& PropertySetHelper::set(Sequence<OUString>& names, Sequence<Any>& values) {
	if (xPropertySet.is())
		for (sal_Int32 i = 0; i < names.getLength(); i++)
			xPropertySet->setPropertyValue(names[i], values[i]);
	return *this;
}

PropertySetHelper& PropertySetHelper::set(NamesValuesHelper& namesValuesHelper) {
	return set(namesValuesHelper.getNames(), namesValuesHelper.getValues());
}

Any PropertySetHelper::get(const char *name) {
	try {
		if (xPropertySet.is())
			return xPropertySet->getPropertyValue(OUString::createFromAscii(name));
	}
	catch(...) {
	}
	return Any();
}

void PropertySetHelper::log(const char* filename) {
	std::ofstream log;
	log.open(filename, std::ios::out);
	if (!log.is_open())
		throw HaathiException("Could not open log file");

	Reference<XPropertySetInfo> xPropertySetInfo = xPropertySet->getPropertySetInfo();
	Sequence<Property> properties = xPropertySetInfo->getProperties();
	for (int i = 0; i < properties.getLength(); i++) {
		Property property = properties[i];
		log << property.Name << std::endl;
	}
	log.close();
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
