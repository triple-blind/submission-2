/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/crypto/Encryptor.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__CRYPTO__ENCRYPTOR_HPP
#	define COM__HAATHI__CRYPTO__ENCRYPTOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/provenance/ProvenanceEventHelper.hpp"
#include "com/haathi/thread/EncryptionThread.hpp"
#include "com/haathi/thread/SynchronizedQueue.hpp"

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
class Encryptor {
	protected:
		com::haathi::thread::SynchronizedQueue<com::haathi::provenance::ProvenanceEventHelper> synchronizedQueue;
		com::haathi::thread::EncryptionThread encryptionThread;

	public:
		Encryptor(com::haathi::document::DocumentMetaInfo& documentMetaInfo,
				std::vector<std::string>& provenanceChain);
		virtual ~Encryptor();

		virtual void wait() = 0;

		virtual void encrypt(com::haathi::provenance::ProvenanceEventHelper provenanceEventHelper);
		virtual void reset(com::haathi::document::DocumentMetaInfo& documentMetaInfo);
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
