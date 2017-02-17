/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/PropertyValuesHelper.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__PROPERTY_VALUES_HELPER_HPP
#	define COM__HAATHI__UTILS__PROPERTY_VALUES_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/HaathiException.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
class PropertyValuesHelper {
	protected:
		int pos;
		int size;
		com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue> propertyValues;
	public:
		PropertyValuesHelper(int size);
		virtual ~PropertyValuesHelper();

		PropertyValuesHelper& add(const char* name, com::sun::star::uno::Any value)
				throw(com::haathi::utils::HaathiException);
		com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& get();
};
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
