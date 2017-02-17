/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/ProvenanceEntry.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/crypto/ProvenanceEntry.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "cppJSON.hpp"
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
		namespace crypto {
/******************************************************************************
Class
******************************************************************************/
// Lots of events are stored, so the shorter these are, the better
const char* const PRINCIPAL_IDENTIFIER = "pi";
const char* const EVENT                = "e";
const char* const DOCUMENT_HASH        = "dh";
const char* const CHECKSUM             = "cs";
const char* const PUBLIC_KEY           = "pk";
const char* const PUBLIC_KEY_SIZE      = "sz";
const char* const KEY_MATERIAL         = "km";

std::string ProvenanceEntry::asJSONString(std::string principalIdentifier, std::string encryptedEvent,
		std::string documentHash, std::string publicKey, std::string keyingMaterialString,
		std::string checksum, int publicKeySize ) {

	cJSON* root = cJSON_CreateObject();
	cppJSON json(root);
	cJSON_AddStringToObject(root, PRINCIPAL_IDENTIFIER, principalIdentifier.c_str());
	cJSON_AddStringToObject(root, EVENT, encryptedEvent.c_str());
	cJSON_AddStringToObject(root, DOCUMENT_HASH, documentHash.c_str());
	cJSON_AddStringToObject(root, PUBLIC_KEY, publicKey.c_str());
	cJSON_AddStringToObject(root, KEY_MATERIAL, keyingMaterialString.c_str());
	cJSON_AddStringToObject(root, CHECKSUM, checksum.c_str());
	cJSON_AddNumberToObject(root, PUBLIC_KEY_SIZE, publicKeySize);

	return json.print();
}

ProvenanceEntry::ProvenanceEntry(const char* jsonString) {
	cJSON* root = cJSON_Parse(jsonString);

	principalIdentifier = cJSON_GetObjectItem(root, PRINCIPAL_IDENTIFIER)->valuestring;
	encryptedEvent = cJSON_GetObjectItem(root, EVENT)->valuestring;
	documentHash = cJSON_GetObjectItem(root, DOCUMENT_HASH)->valuestring;
	publicKey = cJSON_GetObjectItem(root, PUBLIC_KEY)->valuestring;
	keyingMaterialString = cJSON_GetObjectItem(root, KEY_MATERIAL)->valuestring;
	checksum = cJSON_GetObjectItem(root, CHECKSUM)->valuestring;
	publicKeySize = cJSON_GetObjectItem(root, PUBLIC_KEY_SIZE)->valueint;

	keyingMaterial = KeyingMaterial(keyingMaterialString.c_str());

	cJSON_Delete(root);
}

std::string ProvenanceEntry::getPrincipalIdentifier() {
	return principalIdentifier;
}

std::string ProvenanceEntry::getEncryptedEvent() {
	return encryptedEvent;
}

std::string ProvenanceEntry::getDocumentHash() {
	return documentHash;
}

std::string ProvenanceEntry::getPublicKey() {
	return publicKey;
}

std::string ProvenanceEntry::getKeyingMaterialString() {
	return keyingMaterialString;
}

std::string ProvenanceEntry::getChecksum() {
	return checksum;
}

int ProvenanceEntry::getPublicKeySize() {
	return publicKeySize;
}

std::map<int, CryptoInfo>::iterator ProvenanceEntry::findKeyingMaterial(int userId, bool* found) {
	return keyingMaterial.findKeyingMaterial(userId, found);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
