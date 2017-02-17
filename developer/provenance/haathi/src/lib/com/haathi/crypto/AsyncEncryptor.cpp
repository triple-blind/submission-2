/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/AsyncEncryptor.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/crypto/AsyncEncryptor.hpp"
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
AsyncEncryptor::AsyncEncryptor(com::haathi::document::DocumentMetaInfo& documentMetaInfo,
		std::vector<std::string>& provenanceChain):
		Encryptor(documentMetaInfo, provenanceChain) {
	encryptionThread.create();
}

AsyncEncryptor::~AsyncEncryptor() {
	// The NULL event will cause the encryptionThread to terminate.
	encrypt(ProvenanceEventHelper(NULL, std::string()));
	wait();
}

void AsyncEncryptor::wait() {
	while (!encryptionThread.isEncryptionComplete())
		osl::Thread::yield();
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
