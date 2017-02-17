/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/traitorTracer/com/haathi/traitor/GenericTraitorTracer.cpp#7 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/traitor/GenericTraitorTracer.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <cstdlib> // itoa

#ifdef _MSC_VER
#	define ITOA_S(value, buffer, sizeInCharacters, radix) _itoa_s(value, buffer, sizeInCharacters, radix)
#endif

#ifdef __GNUC__
#	define ITOA_S(value, buffer, sizeInCharacters, radix) sprintf(buffer, "%d", value)
#endif
/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace traitor {
/******************************************************************************
GenericTraitorTheory
******************************************************************************/
GenericTraitorTheory::GenericTraitorTheory(Theory* theory): decisionGenerator(0) {
	CryptoPP::SHA256 hasher;

	updateHasher(hasher, theory->userPrivateKey);
	updateHasher(hasher, theory->documentId);
	updateHasher(hasher, theory->userId);

	byte digest[CryptoPP::SHA256::DIGESTSIZE];
	hasher.Final(digest);

	byte kernelSeed[CryptoPP::AES::DEFAULT_KEYLENGTH];
	fillSeed(kernelSeed, theory->versionId, sizeof(kernelSeed));
	kernelGenerator.Reseed(kernelSeed, sizeof(kernelSeed), digest, digest);

	byte chaffSeed[CryptoPP::DES_EDE3::DEFAULT_KEYLENGTH];
	fillSeed(chaffSeed, theory->versionId, sizeof(chaffSeed));
	chaffGenerator.Reseed(chaffSeed, sizeof(chaffSeed), digest, digest);

	int hash = 0;
	int size = theory->versionId.size();
	for (int i = 0; i < size; i++)
		hash += theory->versionId[i];
	decisionGenerator = CryptoPP::LC_RNG(hash);
}

void GenericTraitorTheory::updateHasher(CryptoPP::SHA256& hasher, std::string& seed) {
	hasher.Update((byte*) seed.c_str(), seed.length());
}

void GenericTraitorTheory::generateBlock(unsigned char* block) {
	byte randomNumber = decisionGenerator.GenerateByte();
	bool chaff = 100 < randomNumber && randomNumber < 150;

	return chaff ? chaffGenerator.GenerateBlock(block, 8) : kernelGenerator.GenerateBlock(block, 8);
}

void GenericTraitorTheory::fillSeed(byte seed[], std::string& versionId, int blockSize) {
	int rounds = (versionId.length() + blockSize - 1) / blockSize;

	for (int i = 0; i < blockSize; i++) {
		seed[i] = 0;

		for (int j = 0; j < rounds; j++) {
			int pos = i + j * blockSize;
			seed[i] += pos < (int) versionId.length() ? versionId[pos] : pos;
		}
	}
}
/******************************************************************************
GenericTraitorTracer
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
