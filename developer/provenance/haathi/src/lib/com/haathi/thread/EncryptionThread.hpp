/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/thread/EncryptionThread.hpp#1 $

/*
 *  Created by Aaron Gibson on 3/17/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__THREAD__ENCRYPTION_THREAD_HPP
#	define COM__HAATHI__THREAD__ENCRYPTION_THREAD_HPP
/******************************************************************************
Include
******************************************************************************/
#include "Config.hpp"

#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/crypto/ProvenanceHelper.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"
#include "com/haathi/provenance/ProvenanceEventHelper.hpp"
#include "com/haathi/thread/SynchronizedQueue.hpp"

#include "sdkBeg.hpp"
#include <osl/mutex.hxx>
#include <osl/thread.hxx>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace thread {
/******************************************************************************
Class
******************************************************************************/
class EncryptionThread: public osl::Thread {
	protected:
		//osl::Mutex lock;
		SynchronizedQueue<com::haathi::provenance::ProvenanceEventHelper>& synchronizedQueue;
		std::vector<std::string>& provenanceChain;
		com::haathi::crypto::ProvenanceHelper provenanceHelper;
		std::vector<std::string> checksumHashComponents;
		std::string prevChecksum;
		int publicKeySize;

		virtual void SAL_CALL onTerminated();

	public:
		/*!
		 This constructor will create a thread that will encrypt the provenance passed into
		 the synchronizedQueue with the keys specified in "metaInfo". This thread will output the
		 encrypted provenance into the vector "provenanceChain".
		 
		 @param documentMetaInfo -- an instance of DocumentMetaInfo to handle the encryption keys.
		 @param synchronizedQueue -- a pointer to the "source" queue for provenance events.
		 @param provenanceChain -- a pointer to the "destination" queue for provenance events.
		 */
		EncryptionThread(com::haathi::document::DocumentMetaInfo& documentMetaInfo,
				SynchronizedQueue<com::haathi::provenance::ProvenanceEventHelper>& synchronizedQueue,
				std::vector<std::string>& provenanceChain);
		virtual ~EncryptionThread();

		virtual void SAL_CALL run();
		bool encrypt();
		bool isEncryptionComplete();
		void resetMetaInfo(com::haathi::document::DocumentMetaInfo& documentMetaInfo);
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


