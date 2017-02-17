/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/auditor/ProvenanceChainIntegrityVerifier.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__CRYPTO__PROVENANCE_CHAIN_INTEGRITY_VERIFIER_HPP
#	define COM__HAATHI__CRYPTO__PROVENANCE_CHAIN_INTEGRITY_VERIFIER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/ProvenanceEntry.hpp"

#include <string>
#include <vector>
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
class ProvenanceChainIntegrityVerifier {
	private:
		std::vector<ProvenanceEntry> provenance_chain;
		int currentVerificationIndex;
		int finalVerificationIndex;

	public:
		ProvenanceChainIntegrityVerifier(std::vector<ProvenanceEntry> chain);
		bool IsEndOfChain();
		bool IsCurrentEntryValid();
		bool ValidateProvenanceEntryAt(int entryIndex);
		bool IsValidDocumentHash(std::string finalDocumentContents);
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
