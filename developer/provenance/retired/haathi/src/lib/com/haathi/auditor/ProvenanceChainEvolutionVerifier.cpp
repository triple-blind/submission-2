/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/auditor/ProvenanceChainEvolutionVerifier.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/crypto/ProvenanceChainEvolutionVerifier.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/crypto/CryptoStructures.hpp"
#include "com/haathi/crypto/ProvenanceHelper.hpp"

#include "cppJSON.hpp"

#include <map>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace crypto {
/******************************************************************************
Class or Content
******************************************************************************/
ProvenanceChainEvolutionVerifier::ProvenanceChainEvolutionVerifier(
		std::vector<ProvenanceEntry> chain, int userIDOfAuditor,
		CryptoInfo cryptoInfo, std::string finalDocumentContents) {
	provenance_chain = chain;
	currentVerificationIndex = chain.size() - 1;
	finalVerificationIndex = -1;
	auditorID = userIDOfAuditor;
	finalDocument = finalDocumentContents;
	keyInfo = cryptoInfo;
}

bool ProvenanceChainEvolutionVerifier::IsEndOfChain() {
	return currentVerificationIndex == finalVerificationIndex;
}

bool ProvenanceChainEvolutionVerifier::ValidateProvenanceEntryAt(
		int entryIndex) {
	bool isValid = false;
	ProvenanceEntry entry = provenance_chain[entryIndex];
	ProvenanceHelper auditor_helper;
	bool found;
	std::map<int, CryptoInfo>::iterator it = entry.findKeyingMaterial(auditorID, &found);

	if (found) {
		std::string sessionKey = auditor_helper.DecryptWithRSA(
				auditor_helper.Base64Decode(it->second.sessionKey),
				keyInfo.privateKey, keyInfo.privateKeySize);
		std::string actualEventJSONString = auditor_helper.DecryptData(
				auditor_helper.Base64Decode(entry.getEncryptedEvent()), sessionKey,
				it->second.sessionKeySize);

		EventInfo event;
		event.parseJSONString(actualEventJSONString.c_str());
		std::string eventData = event.getEvent();

		if (event.getEventType() == Insert) {
			// Compute D_(n-1)
			int modificationIndex = finalDocument.size() - 1;
			finalDocument.erase(modificationIndex);

			// Verify the hash of D_(n-1) with the hash in previous provenance event

			std::string currentHash = auditor_helper.GenerateHash(finalDocument);
			std::string existingHash = "";

			if (entryIndex > 0) {
				ProvenanceEntry previousEntry = provenance_chain[entryIndex - 1];
				existingHash = previousEntry.getDocumentHash();
			} else {
				existingHash = auditor_helper.GenerateHash(existingHash);
			}

			if (currentHash == existingHash)
				isValid = true;

		} else if (event.getEventType() == Remove) {
			// Compute D_(n-1)
			finalDocument.append(eventData);

			// Verify the hash of D_(n-1) with the hash in previous provenance event

			std::string currentHash = auditor_helper.GenerateHash(finalDocument);
			std::string existingHash = "";

			if (entryIndex > 0) {
				ProvenanceEntry previousEntry =
						provenance_chain[entryIndex - 1];
				existingHash = previousEntry.getDocumentHash();
			} else {
				existingHash = auditor_helper.GenerateHash(existingHash);
			}

			if (currentHash == existingHash)
				isValid = true;
		} else {
			// unknown event type
		}
	}

	return isValid;

}

bool ProvenanceChainEvolutionVerifier::IsCurrentEntryValid() {
	bool isValid = ValidateProvenanceEntryAt(currentVerificationIndex);
	currentVerificationIndex--;
	return isValid;
}

bool ProvenanceChainEvolutionVerifier::IsAuthorizedToAudit() {
	ProvenanceEntry entry = provenance_chain[currentVerificationIndex];
	bool isValid;

	entry.getKeyInfo().findKeyingMaterial(auditorID, &isValid);
	return isValid;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
