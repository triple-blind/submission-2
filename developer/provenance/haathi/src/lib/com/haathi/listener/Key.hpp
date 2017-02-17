/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/Key.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTENER__KEY_HPP
#	define COM__HAATHI__LISTENER__KEY_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/KeyEvent.hpp>
#include <com/sun/star/awt/XKeyListener.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>
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
class KeyBroadcaster: public Broadcaster<com::sun::star::awt::XWindow, com::sun::star::awt::XKeyListener> {
	public:
		static KeyBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::frame::XFrame> xFrame = xController->getFrame();
			Reference<com::sun::star::awt::XWindow> xBroadcaster = xFrame->getContainerWindow();
//			Reference<com::sun::star::awt::XWindow> xBroadcaster = xFrame->getComponentWindow();

			return KeyBroadcaster(xBroadcaster);
		}

		KeyBroadcaster(Reference<com::sun::star::awt::XWindow> xBroadcaster):
				Broadcaster<com::sun::star::awt::XWindow, com::sun::star::awt::XKeyListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::awt::XKeyListener> xListener) {
			xBroadcaster->addKeyListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::awt::XKeyListener> xListener) {
			xBroadcaster->removeKeyListener(xListener);
		}
};

class KeyForwardee {
	public:
		virtual ~KeyForwardee() {
		}

		virtual void SAL_CALL keyPressed(const com::sun::star::awt::KeyEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL keyReleased(const com::sun::star::awt::KeyEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class KeyListener: public Listener<com::sun::star::awt::XKeyListener,
		KeyBroadcaster, KeyForwardee> {
	public:
		KeyListener(Reference<com::sun::star::lang::XComponent> xComponent, KeyForwardee* forwardee):
				Listener<com::sun::star::awt::XKeyListener,
				KeyBroadcaster, KeyForwardee>(KeyBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		KeyListener(KeyBroadcaster broadcaster, KeyForwardee* forwardee):
				Listener<com::sun::star::awt::XKeyListener,
				KeyBroadcaster, KeyForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL keyPressed(const com::sun::star::awt::KeyEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->keyPressed(event);
		}

		virtual void SAL_CALL keyReleased(const com::sun::star::awt::KeyEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->keyReleased(event);
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
