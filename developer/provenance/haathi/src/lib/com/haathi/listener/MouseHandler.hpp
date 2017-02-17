/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/MouseHandler.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTEN__MOUSE_HANDLER_HPP
#	define COM__HAATHI__LISTEN__MOUSE_HANDLER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/MouseEvent.hpp>
#include <com/sun/star/awt/XMouseClickHandler.hpp>
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
MouseHandler
******************************************************************************/
class MouseHandlerBroadcaster: public Broadcaster<com::sun::star::awt::XUserInputInterception, com::sun::star::awt::XMouseClickHandler> {
	public:
		static MouseHandlerBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::awt::XUserInputInterception> xBroadcaster(xController, com::sun::star::uno::UNO_QUERY);

			return MouseHandlerBroadcaster(xBroadcaster);
		}

		MouseHandlerBroadcaster(Reference<com::sun::star::awt::XUserInputInterception> xBroadcaster):
				Broadcaster<com::sun::star::awt::XUserInputInterception, com::sun::star::awt::XMouseClickHandler>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::awt::XMouseClickHandler> xListener) {
			xBroadcaster->addMouseClickHandler(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::awt::XMouseClickHandler> xListener) {
			xBroadcaster->removeMouseClickHandler(xListener);
		}
};

class MouseHandlerForwardee {
	public:
		virtual ~MouseHandlerForwardee() {
		}

		// These have been changed from sal_Bool to void en ensure that we only observe
		virtual void SAL_CALL mousePressed(const com::sun::star::awt::MouseEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
		virtual void SAL_CALL mouseReleased(const com::sun::star::awt::MouseEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class MouseHandlerListener: public Listener<com::sun::star::awt::XMouseClickHandler,
		MouseHandlerBroadcaster, MouseHandlerForwardee> {
	public:
		MouseHandlerListener(Reference<com::sun::star::lang::XComponent> xComponent, MouseHandlerForwardee* forwardee):
				Listener<com::sun::star::awt::XMouseClickHandler,
				MouseHandlerBroadcaster, MouseHandlerForwardee>(MouseHandlerBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		MouseHandlerListener(MouseHandlerBroadcaster broadcaster, MouseHandlerForwardee* forwardee):
				Listener<com::sun::star::awt::XMouseClickHandler,
				MouseHandlerBroadcaster, MouseHandlerForwardee>(broadcaster, forwardee) {
		}

		virtual sal_Bool SAL_CALL mousePressed(const com::sun::star::awt::MouseEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->mousePressed(event);
			return sal_False;
		}

		virtual sal_Bool SAL_CALL mouseReleased(const com::sun::star::awt::MouseEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->mouseReleased(event);
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
