/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/Modify.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTEN__MODIFY_HPP
#	define COM__HAATHI__LISTEN__MODIFY_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
Modify
******************************************************************************/
class ModifyBroadcaster: public Broadcaster<com::sun::star::util::XModifyBroadcaster, com::sun::star::util::XModifyListener> {
	public:
		static ModifyBroadcaster getBroadcaster(Reference<com::sun::star::lang::XComponent> xComponent) {
			Reference<com::sun::star::util::XModifyBroadcaster> xBroadcaster(xComponent, com::sun::star::uno::UNO_QUERY);

			return ModifyBroadcaster(xBroadcaster);
		}

		ModifyBroadcaster(Reference<com::sun::star::util::XModifyBroadcaster> xBroadcaster):
				Broadcaster<com::sun::star::util::XModifyBroadcaster, com::sun::star::util::XModifyListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::util::XModifyListener> xListener) {
			xBroadcaster->addModifyListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::util::XModifyListener> xListener) {
			xBroadcaster->removeModifyListener(xListener);
		}
};

class ModifyForwardee {
	public:
		virtual ~ModifyForwardee() {
		}

		virtual void SAL_CALL modified(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class ModifyListener: public Listener<com::sun::star::util::XModifyListener,
		ModifyBroadcaster, ModifyForwardee> {
	public:
		ModifyListener(Reference<com::sun::star::lang::XComponent> xComponent, ModifyForwardee* forwardee):
				Listener<com::sun::star::util::XModifyListener,
				ModifyBroadcaster, ModifyForwardee>(ModifyBroadcaster::getBroadcaster(xComponent), forwardee) {
		}

		ModifyListener(ModifyBroadcaster broadcaster, ModifyForwardee* forwardee):
				Listener<com::sun::star::util::XModifyListener,
				ModifyBroadcaster, ModifyForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL modified(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->modified(event);
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
