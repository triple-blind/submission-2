/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/BasicProvenanceEvents.cpp#1 $

/*
 *
 * Implementation of BasicProvenanceEvents
 * 
 * Author: Aaron Gibson
 * Date: August 10, 2011
 */

/*
                            getEventType       getEventDescription       printToTable

CreateDocumentProvenance    create document    printToTable(): ., ., .   Created document (but did not share | and shared with . other(s))
ExtensionDocumentProvenance extension document printToTable()            Extended document with "..."
ExportDocumentProvenance    export document    printToTable(): .         Exported document
ListenDocumentProvenance    listen document    printToTable() on "..."   Listened to "..."
MenuDocumentProvenance      menu document      printToTable()            Selected menu item "..."
OpenDocumentProvenance      open document      printToTable()            Opened document
PrintDocumentProvenance     print document     printToTable(): .         Printed document
PrintJobDocumentProvenance  print document job printToTable()            Printed document job: 
SaveDocumentProvenance      save document      printToTable()            Saved document
SendDocumentProvenance      send document      printToTable(): .         Sent document
ShareDocumentProvenance     share document     printToTable(): ., ., .   Shared document with . other(s)
WatermarkDocumentProvenance watermark document printToTable()            Watermarked document with "..." scheme

CopyTextProvenance          copy text          printToTable() at .       Copied "..."
CutTextProvenance           cut text           printToTable() at .       Cut "..."
PasteTextProvenance         paste text         printToTable() at .       Pasted over "..."

DeleteTextProvenance        delete text        printToTable() at .       Deleted "..."
InsertTextProvenance        insert text        printToTable() at .       Inserted "..."
ReplaceTextProvenance       replace text       printToTable() at .       Replaced "..." with "..."
WatermarkTextProvenance     watermark text     printToTable() at .       Watermarked "..." with "..."
*/
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/BasicProvenanceEvents.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/text/TextHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
//
#include "sdkBeg.hpp"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/uno/Any.hxx>
#include "sdkEnd.hpp"

#include <cmath>
/******************************************************************************
Using
******************************************************************************/
using com::haathi::text::TextHelper;
using com::haathi::utils::convertToOUString;

using com::sun::star::beans::XPropertySet;
using com::sun::star::lang::XComponent;
using com::sun::star::text::XText;
using com::sun::star::text::XTextCursor;
using com::sun::star::text::XTextRange;
using com::sun::star::uno::Any;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::util::Color;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
Static functions
******************************************************************************/
void appendUserEmails(rtl::OUStringBuffer& result, const std::vector<std::string>& userEmails) {
	if (userEmails.size() > 0) {
		result.appendAscii(": ");
		for (unsigned int i = 0; i < userEmails.size(); i++) {
			if (i > 0)
				result.appendAscii(", ");
			result.append(convertToOUString(userEmails[i]));
		}
	}
}

void appendOthers(rtl::OUStringBuffer& result, const std::vector<std::string>& userEmails) {
	result.append((sal_Int32) userEmails.size());
	result.appendAscii(" other");
	if (userEmails.size() != 1)
		result.appendAscii("s");
}

bool moveTo(Reference<XTextCursor> xTextCursor, sal_Int32 pos) {
	bool success = true;

	xTextCursor->gotoStart(sal_False);
	while (pos > 0) {
		sal_Int16 amount = (sal_Int16) std::min((int) 0x00007fff, (int) pos);
		success &= (xTextCursor->goRight(amount, sal_False) == sal_True); // Note this 16-bit limitation!
		pos -= amount;
	}
	return success;
}

bool selectNext(Reference<XTextCursor> xTextCursor, sal_Int32 length) {
	bool success = true;

	while (length > 0) {
		sal_Int16 amount = (sal_Int16) std::min((int) 0x00007fff, (int) length);
		success &= (xTextCursor->goRight(amount, sal_True) == sal_True); // Note this 16-bit limitation!
		length -= amount;
	}
	return success;
}

bool select(Reference<XTextCursor> xTextCursor, sal_Int32 from, OUString text) {
	bool success = moveTo(xTextCursor, from);
	sal_Int32 length = text.getLength();

	if (!length)
		return success;
	success &= selectNext(xTextCursor, length);
	if (success) {
		OUString actualText = TextHelper::getStringFromTextCursor(xTextCursor);

		success = actualText.equals(text) == sal_True;
	}
	return success;
}

void highlight(Reference<XTextCursor> xTextCursor, Color color) {
	Reference<XPropertySet> xProp(xTextCursor, UNO_QUERY);
	xProp->setPropertyValue(OUSTRING("CharBackColor"), Any(color));
}

void unhighlight(Reference<XTextCursor> xTextCursor) {
	Reference<XPropertySet> xProp(xTextCursor, UNO_QUERY);
	xProp->setPropertyValue(OUSTRING("CharBackColor"), Any((sal_Int32) 0xffffffff));
}

void insert(Reference<XText> xText, Reference<XTextCursor> xTextCursor, const OUString& text, Color color) {
	Reference<XTextRange> xTextRange(xTextCursor, UNO_QUERY);
	xText->insertString(xTextRange, text, sal_True);
	highlight(xTextCursor, color);
}

void erase(Reference<XText> xText, Reference<XTextCursor> xTextCursor) {
	unhighlight(xTextCursor);
	Reference<XTextRange> xTextRange(xTextCursor, UNO_QUERY);
	xText->insertString(xTextRange, OUSTRING(""), sal_True);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
