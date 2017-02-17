/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/TrustedServer.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__NETWORK__TRUSTED_SERVER_HPP
#	define COM__HAATHI__NETWORK__TRUSTED_SERVER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
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
class TrustedServer: protected Server {
	private:
		// Not implemented
		TrustedServer(const TrustedServer& other);
		TrustedServer& operator=(const TrustedServer& other);

	public:
		TrustedServer(NetworkInfo info);

		bool authenticate(AuthenticationRequest& request, AuthenticationResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool refreshSessionKey(RefreshSessionKeyRequest& request, RefreshSessionKeyResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool listAvailableDocuments(ListDocumentsRequest& request, ListDocumentsResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool getProvenanceUsers(GetUsersRequest& request, GetUsersResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool shareDocument(ShareDocumentRequest& request, ShareDocumentResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool revokeDocumentAccess(RevokeSharePrivilegeRequest& request, RevokeSharePrivilegeResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool createDocument(CreateDocumentRequest& request, CreateDocumentResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool updateDocument(UpdateDocumentRequest& request, UpdateDocumentResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool accessDocument(AccessDocumentRequest& request, AccessDocumentResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool createUser(CreateUserRequest& request, CreateUserResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool getPublicKey(RetrievePublicKeyRequest& request, RetrievePublicKeyResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool getPublicKey(RetrievePublicKeyByEmailRequest& request, RetrievePublicKeyByEmailResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool addDocumentAuditors(AddDocumentAuditorRequest& request, AddDocumentAuditorResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool getDocumentAuditorsWithKeys(GetDocumentAuditorsRequest& request, GetDocumentAuditorsResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool removeDocumentAuditors(RemoveDocumentAuditorRequest& request, RemoveDocumentAuditorResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool getDocumentEditors(GetDocumentEditorsRequest& request, GetDocumentEditorsResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool getAllDocuments(GetAllDocumentsRequest& request, GetAllDocumentsResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool getAllUsers(GetAllUsersRequest& request, GetAllUsersResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool getHistory(GetHistoryRequest& request, GetHistoryResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool ping(PingRequest& request, PingResponse& response)
				throw (SSLSendDataException, SSLReceiveDataException);

		bool ping(int extra = 0)
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
