/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/utils/WatermarkPropertySetHelper.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__UTILS__WATERMARK_PROPERTY_SET_HELPER_HPP
#	define COM__HAATHI__WATERMARK__UTILS__WATERMARK_PROPERTY_SET_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/PropertySetHelper.hpp"
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
class WatermarkPropertySetHelper: public com::haathi::utils::PropertySetHelper {
	protected:
		void setUserDefinedAttribute(OUString name, com::sun::star::uno::Any anyValue);
		com::sun::star::uno::Any getUserDefinedAttribute(OUString name);

	public:
		WatermarkPropertySetHelper(Reference<com::sun::star::uno::XInterface> xInterface);
		virtual ~WatermarkPropertySetHelper();
		virtual com::haathi::utils::PropertySetHelper& set(const char* name, com::sun::star::uno::Any value);
		virtual com::haathi::utils::PropertySetHelper& set(com::sun::star::uno::Sequence<OUString>& names,
				com::sun::star::uno::Sequence<com::sun::star::uno::Any>& values);
		virtual PropertySetHelper& set(com::haathi::utils::NamesValuesHelper& namesValuesHelper);
		virtual com::sun::star::uno::Any get(const char *name);
};
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
