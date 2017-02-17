/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/KeyHandler.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTEN__KEY_HANDLER_HPP
#	define COM__HAATHI__LISTEN__KEY_HANDLER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/KeyEvent.hpp>
#include <com/sun/star/awt/XKeyHandler.hpp>
#include <com/sun/star/awt/XUserInputInterception.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
KeyHandler
******************************************************************************/
class KeyHandlerBroadcaster: public Broadcaster<com::sun::star::awt::XUserInputInterception, com::sun::star::awt::XKeyHandler> {
	public:
		static KeyHandlerBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::awt::XUserInputInterception> xBroadcaster(xController, com::sun::star::uno::UNO_QUERY);

			return KeyHandlerBroadcaster(xBroadcaster);
		}

		KeyHandlerBroadcaster(Reference<com::sun::star::awt::XUserInputInterception> xBroadcaster):
				Broadcaster<com::sun::star::awt::XUserInputInterception, com::sun::star::awt::XKeyHandler>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::awt::XKeyHandler> xListener) {
			xBroadcaster->addKeyHandler(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::awt::XKeyHandler> xListener) {
			xBroadcaster->removeKeyHandler(xListener);
		}
};

class KeyHandlerForwardee {
	public:
		virtual ~KeyHandlerForwardee() {
		}

		// These have been changed from sal_Bool to void en ensure that we only observe
		virtual void SAL_CALL keyPressed(const com::sun::star::awt::KeyEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL keyReleased(const com::sun::star::awt::KeyEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class KeyHandlerListener: public Listener<com::sun::star::awt::XKeyHandler,
		KeyHandlerBroadcaster, KeyHandlerForwardee> {
	public:
		KeyHandlerListener(Reference<com::sun::star::lang::XComponent> xComponent, KeyHandlerForwardee* forwardee):
				Listener<com::sun::star::awt::XKeyHandler,
				KeyHandlerBroadcaster, KeyHandlerForwardee>(KeyHandlerBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		KeyHandlerListener(KeyHandlerBroadcaster broadcaster, KeyHandlerForwardee* forwardee):
				Listener<com::sun::star::awt::XKeyHandler,
				KeyHandlerBroadcaster, KeyHandlerForwardee>(broadcaster, forwardee) {
		}

		virtual sal_Bool SAL_CALL keyPressed(const com::sun::star::awt::KeyEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->keyPressed(event);
			return sal_False;
		}

		virtual sal_Bool SAL_CALL keyReleased(const com::sun::star::awt::KeyEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->keyReleased(event);
			return sal_False;
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
