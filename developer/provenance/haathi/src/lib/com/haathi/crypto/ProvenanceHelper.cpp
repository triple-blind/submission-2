/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/ProvenanceHelper.cpp#2 $

/*
 *	ProvenanceHelper.cpp
 *
 *	Author: Nitin Shinde
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/crypto/ProvenanceHelper.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Guard.hpp"

#include <string>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::guard;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace crypto {
/******************************************************************************
Class
******************************************************************************/
ProvenanceHelper::ProvenanceHelper(CryptoInfo cryptInfo) {
	this->cryptInfo = cryptInfo;
	if (!cryptInfo.privateKey.empty())
		loadPrivateKey(privateKey, cryptInfo.privateKey, cryptInfo.privateKeySize);
	if (!cryptInfo.publicKey.empty())
		loadPublicKey(publicKey, cryptInfo.publicKey, cryptInfo.publicKeySize);
}

void ProvenanceHelper::loadPrivateKey(CryptoPP::RSA::PrivateKey& privateKey,
		std::string encodedKey, int keySize) {
	try {
		std::string decoded = base64Decode(encodedKey);

		CryptoPP::ByteQueue queue;
		CryptoPP::StringSource source((const byte*) decoded.data(),
				keySize, true);
		source.TransferTo(queue);
		queue.MessageEnd();

		privateKey.Load(queue);
	}
	catch (CryptoPP::Exception& ex) {
		std::cerr << "Exception caught while loading private key: " << std::endl;
		std::cerr << ex.what() << std::endl;
	}
}

void ProvenanceHelper::loadPublicKey(CryptoPP::RSA::PublicKey& publicKey,
		std::string encodedKey, int keySize) {
	try {
		std::string decoded = base64Decode(encodedKey);

		CryptoPP::ByteQueue queue;
		CryptoPP::StringSource source((const byte*) decoded.data(),
				keySize, true);
		source.TransferTo(queue);
		queue.MessageEnd();

		publicKey.Load(queue);
	}
	catch (CryptoPP::Exception& ex) {
		std::cerr << "Exception caught while loading public key : " << std::endl;
		std::cerr << ex.what() << std::endl;
	}
}

std::string ProvenanceHelper::signData(std::string dataToEncrypt) {
	std::string cipherText;

	try {
		CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::SHA256>::Signer signer(
				privateKey);
		CryptoPP::StringSource(dataToEncrypt, true,
				new CryptoPP::SignerFilter(randomPool, signer,
						new CryptoPP::StringSink(cipherText), true));
	}
	catch (CryptoPP::Exception& ex) {
		std::cerr << "Caught exception during data signing: " << std::endl;
		std::cerr << ex.what() << std::endl;
	}
	return cipherText;
}

std::string ProvenanceHelper::encryptWithRSA(std::string dataToEncrypt) {
	std::string cipherText;

	try {
		CryptoPP::RSAES_OAEP_SHA_Encryptor e(publicKey);

		CryptoPP::StringSource(dataToEncrypt, true,
				new CryptoPP::PK_EncryptorFilter(randomPool, e,
						new CryptoPP::StringSink(cipherText)) // PK_EncryptorFilter
								);// StringSource
	}
	catch (CryptoPP::Exception& ex) {
		std::cerr << "Caught exception during encryption: " << std::endl;
		std::cerr << ex.what() << std::endl;
	}
	return cipherText;
}

std::string ProvenanceHelper::encryptData(std::string dataToEncrypt,
		std::string aesKey, int aesKeySize) {
	std::string cipherText;

	try {
		CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption e;

		e.SetKey((const byte*) base64Decode(aesKey).data(), aesKeySize);

		// The StreamTransformationFilter adds padding
		//  as required. ECB and CBC Mode must be padded
		//  to the block size of the cipher.
		CryptoPP::StringSource(dataToEncrypt, true,
				new CryptoPP::StreamTransformationFilter(e,
						new CryptoPP::StringSink(cipherText)) // StreamTransformationFilter
								);// StringSource
	}
	catch (CryptoPP::Exception& ex) {
		std::cerr << "Caught exception during encryption: " << std::endl;
		std::cerr << ex.what() << std::endl;
	}
	catch (std::exception &ex2) {
		std::cerr << "Caught std exception: " << ex2.what();
	}
	return cipherText;
}

std::string ProvenanceHelper::encryptData(std::string dataToEncrypt) {
	return encryptData(dataToEncrypt, cryptInfo.getSessionKey().getData(),
			cryptInfo.getSessionKey().getSize());
}

std::string ProvenanceHelper::encryptData(Key key, std::string dataToDecrypt) {
	return encryptData(dataToDecrypt, key.getData(), key.getSize());
}

bool ProvenanceHelper::verifyDataSignature(std::string dataToVerify,
		std::string& verifiedData, std::string publicKeyToUse, int publicKeySize) {
	std::string plainText;
	bool result = false;

	try {
		CryptoPP::RSA::PublicKey tmpPublicKey;

		loadPublicKey(tmpPublicKey, publicKeyToUse, publicKeySize);
		CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::SHA256>::Verifier verifier(
				tmpPublicKey);
		CryptoPP::StringSource(dataToVerify, true,
				new CryptoPP::SignatureVerificationFilter(verifier,
						new CryptoPP::StringSink(plainText),
						CryptoPP::SignatureVerificationFilter::THROW_EXCEPTION
								| CryptoPP::SignatureVerificationFilter::PUT_MESSAGE));
		result = true;
		verifiedData = plainText;
	}
	catch (CryptoPP::Exception& ex) {
		std::cerr << "Caught exception during decryption: " << std::endl;
		std::cerr << ex.what() << std::endl;
	}
	return result;
}

bool ProvenanceHelper::verifyDataSignature(std::string dataToDecrypt,
		std::string& verifiedData) {
	std::string plainText;
	bool result = false;

	try {
		CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::SHA256>::Verifier verifier(
				publicKey);

		CryptoPP::StringSource(dataToDecrypt, true,
				new CryptoPP::SignatureVerificationFilter(verifier,
						new CryptoPP::StringSink(plainText),
						CryptoPP::SignatureVerificationFilter::THROW_EXCEPTION
								| CryptoPP::SignatureVerificationFilter::PUT_MESSAGE));
		result = true;

		verifiedData = plainText;
	} catch (CryptoPP::Exception& ex) {
		std::cerr << "Caught exception during decryption: " << std::endl;
		std::cerr << ex.what() << std::endl;
	}

	return result;

}

std::string ProvenanceHelper::decryptWithRSA(std::string dataToDecrypt) {
	std::string plainText;

	try {
		CryptoPP::RSAES_OAEP_SHA_Decryptor d(privateKey);

		CryptoPP::StringSource(dataToDecrypt, true,
				new CryptoPP::PK_DecryptorFilter(randomPool, d,
						new CryptoPP::StringSink(plainText)) // PK_EncryptorFilter
								);// StringSource
	}
	catch (CryptoPP::Exception& ex) {
		std::cerr << "Caught exception during decryption: " << std::endl;
		std::cerr << ex.what() << std::endl;
	}
	return plainText;
}

std::string ProvenanceHelper::decryptWithRSA(std::string dataToDecrypt,
		std::string privateKeyToUse, int privateKeySize) {
	std::string plainText;

	try {
		CryptoPP::RSA::PrivateKey tmpPrivateKey;

		loadPrivateKey(tmpPrivateKey, privateKeyToUse, privateKeySize);

		CryptoPP::RSAES_OAEP_SHA_Decryptor d(tmpPrivateKey);

		CryptoPP::StringSource(dataToDecrypt, true,
				new CryptoPP::PK_DecryptorFilter(randomPool, d,
						new CryptoPP::StringSink(plainText)) // PK_EncryptorFilter
								);// StringSource
	} catch (CryptoPP::Exception& ex) {
		std::cerr << "Caught exception during decryption: " << std::endl;
		std::cerr << ex.what() << std::endl;
	}

	return plainText;
}

std::string ProvenanceHelper::decryptData(std::string dataToDecrypt,
		std::string aesKey, int aesKeySize) {
	std::string plainText;

	try {
		CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption d;
		d.SetKey((const byte*) base64Decode(aesKey).data(), aesKeySize);

		// The StreamTransformationFilter removes
		//  padding as required.
		CryptoPP::StringSource s(dataToDecrypt, true,
				new CryptoPP::StreamTransformationFilter(d,
						new CryptoPP::StringSink(plainText)) // StreamTransformationFilter
								);// StringSource
	}
	catch (CryptoPP::Exception& ex) {
		std::cerr << "Caught exception during decryption: " << std::endl;
		std::cerr << ex.what() << std::endl;
	}
	return plainText;
}

std::string ProvenanceHelper::decryptData(std::string dataToDecrypt) {
	return decryptData(dataToDecrypt, cryptInfo.getSessionKey().getData(),
			cryptInfo.getSessionKey().getSize());
}

std::string ProvenanceHelper::decryptData(Key key, std::string dataToDecrypt) {
	return decryptData(dataToDecrypt, key.getData(), key.getSize());
}

std::string ProvenanceHelper::base64Encode(std::string data) {
	CryptoPP::Base64Encoder encoder;
	std::string encoded;

	encoder.Attach(new CryptoPP::StringSink(encoded));
	encoder.Put((const byte*) data.data(), data.size());
	encoder.MessageEnd();
	return encoded;
}

std::string ProvenanceHelper::base64Decode(std::string data) {
	CryptoPP::Base64Decoder decoder;
	std::string decoded;
	
	decoder.Attach(new CryptoPP::StringSink(decoded));
	decoder.Put((const byte*) data.data(), data.size());
	decoder.MessageEnd();	
	return decoded;
}

std::string ProvenanceHelper::generateHash(const byte* address, int length) {
	CryptoPP::SHA256 hash;
	byte digest[CryptoPP::SHA256::DIGESTSIZE];
	hash.CalculateDigest(digest, address, length);

	CryptoPP::HexEncoder encoder;
	std::string itemHash;
	encoder.Attach(new CryptoPP::StringSink(itemHash));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return itemHash;
}

std::string ProvenanceHelper::generateHash(std::string itemToHash) {
	return generateHash((const byte*) itemToHash.data(), itemToHash.length());
}

std::string ProvenanceHelper::generateHash(std::vector<std::string>& itemsToHash) {
	CryptoPP::SHA256 hash;
	byte digest[CryptoPP::SHA256::DIGESTSIZE];

	for (size_t index = 0; index < itemsToHash.size(); index++) {
		hash.Update((const byte*) itemsToHash[index].data(),
				itemsToHash[index].length());
	}

	hash.Final(digest);

	std::string itemsHash;
	CryptoPP::HexEncoder encoder;
	encoder.Attach(new CryptoPP::StringSink(itemsHash));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();
	return itemsHash;
}

void Encode(const std::string& filename,
		const CryptoPP::BufferedTransformation& bt) {
	CryptoPP::FileSink file(filename.c_str());
	bt.CopyTo(file);
	file.MessageEnd();
}

void EncodePublicKey(const std::string& filename,
		const CryptoPP::RSA::PublicKey& key) {
	CryptoPP::ByteQueue queue;
	key.Save(queue);
	Encode(filename, queue);
}

void EncodePrivateKey(const std::string& filename,
		const CryptoPP::RSA::PrivateKey& key) {
	CryptoPP::ByteQueue queue;
	key.Save(queue);
	Encode(filename, queue);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
