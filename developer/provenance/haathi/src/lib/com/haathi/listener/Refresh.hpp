/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/Refresh.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTEN__REFRESH_HPP
#	define COM__HAATHI__LISTEN__REFRESH_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/util/XRefreshable.hpp>
#include <com/sun/star/util/XRefreshListener.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
Refresh
******************************************************************************/
class RefreshBroadcaster: public Broadcaster<com::sun::star::util::XRefreshable, com::sun::star::util::XRefreshListener> {
	public:
		static RefreshBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::util::XRefreshable> xBroadcaster(xComponent, com::sun::star::uno::UNO_QUERY);

			return RefreshBroadcaster(xBroadcaster);
		}

		RefreshBroadcaster(Reference<com::sun::star::util::XRefreshable> xBroadcaster):
				Broadcaster<com::sun::star::util::XRefreshable, com::sun::star::util::XRefreshListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::util::XRefreshListener> xListener) {
			xBroadcaster->addRefreshListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::util::XRefreshListener> xListener) {
			xBroadcaster->removeRefreshListener(xListener);
		}
};

class RefreshForwardee {
	public:
		virtual ~RefreshForwardee() {
		}

		virtual void SAL_CALL refreshed(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class RefreshListener: public Listener<com::sun::star::util::XRefreshListener,
		RefreshBroadcaster, RefreshForwardee> {
	public:
		RefreshListener(Reference<com::sun::star::lang::XComponent> xComponent, RefreshForwardee* forwardee):
				Listener<com::sun::star::util::XRefreshListener,
				RefreshBroadcaster, RefreshForwardee>(RefreshBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		RefreshListener(RefreshBroadcaster broadcaster, RefreshForwardee* forwardee):
				Listener<com::sun::star::util::XRefreshListener,
				RefreshBroadcaster, RefreshForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL refreshed(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->refreshed(event);
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
