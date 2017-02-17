/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/text/TextHelper.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TEXT__TEXT_HELPER_HPP
#	define COM__HAATHI__TEXT__TEXT_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/text/IText.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace text {
/******************************************************************************
Class
******************************************************************************/
class TextHelper: public IText {
	protected:
		Reference<com::sun::star::text::XText> xText;
		Reference<com::sun::star::view::XSelectionSupplier> xSelectionSupplier;

		static OUString getStringFromTextRange(Reference<com::sun::star::text::XText> xText,
				Reference<com::sun::star::text::XTextRange> xTextRange);
	public:
		static OUString getStringFromTextCursor(Reference<com::sun::star::text::XTextCursor> xTextCursor);

		TextHelper(Reference<com::sun::star::lang::XComponent> xComponent);
		virtual sal_Int32 getWidth(Reference<com::sun::star::text::XTextRange> xTextRange);
		virtual sal_Int32 getOffset(Reference<com::sun::star::text::XTextRange> xTextRange);
		virtual sal_Int32 getEndOffset(Reference<com::sun::star::text::XTextRange> xTextRange);
		virtual void setString(const OUString& ouString);
		virtual OUString getString();
		virtual OUString getString(Reference<com::sun::star::text::XTextRange> xTextRange);
		virtual std::vector<com::haathi::text::Selection> getSelections();
		virtual void fillRanges(std::vector<com::haathi::text::Range>& ranges);
		virtual sal_Int32 getCursorPosition();
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
