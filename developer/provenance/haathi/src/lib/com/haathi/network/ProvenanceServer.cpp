/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/ProvenanceServer.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/network/ProvenanceServer.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/network/Packet.hpp"

#include "cppJSON.hpp"

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
ProvenanceServer::ProvenanceServer(NetworkInfo info): Server(info) {
}

bool ProvenanceServer::saveDocument(SaveProvenanceDocumentRequest& request, SaveProvenanceDocumentResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool ProvenanceServer::loadDocument(LoadProvenanceDocumentRequest& request, LoadProvenanceDocumentResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}

bool ProvenanceServer::auditDocument(AuditDocumentRequest& request, AuditDocumentResponse& response)
		throw (SSLSendDataException, SSLReceiveDataException) {
	return execute(request, response);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
