/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/auditor/AuditorRunner.hpp#1 $

/*
 *  Created by Aaron Gibson on 3/17/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__AUDITOR__AUDITOR_RUNNER_HPP
#	define COM__HAATHI__AUDITOR__AUDITOR_RUNNER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/Packet.hpp" // User

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <osl/thread.hxx>
#include "sdkEnd.hpp"

#include "boostBeg.hpp"
#include <boost/shared_ptr.hpp>
#include "boostEnd.hpp"

#include <map>
#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace auditor {
/******************************************************************************
AuditorRunner
******************************************************************************/
class AuditorDialog;

class AuditorRunner: public osl::Thread {
	protected:
		static AuditorRunner* instance;

		com::sun::star::uno::Reference<com::sun::star::uno::XComponentContext> xComponentContext;
		OUString helpUrl;
		std::string document;
		std::vector<std::string> provenances;
		com::haathi::document::DocumentMetaInfo documentMetaInfo;
		com::sun::star::uno::Reference<com::sun::star::lang::XComponent> xDocumentComponent;
		boost::shared_ptr<AuditorDialog> dialog;
		std::map<std::string, com::haathi::network::User> userMap;

		virtual void SAL_CALL run();
		virtual void SAL_CALL onTerminated();

	public:
		static bool isAlreadyRunning();
		static bool toFront();

		AuditorRunner(com::sun::star::uno::Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl,
				std::string document, std::vector<std::string> provenances, com::haathi::document::DocumentMetaInfo documentMetaInfo,
				com::sun::star::uno::Reference<com::sun::star::lang::XComponent> xDocumentComponent, std::map<std::string, com::haathi::network::User> userMap);
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
