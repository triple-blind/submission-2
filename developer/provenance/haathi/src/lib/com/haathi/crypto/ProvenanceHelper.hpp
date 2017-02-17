/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/ProvenanceHelper.hpp#2 $

/*
 *	ProvenanceHelper.hpp
 *
 *	Created by Nitin Shinde on 9/17/11.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__CRYPTO__PROVENANCE_HELPER_HPP
#	define COM__HAATHI__CRYPTO__PROVENANCE_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/crypto/Key.hpp"
#include "com/haathi/network/NetworkInfo.hpp"

#include "cryptoppBeg.hpp"
#include <cryptopp/pssr.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/queue.h>

#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/des.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/rng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>
#include "cryptoppEnd.hpp"

#include <iostream>
#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace crypto {
/******************************************************************************
Class
******************************************************************************/
class ProvenanceHelper {
	private:
		CryptoInfo cryptInfo;
		CryptoPP::AutoSeededRandomPool randomPool;
		CryptoPP::RSA::PrivateKey privateKey;
		CryptoPP::RSA::PublicKey publicKey;

	protected:
		static void loadPrivateKey(CryptoPP::RSA::PrivateKey& privateKey,
				std::string encodedKey, int keySize);
		static void loadPublicKey(CryptoPP::RSA::PublicKey& publicKey,
				std::string encodedKey, int keySize);

	public:
		static std::string encryptData(std::string dataToEncrypt,
				std::string hexEncodedAesKey, int aesKeySize);
		static std::string encryptData(com::haathi::crypto::Key key, std::string dataToEncrypt);

		static std::string decryptData(std::string dataToDecrypt,
				std::string hexEncodedAesKey, int aesKeySize);
		static std::string decryptData(com::haathi::crypto::Key key, std::string dataToEncrypt);
		
		static std::string base64Encode(std::string dataToEncode);
		static std::string base64Decode(std::string dataToDecode);

		static std::string generateHash(const byte* address, int length);
		static std::string generateHash(std::string itemToHash);
		static std::string generateHash(std::vector<std::string>& itemsToHash);

		ProvenanceHelper(CryptoInfo cryptInfo);

		std::string signData(std::string dataToSign);
		bool verifyDataSignature(std::string dataToVerify,
				std::string& verifiedData);
		bool verifyDataSignature(std::string dataToVerify,
				std::string& verifiedData, std::string publicKeyToUse,
				int publicKeySize);

		std::string encryptData(std::string dataToEncrypt);
		std::string decryptData(std::string dataToDecrypt);

		std::string encryptWithRSA(std::string dataToEncrypt);

		std::string decryptWithRSA(std::string dataToDecrypt);
		std::string decryptWithRSA(std::string dataToDecrypt,
				std::string privateKeyToUse, int privateKeySize);
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
