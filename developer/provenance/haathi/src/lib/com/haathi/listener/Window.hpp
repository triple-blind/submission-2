/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/Window.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTENER__WINDOW_HPP
#	define COM__HAATHI__LISTENER__WINDOW_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/WindowEvent.hpp>
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/EventObject.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
Key
******************************************************************************/
class WindowBroadcaster: public Broadcaster<com::sun::star::awt::XWindow, com::sun::star::awt::XWindowListener> {
	public:
		static WindowBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::frame::XFrame> xFrame = xController->getFrame();
			Reference<com::sun::star::awt::XWindow> xBroadcaster = xFrame->getContainerWindow();

			return WindowBroadcaster(xBroadcaster);
		}

		WindowBroadcaster(Reference<com::sun::star::awt::XWindow> xBroadcaster):
				Broadcaster<com::sun::star::awt::XWindow, com::sun::star::awt::XWindowListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::awt::XWindowListener> xListener) {
			xBroadcaster->addWindowListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::awt::XWindowListener> xListener) {
			xBroadcaster->removeWindowListener(xListener);
		}
};

class WindowForwardee {
	public:
		virtual ~WindowForwardee() {
		}

		virtual void SAL_CALL windowResized(const com::sun::star::awt::WindowEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowMoved(const com::sun::star::awt::WindowEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowShown(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowHidden(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class WindowListener: public Listener<com::sun::star::awt::XWindowListener,
		WindowBroadcaster, WindowForwardee> {
	public:
		WindowListener(Reference<com::sun::star::lang::XComponent> xComponent, WindowForwardee* forwardee):
				Listener<com::sun::star::awt::XWindowListener,
				WindowBroadcaster, WindowForwardee>(WindowBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		WindowListener(WindowBroadcaster broadcaster, WindowForwardee* forwardee):
				Listener<com::sun::star::awt::XWindowListener,
				WindowBroadcaster, WindowForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL windowResized(const com::sun::star::awt::WindowEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->windowResized(event);
		}

		virtual void SAL_CALL windowMoved(const com::sun::star::awt::WindowEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->windowMoved(event);
		}

		virtual void SAL_CALL windowShown(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->windowShown(event);
		}

		virtual void SAL_CALL windowHidden(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->windowHidden(event);
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
