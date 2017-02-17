/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/auditor/EncryptedProvenanceAuditor.hpp#1 $

/*!
 
 @header EncryptedProvenanceAuditor.hpp
 
 @author Aaron Gibson
 @date April 1, 2012
 
 This header implements an auditor that will decrypt the provenance events as well as display 
 them in the document.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__AUDITOR__ENCRYPTED_PROVENANCE_AUDITOR_HPP
#	define COM__HAATHI__AUDITOR__ENCRYPTED_PROVENANCE_AUDITOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/crypto/ProvenanceEntry.hpp"
#include "com/haathi/dialog/MessageBox.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"
#include "com/haathi/text/TextHelper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/Color.hpp>
#include "sdkEnd.hpp"

#include <map>
#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace auditor {
/******************************************************************************
Class
******************************************************************************/
class EventSummary {
	protected:
		static const rtl::OUString trueMark;
		static const rtl::OUString falseMark;
		static const rtl::OUString questionMark;
		static const rtl::OUString blank;
		static const rtl::OUString empty;

		void initialize(bool auditor, bool decoded, bool rsaDecrypted,
				bool plainDecrypted, bool deserialized, com::haathi::provenance::ProvenanceEvent* provenanceEvent);

	public:
		bool auditor;
		bool decoded;
		bool rsaDecrypted;
		bool plainDecrypted;
		bool deserialized;

		com::haathi::provenance::ProvenanceEvent* provenanceEvent;
		com::sun::star::util::Color color;

		bool integrity;
		bool forwardEvent;
		bool forwardDocument;
		bool backwardEvent;
		bool backwardDocument;

		EventSummary();
		EventSummary(bool auditor);
		EventSummary(bool auditor, bool decoded);
		EventSummary(bool auditor, bool decoded, bool rsaDecrypted);
		EventSummary(bool auditor, bool decoded, bool rsaDecrypted,
				bool plainDecrypted);
		EventSummary(bool auditor, bool decoded, bool rsaDecrypted,
				bool plainDecrypted, bool deserialized);
		EventSummary(bool auditor, bool decoded, bool rsaDecrypted,
				bool plainDecrypted, bool deserialized, com::haathi::provenance::ProvenanceEvent* provenanceEvent);

		void setColor(com::sun::star::util::Color color);
		void setIntegrity(bool integrity);
		void setForwardEvent(bool event);
		void setForwardDocument(bool document);
		void setBackwardEvent(bool event);
		void setBackwardDocument(bool document);
		OUString toOUString();

		OUString getAuditorStatus();
		OUString getIntegrityStatus();

		OUString getForwardEventStatus();
		OUString getForwardDocumentStatus();

		OUString getBackwardEventStatus();
		OUString getBackwardDocumentStatus();
};

class EncryptedProvenanceAuditor {
	protected:
		Reference<com::sun::star::lang::XComponent> xComponent;
		com::haathi::text::TextHelper textHelper;
		com::haathi::crypto::CryptoInfo keys;
		com::haathi::document::DocumentMetaInfo documentMetaInfo;
		com::haathi::dialog::MessageBoxProvider* messageBoxProvider;

		sal_Int32 eventIndex;
		bool highlighted;
		int colorIndex;

		std::vector<com::haathi::crypto::ProvenanceEntry*> provenanceChain;
		std::vector<com::haathi::provenance::ProvenanceEvent*> provenanceEvents;
		std::map<std::string, com::sun::star::util::Color> userColorMap;
		std::vector<EventSummary> eventSummaries;

		void fillProvenanceChain(cJSON* decryptedChain);
		void fillProvenanceEvents();
		void fillUserColorMap();
		void fillEventSummaries();

		com::sun::star::util::Color findColor(sal_Int32 index);
		com::sun::star::util::Color nextColor();

		bool isEventIndexValid(int eventIndex);
		bool isProvenanceEventValid();

		bool validateIntegrityOfEntry(sal_Int32 index);
		bool validateForwardEvolutionOfEntry(sal_Int32 index);
		bool validateBackwardEvolutionOfEntry(sal_Int32 index);

		virtual bool redoCurrentEvent(bool highlight) throw (com::sun::star::uno::Exception, std::exception);
		virtual bool undoCurrentEvent(bool highlight) throw (com::sun::star::uno::Exception, std::exception);

	public:
		static Reference<com::sun::star::lang::XComponent> createProvenanceFromStream(Reference<com::sun::star::uno::XComponentContext> xContext,
				std::string document, std::string title);

		EncryptedProvenanceAuditor(Reference<com::sun::star::lang::XComponent> xComponent, std::vector<std::string>& provenances,
				com::haathi::document::DocumentMetaInfo documentMetaInfo, com::haathi::dialog::MessageBoxProvider* messageBoxProvider);
		virtual ~EncryptedProvenanceAuditor();

		void loadEventAt(sal_Int32 index);
		std::vector<EventSummary>* getEventSummaries();
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
