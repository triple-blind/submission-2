/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/UndoManager.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTEN__UNDO_MANAGER_HPP
#	define COM__HAATHI__LISTEN__UNDO_MANAGER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/document/UndoManagerEvent.hpp>
#include <com/sun/star/document/XUndoManager.hpp>
#include <com/sun/star/document/XUndoManagerListener.hpp>
#include <com/sun/star/document/XUndoManagerSupplier.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
UndoManager
******************************************************************************/
class UndoManagerBroadcaster: public Broadcaster<com::sun::star::document::XUndoManager, com::sun::star::document::XUndoManagerListener> {
	public:
		static UndoManagerBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::document::XUndoManagerSupplier> xUndoManagerSupplier(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::document::XUndoManager> xBroadcaster = xUndoManagerSupplier->getUndoManager();

			return UndoManagerBroadcaster(xBroadcaster);
		}

		UndoManagerBroadcaster(Reference<com::sun::star::document::XUndoManager> xBroadcaster):
				Broadcaster<com::sun::star::document::XUndoManager, com::sun::star::document::XUndoManagerListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::document::XUndoManagerListener> xListener) {
			xBroadcaster->addUndoManagerListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::document::XUndoManagerListener> xListener) {
			xBroadcaster->removeUndoManagerListener(xListener);
		}
};

class UndoManagerForwardee {
	public:
		virtual ~UndoManagerForwardee() {
		}

		virtual void SAL_CALL undoActionAdded(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL actionUndone(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL actionRedone(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL allActionsCleared(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL redoActionsCleared(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL resetAll(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL enteredContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL enteredHiddenContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL leftContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL leftHiddenContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL cancelledContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class UndoManagerListener: public Listener<com::sun::star::document::XUndoManagerListener,
		UndoManagerBroadcaster, UndoManagerForwardee> {
	public:
		UndoManagerListener(Reference<com::sun::star::lang::XComponent> xComponent, UndoManagerForwardee* forwardee):
				Listener<com::sun::star::document::XUndoManagerListener,
				UndoManagerBroadcaster, UndoManagerForwardee>(UndoManagerBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		UndoManagerListener(UndoManagerBroadcaster broadcaster, UndoManagerForwardee* forwardee):
				Listener<com::sun::star::document::XUndoManagerListener,
				UndoManagerBroadcaster, UndoManagerForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL undoActionAdded(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->undoActionAdded(event);
		}

		virtual void SAL_CALL actionUndone(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->actionUndone(event);
		}

		virtual void SAL_CALL actionRedone(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->actionRedone(event);
		}

		virtual void SAL_CALL allActionsCleared(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->allActionsCleared(event);
		}

		virtual void SAL_CALL redoActionsCleared(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->redoActionsCleared(event);
		}

		virtual void SAL_CALL resetAll(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->resetAll(event);
		}

		virtual void SAL_CALL enteredContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->enteredContext(event);
		}

		virtual void SAL_CALL enteredHiddenContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->enteredHiddenContext(event);
		}

		virtual void SAL_CALL leftContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->leftContext(event);
		}

		virtual void SAL_CALL leftHiddenContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->leftHiddenContext(event);
		}

		virtual void SAL_CALL cancelledContext(const com::sun::star::document::UndoManagerEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->cancelledContext(event);
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
