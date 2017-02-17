/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/text/IText.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TEXT__ITEXT_HPP
#	define COM__HAATHI__TEXT__ITEXT_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/text/Range.hpp"
#include "com/haathi/text/Selection.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/text/XTextRange.hpp>
#include <rtl/ustring.hxx>
#include "sdkEnd.hpp"

#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace text {
/******************************************************************************
Class
******************************************************************************/
class IText {
	public:
		virtual sal_Int32 getWidth(Reference<com::sun::star::text::XTextRange> xTextRange) = 0;
		virtual sal_Int32 getOffset(Reference<com::sun::star::text::XTextRange> xTextRange) = 0;
		virtual OUString getString() = 0;
		virtual OUString getString(Reference<com::sun::star::text::XTextRange> xTextRange) = 0;
		virtual std::vector<com::haathi::text::Selection> getSelections() = 0;
		virtual void fillRanges(std::vector<com::haathi::text::Range>& ranges) = 0;
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
