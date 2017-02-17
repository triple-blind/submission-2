/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/text/PasteTextProvenance.cpp#1 $

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
#include "com/haathi/provenance/text/PasteTextProvenance.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/provenance/BasicProvenanceEvents.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/util/Color.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::escape;

using com::sun::star::lang::XComponent;
using com::sun::star::text::XText;
using com::sun::star::text::XTextCursor;
using com::sun::star::text::XTextDocument;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::util::Color;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
PasteTextProvenance
******************************************************************************/
PasteTextProvenance::PasteTextProvenance(): offset(-1), data() {
}

PasteTextProvenance::PasteTextProvenance(int offset, OUString data):
		offset(offset), data(data) {
}

std::string PasteTextProvenance::getEventType() const {
	return std::string("paste text");
}

OUString PasteTextProvenance::printToTable() const {
	rtl::OUStringBuffer result;

	result.appendAscii("Pasted over \"");
	result.append(escape(data));
	result.appendAscii("\"");
	return result.makeStringAndClear();
}

OUString PasteTextProvenance::getEventDescription() const {
	rtl::OUStringBuffer result;

	result.append(printToTable());
	result.appendAscii(" at ");
	result.append((sal_Int32) offset); // offset is an int and must be converted
	return result.makeStringAndClear();
}

bool PasteTextProvenance::redo(Reference<XComponent> xComponent, Color color) const {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	bool success = select(xTextCursor, offset, data);
	highlight(xTextCursor, color);
	return success;
}

bool PasteTextProvenance::undo(Reference<XComponent> xComponent, Color color) const {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	bool success = select(xTextCursor, offset, data);
	unhighlight(xTextCursor);
	return success;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::PasteTextProvenance);
/*****************************************************************************/
