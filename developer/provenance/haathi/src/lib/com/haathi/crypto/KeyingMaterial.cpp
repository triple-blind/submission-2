/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/KeyingMaterial.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/crypto/KeyingMaterial.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/crypto/ProvenanceHelper.hpp"

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
std::string KeyingMaterial::asJSONString(std::map<int, CryptoInfo> cryptoInfos, Key sessionKey) {
	cJSON* root = cJSON_CreateArray();
	cppJSON json(root);

	std::map<int, CryptoInfo>::iterator it;
	for (it = cryptoInfos.begin(); it != cryptoInfos.end(); it++) {
		ProvenanceHelper provenanceHelper(it->second);
		cJSON* item = cJSON_CreateObject();

		cJSON_AddNumberToObject(item, "id", it->first);
		std::string encryptedSessionKey = provenanceHelper.encryptWithRSA(sessionKey.getData());
		cJSON_AddStringToObject(item, "sk", provenanceHelper.base64Encode(encryptedSessionKey).c_str());
		cJSON_AddNumberToObject(item, "sz", sessionKey.getSize());
		cJSON_AddItemToArray(root, item);
	}
	return json.print();
}

KeyingMaterial::KeyingMaterial() {
}

KeyingMaterial::KeyingMaterial(const char* jsonString) {
	cJSON* root = cJSON_Parse(jsonString);

	for (int i = 0; i < cJSON_GetArraySize(root); i++) {
		cJSON* item = cJSON_GetArrayItem(root, i);
		int userId = cJSON_GetObjectItem(item, "id")->valueint;
		CryptoInfo cryptoInfo;

		cryptoInfo.setSessionKey(item, "sk", "sz");
		cryptoInfos[userId] = cryptoInfo;
	}
	cJSON_Delete(root);
}

std::map<int, CryptoInfo>::iterator KeyingMaterial::findKeyingMaterial(int userId, bool* found) {
	std::map<int, CryptoInfo>::iterator itr = cryptoInfos.find(userId);

	*found = (itr != cryptoInfos.end());
	return itr;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
