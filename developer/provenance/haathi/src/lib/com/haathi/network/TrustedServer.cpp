/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/TrustedServer.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/network/TrustedServer.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <iostream>
#include <cstdlib>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace network {
/******************************************************************************
Class
******************************************************************************/
TrustedServer::TrustedServer(NetworkInfo info): Server(info) {
}

bool TrustedServer::authenticate(AuthenticationRequest& request, AuthenticationResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::refreshSessionKey(RefreshSessionKeyRequest& request, RefreshSessionKeyResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::listAvailableDocuments(ListDocumentsRequest& request, ListDocumentsResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::getProvenanceUsers(GetUsersRequest& request, GetUsersResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::shareDocument(ShareDocumentRequest& request, ShareDocumentResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::revokeDocumentAccess(RevokeSharePrivilegeRequest& request, RevokeSharePrivilegeResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::createDocument(CreateDocumentRequest& request, CreateDocumentResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::updateDocument(UpdateDocumentRequest& request, UpdateDocumentResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::accessDocument(AccessDocumentRequest& request, AccessDocumentResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::createUser(CreateUserRequest& request, CreateUserResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::getPublicKey(RetrievePublicKeyRequest& request, RetrievePublicKeyResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::getPublicKey(RetrievePublicKeyByEmailRequest& request, RetrievePublicKeyByEmailResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::addDocumentAuditors(AddDocumentAuditorRequest& request, AddDocumentAuditorResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::getDocumentAuditorsWithKeys(GetDocumentAuditorsRequest& request, GetDocumentAuditorsResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::removeDocumentAuditors(RemoveDocumentAuditorRequest& request, RemoveDocumentAuditorResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::getDocumentEditors(GetDocumentEditorsRequest& request, GetDocumentEditorsResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::getAllDocuments(GetAllDocumentsRequest& request, GetAllDocumentsResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::getAllUsers(GetAllUsersRequest& request, GetAllUsersResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::getHistory(GetHistoryRequest& request, GetHistoryResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::ping(PingRequest& request, PingResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool TrustedServer::ping(int extra) 
		throw (SSLSendDataException, SSLReceiveDataException) {
	PingRequest request(extra);
	PingResponse response;

	return ping(request, response);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
