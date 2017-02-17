/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/component/ProvenanceManager.cpp#1 $

//
// ProvenanceManager.cxx
//
// Author: Aaron Gibson
// Date: October 15, 2012
//
// This file implements the service haathi::ProvenanceManager and its service manager.
//
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/component/ProvenanceManager.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Configuration.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "haathi/ProvenanceNetworkException.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::sun::star::beans;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

using namespace rtl;
/******************************************************************************
Namespace
******************************************************************************/
namespace haathi {
/******************************************************************************
Class
******************************************************************************/

//==========================================================
// Constructors and Destructors
//==========================================================

ProvenanceManager::ProvenanceManager() {
}

ProvenanceManager::~ProvenanceManager() {
	// We have to make sure that we log off the server if we are logged on already.
}

//Reference<XInterface> SAL_CALL ProvenanceManager_createInstance(const Reference<XComponentContext>& rContext)
//		throw(Exception) {
//	return (cppu::OWeakObject*) new ProvenanceManager(rContext);
//}
//==========================================================
// XServiceInfo
//==========================================================

OUString ProvenanceManager_getImplementationName() throw (RuntimeException) {
	return OUSTRING(PROVENANCE_MANAGER_IMPLEMENTATIONNAME);
}

sal_Bool SAL_CALL ProvenanceManager_supportsService(const OUString& serviceName)
		throw (RuntimeException) {
	return (serviceName.equalsAscii(PROVENANCE_MANAGER_SERVICENAME));
}

Sequence< OUString > SAL_CALL ProvenanceManager_getSupportedServiceNames()
		throw (RuntimeException) {
	Sequence< OUString > result(1);
	result[0] = OUSTRING(PROVENANCE_MANAGER_SERVICENAME);
	return result;
}

sal_Bool SAL_CALL ProvenanceManager::supportsService(
		const OUString& rServiceName) throw (RuntimeException) {
	return ProvenanceManager_supportsService(rServiceName);
}

Sequence< OUString > SAL_CALL ProvenanceManager::getSupportedServiceNames()
		throw (RuntimeException) {
	return ProvenanceManager_getSupportedServiceNames();
}

OUString ProvenanceManager::getImplementationName() throw (RuntimeException) {
	return ProvenanceManager_getImplementationName();
}

//===================================================================
// Utility
//===================================================================
OUString ProvenanceManager::getURLToExtensionRoot() {
	// kwa: doesn't seem to do anything
	return OUString();
}

//==========================================================
// XProvenanceManager
//==========================================================
sal_Bool SAL_CALL ProvenanceManager::login(const OUString& serverURL,
		const OUString& username, const OUString& password)
				throw (ProvenanceNetworkException, RuntimeException) {
	// Implement the login
	return sal_False ;
}

sal_Bool SAL_CALL ProvenanceManager::logout() throw (ProvenanceNetworkException,
		RuntimeException) {
	return sal_False ;
}

Sequence< OUString  > SAL_CALL ProvenanceManager::listUsers()
		throw (ProvenanceNetworkException, RuntimeException) {
	return Sequence< OUString >();
}

Sequence< OUString > SAL_CALL ProvenanceManager::listAvailableAuditors()
		throw (ProvenanceNetworkException, RuntimeException) {
	return Sequence< OUString >();
}

Sequence< OUString > SAL_CALL ProvenanceManager::listEditableDocuments()
		throw (ProvenanceNetworkException, RuntimeException) {
	return Sequence< OUString >();
}

Sequence< OUString > SAL_CALL ProvenanceManager::listAuditableDocuments()
		throw (ProvenanceNetworkException, RuntimeException) {
	Sequence< OUString > result;
/*	try {
		UserType user_type = AUDITOR;
		TrustedServer ts(trusted_server);

		ListDocumentsRequest request(meta_info.user_info.username, password,
				meta_info.user_info.userID, user_type);
		ListDocumentsResponse response = ts.listAvailableDocuments(request);

		if (!response.getResult()) {
			throw new ProvenanceNetworkException();
		}

		audit_load_map = response.getDocuments();
		audit_load_keys_map = response.getDocumentKeys();

		// Now, load the contents of the response into a Sequence to be displayed
		// in an OpenOffice listbox.
		std::map<int, std::string>::iterator itr;
		Sequence<OUString> result(audit_load_map.size());
		int i = 0;
		for (itr = audit_load_map.begin(); itr != audit_load_map.end(); ++itr) {
			result[i++] = convertToOUString((*itr).second);
		}
		return result;
	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Exception"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Exception"), e2.Message);
	} catch (std::exception & e3) {
		showErrorMessageBox(OUSTRING("Standard Library Exception"),
				convertToOUString(e3.what()));
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"),
				OUSTRING("This exception is unrecognized!"));
	}
*/	return Sequence< OUString >();
}

Reference<XComponent> SAL_CALL ProvenanceManager::createProvenanceDocument(
		::haathi::ProvenanceDocumentType documentType,
		const ::rtl::OUString& documentName,
		const Sequence<PropertyValue>& options)
				throw (ProvenanceNetworkException, RuntimeException) {
	return Reference<XComponent>();
}

Reference<XComponent> SAL_CALL ProvenanceManager::loadProvenanceDocument(
		const OUString& document_name) throw (ProvenanceNetworkException,
				RuntimeException) {
	return Reference<XComponent>();
}

//==========================================================
// XMain
//==========================================================

sal_Int32 ProvenanceManager::run(const Sequence< OUString > & args)
		throw (RuntimeException) {
	return 0;
}
/******************************************************************************
Namespace
******************************************************************************/
};
/*****************************************************************************/
