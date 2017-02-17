/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/CryptoInfo.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__CRYPTO__CRYPTO_INFO_HPP
#	define COM__HAATHI__CRYPTO__CRYPTO_INFO_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/Key.hpp"

#include "cppJSON.hpp"

#include <cstdlib>
#include <ostream>
#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace crypto {
/******************************************************************************
CryptoInfo
******************************************************************************/
class CryptoInfo {
	public:
		std::string privateKey;
		int privateKeySize;

		std::string publicKey;
		int publicKeySize;

	protected:
		Key sessionKey;

	public:
		CryptoInfo() {
			privateKey = "";
			privateKeySize = 0;

			publicKey = "";
			publicKeySize = 0;
		}

		CryptoInfo& setPrivateKey(cJSON* item, const char* key, const char* size) {
			privateKey = std::string(cJSON_GetObjectItem(item, key)->valuestring);
			privateKeySize = cJSON_GetObjectItem(item, size)->valueint;
			return *this;
		}

		CryptoInfo& setPublicKey(cJSON* item, const char* key, const char* size) {
			publicKey = std::string(cJSON_GetObjectItem(item, key)->valuestring);
			publicKeySize = cJSON_GetObjectItem(item, size)->valueint;
			return *this;
		}

		CryptoInfo& setSessionKey(Key sessionKey) {
			this->sessionKey = sessionKey;
			return *this;
		}

		CryptoInfo& setSessionKey(cJSON* item, const char* key, const char* size) {
			setSessionKey(com::haathi::crypto::Key(cJSON_GetObjectItem(item, key)->valuestring,
					cJSON_GetObjectItem(item, size)->valueint));
			return *this;
		}

		Key getSessionKey() {
			return sessionKey;
		}

		friend std::ostream& operator<<(std::ostream& stream,
				const CryptoInfo& info);
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
