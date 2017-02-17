/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/NaiveTextDocumentListener.cpp#1 $

/*
 *  Created by Aaron Gibson on 8/2/11.
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/document/NaiveTextDocumentListener.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "Config.hpp"

#include "com/haathi/provenance/text/DeleteTextProvenance.hpp"
#include "com/haathi/provenance/text/InsertTextProvenance.hpp"
#include "com/haathi/provenance/text/ReplaceTextProvenance.hpp"
#include "com/haathi/utils/Guard.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/document/XDocumentEventBroadcaster.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include "sdkEnd.hpp"

#include "Config.hpp"

#include <string>
#include <fstream>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::provenance;
using namespace com::haathi::text;
using namespace com::haathi::utils;

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
NaiveTextDocumentListener::NaiveTextDocumentListener(Reference<XComponentContext> xComponentContext,
		Reference<XComponent> xComponent,
		ProvenanceDocument& provenanceDocument, TextHelper& textHelper):
		TextDocumentListener(xComponentContext, xComponent, provenanceDocument, textHelper),	
		deletedLast(false), current(), last(),
		graphicsSupplier(xComponent, UNO_QUERY),
		textFramesSupplier(xComponent, UNO_QUERY),
		tableSupplier(xComponent, UNO_QUERY) {
	last = current = textHelper.getString();
	guard(&NaiveTextDocumentListener::modified, 62, 7390427, __FILE__, __LINE__);
}

std::vector<ProvenanceEvent*> NaiveTextDocumentListener::getDifferences(OUString before, OUString after)
		throw (std::out_of_range, std::bad_alloc) {
	std::vector<ProvenanceEvent*> result;
	// TODO: These could be remembered as well with state along with cursor position
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

void SAL_CALL NaiveTextDocumentListener::modified(const EventObject& event)
		throw (RuntimeException) {
	thisModified();
}

void NaiveTextDocumentListener::thisModified() {
	if (!shouldListen)
		return;
	if (!isModified())
		return;

	try {
		if (checkText())
			xModifiable->setModified(sal_True); // Just in case changes are not complete
		checkGraphics();
	} catch (com::sun::star::uno::Exception & e) {
		std::cerr << "In ProvenanceModifyListener::modified: UNO Exception: ";
		std::cerr << e.Message << std::endl;
	} catch (std::exception & e2 ) {
		std::cerr << "In ProvenanceModifyListener::modified: std::exception: ";
		std::cerr << e2.what() << std::endl;
	}
}
	
void SAL_CALL NaiveTextDocumentListener::documentEventOccured(const com::sun::star::document::DocumentEvent& event)
		throw (RuntimeException) {
	std::ofstream file("C:\\Users\\Keith Alcock\\Documents\\My Data\\document_events.txt",
			std::ios::out | std::ios::app | std::ios::binary);
	if (file.is_open()) {
		file << "Name: " << event.EventName << std::endl;
//		file << "Supplement: " << convertToString(event.Supplement) << std::endl;
		file.close();
	}
}

bool NaiveTextDocumentListener::checkText() {
	try {
		last = current;
		current = textHelper.getString();
		std::vector<ProvenanceEvent*> diffs = getDifferences(last, current);
		provenanceDocument.addProvenanceEvents(diffs, current);
		return !diffs.empty();
	}
	catch (std::out_of_range&) {
		std::cerr
			<< "std::out_of_range exception thrown... Ignoring this instance of provenance." << std::endl;
	}
	catch (std::exception & e1) {
		std::cerr << "std::exception thrown: " << e1.what() << std::endl;
	}
	return false;
}

bool NaiveTextDocumentListener::checkGraphics() {
	/*	Reference< XNameAccess > graphicsobjects = graphics_supplier->getGraphicObjects();
	 Sequence< OUString > textnames = graphicsobjects->getElementNames();
	 int i;
	 for(i = 0; i < textnames.getLength(); ++i){
	 std::cout << "Graphics: " << textnames[i] << std::endl;
	 Any val = graphicsobjects->getByName(textnames[i]);
	 Reference< XGraphic > xgraphic;
	 val >>= xgraphic;
	 
	 }
	 std::cout << "End Graphics check." << std::endl;
	 */return false;
}

bool NaiveTextDocumentListener::checkTables() {
	return false;
}

void NaiveTextDocumentListener::checkMissingNotifications() {
	// The state needs to be correct now, but it is possible that delayed deletions
	// have not yet occurred.  So, need to check for edits.
	thisModified();
}

OUString NaiveTextDocumentListener::getString() {
	return current;
}

void SAL_CALL NaiveTextDocumentListener::printJobEvent(const PrintJobEvent& event)
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
