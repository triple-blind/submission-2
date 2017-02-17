/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/Window2.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTENER__WINDOW2_HPP
#	define COM__HAATHI__LISTENER__WINDOW2_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/XWindowListener2.hpp>
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
class Window2Broadcaster: public Broadcaster<com::sun::star::awt::XWindow, com::sun::star::awt::XWindowListener2> {
	public:
		static Window2Broadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::frame::XFrame> xFrame = xController->getFrame();
			Reference<com::sun::star::awt::XWindow> xBroadcaster = xFrame->getContainerWindow();

			return Window2Broadcaster(xBroadcaster);
		}

		Window2Broadcaster(Reference<com::sun::star::awt::XWindow> xBroadcaster):
				Broadcaster<com::sun::star::awt::XWindow, com::sun::star::awt::XWindowListener2>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::awt::XWindowListener2> xListener) {
			Reference<com::sun::star::awt::XWindowListener> xListener1(xListener, com::sun::star::uno::UNO_QUERY);

			xBroadcaster->addWindowListener(xListener1);
		}

		virtual void removeListener(Reference<com::sun::star::awt::XWindowListener2> xListener) {
			Reference<com::sun::star::awt::XWindowListener> xListener1(xListener, com::sun::star::uno::UNO_QUERY);

			xBroadcaster->removeWindowListener(xListener1);
		}
};

class Window2Forwardee {
	public:
		virtual ~Window2Forwardee() {
		}

		virtual void SAL_CALL windowEnabled(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL windowDisabled(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class Window2Listener: public Listener<com::sun::star::awt::XWindowListener2,
		Window2Broadcaster, Window2Forwardee> {
	public:
		Window2Listener(Reference<com::sun::star::lang::XComponent> xComponent, Window2Forwardee* forwardee):
				Listener<com::sun::star::awt::XWindowListener2,
				Window2Broadcaster, Window2Forwardee>(Window2Broadcaster::getBroadcaster(xComponent), forwardee) {
		}

		Window2Listener(Window2Broadcaster broadcaster, Window2Forwardee* forwardee):
				Listener<com::sun::star::awt::XWindowListener2,
				Window2Broadcaster, Window2Forwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL windowEnabled(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->windowEnabled(event);
		}

		virtual void SAL_CALL windowDisabled(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->windowDisabled(event);
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
