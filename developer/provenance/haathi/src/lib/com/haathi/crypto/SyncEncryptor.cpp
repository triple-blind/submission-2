/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/SyncEncryptor.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/crypto/SyncEncryptor.hpp"
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
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace crypto {
/******************************************************************************
Class
******************************************************************************/
SyncEncryptor::SyncEncryptor(com::haathi::document::DocumentMetaInfo& documentMetaInfo,
		std::vector<std::string>& provenanceChain):
		Encryptor(documentMetaInfo, provenanceChain) {
}

SyncEncryptor::~SyncEncryptor() {
}

void SyncEncryptor::encrypt(ProvenanceEventHelper provenanceEventHelper) {
	synchronizedQueue.enqueue(provenanceEventHelper);
	// But encrypt it right away
	encryptionThread.encrypt();
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
