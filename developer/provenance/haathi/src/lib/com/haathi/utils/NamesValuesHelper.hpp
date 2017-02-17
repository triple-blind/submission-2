/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/NamesValuesHelper.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__NAMES_VALUES_HELPER_HPP
#	define COM__HAATHI__UTILS__NAMES_VALUES_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/HaathiException.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
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
class NamesValuesHelper {
	protected:
		int pos;
		int size;
		com::sun::star::uno::Sequence<OUString> names;
		com::sun::star::uno::Sequence<com::sun::star::uno::Any> values;
	public:
		NamesValuesHelper();
		NamesValuesHelper(int size);
		virtual ~NamesValuesHelper();

		NamesValuesHelper& add(const rtl::OUString name, com::sun::star::uno::Any value)
				throw (com::haathi::utils::HaathiException);
		NamesValuesHelper& add(const char* name, com::sun::star::uno::Any value)
				throw (com::haathi::utils::HaathiException);
		NamesValuesHelper& add(NamesValuesHelper& other)
				throw (com::haathi::utils::HaathiException);
		com::sun::star::uno::Sequence<OUString>& getNames();
		com::sun::star::uno::Sequence<com::sun::star::uno::Any>& getValues();
		int getSize();
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
