/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/text/InsertTextProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/text/InsertTextProvenance.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/document/IDocumentStateTracker.hpp"
#include "com/haathi/provenance/BasicProvenanceEvents.hpp"
#include "com/haathi/utils/Guard.hpp"
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
using com::haathi::document::IDocumentStateTracker;
using com::haathi::text::Range;
using com::haathi::utils::escape;
using com::haathi::utils::guard;

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
InsertTextProvenance
******************************************************************************/
InsertTextProvenance::InsertTextProvenance(): offset(-1), data() {
	guard(&InsertTextProvenance::getEventDescription, 65, 0, __FILE__, __LINE__);
}

InsertTextProvenance::InsertTextProvenance(int offset, OUString data):
		offset(offset), data(data) {
}

InsertTextProvenance& InsertTextProvenance::operator=(const InsertTextProvenance & other) {
	// We MUST call the superclass operator as well. 
	ProvenanceTextEvent::operator=(other);
	this->offset = other.offset;
	this->data = other.data;
	return *this;
}

InsertTextProvenance::InsertTextProvenance(const InsertTextProvenance& other):
		ProvenanceTextEvent(other), offset(other.offset), data(other.data) {
}

std::string InsertTextProvenance::getEventType() const {
	return std::string("insert text");
}

OUString InsertTextProvenance::getEventDescription() const {
	rtl::OUStringBuffer result;

	result.append(printToTable());
	result.appendAscii(" at ");
	result.append((sal_Int32) offset); // offset is an int and must be converted
	return result.makeStringAndClear();
}

OUString InsertTextProvenance::printToTable() const {
	rtl::OUStringBuffer result;

	result.appendAscii("Inserted \"");
	result.append(escape(data));
	result.appendAscii("\"");
	return result.makeStringAndClear();
}

OUString InsertTextProvenance::toString() const {
	rtl::OUStringBuffer result;
	result.append(ProvenanceTextEvent::toString());
	result.appendAscii("Data: ");
	result.append(escape(data));
	return result.makeStringAndClear();
}

bool InsertTextProvenance::redo(Reference<XComponent> xComponent, Color color) const {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	bool success = moveTo(xTextCursor, offset);
	insert(xText, xTextCursor, data, color);
	return success;
}

bool InsertTextProvenance::undo(Reference<XComponent> xComponent, Color color) const {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	bool success = select(xTextCursor, offset, data);
	erase(xText, xTextCursor);
	return success;
}

// applies to input
OUString InsertTextProvenance::apply(OUString input) {
	return input.copy(0, offset) + data + input.copy(offset);
}

// getting input to output
void InsertTextProvenance::track(IDocumentStateTracker& documentStateTracker) {
	documentStateTracker.insertText(getWatermarkRange());
}

// watermark on the output
Range InsertTextProvenance::getWatermarkRange() {
	return Range(offset, offset + data.getLength());
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::InsertTextProvenance);
/*****************************************************************************/
