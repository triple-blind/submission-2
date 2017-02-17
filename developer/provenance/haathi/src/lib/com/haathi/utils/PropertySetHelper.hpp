/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/PropertySetHelper.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__PROPERTY_SET_HELPER_HPP
#	define COM__HAATHI__UTILS__PROPERTY_SET_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/NamesValuesHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/XInterface.hpp>
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
class PropertySetHelper {
	protected:
		Reference<com::sun::star::beans::XPropertySet> xPropertySet;

	public:
		PropertySetHelper(Reference<com::sun::star::uno::XInterface> xInterface);
		virtual ~PropertySetHelper();
		virtual PropertySetHelper& set(const char* name, com::sun::star::uno::Any value);
		virtual PropertySetHelper& set(com::sun::star::uno::Sequence<OUString>& names, com::sun::star::uno::Sequence<com::sun::star::uno::Any>& values);
		virtual PropertySetHelper& set(NamesValuesHelper& namesValuesHelper);
		virtual com::sun::star::uno::Any get(const char *name);
		virtual void log(const char* filename);
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
