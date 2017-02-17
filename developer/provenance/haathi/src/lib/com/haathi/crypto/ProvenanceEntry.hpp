/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/ProvenanceEntry.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__CRYPTO__PROVENANCE_ENTRY_HPP
#	define COM__HAATHI__CRYPTO__PROVENANCE_ENTRY_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/crypto/KeyingMaterial.hpp"

#include "cppJSON.hpp"

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
class ProvenanceEntry {
	private:
		// Not implemented
		ProvenanceEntry(const ProvenanceEntry& other);
		ProvenanceEntry& operator=(const ProvenanceEntry& other);

	protected:
		std::string principalIdentifier;
		std::string encryptedEvent;
		std::string documentHash;
		std::string publicKey;
		std::string keyingMaterialString;
		std::string checksum;
		int publicKeySize;

		KeyingMaterial keyingMaterial;

	public:
		static std::string asJSONString(std::string principalIdentifier, std::string encryptedEvent,
				std::string documentHash, std::string publicKey, std::string keyingMaterialString,
				std::string checksum, int publicKeySize);

		ProvenanceEntry(const char* jsonString);

		std::string getPrincipalIdentifier();
		std::string getEncryptedEvent();
		std::string getDocumentHash();
		std::string getPublicKey();
		std::string getKeyingMaterialString();
		std::string getChecksum();
		int getPublicKeySize();

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
