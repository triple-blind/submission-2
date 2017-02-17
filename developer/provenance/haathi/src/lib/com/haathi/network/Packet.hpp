/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/Packet.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__NETWORK__PACKET_HPP
#	define COM__HAATHI__NETWORK__PACKET_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/crypto/Key.hpp"
#include "com/haathi/document/DocumentInfo.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/NetworkException.hpp"
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/network/UserInfo.hpp"

#include "cppJSON.hpp"

#include <map>
#include <new>
#include <sstream>
#include <vector>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace network {
/******************************************************************************
Enums
******************************************************************************/
/******************************************************************************
User
******************************************************************************/
class User {
	public:
		enum UserType {
			USER = 1, AUDITOR = 2, ADMINISTRATOR = 3
		};

		enum AccessType {
			NEW = 1, OPEN = 2, SAVE = 3, AUDIT = 4
		};

	private:
		std::string emailAddress;
		std::string firstName;
		std::string lastName;
		int userId;

	public:
		User() {
			userId = -1;
		}

		User(int userId, std::string firstName, std::string lastName, std::string email) {
			this->userId = userId;
			this->firstName = firstName;
			this->lastName = lastName;
			this->emailAddress = email;
		}

		User(cJSON* item) {
			userId = cJSON_GetObjectItem(item, "userId")->valueint;
			firstName = cJSON_GetObjectItem(item, "firstName")->valuestring;
			lastName = cJSON_GetObjectItem(item, "lastName")->valuestring;
			emailAddress = cJSON_GetObjectItem(item, "emailAddress")->valuestring;
		}

		std::string getFirst() {
			return firstName;
		}

		std::string getLast() {
			return lastName;
		}

		int getUserId() const {
			return userId;
		}

		std::string getEmailAddress() {
			return emailAddress;
		}

		std::string getFullName() {
			return firstName +
				(firstName.length() > 0 && lastName.length() > 0 ? " " : "") +
				lastName;
		}

		int compare(User &other) {
			int compare = firstName.compare(other.firstName);

			if (compare != 0)
				return compare;
			compare = lastName.compare(other.lastName);
			if (compare != 0)
				return compare;
			return userId - other.userId;
		}
};

bool operator==(const User& lhs, const User& rhs);
/******************************************************************************
Version
******************************************************************************/
class Version {
	private:
		int id;
		int createdBy;
		std::string createdOn;

	public:
		Version(): id(-1), createdBy(-1) {
		}

		Version(int id, int createdBy, std::string createdOn) {
			this->id = id;
			this->createdBy = createdBy;
			this->createdOn = createdOn;
		}

		Version(cJSON* item);

		int getId() {
			return id;
		}

		int getCreatedBy() {
			return createdBy;
		}

		std::string getCreatedOn() {
			return createdOn;
		}

		int compare(Version &other) {
			return id - other.id;
		}
};
/******************************************************************************
Packet
******************************************************************************/
class Packet {
	public:
		Packet() {
		}

		virtual ~Packet() {
		}
};
/******************************************************************************
RequestPacket
******************************************************************************/
// This is for outgoing data.  It takes the arguments to the constructor and
// formats them as JSON which is then retrieved by the server which converts
// it to a string there.  Subclasses can add to the payload.
class RequestPacket: protected Packet {
	protected:
		cJSON* root;
		cJSON* payload;

	public:
		RequestPacket(std::string command) {
			if ((root = cJSON_CreateObject()) == 0)
				throw std::bad_alloc();
			if ((payload = cJSON_CreateObject()) == 0) {
				cJSON_Delete(root);
				throw std::bad_alloc();
			}
			cJSON_AddItemToObject(root, "payload", payload);
			cJSON_AddItemToObject(root, "command", cJSON_CreateString(command.c_str()));
		}

		virtual ~RequestPacket() {
			// Payload will be deleted since it has been added to root
			cJSON_Delete(root);
		}

		cJSON* getPacket() {
			return root;
		}
};

class AuthenticatingRequestPacket: public RequestPacket {
	public:
		AuthenticatingRequestPacket(std::string command, std::string username, std::string password)
				throw (AuthenticationCredentialsMissingException): RequestPacket(command) {
			if (username.empty() || password.empty())
				throw AuthenticationCredentialsMissingException(
						"Missing username or password");

			cJSON* credentials = cJSON_CreateObject();
			if (credentials == 0)
				throw std::bad_alloc();

			// These c_str must stay around for the life of the request!
			cJSON_AddStringToObject(credentials, "emailAddress", username.c_str());
			cJSON_AddStringToObject(credentials, "password", password.c_str());
			cJSON_AddItemToObject(root, "credentials", credentials);
		}
};
/******************************************************************************
ResponsePacket
******************************************************************************/
// The response takes a (JSON) string via setPacket, places values into
// instance variables and makes them accessible via getters.  Subclasses
// should add their own results in updateFields at which time they have
// temporary access to the payload.  Subclasses can return false and set
// a message if their payload is invalid.
class ResponsePacket: protected Packet {
	private:
		// Not implemented
		ResponsePacket& operator=(const ResponsePacket& other);
//		ResponsePacket(const ResponsePacket& other);

	protected:
		std::string message;

		virtual bool updateFields(cJSON* payload) {
			return true;
		}

	public:
		static std::string getTimestamp(cJSON* item, const char* fieldName);

		ResponsePacket() {
		}

		bool setPacket(std::string jsonString) {
			cJSON* root = cJSON_Parse(jsonString.c_str());
			if (!root) {
				message = "Could not parse response";
				return false;
			}

			cJSON* payload = cJSON_GetObjectItem(root, "payload");
			if (!payload) {
				message = "No payload was returned";
				cJSON_Delete(root);
				return false;
			}

			bool success = cJSON_GetObjectItem(root, "result")->type != 0;
			if (!success) {
				message = payload->valuestring ?
					std::string(payload->valuestring) :
					"No explanation available";
				cJSON_Delete(root);
				return false;
			}

			success = updateFields(payload);
			cJSON_Delete(root);
			return success;
		}

		virtual std::string getMessage() {
			return message;
		}
};
/******************************************************************************
Authentication pair
******************************************************************************/
class AuthenticationRequest: public AuthenticatingRequestPacket {
	public:
		AuthenticationRequest(std::string username, std::string password):
				AuthenticatingRequestPacket("AUTHENTICATE", username, password) {
		}
};

class AuthenticationResponse: public ResponsePacket {
	private:
		UserInfo userInfo;
		NetworkInfo networkInfo;
		com::haathi::crypto::CryptoInfo cryptoInfo;

		void setUserInfo(cJSON* payload);
		void setNetworkInfo(cJSON* payload);
		void setCryptoInfo(cJSON* payload);

	protected:
		bool updateFields(cJSON* payload);

	public:
		UserInfo getUserInfo();
		NetworkInfo getNetworkInfo();
		com::haathi::crypto::CryptoInfo getCryptoInfo();
};
/******************************************************************************
RefreshSessionKey pair
******************************************************************************/
class RefreshSessionKeyRequest: public AuthenticatingRequestPacket {
	public:
		RefreshSessionKeyRequest(std::string username, std::string password):
				AuthenticatingRequestPacket("REFRESH", username, password) {
		}
};

class RefreshSessionKeyResponse: public ResponsePacket {
	private:
		com::haathi::crypto::CryptoInfo cryptoInfo;

	protected:
		bool updateFields(cJSON* payload) {
			cryptoInfo.setSessionKey(payload, "sessionKey", "sessionKeySize");
			return true;
		}

	public:
		com::haathi::crypto::CryptoInfo getCryptoInfo() {
			return cryptoInfo;
		}
};
/******************************************************************************
SaveProvenanceDocument pair
******************************************************************************/
class SaveProvenanceDocumentRequest: public RequestPacket {
	public:
		SaveProvenanceDocumentRequest(std::string versionId, std::string prevVersionId,
				std::string encryptedProvenance, std::string documentContent): RequestPacket("PUT_EDIT") {
			cJSON_AddStringToObject(payload, "versionId", versionId.c_str());
			cJSON_AddStringToObject(payload, "prevVersionId", prevVersionId.c_str());
			cJSON_AddStringToObject(payload, "provenance", encryptedProvenance.c_str());
			cJSON_AddStringToObject(payload, "document", documentContent.c_str());
		}
};

class SaveProvenanceDocumentResponse: public ResponsePacket {
};
/******************************************************************************
LoadProvenanceDocument pair
******************************************************************************/
class LoadProvenanceDocumentRequest: public RequestPacket {
	public:
		LoadProvenanceDocumentRequest(std::string versionId): RequestPacket("GET_EDIT") {
			cJSON_AddStringToObject(payload, "versionId", versionId.c_str());
		}
};

class LoadProvenanceDocumentResponse: public ResponsePacket {
	private:
		std::string provenance;
		std::string document;

	protected:
		bool updateFields(cJSON* payload);

	public:
		std::string getProvenance() {
			return provenance;
		}

		std::string getDocument() {
			return document;
		}
};
/******************************************************************************
Authentication pair
******************************************************************************/
class PingRequest: public RequestPacket {
	public:
		PingRequest(): RequestPacket("PING") {
		}

		PingRequest(int extra): RequestPacket("PING") {
			if (extra > 0) {
				const char* digits = "0123456789";
				std::stringstream stream;

				for (int i = 0; i < extra; i++)
					stream << digits[i % 10];
				cJSON_AddStringToObject(payload, "extra", stream.str().c_str());
			}
		}
};

class PingResponse: public ResponsePacket {
};
/******************************************************************************
ListDocuments pair
******************************************************************************/
class ListDocumentsRequest: public AuthenticatingRequestPacket {
	public:
		ListDocumentsRequest(std::string username, std::string password, int userId,
				enum User::UserType userType):
				AuthenticatingRequestPacket("LIST_DOCUMENTS", username, password) {
			cJSON_AddNumberToObject(payload, "userId", userId);
			cJSON_AddNumberToObject(payload, "userTypeId", (int) userType);
		}
};

class ListDocumentsResponse: public ResponsePacket {
	private:
		std::map<int, std::string> documents;
		std::map<int, com::haathi::crypto::Key> documentKeys;
		std::map<int, com::haathi::document::DocumentInfo> documentInfos;

	protected:
		bool updateFields(cJSON* payload);
	
	public:
		std::map<int, std::string> getDocuments() {
			return documents;
		}

		std::map<int, com::haathi::crypto::Key> getDocumentKeys() {
			return documentKeys;
		}

		std::map<int, com::haathi::document::DocumentInfo> getDocumentInfos() {
			return documentInfos;
		}
};
/******************************************************************************
GetUsers pair
******************************************************************************/
class GetUsersRequest: public AuthenticatingRequestPacket {
	public:
		GetUsersRequest(std::string username, std::string password,
				enum User::UserType userType): 
				AuthenticatingRequestPacket("LIST_USERS", username, password) {
			cJSON_AddNumberToObject(payload, "userTypeId", ( int ) userType);
		}
};


class GetUsersResponse: public ResponsePacket {
	private:
		std::vector<User> users;

	protected:
		bool updateFields(cJSON* payload) {
			cJSON* items = cJSON_GetObjectItem(payload, "users");
			int arraySize = cJSON_GetArraySize(items);

			for (int index = 0; index < arraySize; index++) {
				cJSON* item = cJSON_GetArrayItem(items, index);
				users.push_back(User(item));
			}
			return true;
		}

	public:
		std::vector<User> getUserList() {
			return users;
		}
};
/******************************************************************************
ShareDocument pair
******************************************************************************/
class ShareDocumentRequest: public AuthenticatingRequestPacket {
	public:
		ShareDocumentRequest(std::string username, std::string password,
				int* usersToShareWith, int usersCount, int documentId):
				AuthenticatingRequestPacket("ADD_EDITORS", username, password) {
			cJSON_AddNumberToObject(payload, "documentId", documentId);
			cJSON_AddItemToObject(payload, "userIds", cJSON_CreateIntArray(usersToShareWith, usersCount));
		}
};

class ShareDocumentResponse: public ResponsePacket {
};
/******************************************************************************
RevokeSharePrivilege pair
******************************************************************************/
class RevokeSharePrivilegeRequest: public AuthenticatingRequestPacket {
	public:
		RevokeSharePrivilegeRequest(std::string username, std::string password,
				int* usersToRevoke, int usersCount, int documentId): 
				AuthenticatingRequestPacket("SUB_EDITORS", username, password) {
			cJSON_AddNumberToObject(payload, "documentId", documentId);
			cJSON_AddItemToObject(payload, "userIds", cJSON_CreateIntArray(usersToRevoke, usersCount));
		}
};

class RevokeSharePrivilegeResponse: public ResponsePacket {
};
/******************************************************************************
CreateDocument pair
******************************************************************************/
class CreateDocumentRequest: public AuthenticatingRequestPacket {
	public:
		CreateDocumentRequest(std::string username, std::string password,
				std::string title): 
				AuthenticatingRequestPacket("CREATE_DOCUMENT", username, password) {
			cJSON_AddStringToObject(payload, "title", title.c_str());
		}
};

class CreateDocumentResponse: public ResponsePacket {
	private:
		com::haathi::crypto::Key uniqueKey;
		int documentId;
		std::string versionId;

	protected:
		bool updateFields(cJSON* payload) {
			uniqueKey = com::haathi::crypto::Key(payload, "uniqueKey", "uniqueKeySize");
			documentId = cJSON_GetObjectItem(payload, "documentId")->valueint;
			versionId = cJSON_GetObjectItem(payload, "versionId")->valuestring;
			return true;
		}

	public:
		com::haathi::crypto::Key getUniqueKey() {
			return uniqueKey;
		}

		int getDocumentId() {
			return documentId;
		}

		std::string getVersionId() {
			return versionId;
		}
};
/******************************************************************************
UpdateDocument pair
******************************************************************************/
class UpdateDocumentRequest: public AuthenticatingRequestPacket {
	public:
		UpdateDocumentRequest(std::string username, std::string password,
				int round, int documentId, std::string versionId, std::string prevVersionId): 
				AuthenticatingRequestPacket("UPDATE_DOCUMENT", username, password) {
			cJSON_AddNumberToObject(payload, "round", round);
			cJSON_AddNumberToObject(payload, "documentId", documentId);
			cJSON_AddStringToObject(payload, "versionId", versionId.c_str());
			if (!prevVersionId.empty())
				cJSON_AddStringToObject(payload, "prevVersionId", prevVersionId.c_str());
			else
				cJSON_AddNullToObject(payload, "prevVersionId");
		}
};

class UpdateDocumentResponse: public ResponsePacket {
	private:
		std::string versionId;
		int versionNo;

	protected:
		bool updateFields(cJSON* payload) {
			versionId = cJSON_GetObjectItem(payload, "versionId")->valuestring;
			versionNo = cJSON_GetObjectItem(payload, "versionNo")->valueint;
			return true;
		}

	public:
		std::string getVersionId() {
			return versionId;
		}

		int getVersionNo() {
			return versionNo;
		}
};
/******************************************************************************
AccessDocument pair
******************************************************************************/
class AccessDocumentRequest: public AuthenticatingRequestPacket {
	public:
		AccessDocumentRequest(std::string username, std::string password,
				int round, int documentId, std::string versionId, User::AccessType accessType): 
				AuthenticatingRequestPacket("ACCESS_DOCUMENT", username, password) {
			cJSON_AddNumberToObject(payload, "round", round);
			cJSON_AddNumberToObject(payload, "documentId", documentId);
			cJSON_AddStringToObject(payload, "versionId", versionId.c_str());
			cJSON_AddNumberToObject(payload, "accessTypeId", accessType);
	}
};

class AccessDocumentResponse: public ResponsePacket {
};
/******************************************************************************
CreateUser pair
******************************************************************************/
class CreateUserRequest: public AuthenticatingRequestPacket {
	public:
		CreateUserRequest(std::string username, std::string password,
				std::string firstName, std::string lastName,
				std::string emailAddress, int userTypeId, std::string pwd):
				AuthenticatingRequestPacket("CREATE_USER", username, password) {
			cJSON_AddStringToObject(payload, "emailAddress", emailAddress.c_str());
			cJSON_AddStringToObject(payload, "firstName", firstName.c_str());
			cJSON_AddStringToObject(payload, "lastName", lastName.c_str());
			cJSON_AddNumberToObject(payload, "userTypeId", userTypeId);
			cJSON_AddStringToObject(payload, "password", pwd.c_str());
		}
};

class CreateUserResponse: public ResponsePacket {
};
/******************************************************************************
RetrievePublicKey pair
******************************************************************************/
class RetrievePublicKeyRequest: public AuthenticatingRequestPacket {
	public:
		RetrievePublicKeyRequest(std::string username, std::string password,
				int* usersToRetrieve, int usersCount):
				AuthenticatingRequestPacket("GET_PUBLIC_KEYS", username, password) {
			cJSON_AddItemToObject(payload, "userIds", cJSON_CreateIntArray(usersToRetrieve, usersCount));
		}
};

class RetrievePublicKeyResponse: public ResponsePacket {
	private:
		std::map<int, com::haathi::crypto::CryptoInfo> publicKeys;

	protected:
		bool updateFields(cJSON *payload) {
			cJSON* items = cJSON_GetObjectItem(payload, "publicKeys");
			int arraySize = cJSON_GetArraySize(items);

			for (int index = 0; index < arraySize; index++) {
				cJSON* item = cJSON_GetArrayItem(items, index);
				int userId = cJSON_GetObjectItem(item, "userId")->valueint;
				com::haathi::crypto::CryptoInfo cryptoInfo;

				cryptoInfo.setPublicKey(item, "publicKey", "publicKeySize");
				publicKeys[userId] = cryptoInfo;
			}
			return true;
		}

	public:
		std::map<int, com::haathi::crypto::CryptoInfo> getKeys() {
			return publicKeys;
		}
};
/******************************************************************************
RetrievePublicKeyByEmail pair
******************************************************************************/
class RetrievePublicKeyByEmailRequest: public AuthenticatingRequestPacket {
	public:
		RetrievePublicKeyByEmailRequest(std::string username, std::string password,
				std::string emailAddress):
				AuthenticatingRequestPacket("GET_PUBLIC_KEY_BY_EMAIL", username, password) {
			cJSON_AddStringToObject(payload, "emailAddress", emailAddress.c_str());
		}
};

class RetrievePublicKeyByEmailResponse: public ResponsePacket {
	private:
		com::haathi::crypto::CryptoInfo cryptoInfo;

	protected:
		bool updateFields(cJSON* payload) {
			cryptoInfo.setPublicKey(payload, "publicKey", "publicKeySize");
			return true;
		}

	public:
		com::haathi::crypto::CryptoInfo getKey() {
			return cryptoInfo;
		}
};
/******************************************************************************
AddDocumentAuditor pair
******************************************************************************/
class AddDocumentAuditorRequest: public AuthenticatingRequestPacket {
	public:
		AddDocumentAuditorRequest(std::string username, std::string password,
				int* usersToShareWith, int usersCount, int documentId):
				AuthenticatingRequestPacket("ADD_AUDITORS", username, password) {
			cJSON_AddNumberToObject(payload, "documentId", documentId);
			cJSON_AddItemToObject(payload, "userIds", cJSON_CreateIntArray(usersToShareWith, usersCount));
		}
};

class AddDocumentAuditorResponse: public ResponsePacket {
};
/******************************************************************************
GetDocumentAuditors pair
******************************************************************************/
class GetDocumentAuditorsRequest: public AuthenticatingRequestPacket {
	public:
		GetDocumentAuditorsRequest(std::string username, std::string password,
				int documentId):
				AuthenticatingRequestPacket("GET_DOCUMENT_AUDITORS", username, password) {
			cJSON_AddNumberToObject(payload, "documentId", documentId);
		}
};

class GetDocumentAuditorsResponse: public ResponsePacket {
	private:
		std::map<int, com::haathi::crypto::CryptoInfo> publicKeys;

	protected:
		bool updateFields(cJSON* payload) {
			cJSON* items = cJSON_GetObjectItem(payload, "auditors");
			int arraySize = cJSON_GetArraySize(items);

			for (int index = 0; index < arraySize; index++) {
				cJSON* item = cJSON_GetArrayItem(items, index);
				int userId = cJSON_GetObjectItem(item, "userId")->valueint;
				com::haathi::crypto::CryptoInfo cryptoInfo;

				cryptoInfo.setPublicKey(item, "publicKey", "publicKeySize");
				publicKeys[userId] = cryptoInfo;
			}
			return true;
		}

	public:
		std::map<int, com::haathi::crypto::CryptoInfo> getKeys() {
			return publicKeys;
		}
};
/******************************************************************************
GetDocumentEditors pair
******************************************************************************/
class GetDocumentEditorsRequest: public AuthenticatingRequestPacket {
	public:
		GetDocumentEditorsRequest(std::string username, std::string password,
				int documentId):
				AuthenticatingRequestPacket("GET_DOCUMENT_EDITORS", username, password) {
			cJSON_AddNumberToObject(payload, "documentId", documentId);
		}
};

class GetDocumentEditorsResponse: public ResponsePacket {
	private:
		std::vector<User> users;

	protected:
		bool updateFields(cJSON* payload) {
			cJSON* items = cJSON_GetObjectItem(payload, "editors");
			int arraySize = cJSON_GetArraySize(items);

			for (int index = 0; index < arraySize; index++) {
				cJSON* item = cJSON_GetArrayItem(items, index);
				
				users.push_back(User(item));
			}
			return true;
		}

	public:
		std::vector<User> getUsers() {
			return users;
		}
};
/******************************************************************************
RemoveDocumentAuditor pair
******************************************************************************/
class RemoveDocumentAuditorRequest: public AuthenticatingRequestPacket {
	public:
		RemoveDocumentAuditorRequest(std::string username, std::string password,
				int* usersToRemove, int usersCount, int documentId):
				AuthenticatingRequestPacket("SUB_AUDITORS", username, password) {
			cJSON_AddNumberToObject(payload, "documentId", documentId);
			cJSON_AddItemToObject(payload, "userIds", cJSON_CreateIntArray(usersToRemove, usersCount));
		}
};

class RemoveDocumentAuditorResponse: public ResponsePacket {
};
/******************************************************************************
AuditDocument pair
******************************************************************************/
class AuditDocumentRequest: public RequestPacket {
	public:
		AuditDocumentRequest(std::string versionId):
				RequestPacket("GET_AUDIT") {
			cJSON_AddStringToObject(payload, "versionId", versionId.c_str());
		}
};

class AuditDocumentResponse: public ResponsePacket {
	private:
		std::vector<std::string> provenances;
		std::string document;

	protected:
		bool updateFields(cJSON* payload);

	public:
		std::vector<std::string> getProvenances() {
			return provenances;
		}

		std::string getDocument() {
			return document;
		}
};
/******************************************************************************
GetAllUsers pair
******************************************************************************/
class GetAllUsersRequest: public AuthenticatingRequestPacket {
	public:
		GetAllUsersRequest(std::string username, std::string password):
				AuthenticatingRequestPacket("GET_ALL_USERS", username, password) {
		}
};

class GetAllUsersResponse: public ResponsePacket {
	private:
		std::map<int, com::haathi::crypto::CryptoInfo> users;

	protected:
		bool updateFields(cJSON* payload) {
			cJSON* items = cJSON_GetObjectItem(payload, "users");
			int arraySize = cJSON_GetArraySize(items);

			for (int index = 0; index < arraySize; index++) {
				cJSON* item = cJSON_GetArrayItem(items, index);
				int userId = cJSON_GetObjectItem(item, "userId")->valueint;
				com::haathi::crypto::CryptoInfo cryptoInfo;

				cryptoInfo.setPublicKey(item, "publicKey", "publicKeySize");
				cryptoInfo.setPrivateKey(item, "privateKey", "privateKeySize");
				users[userId] = cryptoInfo;
			}
			return true;
		}

	public:
		std::map<int,com::haathi::crypto::CryptoInfo> getUsers() {
			return users;
		}
};
/******************************************************************************
GetAllDocuments pair
******************************************************************************/
class GetAllDocumentsRequest: public AuthenticatingRequestPacket {
	public:
		GetAllDocumentsRequest(std::string username, std::string password):
				AuthenticatingRequestPacket("GET_ALL_DOCUMENTS", username, password) {
		}
};

class GetAllDocumentsResponse: public ResponsePacket {
	private:
		std::map<int, std::string> documents;
		std::map<int, com::haathi::crypto::CryptoInfo> documentKeys;

	protected:
		bool updateFields(cJSON* payload) {
			cJSON* items = cJSON_GetObjectItem(payload, "documents");
			int arraySize = cJSON_GetArraySize(items);

			for (int index = 0; index < arraySize; index++) {
				cJSON* item = cJSON_GetArrayItem(items, index);
				int documentId = cJSON_GetObjectItem(item, "documentId")->valueint;
				std::string documentTitle = cJSON_GetObjectItem(item, "title")->valuestring;
				com::haathi::crypto::CryptoInfo cryptoInfo;

				cryptoInfo.setSessionKey(item, "uniqueKey", "uniqueKeySize");
				documents[documentId] = documentTitle;
				documentKeys[documentId] = cryptoInfo;
			}
			return true;
		}

	public:
		std::map<int, std::string> getDocuments() {
			return documents;
		}

		std::map<int, com::haathi::crypto::CryptoInfo> getDocumentKeys() {
			return documentKeys;
		}
};
/******************************************************************************
GetHistory pair
******************************************************************************/
class GetHistoryRequest: public AuthenticatingRequestPacket {
	public:
		GetHistoryRequest(std::string username, std::string password, int documentId):
				AuthenticatingRequestPacket("GET_DOCUMENT_HISTORY", username, password) {
			cJSON_AddNumberToObject(payload, "documentId", documentId);
	}
};

class GetHistoryResponse: public ResponsePacket {
	private:
		std::vector<Version> versions;

	protected:
		bool updateFields(cJSON* payload) {
			cJSON* items = cJSON_GetObjectItem(payload, "history");
			int arraySize = cJSON_GetArraySize(items);

			for (int index = 0; index < arraySize; index++) {
				cJSON* item = cJSON_GetArrayItem(items, index);
				Version version(item);

				versions.push_back(version);
			}
			return true;
		}

	public:
		std::vector<Version> getVersions() {
			return versions;
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
