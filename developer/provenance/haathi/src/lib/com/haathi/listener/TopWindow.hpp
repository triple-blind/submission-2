/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/TopWindow.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTENER__TOP_WINDOW_HPP
#	define COM__HAATHI__LISTENER__TOP_WINDOW_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"
#include "com/haathi/utils/PropertySetHelper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/XTopWindow.hpp>
#include <com/sun/star/awt/XTopWindowListener.hpp>
#include <com/sun/star/awt/XWindow.hpp>

#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/EventObject.hpp>
#include "sdkEnd.hpp"

#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
Key
******************************************************************************/
class TopWindowBroadcaster: public Broadcaster<com::sun::star::awt::XTopWindow, com::sun::star::awt::XTopWindowListener> {
	public:
		static TopWindowBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::frame::XFrame> xFrame = xController->getFrame();
			Reference<com::sun::star::awt::XWindow> xWindow = xFrame->getContainerWindow();
			Reference<com::sun::star::awt::XTopWindow> xBroadcaster(xWindow, com::sun::star::uno::UNO_QUERY);

			return TopWindowBroadcaster(xBroadcaster);
		}

		TopWindowBroadcaster(Reference<com::sun::star::awt::XTopWindow> xBroadcaster):
				Broadcaster<com::sun::star::awt::XTopWindow, com::sun::star::awt::XTopWindowListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::awt::XTopWindowListener> xListener) {
			xBroadcaster->addTopWindowListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::awt::XTopWindowListener> xListener) {
			xBroadcaster->removeTopWindowListener(xListener);
		}
};

class TopWindowForwardee {
	public:
		virtual ~TopWindowForwardee() {
		}

		virtual void SAL_CALL windowOpened(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowClosing(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowClosed(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowMinimized(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowNormalized(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowActivated(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowDeactivated(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class TopWindowListener: public Listener<com::sun::star::awt::XTopWindowListener,
	TopWindowBroadcaster, IRecorder> {
	protected:
		Reference<com::sun::star::awt::XWindow> xWindow;

	public:
		TopWindowListener(Reference<com::sun::star::lang::XComponent> xComponent, IRecorder* forwardee): // TopWindowForwardee* forwardee):
				Listener<com::sun::star::awt::XTopWindowListener,
				TopWindowBroadcaster, IRecorder>(TopWindowBroadcaster::getBroadcaster(xComponent), forwardee) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::frame::XFrame> xFrame = xController->getFrame();

			xWindow = xFrame->getComponentWindow();
		}

		TopWindowListener(TopWindowBroadcaster broadcaster, IRecorder* forwardee): // TopWindowForwardee* forwardee):
				Listener<com::sun::star::awt::XTopWindowListener,
				TopWindowBroadcaster, IRecorder>(broadcaster, forwardee) {
		}

		void record(const char* message, bool modify = true) {
			// Don't modify just to record these events.  They interfere, especially,
			// with the closing of the document.
			forwardee->record(std::string("XTopWindow"), message, false);
		}

		virtual void SAL_CALL windowOpened(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			record("windowOpened");
		}

		virtual void SAL_CALL windowClosing(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			record("windowClosing", false);
		}

		virtual void SAL_CALL windowClosed(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			record("windowClosed");
		}

		virtual void SAL_CALL windowMinimized(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			record("windowMinimized");
		}

		virtual void SAL_CALL windowNormalized(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			record("windowNormalized");
		}

		virtual void SAL_CALL windowActivated(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			record("windowActivated");
			xWindow->setVisible(sal_True);
		}

		virtual void SAL_CALL windowDeactivated(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			record("windowDeactivated");
			xWindow->setVisible(sal_False);
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
