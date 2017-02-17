/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/DocumentEvent.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTEN__DOCUMENT_EVENT_HPP
#	define COM__HAATHI__LISTEN__DOCUMENT_EVENT_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/document/DocumentEvent.hpp>
#include <com/sun/star/document/XDocumentEventBroadcaster.hpp>
#include <com/sun/star/document/XDocumentEventListener.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
DocumentEvent
******************************************************************************/
class DocumentEventBroadcaster: public Broadcaster<com::sun::star::document::XDocumentEventBroadcaster, com::sun::star::document::XDocumentEventListener> {
	public:
		static DocumentEventBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::document::XDocumentEventBroadcaster> xBroadcaster(xComponent, com::sun::star::uno::UNO_QUERY);

			return DocumentEventBroadcaster(xBroadcaster);
		}

		DocumentEventBroadcaster(Reference<com::sun::star::document::XDocumentEventBroadcaster> xBroadcaster):
				Broadcaster<com::sun::star::document::XDocumentEventBroadcaster, com::sun::star::document::XDocumentEventListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::document::XDocumentEventListener> xListener) {
			xBroadcaster->addDocumentEventListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::document::XDocumentEventListener> xListener) {
			xBroadcaster->removeDocumentEventListener(xListener);
		}
};

class DocumentEventForwardee {
	public:
		virtual ~DocumentEventForwardee() {
		}

		virtual void SAL_CALL documentEventOccured(const com::sun::star::document::DocumentEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class DocumentEventListener: public Listener<com::sun::star::document::XDocumentEventListener,
		DocumentEventBroadcaster, DocumentEventForwardee> {
	public:
		DocumentEventListener(Reference<com::sun::star::lang::XComponent> xComponent, DocumentEventForwardee* forwardee):
				Listener<com::sun::star::document::XDocumentEventListener,
				DocumentEventBroadcaster, DocumentEventForwardee>(DocumentEventBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		DocumentEventListener(DocumentEventBroadcaster broadcaster, DocumentEventForwardee* forwardee):
				Listener<com::sun::star::document::XDocumentEventListener,
				DocumentEventBroadcaster, DocumentEventForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL documentEventOccured(const com::sun::star::document::DocumentEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->documentEventOccured(event);
		}
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
