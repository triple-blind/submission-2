/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/text/CopyTextProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/text/CopyTextProvenance.hpp"
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
CopyTextProvenance
******************************************************************************/
CopyTextProvenance::CopyTextProvenance(): offset(-1), data() {
}

CopyTextProvenance::CopyTextProvenance(int offset, OUString data):
		offset(offset), data(data) {
}

std::string CopyTextProvenance::getEventType() const {
	return std::string("copy text");
}

OUString CopyTextProvenance::printToTable() const {
	rtl::OUStringBuffer result;

	result.appendAscii("Copied \"");
	result.append(escape(data));
	result.appendAscii("\"");
	return result.makeStringAndClear();
}

OUString CopyTextProvenance::getEventDescription() const {
	rtl::OUStringBuffer result;

	result.append(printToTable());
	result.appendAscii(" at ");
	result.append((sal_Int32) offset); // offset is an int and must be converted
	return result.makeStringAndClear();
}

bool CopyTextProvenance::redo(Reference<XComponent> xComponent, Color color) const {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	bool success = select(xTextCursor, offset, data);
	highlight(xTextCursor, color);
	return success;
}

bool CopyTextProvenance::undo(Reference<XComponent> xComponent, Color color) const {
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
BOOST_CLASS_EXPORT(com::haathi::provenance::CopyTextProvenance);
/*****************************************************************************/
