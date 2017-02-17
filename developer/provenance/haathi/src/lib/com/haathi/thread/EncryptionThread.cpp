/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/thread/EncryptionThread.cpp#1 $
/*
 *  Created by Aaron Gibson on 3/17/12.
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/thread/EncryptionThread.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/crypto/KeyingMaterial.hpp"
#include "com/haathi/crypto/ProvenanceEntry.hpp"
#include "com/haathi/exploit/EditHook.hpp"
#include "com/haathi/exploit/OpenHook.hpp"
#include "com/haathi/exploit/SaveHook.hpp"
#include "com/haathi/exploit/SpyPrincipalIdentifier.hpp"

#include "com/haathi/utils/Guard.hpp"
#include "com/haathi/utils/Serialization.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/utils/Timer.hpp"

#include <memory>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::crypto::KeyingMaterial;
using com::haathi::crypto::ProvenanceEntry;
using com::haathi::document::DocumentMetaInfo;
using com::haathi::provenance::ProvenanceEvent;
using com::haathi::provenance::ProvenanceEventHelper;
using com::haathi::provenance::ProvenanceEventPtr;
using com::haathi::utils::guard;
using com::haathi::utils::HAATHI_OARCHIVE;
using com::haathi::utils::TimeKeeper;
using com::haathi::utils::encryptorTimer;
using com::haathi::utils::Timer;

using namespace com::haathi::exploit;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace thread {
/******************************************************************************
Class
******************************************************************************/
enum ChecksumIndex {
	PRINCIPAL_IDENTIFIER   = 0,
	ENCRYPTED_EVENT        = 1,
	DOCUMENT_HASH          = 2,
	PUBLIC_KEY             = 3,
	KEYING_MATERIAL_STRING = 4,

	CHECKSUM_INDEX_SIZE    = 5
};

EncryptionThread::EncryptionThread(DocumentMetaInfo& documentMetaInfo,
		SynchronizedQueue<ProvenanceEventHelper>& synchronizedQueue,
		std::vector<std::string>& provenanceChain):
		synchronizedQueue(synchronizedQueue), provenanceChain(provenanceChain),
		provenanceHelper(documentMetaInfo.provenanceEncryption) {
	checksumHashComponents = std::vector<std::string>(CHECKSUM_INDEX_SIZE);

	std::string keyingMaterialString = KeyingMaterial::asJSONString(documentMetaInfo.auditorList,
			documentMetaInfo.provenanceEncryption.getSessionKey());

#if EXPLOIT == EXPLOIT_SPY_PRINCIPAL_IDENTIFIER
	// Checksum, so has to match somewhere.  See provenanceEvent->setUsername below.
	spyPrincipalIdentifier.captureChecksumHashComponents(&checksumHashComponents, PRINCIPAL_IDENTIFIER);
	spyPrincipalIdentifier.hack();
#elif EXPLOIT == EXPLOIT_OPEN_HOOK
	openHook.captureChecksumHashComponents(&checksumHashComponents, PRINCIPAL_IDENTIFIER);
	checksumHashComponents[PRINCIPAL_IDENTIFIER] = documentMetaInfo.userInfo.username;
#elif EXPLOIT == EXPLOIT_SAVE_HOOK
	saveHook.captureChecksumHashComponents(&checksumHashComponents, PRINCIPAL_IDENTIFIER);
	checksumHashComponents[PRINCIPAL_IDENTIFIER] = documentMetaInfo.userInfo.username;
#elif EXPLOIT == EXPLOIT_EDIT_HOOK
	editHook.captureChecksumHashComponents(&checksumHashComponents, PRINCIPAL_IDENTIFIER);
	checksumHashComponents[PRINCIPAL_IDENTIFIER] = documentMetaInfo.userInfo.username;
#else
	checksumHashComponents[PRINCIPAL_IDENTIFIER] = documentMetaInfo.userInfo.username;
#endif
	checksumHashComponents[PUBLIC_KEY] = documentMetaInfo.provenanceEncryption.publicKey;
	checksumHashComponents[KEYING_MATERIAL_STRING] = keyingMaterialString;

	if (!documentMetaInfo.previousSavedEvent.empty()) {
		ProvenanceEntry provenanceEntry(documentMetaInfo.previousSavedEvent.c_str());

		prevChecksum = provenanceEntry.getChecksum();
	}
	else
		prevChecksum = "";
	publicKeySize = documentMetaInfo.provenanceEncryption.publicKeySize;

	guard(&EncryptionThread::run, 3250, 0, __FILE__, __LINE__);
}

void SAL_CALL EncryptionThread::onTerminated() {
//	delete this; // Wrong because the destructor is (now) called on its own
}

EncryptionThread::~EncryptionThread() {
}

bool EncryptionThread::isEncryptionComplete() {
	return synchronizedQueue.empty();
}

void EncryptionThread::resetMetaInfo(DocumentMetaInfo& documentMetaInfo) {
	// Presently the calling code is always waiting for the
	// input queue to be empty before changing the meta info.
	// It should not be a problem that run is using meta info,
	// since it should be blocked with an empty queue.
	// osl::MutexGuard mutexGuard(lock);

	std::string keyingMaterialString = KeyingMaterial::asJSONString(documentMetaInfo.auditorList,
			documentMetaInfo.provenanceEncryption.getSessionKey());

	checksumHashComponents[KEYING_MATERIAL_STRING] = keyingMaterialString;
}

std::string serialize(ProvenanceEvent *provenanceEvent) {
	// We change the scope to force boost's destructor to 
	// run before continuing.  Otherwise serialization is incomplete!
	std::stringstream tempStream;
	{
		HAATHI_OARCHIVE oa(tempStream);
		oa & HAATHI_SERIALIZATION_NVP("ev", provenanceEvent);
	}
	std::string encryptedEvent = tempStream.str();
	return encryptedEvent;
}

inline ProvenanceEvent* get(ProvenanceEventPtr provenanceEventPtr) {
#if SHARED_EVENT_PTR == SHARED_EVENT_PTR_OFF
		return provenanceEventPtr;
#else
		// eclipse doesn't like this even though the compiler is fine with it!
		return provenanceEventPtr.get();
#endif
}

bool EncryptionThread::encrypt() {
	TimeKeeper timeKeeper(encryptorTimer);

	try {
		// If it is dequeued now, a save can start before the
		// save event has been added to the Provenance chain far below.
		// Therefore, pass in false so the event is not yet popped.
		ProvenanceEventHelper provenanceEventHelper = synchronizedQueue.dequeue(false);
		ProvenanceEventPtr provenanceEventPtr = provenanceEventHelper.getProvenanceEvent();
		ProvenanceEvent* provenanceEvent = get(provenanceEventPtr);

		if (provenanceEvent == NULL) {
			synchronizedQueue.pop();
			return false;
		}
		provenanceEvent->setUsername(checksumHashComponents[PRINCIPAL_IDENTIFIER]);
		std::string encryptedEvent = serialize(provenanceEvent);
#if SHARED_EVENT_PTR == SHARED_EVENT_PTR_OFF
		// Note where this happens!
		delete provenanceEvent; // This is approved for this configuration
#endif

		encryptedEvent = provenanceHelper.encryptData(encryptedEvent);
		encryptedEvent = provenanceHelper.base64Encode(encryptedEvent);
		checksumHashComponents[ENCRYPTED_EVENT] = encryptedEvent;

		std::string documentHash = provenanceEventHelper.getDocumentState();
		documentHash = provenanceHelper.generateHash(documentHash);
		checksumHashComponents[DOCUMENT_HASH] = documentHash;

		std::string checksum = provenanceHelper.generateHash(checksumHashComponents);
		checksum.append(prevChecksum);
		checksum = provenanceHelper.generateHash(checksum);
		checksum = provenanceHelper.signData(checksum);
		checksum = provenanceHelper.base64Encode(checksum);

		std::string provenanceEntryString = ProvenanceEntry::asJSONString(
				checksumHashComponents[PRINCIPAL_IDENTIFIER],
				checksumHashComponents[ENCRYPTED_EVENT],
				checksumHashComponents[DOCUMENT_HASH],
				checksumHashComponents[PUBLIC_KEY],
				checksumHashComponents[KEYING_MATERIAL_STRING],
				prevChecksum = checksum,
				publicKeySize);
		provenanceChain.push_back(provenanceEntryString);
		// This pop could enable a save to start now, which is OK.
		synchronizedQueue.pop();
		// After encrypting one event, we let the other processes run, if they can.
		// This prevents this thread from sucking up most of processor time allocated,
		// which can make the extension very... difficult to use.
		yield();
	}
	catch (CryptoPP::Exception & e1) {
		std::cout << "CryptoPP Exception: " << e1.what() << std::endl;
	}
	catch (std::exception & e2) {
		std::cout << "std::exception: " << e2.what() << std::endl;
	}
	catch (com::sun::star::uno::Exception & e3) {
		std::cout << "Uno Exception: " << e3.Message << std::endl;
	}
	catch (...) {
		std::cout << "I have no idea what happened." << std::endl;
	}
	return true;
}

void EncryptionThread::run() {
	while (true)
		if (!encrypt())
			return;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
