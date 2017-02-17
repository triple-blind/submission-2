/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/KeyingMaterial.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__CRYPTO__KEYING_MATERIAL_HPP
#	define COM__HAATHI__CRYPTO__KEYING_MATERIAL_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/crypto/Key.hpp"

#include "cppJSON.hpp"

#include <map>
#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace crypto {
/******************************************************************************
Class
******************************************************************************/
class KeyingMaterial {
	protected:
		Key sessionKey;
		std::map<int, CryptoInfo> cryptoInfos;

	public:
		static std::string asJSONString(std::map<int, CryptoInfo> cryptoInfos, Key sessionKey);

		KeyingMaterial();
		KeyingMaterial(const char* jsonString);

		std::map<int, CryptoInfo>::iterator findKeyingMaterial(int userId, bool* found);
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
