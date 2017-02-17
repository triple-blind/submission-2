/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/text/DeleteTextProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/text/DeleteTextProvenance.hpp"
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
DeleteTextProvenance
******************************************************************************/
DeleteTextProvenance::DeleteTextProvenance():
		offset(-1), data() {
	guard(&DeleteTextProvenance::getEventDescription, 65, 0, __FILE__, __LINE__);
}

DeleteTextProvenance::DeleteTextProvenance(int offset, OUString data):
		offset(offset), data(data) {
}

DeleteTextProvenance& DeleteTextProvenance::operator=(const DeleteTextProvenance & other) {
	// We MUST call the superclass operator as well. 
	ProvenanceTextEvent::operator=(other);
	this->offset = other.offset;
	this->data = other.data;
	return *this;
}

DeleteTextProvenance::DeleteTextProvenance(const DeleteTextProvenance& other):
		ProvenanceTextEvent(other), offset(other.offset), data(other.data) {
}

std::string DeleteTextProvenance::getEventType() const {
	return std::string("delete text");
}

OUString DeleteTextProvenance::getEventDescription() const {
	rtl::OUStringBuffer result;

	result.append(this->printToTable());
	result.appendAscii(" at ");
	result.append((sal_Int32) offset); // offset is an int and must be converted
	return result.makeStringAndClear();
}

OUString DeleteTextProvenance::printToTable() const {
	rtl::OUStringBuffer result;

	result.appendAscii("Deleted \"");
	result.append(escape(data));
	result.appendAscii("\"");
	return result.makeStringAndClear();
}

OUString DeleteTextProvenance::toString() const {
	rtl::OUStringBuffer result;
	result.append(ProvenanceTextEvent::toString());
	result.appendAscii("Data: ");
	result.append(escape(data));
	return result.makeStringAndClear();
}

bool DeleteTextProvenance::redo(Reference<XComponent> xComponent, Color color) const {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	bool success = select(xTextCursor, offset, data);
	erase(xText, xTextCursor);
	return success;
}

bool DeleteTextProvenance::undo(Reference<XComponent> xComponent, Color color) const {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	bool success = moveTo(xTextCursor, offset);
	insert(xText, xTextCursor, data, color);
	return success;
}

OUString DeleteTextProvenance::apply(OUString input) {
	return input.copy(0, offset) + input.copy(offset + data.getLength());
}

void DeleteTextProvenance::track(IDocumentStateTracker& documentStateTracker) {
	documentStateTracker.deleteText(getWatermarkRange());
}

Range DeleteTextProvenance::getWatermarkRange() {
	return Range(offset);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::DeleteTextProvenance);
/*****************************************************************************/
