/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/ProvenanceServer.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__NETWORK__PROVENANCE_SERVER_HPP
#	define COM__HAATHI__NETWORK__PROVENANCE_SERVER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/network/Packet.hpp"
#include "com/haathi/network/Server.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace network {
/******************************************************************************
Class
******************************************************************************/
class ProvenanceServer: protected Server {
	public:
		ProvenanceServer(NetworkInfo info);

		bool saveDocument(SaveProvenanceDocumentRequest& request, SaveProvenanceDocumentResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool loadDocument(LoadProvenanceDocumentRequest& request, LoadProvenanceDocumentResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool auditDocument(AuditDocumentRequest& request, AuditDocumentResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);
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
