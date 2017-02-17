/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/NaiveTextDocumentListener.hpp#1 $

/*
 *  Created by Aaron Gibson on 8/2/11.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__NAIVE_TEXT_DOCUMENT_LISTENER_HPP
#	define COM__HAATHI__DOCUMENT__NAIVE_TEXT_DOCUMENT_LISTENER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/TextDocumentListener.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/text/XTextFramesSupplier.hpp>
#include <com/sun/star/text/XTextGraphicObjectsSupplier.hpp>
#include <com/sun/star/text/XTextTablesSupplier.hpp>
#include "sdkEnd.hpp"

#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class
******************************************************************************/
class NaiveTextDocumentListener: public TextDocumentListener {
	private:
		bool deletedLast;
		OUString current;
		OUString last;

		// These are used to identify the actual changes in the document.
		Reference<com::sun::star::text::XTextGraphicObjectsSupplier> graphicsSupplier;
		Reference<com::sun::star::text::XTextFramesSupplier> textFramesSupplier;
		Reference<com::sun::star::text::XTextTablesSupplier> tableSupplier;

		std::vector<com::haathi::provenance::ProvenanceEvent*> getDifferences(OUString before, OUString after)
				throw (std::out_of_range, std::bad_alloc);

		// These functions are helper functions used to determine the actual action
		// performed.
		bool checkText();
		bool checkGraphics();
		bool checkTables();
		void thisModified();

		// com::sun::star::util::XModifyListener
		virtual void SAL_CALL modified(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException);
		// com::sun::star::document::XDocumentEventListener
		virtual void SAL_CALL documentEventOccured(const com::sun::star::document::DocumentEvent& event)
				throw (com::sun::star::uno::RuntimeException);

		virtual void SAL_CALL printJobEvent(const com::sun::star::view::PrintJobEvent& event)
			throw (com::sun::star::uno::RuntimeException);

	public:
		NaiveTextDocumentListener(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent,
				com::haathi::document::ProvenanceDocument& provenanceDocument,
				com::haathi::text::TextHelper& textHelper);
		virtual void checkMissingNotifications();
		virtual OUString getString();
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
