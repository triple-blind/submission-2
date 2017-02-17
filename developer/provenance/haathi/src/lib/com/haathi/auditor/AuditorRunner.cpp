/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/auditor/AuditorRunner.cpp#1 $

/*
 *  Created by Aaron Gibson on 3/17/12.
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/auditor/AuditorRunner.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/auditor/AuditorDialog.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::document::DocumentMetaInfo;
using com::haathi::network::User;

using com::sun::star::lang::XComponent;
using com::sun::star::uno::XComponentContext;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace auditor {
/******************************************************************************
Class
******************************************************************************/
AuditorRunner* AuditorRunner::instance = NULL;

AuditorRunner::AuditorRunner(Reference<XComponentContext> xComponentContext, OUString helpUrl,
		std::string document, std::vector<std::string> provenances, DocumentMetaInfo documentMetaInfo,
		Reference<XComponent> xDocumentComponent, std::map<std::string, User> userMap):
		xComponentContext(xComponentContext), helpUrl(helpUrl), document(document), provenances(provenances),
		documentMetaInfo(documentMetaInfo), xDocumentComponent(xDocumentComponent), userMap(userMap) {
	instance = this;
}

void SAL_CALL AuditorRunner::run() {
	dialog = boost::shared_ptr<AuditorDialog>(new AuditorDialog(xComponentContext, helpUrl, document, provenances, documentMetaInfo, xDocumentComponent, userMap));
	dialog->run();
}

void SAL_CALL AuditorRunner::onTerminated() {
	instance = NULL;
	delete this; // Approved for thread
}

bool AuditorRunner::isAlreadyRunning() {
	return instance != NULL;
}

bool AuditorRunner::toFront() {
	bool alreadyRunning = isAlreadyRunning();

	if (alreadyRunning)
		instance->dialog->toFront();
	return alreadyRunning;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
