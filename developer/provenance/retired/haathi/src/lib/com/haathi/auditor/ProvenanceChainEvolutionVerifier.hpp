/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/auditor/ProvenanceChainEvolutionVerifier.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__CRYPTO__PROVENANCE_CHAIN_EVOLUTION_VERIFIER_HPP
#	define COM__HAATHI__CRYPTO__PROVENANCE_CHAIN_EVOLUTION_VERIFIER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoStructures.hpp"
#include "com/haathi/crypto/ProvenanceEntry.hpp"

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
class ProvenanceChainEvolutionVerifier {
	private:
		std::vector<ProvenanceEntry> provenance_chain;
		int currentVerificationIndex;
		int finalVerificationIndex;
		int auditorID;
		std::string finalDocument;
		CryptoInfo keyInfo;
	public:
		ProvenanceChainEvolutionVerifier(std::vector<ProvenanceEntry> chain,
				int userIDOfAuditor, CryptoInfo cryptoInfo,
				std::string finalDocumentContents);
		bool IsEndOfChain();
		bool IsAuthorizedToAudit();
		bool IsCurrentEntryValid();
		bool ValidateProvenanceEntryAt(int entryIndex);
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
