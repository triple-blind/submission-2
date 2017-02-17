/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/Encryptor.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/crypto/Encryptor.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::document::DocumentMetaInfo;
using com::haathi::provenance::ProvenanceEventHelper;
using com::haathi::thread::EncryptionThread;
using com::haathi::thread::SynchronizedQueue;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace crypto {
/******************************************************************************
Class
******************************************************************************/
Encryptor::Encryptor(com::haathi::document::DocumentMetaInfo& documentMetaInfo,
		std::vector<std::string>& provenanceChain):
		encryptionThread(documentMetaInfo, synchronizedQueue, provenanceChain) {
}

Encryptor::~Encryptor() {
}

void Encryptor::encrypt(ProvenanceEventHelper provenanceEventHelper) {
	synchronizedQueue.enqueue(provenanceEventHelper);
}

void Encryptor::reset(com::haathi::document::DocumentMetaInfo& documentMetaInfo) {
	wait();
	encryptionThread.resetMetaInfo(documentMetaInfo);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
