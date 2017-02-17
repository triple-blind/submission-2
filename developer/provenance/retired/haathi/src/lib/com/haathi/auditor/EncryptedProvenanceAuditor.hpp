// $Header: //MyDataDepot/Projects/provenance/haathi/src/lib/com/haathi/auditor/EncryptedProvenanceAuditor.hpp#3 $

#ifndef EncryptedProvenanceAuditor_HPP
#define EncryptedProvenanceAuditor_HPP
/*!
 
 @header EncryptedProvenanceAuditor.hpp
 
 @author Aaron Gibson
 @date April 1, 2012
 
 This header implements an auditor that will decrypt the provenance events as well as display 
 them in the document.
 */

// UNO includes
// In com::sun::star::lang
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
// In com::sun::star::uno
#include <com/sun/star/uno/Sequence.hxx>

// My includes
#include "com/haathi/auditor/AuditorDialog.hpp"
#include "com/haathi/crypto/CryptoStructures.hpp"
#include "com/haathi/crypto/ProvenanceEntry.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include <osl/thread.hxx>

using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

namespace com {
namespace haathi {
namespace auditor {

class EncryptedProvenanceAuditor: protected osl::Thread {
private:
	Reference<XMultiServiceFactory> xMSF;
	Reference<XComponent> xComponent;

	com::haathi::crypto::CryptoInfo keys;
	com::haathi::document::DocumentMetaInfo meta_info;

	std::vector<com::haathi::crypto::ProvenanceEntry> provenance_chain;
	std::vector<com::haathi::provenance::ProvenanceEvent*> events;

	std::map<std::string, com::sun::star::util::Color> user_color_map;

	AuditorDialog* dialog;
	sal_Int32 event_index;
	bool eventLoaded;

	bool verify_hash;
	bool evolution_hash;

	bool validateIntegrityOfEntry(sal_Int32 entryIndex);
	bool validateEvolutionOfEntry(sal_Int32 entryIndex);

	void decryptProvenanceEvents();
protected:
	// Much of the "big" stuff is already handled in the base class.
	// We only have to implement the details that are specific to handling provenance.
	virtual bool loadCurrentEvent() throw (com::sun::star::uno::Exception,
			std::exception);
	virtual bool unloadCurrentEvent() throw (com::sun::star::uno::Exception,
			std::exception);

	/*!
	 This function will validate the evolution of the current provenance event in
	 the chain. It will return true if the hash of the document matches the hash of
	 stored in the provenance chain. It will return false otherwise, indicating a
	 potential tampering.
	 */
	bool validateEvolution(bool load);
	virtual void SAL_CALL run();

public:
	EncryptedProvenanceAuditor(
			Reference<XComponentContext> xContext, Reference<XComponent> _xComponent,
			Reference<XMultiServiceFactory> _xMSF, std::string raw_provenance,
			com::haathi::document::DocumentMetaInfo info);
	~EncryptedProvenanceAuditor();

	Sequence<OUString> getEventList();

	void updateDialog();

	bool loadNextEvent();
	bool loadPreviousEvent();
	void loadEventAt(sal_Int32 index);
	/*!
	 This function will validate the integrity of the current provenance chain.
	 It will return an integer indicating the index of the last valid event.
	 */
	int validateIntegrity();

	int getCurrentIndex() const {
		return event_index;
	}
	static void auditProvenanceFromStream(Reference<XComponentContext> xContext,
			std::string document_stream,
			std::string provenance,
			com::haathi::document::DocumentMetaInfo info);
};
// class EncryptedProvenanceAuditor

}
;
}
;
}
;
// namespace com::haathi::auditor

#endif

