/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/SmartTextDocumentListener.cpp#2 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/document/SmartTextDocumentListener.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/document/CloudProvenanceDocument.hpp"
#include "com/haathi/exploit/DocInsert.hpp"
#include "com/haathi/exploit/MixInsert.hpp"
#include "com/haathi/exploit/DocDelete.hpp"
#include "com/haathi/exploit/MixDelete.hpp"

#include "com/haathi/exploit/EditPaste.hpp"
#include "com/haathi/exploit/HidePaste.hpp"

#include "com/haathi/exploit/SpacePaste.hpp"
#include "com/haathi/provenance/text/DeleteTextProvenance.hpp"
#include "com/haathi/provenance/text/InsertTextProvenance.hpp"
#include "com/haathi/provenance/text/ReplaceTextProvenance.hpp"
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/Guard.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/utils/Timer.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/document/XDocumentEventBroadcaster.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include "sdkEnd.hpp"

#include <algorithm>
#include <string>
#include <vector>
/******************************************************************************
Define
******************************************************************************/
#define DEBUG_OFF 0
#define DEBUG_ON  1

#define DEBUG DEBUG_OFF

// It seems that when using the extension code via the library from a C++
// executable, the selection is unavailable during the modified notification.
// See the bugModify project for details.  In order to run automated tests
// from C++, some assumptions are made and they are isolated using these defines.

#define GET_SELECTION_OFF 0
#define GET_SELECTION_ON  1

#define GET_SELECTION GET_SELECTION_ON
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::exploit;
using namespace com::haathi::provenance;
using namespace com::haathi::text;
using namespace com::haathi::utils;

using namespace com::sun::star::container;
using namespace com::sun::star::document;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
using namespace com::sun::star::view;

using namespace rtl;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class
******************************************************************************/
SmartTextDocumentListener::SmartTextDocumentListener(Reference<XComponentContext> xComponentContext,
		Reference<XComponent> xComponent,
		ProvenanceDocument& provenanceDocument, TextHelper& textHelper, bool interactive):
		TextDocumentListener(xComponentContext, xComponent, provenanceDocument, textHelper, interactive), deletedLast(false),
		skipNextSelectionChange(false), tmpShouldListen(true) {
#if EXPLOIT == EXPLOIT_DOC_INSERT
	docInsert.captureDocumentHistory(&documentHistory);
#elif EXPLOIT == EXPLOIT_MIX_INSERT
	mixInsert.captureDocumentHistory(&documentHistory);
#elif EXPLOIT == EXPLOIT_DOC_DELETE
	docDelete.captureDocumentHistory(&documentHistory);
#elif EXPLOIT == EXPLOIT_MIX_DELETE
	mixDelete.captureDocumentHistory(&documentHistory);
#elif EXPLOIT == EXPLOIT_HIDE_PASTE
	hidePaste.setDocumentHistory(&documentHistory);
#elif EXPLOIT == EXPLOIT_SPACE_PASTE
	spacePaste.setDocumentHistory(&documentHistory);
#endif

	documentHistory.setOldText(textHelper.getString());
	thisSelectionChanged(documentHistory.getOldSelectionsForUpdate());
	guard(&SmartTextDocumentListener::modified, 62, 7390427, __FILE__, __LINE__);
}

SmartTextDocumentListener::~SmartTextDocumentListener() {
}

std::vector<ProvenanceEvent*> SmartTextDocumentListener::getPartDifferences() {
	TimeKeeper timeKeeper(differencerTimer);

	OUString oldText = documentHistory.getOldText();
	OUString newText = documentHistory.getNewText();

	if (documentHistory.getNewSelectionSize() != 1 ||
			documentHistory.getNewSelectionLength() != 0)
		return getFullDifferences();

	std::vector<ProvenanceEvent*> result;
	int oldLength = oldText.getLength();
	int newLength = newText.getLength();
	int newLeft = documentHistory.getNewLeft();
	int oldSelectionLength = documentHistory.getOldSelectionLength();

	// This is the normal case with just an insertion cursor and no selection
	if (oldSelectionLength == 0) {
		int insertedLength = newLength - oldLength;

		if (documentHistory.getOldSelectionSize() != 1 || insertedLength == 0)
			return getFullDifferences();

		if (insertedLength > 0) {
			// This is assuming left to right insertion, isn't it?
			int insertedPos = newLeft - insertedLength; // should be same as oldLeft
			OUString insertedText = newText.copy(insertedPos, insertedLength);
			deletedLast = false;

#if EXPLOIT == EXPLOIT_HIDE_PASTE
			if (hidePaste.push_back(&result, insertedPos, insertedText))
				return result;
#elif EXPLOIT == EXPLOIT_SPACE_PASTE
			if (spacePaste.push_back(&result, insertedPos, insertedText))
				return result;
#elif EXPLOIT == EXPLOIT_EDIT_PASTE
			if (editPaste.push_back(&result, insertedPos, insertedText, documentHistory))
				return result;
#else
			result.push_back(new InsertTextProvenance(insertedPos, insertedText));
#endif
		}
		else {
			int deletedLength = -insertedLength;
			int deletedPos = newLeft;
			OUString deletedText = oldText.copy(deletedPos, deletedLength);

			result.push_back(new DeleteTextProvenance(deletedPos, deletedText));
			deletedLast = true;
		}
		return result;
	}

	// Something was or some things were selected and must have been replaced
	int oldSelectionSize = documentHistory.getOldSelectionSize();
	int insertedLength = newLength - (oldLength - oldSelectionLength);

	if (oldSelectionSize == 0 || (insertedLength % oldSelectionSize) != 0)
		return getFullDifferences();
	insertedLength /= oldSelectionSize;
	
	Selections& oldSelections = documentHistory.getOldSelections();
	OUString insertedText = newText.copy(documentHistory.getOldLeft(), insertedLength);
	int totalDeleted = 0;

	for (int i = 0; i < oldSelectionSize; i++) {
		int deletedPos = oldSelections[i].getLeft();
		int deletedLength = oldSelections[i].getWidth();
		OUString deletedText = oldText.copy(deletedPos, deletedLength);

		if (insertedLength == 0)
			result.push_back(new DeleteTextProvenance(deletedPos - deletedLength, deletedText));
		else
			result.push_back(new ReplaceTextProvenance(deletedPos - deletedLength, deletedText, insertedText));
		totalDeleted += (deletedLength - insertedLength);
	}
	deletedLast = true;
	return result;
}

// This is from NaiveTextDocumentListener and is used as the fallback position.
// It may not get the correct edits, but gets a possible edit or edits that
// explain the differences.
std::vector<ProvenanceEvent*> SmartTextDocumentListener::getFullDifferences() {
	OUString before = documentHistory.getOldText();
	OUString after = documentHistory.getNewText();

	std::vector<ProvenanceEvent*> result;
	int beforeLength = before.getLength();
	int afterLength = after.getLength();

	if (beforeLength < afterLength) {
		// Characters were inserted.
		OUString inserted;
		int insertedLength = 0;
		sal_Int32 insertedPos;
		sal_Int32 beforePos = 0;
		sal_Int32 afterPos = 0;
		for (; beforePos < beforeLength && afterPos < afterLength; afterPos++)
			if (before[beforePos] != after[afterPos]) {
				if (insertedLength == 0)
					insertedPos = afterPos; // start inserting
				inserted += OUString(after[afterPos]); // keep inserting
				insertedLength++;
			}
			else {
				if (insertedLength != 0) {
					result.push_back(new InsertTextProvenance(insertedPos, inserted));
					inserted = OUString(); // transition away from inserting
					insertedLength = 0;
				}
				beforePos++;
			}
		if (afterPos < afterLength) { // need to insert the rest
			if (insertedLength == 0)
				insertedPos = afterPos;
			inserted += after.copy(afterPos);
			result.push_back(new InsertTextProvenance(insertedPos, inserted));
			return result;
		}
		if (beforePos < beforeLength) { // need to delete the rest
			if (insertedLength != 0)
				result.push_back(new InsertTextProvenance(insertedPos, inserted));
			result.push_back(new DeleteTextProvenance(afterPos, before.copy(beforePos)));
			return result;
		}
		if (insertedLength != 0)
			result.push_back(new InsertTextProvenance(insertedPos, inserted));
	}

	if (beforeLength > afterLength) {
		// Characters were deleted.
		OUString deleted;
		int deletedLength = 0;
		sal_Int32 deletedPos;
		sal_Int32 beforePos = 0;
		sal_Int32 afterPos = 0;
		for (; beforePos < beforeLength && afterPos < afterLength; beforePos++)
			if (before[beforePos] != after[afterPos]) {
				if (deletedLength == 0)
					deletedPos = afterPos; // start deleting
				deleted += OUString(before[beforePos]); // keep deleting
				deletedLength++;
			}
			else {
				if (deletedLength != 0) {
					result.push_back(new DeleteTextProvenance(deletedPos, deleted));
					deleted = OUString(); // transition away from deleting
					deletedLength = 0;
				}
				afterPos++;
			}
		if (beforePos < beforeLength) { // need to delete the rest
			if (deletedLength == 0)
				deletedPos = afterPos;
			deleted += before.copy(beforePos);
			result.push_back(new DeleteTextProvenance(deletedPos, deleted));
			return result;
		}
		if (afterPos < afterLength) { // need to insert the rest
			if (deletedLength != 0)
				result.push_back(new DeleteTextProvenance(deletedPos, deleted));
			result.push_back(new InsertTextProvenance(afterPos, after.copy(afterPos)));
			return result;
		}
		if (deletedLength != 0)
			result.push_back(new DeleteTextProvenance(deletedPos, deleted));
	}

	if (beforeLength == afterLength) {
		// Characters were replaced.
		OUString deleted;
		int deletedLength = 0;
		OUString inserted;
		int insertedLength = 0;
		sal_Int32 replacedPos;
		sal_Int32 pos = 0;
		for (; pos < beforeLength; pos++)
			if (before[pos] != after[pos]) {
				if (deleted.getLength() == 0)
					replacedPos = pos; // start replacing
				deleted += OUString(before[pos]);
				deletedLength++;
				inserted += OUString(after[pos]);
				insertedLength++;
			}
			else {
				if (deleted.getLength() != 0) {
					result.push_back(new ReplaceTextProvenance(replacedPos, deleted, inserted));
					deleted = OUString();
					deletedLength = 0;
					inserted = OUString();
					insertedLength = 0;
				}
			}
		if (deleted.getLength() != 0)
			result.push_back(new ReplaceTextProvenance(replacedPos, deleted, inserted));
	}
	return result;
}

void SAL_CALL SmartTextDocumentListener::modified(const EventObject& event)
		throw (RuntimeException) {
	if (!shouldListen || !tmpShouldListen)
		return;
	if (!isModified())
		return;

	tmpShouldListen = false;
	thisModified();
	tmpShouldListen = true;
	// Just checked on selection, so don't have to next time
	skipNextSelectionChange = true;
}

void SmartTextDocumentListener::thisModified() {
	checkText();
}

void SAL_CALL SmartTextDocumentListener::selectionChanged(const EventObject& event)
		throw (com::sun::star::uno::RuntimeException) {
	static int count = 0;

	if (!shouldListen || !tmpShouldListen)
		return;
	if (skipNextSelectionChange) {
		skipNextSelectionChange = false;
		return;
	}

	count++;

	tmpShouldListen = false;
	// This below causes events all on its own and they must be supressed
	thisSelectionChanged(documentHistory.getOldSelectionsForUpdate());
	tmpShouldListen = true;
}

void SmartTextDocumentListener::thisSelectionChanged(std::vector<Range>& ranges) {
#if GET_SELECTION == GET_SELECTION_ON
	textHelper.fillRanges(ranges);
#else
	// The initial value depends on how many times this is called before typing commences.
	// Unfortunately, this hack does not prevent OpenOffice from hanging after the call to
	// getPartDifferences below.  Is it the fault of textHelper.getString()?
	static int pos = -6;

	pos++;
	ranges.clear();
	ranges.push_back(Range(pos));
#endif
}

bool SmartTextDocumentListener::checkText() {
	try {
		documentHistory.setNewText(textHelper.getString());
		thisSelectionChanged(documentHistory.getNewSelectionsForUpdate());

		std::vector<ProvenanceEvent*> diffs = getPartDifferences();
		if (diffs.size() > 1)
			std::cerr << "Edit was explained by more than one difference!" << std::endl;

#if DEBUG == DEBUG_ON
		{
			OUString oldText = documentHistory.getOldText();
			OUString newText = documentHistory.getNewText();

			std::vector<ProvenanceEvent*>::iterator itr;

			for (itr = diffs.begin(); itr != diffs.end(); ++itr)
				oldText = (*itr)->apply(oldText);
			if (isFalse(oldText.equals(newText))) {
				std::cerr << "Edit is not explained properly!" << std::endl;
			}
		}
#endif
		if (diffs.size() > 0)
			provenanceDocument.addProvenanceEvents(diffs, documentHistory.getNewText());
		documentHistory.advance();
		return !diffs.empty();
	}
	catch (...) {
	}
	return false;
}

void SmartTextDocumentListener::checkMissingNotifications() {
	// The CloudDocument calls this when menu selections cause events.  Some, but
	// not all of the events involve text changes which require a good selection.
	// They could pass in an argument, checkSelection.

	if (deletedLast)
		thisModified();
	else
		thisSelectionChanged(documentHistory.getOldSelectionsForUpdate());
}

OUString SmartTextDocumentListener::getString() {
	return documentHistory.getOldText();
}

void SAL_CALL SmartTextDocumentListener::printJobEvent(const PrintJobEvent& event)
	throw (RuntimeException) {
		provenanceDocument.printJobEvent(event);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
