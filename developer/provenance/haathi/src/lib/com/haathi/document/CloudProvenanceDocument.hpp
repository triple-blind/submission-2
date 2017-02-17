/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/CloudProvenanceDocument.hpp#9 $

/*!
 
 @header CloudProvenanceDocument.hpp
 
 @author Aaron Gibson
 @date March 23, 2012
 
 This header implements the instance of ProvenanceDocument that will actually output 
 the generated provenance to the cloud after first encrypting it.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__CLOUD_PROVENANCE_DOCUMENT_HPP
#	define COM__HAATHI__DOCUMENT__CLOUD_PROVENANCE_DOCUMENT_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/document/TextDocumentListener.hpp"
#include "com/haathi/menu/DocumentMenu.hpp"
#include "com/haathi/menu/SubCommand.hpp"
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/text/TextHelper.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/task/XStatusIndicator.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include "sdkEnd.hpp"

#include "boostBeg.hpp"
#include <boost/shared_ptr.hpp>
#include "boostEnd.hpp"

#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace crypto {
			class Encryptor;
		};
		namespace watermark {
			namespace engine {
				class IWatermarkEngine;
			};
		};
	};
};
	
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
WatermarkEngineConfig
******************************************************************************/
class WatermarkEngineConfig {
	public:
		virtual com::haathi::watermark::engine::IWatermarkEngine* getWatermarkEngine() = 0;
		virtual com::haathi::document::IDocumentStateTracker* getDocumentStateTracker() = 0;
		virtual ~WatermarkEngineConfig() { };
};
/******************************************************************************
ExploitReleaser
******************************************************************************/
class ExploitReleaser {
	public:
		ExploitReleaser();
		virtual ~ExploitReleaser();
};
/******************************************************************************
CloudProvenanceDocument
******************************************************************************/
class CloudProvenanceDocument: public ProvenanceDocument {
	private:
		std::vector<std::string> provenanceChain;
		boost::shared_ptr<com::haathi::crypto::Encryptor> encryptor;
		//com::haathi::crypto::Encryptor* encryptor;

	protected:
		com::haathi::document::DocumentMetaInfo documentMetaInfo;
		com::haathi::network::NetworkInfo& networkInfo;

		boost::shared_ptr<WatermarkEngineConfig> watermarkEngineConfig;
		com::haathi::document::IDocumentStateTracker* documentStateTracker;
		boost::shared_ptr<com::haathi::document::TextDocumentListener> textDocumentListener;
		com::haathi::watermark::engine::IWatermarkEngine* watermarkEngine;
		ExploitReleaser exploitReleaser;

		com::haathi::text::TextHelper textHelper;
		Reference<com::sun::star::util::XModifiable> xModifiable;

		com::haathi::crypto::Encryptor* newEncryptor(
				com::haathi::document::DocumentMetaInfo& documentMetaInfo,
				std::vector<std::string>& provenanceChain);
		WatermarkEngineConfig* newWatermarkEngineConfig(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				com::haathi::document::DocumentHistory& documentHistory, int schemes, int vis);
		com::haathi::document::TextDocumentListener* newTextDocumentListener(Reference<com::sun::star::uno::XComponentContext> xComponentContext);

		void rewatermarkDocument();
		void unwatermarkDocument();

		bool isModified();
		void setModified(bool modified);

		bool provenanceOn;
		bool watermarkOn;
		bool interactive;

		std::string getProvenance();

		Reference<com::sun::star::task::XStatusIndicator> getXStatusIndicator();
		void watermarkEvent(com::haathi::provenance::ProvenanceEvent* currentProvenanceEvent, rtl::OUString documentState);

	public:
		// New with default schemes, interactive = true
		static CloudProvenanceDocument* newCloudProvenanceDocument(
				Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				com::haathi::document::DocumentMetaInfo documentMetaInfo,
				com::haathi::network::NetworkInfo& networkInfo,
				std::vector<std::string> userEmails);

		// New with runtime schemes, interactive specified
		static CloudProvenanceDocument* newCloudProvenanceDocument(
				Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				com::haathi::document::DocumentMetaInfo documentMetaInfo,
				com::haathi::network::NetworkInfo& networkInfo,
				std::vector<std::string> userEmails, bool provenanceOn, bool watermarkOn, int schemes, int vis, bool interactive);

		// New with runtime schemes and full autotype support, interactive = false
		static CloudProvenanceDocument* newCloudProvenanceDocument(
				Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				com::haathi::document::DocumentMetaInfo documentMetaInfo,
				com::haathi::network::NetworkInfo& networkInfo,
				std::vector<std::string> userEmails, bool provenanceOn, bool watermarkOn, int schemes, int vis,
				std::string pathToInputFile, std::string pathToLogFile);

		// Load with default schemes, interactive = true
		static CloudProvenanceDocument* newCloudProvenanceDocument(
				Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				com::haathi::document::DocumentMetaInfo documentMetaInfo,
				com::haathi::network::NetworkInfo& networkInfo, std::string document); 

		// Load with runtime schemes, interactive specified
		static CloudProvenanceDocument* newCloudProvenanceDocument(
				Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				com::haathi::document::DocumentMetaInfo documentMetaInfo,
				com::haathi::network::NetworkInfo& networkInfo,
				std::string document, bool provenanceOn, bool watermarkOn, int schemes, int vis, bool interactive,
				com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue> propertyValues = com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>());

		// Only allow creation through factory methods below (except for testing like now)
		CloudProvenanceDocument(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent,
				com::haathi::document::DocumentMetaInfo documentMetaInfo,
				com::haathi::network::NetworkInfo& networkInfo, bool provenanceOn, bool watermarkOn,
				int schemes, int vis, bool interactive);
		virtual ~CloudProvenanceDocument();

		virtual bool addProvenanceEvent(com::haathi::provenance::ProvenanceEvent* provenanceEvent, bool modify = true);
		virtual bool addProvenanceEvent(com::haathi::provenance::ProvenanceEvent* provenanceEvent,
				rtl::OUString documentState);
		virtual bool addProvenanceEvents(std::vector<com::haathi::provenance::ProvenanceEvent*>& provenanceEvents,
				rtl::OUString documentState);
		// This is a new one that has document state as an std::string.
		virtual bool addProvenanceEvent(com::haathi::provenance::ProvenanceEvent* provenanceEvent,
				std::string documentState);

		virtual void save();
		virtual void save(OUString tempFileURL);
		virtual void saveAs();
		virtual void saveAsTemplate();
		virtual void menu(const com::sun::star::util::URL& url);
		virtual void close();

		virtual void copy(int subCommand);
		virtual void cut(int subCommand);
		virtual void paste(int subCommand);

		virtual void print(int subCommand);
		virtual void exportIt(int subCommand);
		virtual void send(int subCommand);

		virtual void SAL_CALL printJobEvent(const com::sun::star::view::PrintJobEvent& event)
			throw (com::sun::star::uno::RuntimeException);

		virtual void share();
		virtual void history();
		virtual void exploit();
		virtual void watermark();

		virtual std::string getWatermarkAbbrev(int watermarkId);
		virtual std::vector<int> getWatermarkBits(int watermarkId);
		virtual std::vector<int> getWatermarkBits(int watermarkId, Reference<com::sun::star::text::XTextRange> xTextRange);
		virtual com::haathi::watermark::utils::Watermark generateWatermark();

		void forceModified();
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

