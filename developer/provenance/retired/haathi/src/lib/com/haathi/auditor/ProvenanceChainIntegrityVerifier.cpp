/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/auditor/ProvenanceChainIntegrityVerifier.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/crypto/ProvenanceChainIntegrityVerifier.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/crypto/ProvenanceHelper.hpp"

#include "cppJSON.hpp"
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
ProvenanceChainIntegrityVerifier::ProvenanceChainIntegrityVerifier(
		std::vector<ProvenanceEntry> chain) {
	provenance_chain = chain;
	currentVerificationIndex = 0;
	finalVerificationIndex = chain.size();
}

bool ProvenanceChainIntegrityVerifier::IsEndOfChain() {
	return currentVerificationIndex == finalVerificationIndex;
}

bool ProvenanceChainIntegrityVerifier::ValidateProvenanceEntryAt(
		int entryIndex) {
	bool isValid = false;
	std::string verified_data;
	ProvenanceHelper integrity_helper;
	// First verify if the signature is correct
	ProvenanceEntry entry = provenance_chain[entryIndex];
	bool isValidSignature = integrity_helper.VerifyDataSignature(
			integrity_helper.Base64Decode(entry.getChecksum()), verified_data,
			(char*) entry.getPublicKey().c_str(), entry.getPublicKeySize());

	if (isValidSignature) {
		// recompute the hash corresponding to the checksum and compare it with the value in the entry
		std::vector<std::string> checksum_components;
		std::string prev_checksum = "";

		if (entryIndex > 0)
			prev_checksum = provenance_chain[entryIndex - 1].getChecksum();

		checksum_components.push_back(
				entry.getPrincipalIdentifier().getJSONString());

		checksum_components.push_back(entry.getEncryptedEvent());

		checksum_components.push_back(entry.getDocumentHash());

		checksum_components.push_back(entry.getPublicKey());

		checksum_components.push_back(entry.getKeyInfo().getJSONString());

		checksum_components.push_back(prev_checksum);

		std::string computed_hash = integrity_helper.GenerateHash(
				checksum_components);

		if (computed_hash == verified_data) {
			isValid = true;
		} else {
			isValid = false;
		}
	} else {
		isValid = false;
	}

	return isValid;
}

bool ProvenanceChainIntegrityVerifier::IsCurrentEntryValid() {
	bool isValid = ValidateProvenanceEntryAt(currentVerificationIndex);

	currentVerificationIndex++;

	return isValid;
}

bool ProvenanceChainIntegrityVerifier::IsValidDocumentHash(
		std::string finalDocumentContents) {
	bool result = false;
	ProvenanceHelper integrity_helper;
	std::string originalHash = provenance_chain[finalVerificationIndex - 1].getDocumentHash();
	std::string documentHash = integrity_helper.GenerateHash(finalDocumentContents);

	if (originalHash == documentHash)
		result = true;

	return result;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
