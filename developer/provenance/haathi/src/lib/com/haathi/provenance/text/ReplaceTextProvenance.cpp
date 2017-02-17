/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/text/ReplaceTextProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/text/ReplaceTextProvenance.hpp"
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
ReplaceTextProvenance
******************************************************************************/
ReplaceTextProvenance::ReplaceTextProvenance():
		offset(-1), before(), after() {
}

ReplaceTextProvenance::ReplaceTextProvenance(int offset, OUString before,
		OUString after): offset(offset), before(before), after(after) {
}

ReplaceTextProvenance& ReplaceTextProvenance::operator=(
		const ReplaceTextProvenance& other) {
	ProvenanceTextEvent::operator=(other);
	offset = other.offset;
	before = other.before;
	after = other.after;
	return *this;
}

ReplaceTextProvenance::ReplaceTextProvenance(const ReplaceTextProvenance& other):
		ProvenanceTextEvent(other), offset(other.offset), before(other.before), after(other.after) {
}

std::string ReplaceTextProvenance::getEventType() const {
	return std::string("replace text");
}

OUString ReplaceTextProvenance::getEventDescription() const {
	rtl::OUStringBuffer result;

	result.append(printToTable());
	result.appendAscii(" at ");
	result.append((sal_Int32) offset); // offset is an int and must be converted
	return result.makeStringAndClear();
}

bool ReplaceTextProvenance::redo(Reference<XComponent> xComponent, Color color) const {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	bool success = select(xTextCursor, offset, OUString(before));
	insert(xText, xTextCursor, OUString(after), color);
	return success;
}

bool ReplaceTextProvenance::undo(Reference<XComponent> xComponent, Color color) const {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	bool success = select(xTextCursor, offset, OUString(after));
	insert(xText, xTextCursor, OUString(before), color);
	return success;
}

OUString ReplaceTextProvenance::printToTable() const {
	rtl::OUStringBuffer result;

	result.appendAscii("Replaced \"");
	result.append(escape(before));
	result.appendAscii("\" with \"");
	result.append(escape(after));
	result.appendAscii("\"");
	return result.makeStringAndClear();
}

OUString ReplaceTextProvenance::apply(OUString input) {
	return input.copy(0, offset) + after + input.copy(offset + before.getLength());
}

void ReplaceTextProvenance::track(IDocumentStateTracker& documentStateTracker) {
	documentStateTracker.replaceText(
			Range(offset, offset + before.getLength()),
			Range(offset, offset + after.getLength())
	);
}

Range ReplaceTextProvenance::getWatermarkRange() {
	return Range(offset, offset + after.getLength());
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::ReplaceTextProvenance);
/*****************************************************************************/
