/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/PrintJob.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTENER__PRINT_JOB_HPP
#	define COM__HAATHI__LISTENER__PRINT_JOB_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/view/PrintJobEvent.hpp>
#include <com/sun/star/view/XPrintJobBroadcaster.hpp>
#include <com/sun/star/view/XPrintJobListener.hpp>
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
class PrintJobBroadcaster: public Broadcaster<com::sun::star::view::XPrintJobBroadcaster, com::sun::star::view::XPrintJobListener> {
	public:
		static PrintJobBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::view::XPrintJobBroadcaster> xBroadcaster(xComponent, com::sun::star::uno::UNO_QUERY);

			return PrintJobBroadcaster(xBroadcaster);
		}

		PrintJobBroadcaster(Reference<com::sun::star::view::XPrintJobBroadcaster> xBroadcaster):
				Broadcaster<com::sun::star::view::XPrintJobBroadcaster, com::sun::star::view::XPrintJobListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::view::XPrintJobListener> xListener) {
			xBroadcaster->addPrintJobListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::view::XPrintJobListener> xListener) {
			xBroadcaster->removePrintJobListener(xListener);
		}
};

class PrintJobForwardee {
	public:
		virtual ~PrintJobForwardee() {
		}

		virtual void SAL_CALL printJobEvent(const com::sun::star::view::PrintJobEvent& event)
			throw (com::sun::star::uno::RuntimeException) { }
};

class PrintJobListener: public Listener<com::sun::star::view::XPrintJobListener,
		PrintJobBroadcaster, PrintJobForwardee> {
	public:
		PrintJobListener(Reference<com::sun::star::lang::XComponent> xComponent, PrintJobForwardee* forwardee):
				Listener<com::sun::star::view::XPrintJobListener,
				PrintJobBroadcaster, PrintJobForwardee>(PrintJobBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		PrintJobListener(PrintJobBroadcaster broadcaster, PrintJobForwardee* forwardee):
				Listener<com::sun::star::view::XPrintJobListener,
				PrintJobBroadcaster, PrintJobForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL printJobEvent(const com::sun::star::view::PrintJobEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->printJobEvent(event);
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
