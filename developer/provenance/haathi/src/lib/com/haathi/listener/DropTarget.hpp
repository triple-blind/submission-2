/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/DropTarget.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTEN__DROP_TARGET_HPP
#	define COM__HAATHI__LISTEN__DROP_TARGET_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/XDataTransferProviderAccess.hpp>
#include <com/sun/star/datatransfer/dnd/DropTargetDragEvent.hpp>
#include <com/sun/star/datatransfer/dnd/DropTargetDragEnterEvent.hpp>
#include <com/sun/star/datatransfer/dnd/DropTargetDropEvent.hpp>
#include <com/sun/star/datatransfer/dnd/DropTargetEvent.hpp>
#include <com/sun/star/datatransfer/dnd/XDropTarget.hpp>
#include <com/sun/star/datatransfer/dnd/XDropTargetListener.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
DropTarget
******************************************************************************/
class DropTargetBroadcaster: public Broadcaster<com::sun::star::datatransfer::dnd::XDropTarget, com::sun::star::datatransfer::dnd::XDropTargetListener> {
	public:
		static DropTargetBroadcaster getBroadcaster(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::frame::XFrame> xFrame = xController->getFrame();
			Reference<com::sun::star::awt::XWindow> xWindow = xFrame->getContainerWindow();
//			Reference<com::sun::star::awt::XWindow> xWindow = xFrame->getComponentWindow();

			com::haathi::extension::ExtensionHelper extensionHelper(xComponentContext);
			Reference<com::sun::star::awt::XDataTransferProviderAccess> xDataTransferProviderAccess(extensionHelper.getXToolkit(), com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::datatransfer::dnd::XDropTarget> xBroadcaster = xDataTransferProviderAccess->getDropTarget(xWindow);

			return DropTargetBroadcaster(xBroadcaster);
		}

		DropTargetBroadcaster(Reference<com::sun::star::datatransfer::dnd::XDropTarget> xBroadcaster):
				Broadcaster<com::sun::star::datatransfer::dnd::XDropTarget, com::sun::star::datatransfer::dnd::XDropTargetListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::datatransfer::dnd::XDropTargetListener> xListener) {
			xBroadcaster->addDropTargetListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::datatransfer::dnd::XDropTargetListener> xListener) {
			xBroadcaster->removeDropTargetListener(xListener);
		}
};

class DropTargetForwardee {
	public:
		virtual ~DropTargetForwardee() {
		}

		virtual void SAL_CALL drop(const com::sun::star::datatransfer::dnd::DropTargetDropEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL dragEnter(const com::sun::star::datatransfer::dnd::DropTargetDragEnterEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL dragExit(const com::sun::star::datatransfer::dnd::DropTargetEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL dragOver(const com::sun::star::datatransfer::dnd::DropTargetDragEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL dropActionChanged(const com::sun::star::datatransfer::dnd::DropTargetDragEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class DropTargetListener: public Listener<com::sun::star::datatransfer::dnd::XDropTargetListener,
		DropTargetBroadcaster, DropTargetForwardee> {
	public:
		DropTargetListener(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent, DropTargetForwardee* forwardee):
				Listener<com::sun::star::datatransfer::dnd::XDropTargetListener,
				DropTargetBroadcaster, DropTargetForwardee>(DropTargetBroadcaster::getBroadcaster(xComponentContext, xComponent), forwardee) {
		}

		DropTargetListener(DropTargetBroadcaster broadcaster, DropTargetForwardee* forwardee):
				Listener<com::sun::star::datatransfer::dnd::XDropTargetListener,
				DropTargetBroadcaster, DropTargetForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL drop(const com::sun::star::datatransfer::dnd::DropTargetDropEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->drop(event);
		}

		virtual void SAL_CALL dragEnter(const com::sun::star::datatransfer::dnd::DropTargetDragEnterEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->dragEnter(event);
		}

		virtual void SAL_CALL dragExit(const com::sun::star::datatransfer::dnd::DropTargetEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->dragExit(event);
		}

		virtual void SAL_CALL dragOver(const com::sun::star::datatransfer::dnd::DropTargetDragEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->dragOver(event);
		}

		virtual void SAL_CALL dropActionChanged(const com::sun::star::datatransfer::dnd::DropTargetDragEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->dropActionChanged(event);
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
