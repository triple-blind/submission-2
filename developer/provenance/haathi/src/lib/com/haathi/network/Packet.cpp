/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/Packet.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/network/Packet.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "Config.hpp"
#if OSYS == OSYS_WIN
#	include <ctime>
#else
#	include <cstdlib>
#endif
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::crypto::CryptoInfo;
using com::haathi::crypto::Key;
using com::haathi::document::DocumentInfo;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace network {
/******************************************************************************
User
******************************************************************************/
bool operator==(const User& lhs, const User& rhs) {
    return lhs.getUserId() == rhs.getUserId();
}
/******************************************************************************
Version
******************************************************************************/
Version::Version(cJSON* item) {
	id = cJSON_GetObjectItem(item, "versionNo")->valueint;
	createdBy = cJSON_GetObjectItem(item, "createdBy")->valueint;
	createdOn = ResponsePacket::getTimestamp(item, "createdOn");
}
/******************************************************************************
ResponsePacket
******************************************************************************/
std::string ResponsePacket::getTimestamp(cJSON* item, const char* fieldName) {
	time_t ticks;
	struct tm dateTime;
	struct tm* pDateTime = &dateTime;
	char dateTimeBuf[100];
	std::string timestamp;

	try {
		cJSON* fieldValue = cJSON_GetObjectItem(item, fieldName);
		std::string tickString = fieldValue->valuestring;
		ticks = atol(tickString.c_str());
#if OSYS == OSYS_WIN
		localtime_s(&dateTime, &ticks);
		// %c -> 10/30/14 11:26:31
		// So help out Windows by adding %a for day of week
		strftime(dateTimeBuf, 100, "%a %c", pDateTime);
#else
		pDateTime = localtime_r(&ticks, &dateTime);
		// Linux seems to include day of week
		strftime(dateTimeBuf, 100, "%c", pDateTime);
#endif
		timestamp = dateTimeBuf;
	}
	catch (...) {
		timestamp = "Unknown";
	}
	return timestamp;
}
/******************************************************************************
AuthenticationResponse
******************************************************************************/
void AuthenticationResponse::setUserInfo(cJSON* payload) {
	userInfo.userId = cJSON_GetObjectItem(payload, "userId")->valueint;
	userInfo.first = cJSON_GetObjectItem(payload, "firstName")->valuestring;
	userInfo.last = cJSON_GetObjectItem(payload, "lastName")->valuestring;
}

void AuthenticationResponse::setNetworkInfo(cJSON* payload) {
	networkInfo.host = std::string(cJSON_GetObjectItem(payload, "host")->valuestring);
	networkInfo.portNumber = cJSON_GetObjectItem(payload, "port")->valueint;
	networkInfo.privateKey.key = cJSON_GetObjectItem(payload, "sslKey")->valuestring;
	networkInfo.privateKey.keySize = cJSON_GetObjectItem(payload, "sslKeySize")->valueint;
	networkInfo.publicCertificate.key = cJSON_GetObjectItem(payload, "sslCertificate")->valuestring;
	networkInfo.publicCertificate.keySize = cJSON_GetObjectItem(payload, "sslCertificateSize")->valueint;
}

void AuthenticationResponse::setCryptoInfo(cJSON* payload) {
	cryptoInfo.setPrivateKey(payload, "privateKey", "privateKeySize");
	cryptoInfo.setPublicKey(payload, "publicKey", "publicKeySize");
	cryptoInfo.setSessionKey(payload, "sessionKey", "sessionKeySize");
}

bool AuthenticationResponse::updateFields(cJSON* payload) {
	setUserInfo(payload);
	setNetworkInfo(payload);
	setCryptoInfo(payload);
	return true;
}

UserInfo AuthenticationResponse::getUserInfo() {
	return userInfo;
}

NetworkInfo AuthenticationResponse::getNetworkInfo() {
	return networkInfo;
}

CryptoInfo AuthenticationResponse::getCryptoInfo() {
	return cryptoInfo;
}
/******************************************************************************
ListDocumentsResponse
******************************************************************************/
bool ListDocumentsResponse::updateFields(cJSON* payload) {
	cJSON* docs = cJSON_GetObjectItem(payload, "documents");
	int arraySize = cJSON_GetArraySize(docs);

	for (int index = 0; index < arraySize; index++) {
		cJSON* item = cJSON_GetArrayItem(docs, index);
		int documentId = cJSON_GetObjectItem(item, "documentId")->valueint;

		std::string documentTitle = cJSON_GetObjectItem(item, "title")->valuestring;
		documents[documentId] = documentTitle;
		documentKeys[documentId] = Key(item, "uniqueKey", "uniqueKeySize");

		DocumentInfo documentInfo;
		documentInfo.name  = documentTitle;
		documentInfo.first = cJSON_GetObjectItem(item, "firstName")->valuestring;
		documentInfo.last  = cJSON_GetObjectItem(item, "lastName" )->valuestring;
		documentInfo.email = cJSON_GetObjectItem(item, "emailAddress")->valuestring;
		documentInfo.updatedOn = getTimestamp(item, "updatedOn");
		documentInfo.createdOn = getTimestamp(item, "createdOn");
		documentInfo.versionNo = cJSON_GetObjectItem(item, "versionNo")->valueint;
		documentInfo.versionId = cJSON_GetObjectItem(item, "versionId")->valuestring;
		documentInfos[documentId] = documentInfo;
	}
	return true;
}
/******************************************************************************
LoadProvenanceDocument
******************************************************************************/
bool LoadProvenanceDocumentResponse::updateFields(cJSON* payload) {
	provenance = cJSON_GetObjectItem(payload, "provenance")->valuestring;
	document = cJSON_GetObjectItem(payload, "document")->valuestring;
	return true;
}
/******************************************************************************
AuditDocumentResponse
******************************************************************************/
bool AuditDocumentResponse::updateFields(cJSON* payload) {	
	cJSON* outerProvenances = cJSON_GetObjectItem(payload, "provenances");
	int outerCount = cJSON_GetArraySize(outerProvenances);

	for (int outer = 0; outer < outerCount; outer++)
		provenances.push_back(std::string(cJSON_GetArrayItem(outerProvenances, outer)->valuestring));
	document = cJSON_GetObjectItem(payload, "document")->valuestring;
	return true;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
