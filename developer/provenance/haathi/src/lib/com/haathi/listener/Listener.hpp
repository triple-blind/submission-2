/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/Listener.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTENER__LISTENER_HPP
#	define COM__HAATHI__LISTENER__LISTENER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/lang/EventObject.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>

#include <cppuhelper/implbase1.hxx>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
Broadcaster and Listener
******************************************************************************/
template<class TXBroadcaster, class TXListener>
class Broadcaster {
	protected:
		Reference<TXBroadcaster> xBroadcaster;
	public:
		Broadcaster(Reference<TXBroadcaster> xBroadcaster): xBroadcaster(xBroadcaster) {
		}

		virtual ~Broadcaster() {
		}

		bool is() {
			return xBroadcaster.is() == sal_True;
		}

		virtual void addListener(Reference<TXListener>) = 0;
		virtual void removeListener(Reference<TXListener>) = 0;
};

template<class TXListener, class TBroadcaster, class TForwardee>
class Listener: public cppu::WeakImplHelper1<TXListener> {
	protected:
		TBroadcaster broadcaster;
		TForwardee* forwardee;
		bool added;
	public:
		Listener(TBroadcaster broadcaster, TForwardee* forwardee): broadcaster(broadcaster), forwardee(forwardee), added(false) {
			addListener();
		}

		virtual ~Listener() {
			// Some listeners, like the PrintJobListener are not disposed by the broadcaster.
			// However, don't help them here because it results in the destructor being called twice!
			//removeListener();
		}

		virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			// See https://wiki.openoffice.org/wiki/Documentation/DevGuide/ProUNO/Event_Model
			removeListener();
		}

		virtual bool addListener() {
			bool success = false;

			if (added || !broadcaster.is())
				return success;

			Reference<TXListener> xListener = static_cast<TXListener*>(this);

			added = true;
			broadcaster.addListener(xListener);
			success = true;
			return success;
		}

		virtual bool removeListener() {
			bool success = false;

			if (!added || !broadcaster.is())
				return success;

			Reference<TXListener> xListener = static_cast<TXListener*>(this);

			added = false;
			broadcaster.removeListener(xListener);
			success = true;
			return success;
		}
};

class IRecorder {
	public:
		virtual ~IRecorder() {
		}

		virtual void record(std::string channel, std::string message, bool modify = true) = 0;
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
