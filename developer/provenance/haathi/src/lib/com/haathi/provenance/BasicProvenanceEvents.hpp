/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/BasicProvenanceEvents.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__BASIC_PROVENANCE_EVENTS_H
#	define COM__HAATHI__PROVENANCE__BASIC_PROVENANCE_EVENTS_H
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/util/Color.hpp>

#include <rtl/ustrbuf.hxx>
#include "sdkEnd.hpp"

#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
Static functions
******************************************************************************/
void appendUserEmails(rtl::OUStringBuffer& result, const std::vector<std::string>& userEmails);
void appendOthers(rtl::OUStringBuffer& result, const std::vector<std::string>& userEmails);

bool moveTo(Reference<com::sun::star::text::XTextCursor> xTextCursor, sal_Int32 pos);
bool selectNext(Reference<com::sun::star::text::XTextCursor> xTextCursor, sal_Int32 length);
bool select(Reference<com::sun::star::text::XTextCursor> xTextCursor, sal_Int32 from, OUString text);
void highlight(Reference<com::sun::star::text::XTextCursor> xTextCursor, com::sun::star::util::Color color);
void unhighlight(Reference<com::sun::star::text::XTextCursor> xTextCursor);
void insert(Reference<com::sun::star::text::XText> xText, Reference<com::sun::star::text::XTextCursor> xTextCursor,
		const OUString& text, com::sun::star::util::Color color);
void erase(Reference<com::sun::star::text::XText> xText, Reference<com::sun::star::text::XTextCursor> xTextCursor);
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
