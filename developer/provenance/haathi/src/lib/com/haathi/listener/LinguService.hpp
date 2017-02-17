/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/listener/LinguService.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__LISTEN__LINGU_SERVICE_HPP
#	define COM__HAATHI__LISTEN__LINGU_SERVICE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/listener/Listener.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/linguistic2/LinguServiceEvent.hpp>
#include <com/sun/star/linguistic2/XLinguServiceEventBroadcaster.hpp>
#include <com/sun/star/linguistic2/XLinguServiceEventListener.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace listener {
/******************************************************************************
LinguService
******************************************************************************/
class LinguServiceBroadcaster: public Broadcaster<com::sun::star::linguistic2::XLinguServiceEventBroadcaster, com::sun::star::linguistic2::XLinguServiceEventListener> {
	public:
		static LinguServiceBroadcaster getBroadcaster(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent) {
			com::haathi::extension::ExtensionHelper extensionHelper(xComponentContext);
			// This is just the toolkit, so thange in DropTarget.  Just get the toolkit there

			Reference<com::sun::star::lang::XMultiServiceFactory> xMultiServiceFactory = extensionHelper.getXMultiServiceFactory();
			Reference<com::sun::star::linguistic2::XLinguServiceEventBroadcaster> xBroadcaster(xMultiServiceFactory->createInstance(
					OUSTRING("com.sun.star.linguistic2.SpellChecker")), com::sun::star::uno::UNO_QUERY);

			return LinguServiceBroadcaster(xBroadcaster);
		}

		LinguServiceBroadcaster(Reference<com::sun::star::linguistic2::XLinguServiceEventBroadcaster> xBroadcaster):
				Broadcaster<com::sun::star::linguistic2::XLinguServiceEventBroadcaster, com::sun::star::linguistic2::XLinguServiceEventListener>(xBroadcaster) {
		}

		virtual void addListener(Reference<com::sun::star::linguistic2::XLinguServiceEventListener> xListener) {
			xBroadcaster->addLinguServiceEventListener(xListener);
		}

		virtual void removeListener(Reference<com::sun::star::linguistic2::XLinguServiceEventListener> xListener) {
			xBroadcaster->removeLinguServiceEventListener(xListener);
		}
};

class LinguServiceForwardee {
	public:
		virtual ~LinguServiceForwardee() {
		}

		virtual void SAL_CALL processLinguServiceEvent(const com::sun::star::linguistic2::LinguServiceEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
};

class LinguServiceListener: public Listener<com::sun::star::linguistic2::XLinguServiceEventListener,
		LinguServiceBroadcaster, LinguServiceForwardee> {
	public:
		LinguServiceListener(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent, LinguServiceForwardee* forwardee):
				Listener<com::sun::star::linguistic2::XLinguServiceEventListener,
				LinguServiceBroadcaster, LinguServiceForwardee>(LinguServiceBroadcaster::getBroadcaster(xComponentContext, xComponent), forwardee) {
		}

		LinguServiceListener(LinguServiceBroadcaster broadcaster, LinguServiceForwardee* forwardee):
				Listener<com::sun::star::linguistic2::XLinguServiceEventListener,
				LinguServiceBroadcaster, LinguServiceForwardee>(broadcaster, forwardee) {
		}

		virtual void SAL_CALL processLinguServiceEvent(const com::sun::star::linguistic2::LinguServiceEvent& event)
				throw (com::sun::star::uno::RuntimeException) {
			if (forwardee)
				forwardee->processLinguServiceEvent(event);
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
