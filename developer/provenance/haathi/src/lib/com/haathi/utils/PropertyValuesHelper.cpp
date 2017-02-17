/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/PropertyValuesHelper.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/PropertyValuesHelper.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::sun::star::beans::PropertyValue;
using com::sun::star::uno::Any;
using com::sun::star::uno::Sequence;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
PropertyValuesHelper::PropertyValuesHelper(int size):
		size(size), propertyValues(Sequence<PropertyValue>(size)) {
	pos = 0;
}

PropertyValuesHelper::~PropertyValuesHelper() {
}

PropertyValuesHelper& PropertyValuesHelper::add(const char* name, Any value)
		throw (HaathiException) {
	if (pos < size) {
		propertyValues[pos].Name = OUString::createFromAscii(name);
		propertyValues[pos].Value = value;
		pos++;
	}
	else
		throw HaathiException("Capacity exceeded!");
	return *this;
}

Sequence<PropertyValue>& PropertyValuesHelper::get() {
	return propertyValues;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
