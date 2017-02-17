/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/Key.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__CRYPTO__KEY_HPP
#	define COM__HAATHI__CRYPTO__KEY_HPP
/******************************************************************************
Include
******************************************************************************/
#include "cppJSON.hpp"

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
// Size is necessary because the key is encoded
class Key {
	protected:
		std::string data;
		int size;

	public:
		Key(): data(""), size(0) {
		}

		Key(std::string data, int size): data(data), size(size) {
		}

		Key(cJSON* item, const char* key, const char* size):
				data(cJSON_GetObjectItem(item, key)->valuestring),
				size(cJSON_GetObjectItem(item, size)->valueint) {
		}

		Key(const Key& other): data(other.data), size(other.size) {
		}

		Key& operator=(const Key& other) {
			this->data = other.data;
			this->size = other.size;
			return *this;
		}

		std::string getData() {
			return data;
		}

		int getSize() {
			return size;
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
