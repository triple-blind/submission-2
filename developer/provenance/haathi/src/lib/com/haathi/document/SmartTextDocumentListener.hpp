/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/SmartTextDocumentListener.hpp#2 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__SMART_TEXT_DOCUMENT_LISTENER_HPP
#	define COM__HAATHI__DOCUMENT__SMART_TEXT_DOCUMENT_LISTENER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/TextDocumentListener.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"
#include "com/haathi/text/Range.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/view/PrintJobEvent.hpp>
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
class SmartTextDocumentListener: public TextDocumentListener {
	protected:
		bool deletedLast;
		bool skipNextSelectionChange;
		bool tmpShouldListen;

		std::vector<com::haathi::provenance::ProvenanceEvent*> getPartDifferences();
		std::vector<com::haathi::provenance::ProvenanceEvent*> getFullDifferences();

		bool checkText();
		void thisModified();
		void thisSelectionChanged(std::vector<com::haathi::text::Range>& ranges);

		// com::sun::star::util::XModifyListener
		virtual void SAL_CALL modified(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException);
		// com::sun::star::view:XSelectionChangeListener
		virtual void SAL_CALL selectionChanged(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException);

		virtual void SAL_CALL printJobEvent(const com::sun::star::view::PrintJobEvent& event)
			throw (com::sun::star::uno::RuntimeException);

	public:
		SmartTextDocumentListener(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent,
				com::haathi::document::ProvenanceDocument& provenanceDocument,
				com::haathi::text::TextHelper& textHelper, bool interactive);
		virtual ~SmartTextDocumentListener();
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
