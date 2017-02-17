/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/extension/CommandEnvironment.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__EXTENSION__COMMAND_ENVIRONMENT_HPP
#	define COM__HAATHI__EXTENSION__COMMAND_ENVIRONMENT_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/task/XInteractionApprove.hpp>
#include <com/sun/star/task/XInteractionContinuation.hpp>
#include <com/sun/star/task/XInteractionHandler.hpp>
#include <com/sun/star/task/XInteractionRequest.hpp>
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#include <com/sun/star/ucb/XProgressHandler.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <cppuhelper/implbase1.hxx>
#include "sdkEnd.hpp"

#include <iostream>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace extension {
/******************************************************************************
Class
******************************************************************************/
class ProgressHandler: public cppu::WeakImplHelper1<com::sun::star::ucb::XProgressHandler> {
	public:
		ProgressHandler() {
		}

		void SAL_CALL push(const com::sun::star::uno::Any& status) throw (com::sun::star::uno::RuntimeException) {
		}

		void SAL_CALL pop() throw (com::sun::star::uno::RuntimeException) {
		}

		void SAL_CALL update(const com::sun::star::uno::Any& status) throw (com::sun::star::uno::RuntimeException) {
		}
};

class InteractionHandler: public cppu::WeakImplHelper1<com::sun::star::task::XInteractionHandler> {
	public:
		InteractionHandler() {
		}

		void SAL_CALL handle(const Reference<com::sun::star::task::XInteractionRequest>& request)
				throw (com::sun::star::uno::RuntimeException) {
			std::cout << "Inside handle" << std::endl;
			com::sun::star::uno::Any message = request->getRequest();
			com::sun::star::uno::Exception ex;
			message >>= ex;
			std::cout << "Message : " << ex.Message << std::endl;
			com::sun::star::uno::Sequence<Reference<com::sun::star::task::XInteractionContinuation> > continuations =
					request->getContinuations();

			for (int i = 0; i < continuations.getLength(); i++) {
				Reference<com::sun::star::task::XInteractionContinuation> xcontinuation = continuations[i];
				Reference<com::sun::star::task::XInteractionApprove> xapprove(xcontinuation, com::sun::star::uno::UNO_QUERY);
				if (xapprove.is()) {
					xapprove->select();
				}
			}
		}
};

class UpdateCommandEnvironment: public cppu::WeakImplHelper1<com::sun::star::ucb::XCommandEnvironment> {
	public:
		UpdateCommandEnvironment() {
		}

		Reference<com::sun::star::task::XInteractionHandler> SAL_CALL getInteractionHandler()
				throw (com::sun::star::uno::RuntimeException) {
			InteractionHandler* iHandler = new InteractionHandler();
			Reference<com::sun::star::task::XInteractionHandler> result =
					static_cast<com::sun::star::task::XInteractionHandler*>(iHandler);
			return result;
		}

		Reference<com::sun::star::ucb::XProgressHandler> SAL_CALL getProgressHandler()
				throw (com::sun::star::uno::RuntimeException) {
			ProgressHandler* pHandler = new ProgressHandler();
			Reference<com::sun::star::ucb::XProgressHandler> result =
					static_cast<com::sun::star::ucb::XProgressHandler*>(pHandler);
			return result;
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
