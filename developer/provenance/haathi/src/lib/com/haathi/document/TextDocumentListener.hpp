/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/TextDocumentListener.hpp#2 $

/*
 *  Created by Aaron Gibson on 8/2/11.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__TEXT_DOCUMENT_LISTENER_HPP
#	define COM__HAATHI__DOCUMENT__TEXT_DOCUMENT_LISTENER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/DocumentHistory.hpp"
#include "com/haathi/document/ProvenanceDocument.hpp"
#include "com/haathi/listener/DocumentEvent.hpp"
#include "com/haathi/listener/Listener.hpp"
#include "com/haathi/listener/Modify.hpp"
#include "com/haathi/listener/PrintJob.hpp"
#include "com/haathi/listener/SelectionChange.hpp"
#include "com/haathi/listener/TopWindow.hpp"
#include "com/haathi/text/TextHelper.hpp"


#include "sdkBeg.hpp"
#include <com/sun/star/lang/EventObject.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XModifiable.hpp>
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
class TextDocumentListener:
		public com::haathi::listener::ModifyForwardee,
		public com::haathi::listener::SelectionChangeForwardee,
		public com::haathi::listener::DocumentEventForwardee, // doesn't do anything
		public com::haathi::listener::PrintJobForwardee,
		public com::haathi::listener::IRecorder {
	protected:
		Reference<com::sun::star::uno::XComponentContext> xComponentContext;
		Reference<com::sun::star::lang::XComponent> xComponent;
		ProvenanceDocument& provenanceDocument;
		com::haathi::text::TextHelper &textHelper;
		Reference<com::sun::star::util::XModifiable> xModifiable;
		std::vector<Reference<com::sun::star::lang::XEventListener> > undisposedEventListeners;

		com::haathi::document::DocumentHistory documentHistory;
		bool shouldListen;
		virtual void addListeners(bool interactive);

	public:
		TextDocumentListener(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent,
				com::haathi::document::ProvenanceDocument& provenanceDocument,
				com::haathi::text::TextHelper& textHelper, bool interactive = true);
		virtual ~TextDocumentListener();

		virtual void listen(bool shouldListen);
		virtual void checkMissingNotifications() = 0;
		virtual OUString getString();
		virtual bool isModified();
		virtual void record(std::string channel, std::string message, bool modify);

		virtual com::haathi::document::DocumentHistory& getDocumentHistory() {
			return documentHistory;
		}
		// com::sun::star::util::XModifyListener
		virtual void SAL_CALL modified(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {}
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
