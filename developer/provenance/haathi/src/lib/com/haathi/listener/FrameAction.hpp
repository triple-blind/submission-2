/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/FrameAction.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTENER__FRAME_ACTION_HPP
#	define COM__HAATHI__LISTENER__FRAME_ACTION_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/frame/FrameActionEvent.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XFrameActionListener.hpp>
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
class FrameActionBroadcaster: public Broadcaster<com::sun::star::frame::XFrame, com::sun::star::frame::XFrameActionListener> {
	public:
		static FrameActionBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::frame::XFrame> xBroadcaster = xController->getFrame();

			return FrameActionBroadcaster(xBroadcaster);
		}

		FrameActionBroadcaster(Reference<com::sun::star::frame::XFrame> xBroadcaster):
				Broadcaster<com::sun::star::frame::XFrame, com::sun::star::frame::XFrameActionListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::frame::XFrameActionListener> xListener) {
			xBroadcaster->addFrameActionListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::frame::XFrameActionListener> xListener) {
			xBroadcaster->removeFrameActionListener(xListener);
		}
};

class FrameActionForwardee {
	public:
		virtual ~FrameActionForwardee() {
		}

		virtual void SAL_CALL frameAction(const com::sun::star::frame::FrameActionEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class FrameActionListener: public Listener<com::sun::star::frame::XFrameActionListener,
		FrameActionBroadcaster, FrameActionForwardee> {
	public:
		virtual ~FrameActionListener() {
		}

		FrameActionListener(Reference<com::sun::star::lang::XComponent> xComponent, FrameActionForwardee* forwardee):
				Listener<com::sun::star::frame::XFrameActionListener,
				FrameActionBroadcaster, FrameActionForwardee>(FrameActionBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		FrameActionListener(FrameActionBroadcaster broadcaster, FrameActionForwardee* forwardee):
				Listener<com::sun::star::frame::XFrameActionListener,
				FrameActionBroadcaster, FrameActionForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL frameAction(const com::sun::star::frame::FrameActionEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->frameAction(event);
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
