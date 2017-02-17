/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/SelectionChange.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTENER__SELECTION_CHANGE_HPP
#	define COM__HAATHI__LISTENER__SELECTION_CHANGE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/view/XSelectionChangeListener.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
SelectionChange
******************************************************************************/
class SelectionChangeBroadcaster: public Broadcaster<com::sun::star::view::XSelectionSupplier, com::sun::star::view::XSelectionChangeListener> {
	public:
		static SelectionChangeBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::frame::XModel> xModel(xComponent, com::sun::star::uno::UNO_QUERY);
			Reference<com::sun::star::frame::XController> xController = xModel->getCurrentController();
			Reference<com::sun::star::view::XSelectionSupplier> xBroadcaster(xController, com::sun::star::uno::UNO_QUERY);

			return SelectionChangeBroadcaster(xBroadcaster);
		}

		SelectionChangeBroadcaster(Reference<com::sun::star::view::XSelectionSupplier> xBroadcaster):
				Broadcaster<com::sun::star::view::XSelectionSupplier, com::sun::star::view::XSelectionChangeListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::view::XSelectionChangeListener> xListener) {
			xBroadcaster->addSelectionChangeListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::view::XSelectionChangeListener> xListener) {
			xBroadcaster->removeSelectionChangeListener(xListener);
		}
};

class SelectionChangeForwardee {
	public:
		virtual ~SelectionChangeForwardee() {
		}

		virtual void SAL_CALL selectionChanged(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class SelectionChangeListener: public Listener<com::sun::star::view::XSelectionChangeListener,
		SelectionChangeBroadcaster, SelectionChangeForwardee> {
	public:
		SelectionChangeListener(Reference<com::sun::star::lang::XComponent> xComponent, SelectionChangeForwardee* forwardee):
				Listener<com::sun::star::view::XSelectionChangeListener,
				SelectionChangeBroadcaster, SelectionChangeForwardee>(SelectionChangeBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		SelectionChangeListener(SelectionChangeBroadcaster broadcaster, SelectionChangeForwardee* forwardee):
				Listener<com::sun::star::view::XSelectionChangeListener,
				SelectionChangeBroadcaster, SelectionChangeForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL selectionChanged(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->selectionChanged(event);
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
