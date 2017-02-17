/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/CloudProvenanceDocument.cpp#12 $

//
// Author: Aaron Gibson
// Date: March 23, 2012
//
// This file implements the class CloudProvenanceDocument that will send the collected provenance
// to the cloud on a save operation.
//
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/document/CloudProvenanceDocument.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "Config.hpp"

#include "com/haathi/crypto/AsyncEncryptor.hpp"
#include "com/haathi/crypto/SyncEncryptor.hpp"
#include "com/haathi/dialog/HistoryDialog.hpp"
#include "com/haathi/dialog/MessageBox.hpp"
#include "com/haathi/dialog/ShareDialog.hpp"
#include "com/haathi/document/DocumentHistory.hpp"
#include "com/haathi/document/DocumentStore.hpp"
#include "com/haathi/document/DocumentTracker.hpp"
#include "com/haathi/document/NaiveTextDocumentListener.hpp"
#include "com/haathi/document/SmartTextDocumentListener.hpp"

#include "com/haathi/exploit/DocDelete.hpp"
#include "com/haathi/exploit/DocInsert.hpp"
#include "com/haathi/exploit/EvtDelete.hpp"
#include "com/haathi/exploit/EvtInsert.hpp"
#include "com/haathi/exploit/MixDelete.hpp"
#include "com/haathi/exploit/MixInsert.hpp"

#include "com/haathi/exploit/SpyAudit.hpp"
#include "com/haathi/exploit/SpyCreate.hpp"
#include "com/haathi/exploit/SpyShare.hpp"

#include "com/haathi/exploit/EditPaste.hpp"
#include "com/haathi/exploit/HidePaste.hpp"
#include "com/haathi/exploit/SpacePaste.hpp"

#include "com/haathi/exploit/EditHook.hpp"
#include "com/haathi/exploit/OpenHook.hpp"
#include "com/haathi/exploit/SaveHook.hpp"

#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/extension/UpdateManager.hpp"
#include "com/haathi/menu/MenuConfig.hpp"
#include "com/haathi/network/ProvenanceServer.hpp"
#include "com/haathi/network/TrustedServer.hpp"

#include "com/haathi/provenance/document/CreateDocumentProvenance.hpp"
#include "com/haathi/provenance/document/ExportDocumentProvenance.hpp"
#include "com/haathi/provenance/document/ExtensionDocumentProvenance.hpp"
#include "com/haathi/provenance/document/MemHashDocumentProvenance.hpp"
#include "com/haathi/provenance/document/MenuDocumentProvenance.hpp"
#include "com/haathi/provenance/document/OpenDocumentProvenance.hpp"
#include "com/haathi/provenance/document/PrintDocumentProvenance.hpp"
#include "com/haathi/provenance/document/PrintJobDocumentProvenance.hpp"
#include "com/haathi/provenance/document/ProcessTableDocumentEvent.hpp"
#include "com/haathi/provenance/document/SaveDocumentProvenance.hpp"
#include "com/haathi/provenance/document/SendDocumentProvenance.hpp"
#include "com/haathi/provenance/document/ShareDocumentProvenance.hpp"
#include "com/haathi/provenance/document/WatermarkDocumentProvenance.hpp"

#include "com/haathi/provenance/text/CopyTextProvenance.hpp"
#include "com/haathi/provenance/text/CutTextProvenance.hpp"
#include "com/haathi/provenance/text/PasteTextProvenance.hpp"

#include "com/haathi/provenance/ProvenanceEventHelper.hpp"
#include "com/haathi/text/Selection.hpp"
#include "com/haathi/utils/AuthenticationCredentials.hpp"
#include "com/haathi/utils/BeforeAndAfter.hpp"
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/HaathiException.hpp"
#include "com/haathi/utils/Timer.hpp"
#include "com/haathi/watermark/embedder/AttributeEmbedder.hpp"
#include "com/haathi/watermark/embedder/StyleEmbedder.hpp"
#include "com/haathi/watermark/engine/BothwhereEngine.hpp"
#include "com/haathi/watermark/engine/EverywhereEngine.hpp"
#include "com/haathi/watermark/engine/NowhereEngine.hpp"
#include "com/haathi/watermark/engine/OffScreenEngine.hpp"
#include "com/haathi/watermark/engine/SomewhereEngine.hpp"
#include "com/haathi/watermark/engine/SynonymEngine.hpp"
#include "com/haathi/watermark/generator/GrainGenerator.hpp"
#include "com/haathi/watermark/scheme/WatermarkSchemes.hpp"
#include "com/haathi/watermark/scheme/SchemeConfig.hpp"

#include "com/haathi/document/CloudConfig.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/Ambiguous.hpp>
#include <com/sun/star/beans/Optional.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/deployment/XPackage.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/task/XAbortChannel.hpp>
#include <com/sun/star/task/XStatusIndicatorFactory.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/text/XTextRangeCompare.hpp>
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include "sdkEnd.hpp"

#include <fstream>
#include <vector>

#if OSYS == OSYS_WIN
#	include <Windows.h>
#	define WAITFORLOAD Sleep(200);
#else
#	define WAITFORLOAD
#endif
/******************************************************************************
Define
******************************************************************************/
static const char* DICTIONARY_FILENAME = "/dictionary.txt";
static const int SCHEMES = FG_LUMINANCE_WATERMARK | BG_LUMINANCE_WATERMARK;
//static const int SCHEMES = WORD_SPACING_WATERMARK;
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::crypto;
using namespace com::haathi::dialog;
using namespace com::haathi::document;
using namespace com::haathi::exploit;
using namespace com::haathi::extension;
using namespace com::haathi::menu;
using namespace com::haathi::network;
using namespace com::haathi::text;
using namespace com::haathi::provenance;
using namespace com::haathi::thread;
using namespace com::haathi::utils;
using namespace com::haathi::watermark::embedder;
using namespace com::haathi::watermark::engine;
using namespace com::haathi::watermark::generator;
using namespace com::haathi::watermark::scheme;
using namespace com::haathi::watermark::utils;

using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::deployment;
using namespace com::sun::star::document;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::style;
using namespace com::sun::star::task;
using namespace com::sun::star::text;
using namespace com::sun::star::ucb;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
using namespace com::sun::star::view;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
PushAndPopModified
******************************************************************************/
// Upon construction this notes whether the XComponent has been modified.
// When destructed, it checks again and if modified has been changed, it
// sets the XComponent back to how it started.
class PushAndPopModified: BeforeAndAfter {
	protected:
		Reference<XModifiable> xModifiable;
		sal_Bool modified;

		void before() {
			modified = xModifiable->isModified();
		}

		void after() {
			if (modified != xModifiable->isModified())
				xModifiable->setModified(modified);
		}

	public:
		PushAndPopModified(Reference<XModifiable> xModifiable):
				xModifiable(xModifiable) {
			before();
		}

		virtual ~PushAndPopModified() {
			after();
		}
};
/******************************************************************************
LockAndUnlockControllers
******************************************************************************/
class LockAndUnlockControllers: BeforeAndAfter {
	protected:
		Reference<XModel> xModel;

		void before(Reference<XComponent> xComponent) {
			xModel = Reference<XModel>(xComponent, UNO_QUERY);
			xModel->lockControllers();
		}

		void after() {
			// This seems to result in a selectionChanged
			xModel->unlockControllers();
		}

	public:
		LockAndUnlockControllers(Reference<XComponent> xComponent) {
			before(xComponent);
		}

		virtual ~LockAndUnlockControllers() {
			after();
		}
};
/******************************************************************************
IgnoreAndHeedEvents
******************************************************************************/
class IgnoreAndHeedEvents: BeforeAndAfter {
	protected:
		TextDocumentListener* textDocumentListener;

		void before() {
			textDocumentListener->listen(false);
		}

		void after() {
			textDocumentListener->listen(true);
		}

	public:
		IgnoreAndHeedEvents(TextDocumentListener* textDocumentListener): textDocumentListener(textDocumentListener) {
			before();
		}

		virtual ~IgnoreAndHeedEvents() {
			after();
		}
};
/******************************************************************************
BeforeAndAfterWatermarking
******************************************************************************/
class BeforeAndAfterWatermarking: BeforeAndAfter {
	protected:
		// Want to do this one first and then last because others, especially
		// unlocking, cause some events to be triggered
		IgnoreAndHeedEvents ignoreAndHeedEvents;
		PushAndPopModified pushAndPopModified;
		LockAndUnlockControllers lockAndUnlockControllers;

		void before() { }
		void after() { }

	public:
		BeforeAndAfterWatermarking(Reference<XComponent> xComponent, Reference<XModifiable> xModifiable, TextDocumentListener* textDocumentListener):
				ignoreAndHeedEvents(textDocumentListener), pushAndPopModified(xModifiable), lockAndUnlockControllers(xComponent) {
			before();
		}

		virtual ~BeforeAndAfterWatermarking() {
			after();
		}
};
/******************************************************************************
BeforeAndAfterSaving
******************************************************************************/
class BeforeAndAfterSaving: BeforeAndAfter {
	protected:
		IgnoreAndHeedEvents ignoreAndHeedEvents;
		LockAndUnlockControllers lockAndUnlockControllers;
		Reference<XModifiable> xModifiable;

		IWatermarkEngine* watermarkEngine;
		Reference<XStatusIndicator> xStatusIndicator;
		sal_Int32 length;

		// Want to do these two without user seeing the intermediate document
		// that is temporarily unwatermarked
		void before() {
			// Modified isn't affected by this
			PushAndPopModified pushAndPopModified(xModifiable);
			if (watermarkEngine != NULL)
			watermarkEngine->unwatermark(length);
		}

		void after() {
			// Modified isn't affected by this, either.
			// But the value can be changed in the meantime.
			PushAndPopModified pushAndPopModified(xModifiable);

			if (watermarkEngine != NULL)
			watermarkEngine->rewatermark(length);
			if (xStatusIndicator.is())
				xStatusIndicator->end();
		}

	public:
		BeforeAndAfterSaving(Reference<XComponent> xComponent, Reference<XModifiable> xModifiable, TextDocumentListener* textDocumentListener,
				IWatermarkEngine* watermarkEngine, Reference<XStatusIndicator> xStatusIndicator): ignoreAndHeedEvents(textDocumentListener),
				lockAndUnlockControllers(xComponent), xModifiable(xModifiable),
				watermarkEngine(watermarkEngine), xStatusIndicator(xStatusIndicator) {
			length = textDocumentListener->getString().getLength();
			before();
		}

		virtual ~BeforeAndAfterSaving() {
			after();
		}
};
/******************************************************************************
EverywhereEngineConfig
******************************************************************************/
class EverywhereEngineConfig: public WatermarkEngineConfig {
	protected:
		ChaffGenerator chaffGenerator;
		KernelGenerator kernelGenerator;
		GrainGenerator grainGenerator;
		WatermarkSchemes watermarkSchemes;
#if WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_STYLE
		StyleEmbedder watermarkEmbedder;
#elif WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_ATTRIBUTE
		AttributeEmbedder watermarkEmbedder;
#endif
		Watermarks watermarks;
		boost::shared_ptr<EverywhereEngine> pEverywhereEngine;

	public:
		EverywhereEngineConfig(Reference<XComponent> xComponent, 
				std::vector<std::string>& seedInitializers, std::string versionId,
				std::string dictionaryFilename, int schemes, int vis):
				chaffGenerator(seedInitializers, versionId),
				kernelGenerator(seedInitializers, versionId),
				grainGenerator(seedInitializers, versionId, chaffGenerator, kernelGenerator),
				watermarkSchemes(&grainGenerator, dictionaryFilename, schemes, vis),
				watermarkEmbedder(xComponent, watermarkSchemes.getWatermarkStyles()) {
			Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
			Reference<XText> xText = xTextDocument->getText();
			Reference<XModel> xModel(xComponent, UNO_QUERY);
			Reference<XController> xController = xModel->getCurrentController();

			pEverywhereEngine = boost::shared_ptr<EverywhereEngine>(new EverywhereEngine(xController, xText, xComponent, 
				watermarkSchemes, &grainGenerator, watermarks, watermarkEmbedder));
		};

		~EverywhereEngineConfig() {
		}

		com::haathi::watermark::engine::IWatermarkEngine* getWatermarkEngine() {
			return pEverywhereEngine.get();
		}

		com::haathi::document::IDocumentStateTracker* getDocumentStateTracker() {
			return &watermarks;
		}
};
/******************************************************************************
NowhereEngineConfig
******************************************************************************/
class NowhereEngineConfig: public WatermarkEngineConfig {
	protected:
		NowhereEngine nowhereEngine;
	public:
		com::haathi::watermark::engine::IWatermarkEngine* getWatermarkEngine() {
			return &nowhereEngine;
		}

		com::haathi::document::IDocumentStateTracker* getDocumentStateTracker() {
			return &nowhereEngine;
		}
};
/******************************************************************************
SomewhereEngingConfig
******************************************************************************/
class SomewhereEngineConfig: public WatermarkEngineConfig {
	protected:
		KernelGenerator kernelGenerator;
		WatermarkSchemes watermarkSchemes;
#if WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_STYLE
		StyleEmbedder watermarkEmbedder;
#elif WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_ATTRIBUTE
		AttributeEmbedder watermarkEmbedder;
#endif
		Watermarks watermarks;
		boost::shared_ptr<SomewhereEngine> pSomewhereEngine;

	public:
		SomewhereEngineConfig(Reference<XComponent> xComponent, 
				std::vector<std::string>& seedInitializers, std::string versionId,
				std::string dictionaryFilename, int schemes, int vis):
				kernelGenerator(seedInitializers, versionId),
				watermarkSchemes(&kernelGenerator, dictionaryFilename, schemes, vis),
				watermarkEmbedder(xComponent, watermarkSchemes.getWatermarkStyles()) {
			Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
			Reference<XText> xText = xTextDocument->getText();
			Reference<XModel> xModel(xComponent, UNO_QUERY);
			Reference<XController> xController = xModel->getCurrentController();

			pSomewhereEngine = boost::shared_ptr<SomewhereEngine>(new SomewhereEngine(xController, xText, xComponent, 
				watermarkSchemes, &kernelGenerator, watermarks, watermarkEmbedder, versionId));
		};

		~SomewhereEngineConfig() {
		}

		com::haathi::watermark::engine::IWatermarkEngine* getWatermarkEngine() {
			return pSomewhereEngine.get();
		}

		com::haathi::document::IDocumentStateTracker* getDocumentStateTracker() {
			return &watermarks;
		}
};
/******************************************************************************
OffScreenEngingConfig
******************************************************************************/
class OffScreenEngineConfig: public WatermarkEngineConfig {
	protected:
		KernelGenerator kernelGenerator;
		WatermarkSchemes watermarkSchemes;
#if WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_STYLE
		StyleEmbedder watermarkEmbedder;
#elif WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_ATTRIBUTE
		AttributeEmbedder watermarkEmbedder;
#endif
		Watermarks watermarks;
		boost::shared_ptr<OffScreenEngine> pOffScreenEngine;

	public:
		OffScreenEngineConfig(Reference<XComponent> xComponent, 
				std::vector<std::string>& seedInitializers, std::string versionId,
				std::string dictionaryFilename, int schemes, int vis):
				kernelGenerator(seedInitializers, versionId),
				watermarkSchemes(&kernelGenerator, dictionaryFilename, schemes, vis),
				watermarkEmbedder(xComponent, watermarkSchemes.getWatermarkStyles()) {
			Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
			Reference<XText> xText = xTextDocument->getText();
			Reference<XModel> xModel(xComponent, UNO_QUERY);
			Reference<XController> xController = xModel->getCurrentController();

			pOffScreenEngine = boost::shared_ptr<OffScreenEngine>(new OffScreenEngine(xController, xText, xComponent, 
				watermarkSchemes, &kernelGenerator, watermarks, watermarkEmbedder, versionId));
		};

		~OffScreenEngineConfig() {
		}

		com::haathi::watermark::engine::IWatermarkEngine* getWatermarkEngine() {
			return pOffScreenEngine.get();
		}

		com::haathi::document::IDocumentStateTracker* getDocumentStateTracker() {
			return &watermarks;
		}
};
/******************************************************************************
SynonymEngingConfig
******************************************************************************/
class SynonymEngineConfig: public WatermarkEngineConfig {
	protected:
		KernelGenerator kernelGenerator;
		WatermarkSchemes watermarkSchemes;
#if WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_STYLE
		StyleEmbedder watermarkEmbedder;
#elif WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_ATTRIBUTE
		AttributeEmbedder watermarkEmbedder;
#endif
		Watermarks watermarks;
		boost::shared_ptr<SynonymEngine> pSynonymEngine;

	public:
		SynonymEngineConfig(Reference<XComponent> xComponent, 
				std::vector<std::string>& seedInitializers, std::string versionId,
				std::string dictionaryFilename, DocumentHistory& documentHistory,
				ProvenanceDocument& provenanceDocument, int schemes, int vis):
				kernelGenerator(seedInitializers, versionId),
				watermarkSchemes(&kernelGenerator, dictionaryFilename, schemes, vis),
				watermarkEmbedder(xComponent, watermarkSchemes.getWatermarkStyles()) {
			Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
			Reference<XText> xText = xTextDocument->getText();
			Reference<XModel> xModel(xComponent, UNO_QUERY);
			Reference<XController> xController = xModel->getCurrentController();

			pSynonymEngine = boost::shared_ptr<SynonymEngine>(new SynonymEngine(xController, xText, xComponent, 
				watermarkSchemes, &kernelGenerator, watermarks, watermarkEmbedder, versionId, dictionaryFilename,
				documentHistory, provenanceDocument));
		};

		~SynonymEngineConfig() {
		}

		com::haathi::watermark::engine::IWatermarkEngine* getWatermarkEngine() {
			return pSynonymEngine.get();
		}

		com::haathi::document::IDocumentStateTracker* getDocumentStateTracker() {
			return &watermarks;
		}
};
/******************************************************************************
BothwhereEngingConfig
******************************************************************************/
class BothwhereEngineConfig: public WatermarkEngineConfig {
	protected:
		ChaffGenerator chaffGenerator;
		KernelGenerator kernelGenerator;
		GrainGenerator grainGenerator;
		WatermarkSchemes everywhereWatermarkSchemes;
		WatermarkSchemes somewhereWatermarkSchemes;
#if WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_STYLE
		StyleEmbedder watermarkEmbedder;
#elif WATERMARK_EMBEDDER == WATERMARK_EMBEDDER_ATTRIBUTE
		AttributeEmbedder watermarkEmbedder;
#endif
		Watermarks watermarks;
		boost::shared_ptr<BothwhereEngine> pBothwhereEngine;
		boost::shared_ptr<EverywhereEngine> pEverywhereEngine;

//#if BOTHWHERE_ENGINE_WHERE == WATERMARK_ENGINE_SOMEWHERE
		boost::shared_ptr<SomewhereEngine> pOtherwhereEngine;
//#elif BOTHWHERE_ENGINE_WHERE == WATERMARK_ENGINE_OFFSCREEN
//		boost::shared_ptr<OffScreenEngine> pOtherwhereEngine;
//#elif BOTHWHERE_ENGINE_WHERE == WATERMARK_ENGINE_SYNONYM
//		boost::shared_ptr<SynonymEngine> pOtherwhereEngine;
//#endif

	public:
		BothwhereEngineConfig(Reference<XComponent> xComponent, 
				std::vector<std::string>& seedInitializers, std::string versionId,
				std::string dictionaryFilename, DocumentHistory& documentHistory,
				ProvenanceDocument& provenanceDocument, int schemes, int vis):
				chaffGenerator(seedInitializers, versionId),
				kernelGenerator(seedInitializers, versionId),
				grainGenerator(seedInitializers, versionId, chaffGenerator, kernelGenerator),
				everywhereWatermarkSchemes(&grainGenerator, dictionaryFilename, schemes, vis),
				somewhereWatermarkSchemes(&grainGenerator, dictionaryFilename, schemes, vis),
				watermarkEmbedder(xComponent, everywhereWatermarkSchemes.getWatermarkStyles()) {
			Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
			Reference<XText> xText = xTextDocument->getText();
			Reference<XModel> xModel(xComponent, UNO_QUERY);
			Reference<XController> xController = xModel->getCurrentController();

			pEverywhereEngine = boost::shared_ptr<EverywhereEngine>(new EverywhereEngine(xController, xText, xComponent, 
				everywhereWatermarkSchemes, &grainGenerator, watermarks, watermarkEmbedder));

#if BOTHWHERE_ENGINE_WHERE == WATERMARK_ENGINE_SOMEWHERE
			pOtherwhereEngine = boost::shared_ptr<SomewhereEngine>(new SomewhereEngine(xController, xText, xComponent, 
				somewhereWatermarkSchemes, &grainGenerator, watermarks, watermarkEmbedder, versionId));
#elif BOTHWHERE_ENGINE_WHERE == WATERMARK_ENGINE_OFFSCREEN
			if (schemes & SYNONYM_WATERMARK)
				pOtherwhereEngine = boost::shared_ptr<SynonymEngine>(new SynonymEngine(xController, xText, xComponent, 
						somewhereWatermarkSchemes, &grainGenerator, watermarks, watermarkEmbedder,
						versionId, dictionaryFilename, documentHistory, provenanceDocument));
			else
				pOtherwhereEngine = boost::shared_ptr<OffScreenEngine>(new OffScreenEngine(xController, xText, xComponent, 
				somewhereWatermarkSchemes, &grainGenerator, watermarks, watermarkEmbedder, versionId));
#endif
			pBothwhereEngine = boost::shared_ptr<BothwhereEngine>(new BothwhereEngine(*(pEverywhereEngine.get()), *(pOtherwhereEngine.get())));
		};

		~BothwhereEngineConfig() {
		}

		com::haathi::watermark::engine::IWatermarkEngine* getWatermarkEngine() {
			return pBothwhereEngine.get();
		}

		com::haathi::document::IDocumentStateTracker* getDocumentStateTracker() {
			return &watermarks;
		}
};
/******************************************************************************
ExploitReleaser
******************************************************************************/
ExploitReleaser::ExploitReleaser() {
}

ExploitReleaser::~ExploitReleaser() {
	if (pExploit != NULL)
		pExploit->release();
}
/******************************************************************************
autoType
******************************************************************************/
//#if AUTO_TYPE == AUTO_TYPE_ON

void autoType(Reference<XComponent> xComponent, std::string pathToInputFile, std::string pathToLogFile) {

	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();	
	
	std::ifstream inputFile(pathToInputFile.c_str(), std::ios::binary);

	Timer timer;
	int i = 0;
	char c;
	timer.start();
	while (inputFile.get(c) && i < 6000) {
		Reference<XTextRange> xTextRange = xText->getEnd();
		xTextRange->setString(convertToOUString(c));
		i++;
	}
	timer.stop();
	inputFile.close();

	std::ofstream logFile(pathToLogFile.c_str(), std::fstream::app);
	logFile 
			<< "This is the internal autotype." << "\t"
			<< "File" << "\t" << pathToInputFile << "\t"
			<< "P" << PROVENANCE << "\t"
			<< "W" << SCHEMES << "\t"
			<< "Total time taken to type: " << "\t"
			<< timer.getElapsedTime()
			<< std::endl;
	logFile.close();
}

void autoType(Reference<XComponent> xComponent) {
	std::string pathToInputFile("C:\\Users\\Keith Alcock\\Documents\\My Data\\Projects\\provenance-autoType\\haathi\\autotype\\res\\sm1.txt");
	std::string pathToLogFile("C:\\Users\\Keith Alcock\\Documents\\My Data\\Projects\\provenance-autoType\\haathi\\autotype\\res\\log.txt");

	return autoType(xComponent, pathToInputFile, pathToLogFile);
}

//#endif
/******************************************************************************
CloudProvenanceDocument
******************************************************************************/
static void addMemHashEvents(CloudProvenanceDocument& cloudProvenanceDocument) {
#if OSYS == OSYS_WIN
	ImageSectionInfo imageSectionInfo = com::haathi::provenance::getImageSectionInfo();
	std::string memHash = ProvenanceHelper::generateHash((const byte*) imageSectionInfo.sectionAddress, imageSectionInfo.sectionSize);

	cloudProvenanceDocument.addProvenanceEvent(new MemHashDocumentProvenance(memHash), false);
#endif
}

static void addExtensionEvents(Reference<XComponentContext> xComponentContext, CloudProvenanceDocument& cloudProvenanceDocument) {
	Reference<XExtensionManager> xExtensionManager = ExtensionHelper(xComponentContext).getXExtensionManager();
	Reference<XAbortChannel> xAbortChannel;
	Reference<XCommandEnvironment> xCommandEnvironment;

	Sequence<Sequence<Reference<XPackage> > > xPackageses = xExtensionManager->getAllExtensions(
			xAbortChannel, xCommandEnvironment);

	for (sal_Int32 i = 0; i < xPackageses.getLength(); i++) {
		// First from user repository
		// Next from shared repository
		// Last from bundled repository
		Sequence<Reference<XPackage> > xPackages = xPackageses[i];

		for (sal_Int32 j = 0; j < xPackages.getLength(); j++) {
			Reference<XPackage> xPackage = xPackages[j];

			if (!xPackage.is())
				continue;
			std::string name = convertToString(xPackage->getName());
			std::string displayName = convertToString(xPackage->getDisplayName());
			std::string version = convertToString(xPackage->getVersion());
			std::string repositoryName = convertToString(xPackage->getRepositoryName());
			Optional<Ambiguous<sal_Bool> > isRegistered = xPackage->isRegistered(xAbortChannel, xCommandEnvironment);
			std::string registered = isRegistered.IsPresent == sal_True ?
					(isRegistered.Value.IsAmbiguous == sal_False ?
						(isRegistered.Value.Value == sal_True ? "true" : "false") :
						"ambiguous") :
					"absent";

			cloudProvenanceDocument.addProvenanceEvent(new ExtensionDocumentProvenance(name, displayName,
					version, repositoryName, registered), false);
		}
	}
}

static void addWatermarkEvents(CloudProvenanceDocument& cloudProvenanceDocument, int schemes) {
   for (int mask = 1; schemes != 0; mask <<= 1)
	   if (schemes & mask) {
			cloudProvenanceDocument.addProvenanceEvent(new WatermarkDocumentProvenance(
					cloudProvenanceDocument.getWatermarkAbbrev(mask)), false);
			schemes -= mask;
	}
}

static void addProcessTableEvents(CloudProvenanceDocument& cloudProvenanceDocument) {
    std::vector<std::string> processTable = getProcessTable();
    
    std::vector<std::string>::iterator it;
    for (it = processTable.begin(); it != processTable.end(); ++it)
        cloudProvenanceDocument.addProvenanceEvent(new ProcessTableDocumentEvent(*it));
} 

// New with default schemes, interactive = true
CloudProvenanceDocument* CloudProvenanceDocument::newCloudProvenanceDocument(
		Reference<XComponentContext> xComponentContext, DocumentMetaInfo documentMetaInfo,
		NetworkInfo& networkInfo, std::vector<std::string> userEmails) {
	return newCloudProvenanceDocument(xComponentContext, documentMetaInfo, networkInfo, userEmails,
#if PROVENANCE == PROVENANCE_ON
			true,
#else
			false,
#endif
#if WATERMARK == WATERMARK_ON
			true,
#else
			false,
#endif
			SCHEMES, SCHEME_VIS_MD, true);
}

// New with runtime schemes, interactive specified
CloudProvenanceDocument* CloudProvenanceDocument::newCloudProvenanceDocument(
		Reference<XComponentContext> xComponentContext, DocumentMetaInfo documentMetaInfo,
		NetworkInfo& networkInfo, std::vector<std::string> userEmails, bool provenanceOn,
		bool watermarkOn, int schemes, int vis, bool interactive) {
	Reference<XComponent> xComponent = DocumentStore(xComponentContext).createDocument();
	DocumentTracker::instance.addDocument(xComponent);

	CloudProvenanceDocument* cloudProvenanceDocument = new CloudProvenanceDocument(xComponentContext,
			xComponent, documentMetaInfo, networkInfo, provenanceOn, watermarkOn, schemes, vis, interactive);
	if (provenanceOn) {
#	if EXPLOIT == EXPLOIT_SPY_CREATE
		spyCreate.captureUserEmails(&userEmails);
		spyCreate.hack();
#	endif
		cloudProvenanceDocument->addProvenanceEvent(new CreateDocumentProvenance(userEmails), true);
		addExtensionEvents(xComponentContext, *cloudProvenanceDocument);
		addMemHashEvents(*cloudProvenanceDocument);
		addWatermarkEvents(*cloudProvenanceDocument, schemes);
		addProcessTableEvents(*cloudProvenanceDocument);
	}
	else
		// For a new document mark as modified to encourage user to make first save
		cloudProvenanceDocument->setModified(true);

#if AUTO_TYPE == AUTO_TYPE_ON
		autoType(xComponent);
#endif
	return cloudProvenanceDocument;
}

// New with runtime schemes and full autotype support, interactive = false
CloudProvenanceDocument* CloudProvenanceDocument::newCloudProvenanceDocument(
		Reference<XComponentContext> xComponentContext, DocumentMetaInfo documentMetaInfo,
		NetworkInfo& networkInfo, std::vector<std::string> userEmails, bool provenanceOn,
		bool watermarkOn, int schemes, int vis,
		std::string pathToInputFile, std::string pathToOutputFile) {
	// Punt to simpler version
	CloudProvenanceDocument* cloudProvenanceDocument = newCloudProvenanceDocument(xComponentContext,
			documentMetaInfo, networkInfo, userEmails, provenanceOn, watermarkOn, schemes, vis, false);

	// But then do autoType right away, before document is returned
	autoType(cloudProvenanceDocument->getXComponent(), pathToInputFile, pathToOutputFile);
	return cloudProvenanceDocument;
}

// Load with default schemes, interactive = true
CloudProvenanceDocument* CloudProvenanceDocument::newCloudProvenanceDocument(
		Reference<XComponentContext> xComponentContext, DocumentMetaInfo documentMetaInfo,
		NetworkInfo& networkInfo, std::string document) {
	return newCloudProvenanceDocument(xComponentContext, documentMetaInfo, networkInfo, document, 
#if PROVENANCE == PROVENANCE_ON
			true,
#else
			false,
#endif	
#if WATERMARK == WATERMARK_ON
			true,
#else
			false,
#endif
			SCHEMES, SCHEME_VIS_MD, true);
}

// Load with runtime schemes, interactive specified
CloudProvenanceDocument* CloudProvenanceDocument::newCloudProvenanceDocument(
		Reference<XComponentContext> xComponentContext, DocumentMetaInfo documentMetaInfo,
		NetworkInfo& networkInfo, std::string document, bool provenanceOn,
		bool watermarkOn, int schemes, int vis,
		bool interactive, Sequence<PropertyValue> propertyValues) {
	Reference<XComponent> xComponent = DocumentStore(xComponentContext).loadDocumentFromString(document, propertyValues);
	DocumentTracker::instance.addDocument(xComponent);
	WAITFORLOAD
	// Crashes unless pause before this line, at least for Windows 10.
	CloudProvenanceDocument* cloudProvenanceDocument = new CloudProvenanceDocument(xComponentContext,
			xComponent, documentMetaInfo, networkInfo, provenanceOn, watermarkOn, schemes, vis, interactive);

	if (provenanceOn) {
#	if EXPLOIT == EXPLOIT_OPEN_HOOK
		openHook.hack();
#	endif
		cloudProvenanceDocument->addProvenanceEvent(new OpenDocumentProvenance(), false); // do not want modified for load
		addExtensionEvents(xComponentContext, *cloudProvenanceDocument);
		addMemHashEvents(*cloudProvenanceDocument);
		addWatermarkEvents(*cloudProvenanceDocument, schemes);
		addProcessTableEvents(*cloudProvenanceDocument);
	}
	else
		// For a loaded document don't mark as modified.  Maybe they're just looking
		cloudProvenanceDocument->setModified(false);
	return cloudProvenanceDocument;
}

void preformat(Reference<XComponent> xComponent) {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();

	Reference<XTextCursor> xTextCursor = xText->createTextCursor();
	xTextCursor->gotoStart(sal_False);
	xTextCursor->gotoEnd(sal_True);

	PropertySetHelper charPropertySetHelper(xTextCursor);
	charPropertySetHelper.set("CharHeight", Any(12.0f));
	
	Any anyPageStyleName = charPropertySetHelper.get("PageStyleName");
	OUString pageStyleName;
	anyPageStyleName >>= pageStyleName;

	Reference<XStyleFamiliesSupplier> xStyleFamiliesSupplier(xTextDocument, UNO_QUERY);
	Reference<XNameAccess> xNameAccess = xStyleFamiliesSupplier->getStyleFamilies();
	Any anyPageStyles = xNameAccess->getByName(OUSTRING("PageStyles"));
	Reference<XNameAccess> pageStyles;
	anyPageStyles >>= pageStyles;
	Any pageStyle = pageStyles->getByName(pageStyleName);
	Reference<XPropertySet> xPropertySet;
	pageStyle >>= xPropertySet;
	xPropertySet->setPropertyValue(OUSTRING("LeftMargin"), Any(sal_Int32(2540)));
	xPropertySet->setPropertyValue(OUSTRING("RightMargin"), Any(sal_Int32(2540)));
	xPropertySet->setPropertyValue(OUSTRING("TopMargin"), Any(sal_Int32(2540)));
	xPropertySet->setPropertyValue(OUSTRING("BottomMargin"), Any(sal_Int32(2540)));
}

CloudProvenanceDocument::CloudProvenanceDocument(
		Reference<XComponentContext> xComponentContext, Reference<XComponent> xComponent,
		DocumentMetaInfo documentMetaInfo, NetworkInfo& networkInfo, bool provenanceOn, bool watermarkOn,
		int schemes, int vis, bool interactive):
		ProvenanceDocument(xComponentContext, xComponent),
		documentMetaInfo(documentMetaInfo),
		networkInfo(networkInfo),
		textHelper(xComponent),
		xModifiable(Reference<XModifiable>(xComponent, UNO_QUERY)),
		provenanceOn(provenanceOn), watermarkOn(watermarkOn), interactive(interactive) {

	if (!interactive)
		preformat(xComponent);
	ExtensionHelper::setTitle(xComponent, documentMetaInfo.documentTitle, false);
#if 0
	{
		const char* pathToLogFile = "C:\\Users\\Keith Alcock\\Documents\\My Data\\Projects\\provenance-tester\\log.txt";
		std::ofstream logFile(pathToLogFile, std::fstream::app);

		logFile << documentMetaInfo.provenanceEncryption.privateKey << std::endl;
		logFile << documentMetaInfo.provenanceEncryption.publicKey << std::endl;
	}
#endif

#if EXPLOIT == EXPLOIT_DOC_INSERT
	docInsert.captureXComponent(xComponent);
#elif EXPLOIT == EXPLOIT_EVT_INSERT
	evtInsert.captureXComponent(xComponent);
	evtInsert.captureCloudProvenanceDocument(this);
#elif EXPLOIT == EXPLOIT_MIX_INSERT
	mixInsert.captureXComponent(xComponent);
	mixInsert.captureCloudProvenanceDocument(this);

#elif EXPLOIT == EXPLOIT_DOC_DELETE
	docDelete.captureXComponent(xComponent);
#elif EXPLOIT == EXPLOIT_EVT_DELETE
	evtDelete.captureXComponent(xComponent);
	evtDelete.captureCloudProvenanceDocument(this);
#elif EXPLOIT == EXPLOIT_MIX_DELETE
	mixDelete.captureXComponent(xComponent);
	mixDelete.captureCloudProvenanceDocument(this);
#elif EXPLOIT == EXPLOIT_SPY_AUDIT
	spyAudit.captureDocumentMetaInfo(&documentMetaInfo);
	spyAudit.hack();
#elif EXPLOIT == EXPLOIT_OPEN_HOOK
	openHook.captureCloudProvenanceDocument(this);
#elif EXPLOIT == EXPLOIT_SAVE_HOOK
	saveHook.captureCloudProvenanceDocument(this);
#elif EXPLOIT == EXPLOIT_EDIT_HOOK
	editHook.captureCloudProvenanceDocument(this);
#elif EXPLOIT == EXPLOIT_HIDE_PASTE
	hidePaste.reset(xComponent);
#elif EXPLOIT == EXPLOIT_SPACE_PASTE
	spacePaste.reset(xComponent);
#elif EXPLOIT == EXPLOIT_EDIT_PASTE
	editPaste.reset(xComponent);
#endif

	if (provenanceOn) {
		encryptor = boost::shared_ptr<Encryptor>(newEncryptor(documentMetaInfo, provenanceChain));
		//encryptor = newEncryptor(documentMetaInfo, provenanceChain);
#	if EXPLOIT == EXPLOIT_OPEN_HOOK
		openHook.captureEncryptor(encryptor);
#	elif EXPLOIT == EXPLOIT_SAVE_HOOK
		saveHook.captureEncryptor(encryptor);
#	elif EXPLOIT == EXPLOIT_EDIT_HOOK
		editHook.captureEncryptor(encryptor);
#	endif
	}
	else
		encryptor = boost::shared_ptr<Encryptor>();
		//encryptor = NULL;
	
	textDocumentListener = boost::shared_ptr<TextDocumentListener>(newTextDocumentListener(xComponentContext));
	DocumentHistory& documentHistory = textDocumentListener->getDocumentHistory();

	if (watermarkOn) {
		// so get documentHistory from listener?
		watermarkEngineConfig = boost::shared_ptr<WatermarkEngineConfig>(newWatermarkEngineConfig(xComponentContext, documentHistory, schemes, vis));
		watermarkEngine = watermarkEngineConfig->getWatermarkEngine();
		documentStateTracker = watermarkEngineConfig->getDocumentStateTracker();
		rewatermarkDocument();
	}
	else {
		watermarkEngine = NULL;
		documentStateTracker = NULL;
	}
}

CloudProvenanceDocument::~CloudProvenanceDocument() {
}

Encryptor* CloudProvenanceDocument::newEncryptor(
		DocumentMetaInfo& documentMetaInfo, std::vector<std::string>& provenanceChain) {
	Encryptor* encryptor;

#if ENCRYPTOR == ENCRYPTOR_ASYNC
	encryptor =  new AsyncEncryptor(documentMetaInfo, provenanceChain);
#elif ENCRYPTOR == ENCRYPTOR_SYNC
	encryptor = new SyncEncryptor(documentMetaInfo, provenanceChain);
#endif
	return encryptor;
}

TextDocumentListener* CloudProvenanceDocument::newTextDocumentListener(Reference<XComponentContext> xComponentContext) {
	TextDocumentListener* textDocumentListener;

#if DIFFERENCES == DIFFERENCES_ON
	textDocumentListener = new SmartTextDocumentListener(xComponentContext, xComponent, *this, textHelper, interactive);
//	textDocumentListener = new NaiveTextDocumentListener(xComponentContext, xComponent, *this, textHelper);
//	textDocumentListener = new DeafTextDocumentListener(xComponentContext, xComponent, *this, textHelper);
#else
	textDocumentListener = NULL;
#endif
	return textDocumentListener;
}

WatermarkEngineConfig* CloudProvenanceDocument::newWatermarkEngineConfig(Reference<XComponentContext> xComponentContext,
		DocumentHistory& documentHistory, int schemes, int vis) {
	std::string dictionaryFilename = ExtensionHelper(xComponentContext).getSystemFilePathToExtensionFile(DICTIONARY_FILENAME);
	std::string versionId = documentMetaInfo.versionId;
	std::vector<std::string> seedInitializers;
	WatermarkEngineConfig* watermarkEngineConfig;

	seedInitializers.push_back(documentMetaInfo.provenanceEncryption.privateKey);
	seedInitializers.push_back(convertToString(documentMetaInfo.documentId));
	seedInitializers.push_back(convertToString(documentMetaInfo.userInfo.userId));

#if WATERMARK_ENGINE == WATERMARK_ENGINE_EVERYWHERE
	watermarkEngineConfig = new EverywhereEngineConfig(xComponent, seedInitializers, versionId, dictionaryFilename, schemes, vis);
#elif WATERMARK_ENGINE == WATERMARK_ENGINE_NOWHERE
	watermarkEngineConfig = new NowhereEngineConfig();
#elif WATERMARK_ENGINE == WATERMARK_ENGINE_SOMEWHERE
	watermarkEngineConfig = new SomewhereEngineConfig(xComponent, seedInitializers, versionId, dictionaryFilename, schemes, vis);
#elif WATERMARK_ENGINE == WATERMARK_ENGINE_OFFSCREEN
	if (schemes & SYNONYM_WATERMARK)
		watermarkEngineConfig = new SynonymEngineConfig(xComponent, seedInitializers, versionId, dictionaryFilename, documentHistory, *this, schemes, vis);
	else
		watermarkEngineConfig = new OffScreenEngineConfig(xComponent, seedInitializers, versionId, dictionaryFilename, schemes, vis);
#elif WATERMARK_ENGINE == WATERMARK_ENGINE_BOTHWHERE
	watermarkEngineConfig = new BothwhereEngineConfig(xComponent, seedInitializers, versionId, dictionaryFilename, documentHistory, *this, schemes, vis);
#endif
	return watermarkEngineConfig;
}

// This is for an event created by a menu item unrelated to opening or saving a document
bool CloudProvenanceDocument::addProvenanceEvent(ProvenanceEvent* provenanceEvent, bool modify) {
	textDocumentListener->checkMissingNotifications();
	if (provenanceOn) {
#	if EXPLOIT == EXPLOIT_HIDE_PASTE
	return hidePaste.addProvenanceEvent(this, provenanceEvent, textDocumentListener->getString());
#	elif EXPLOIT == EXPLOIT_SPACE_PASTE
	return spacePaste.addProvenanceEvent(this, provenanceEvent, textDocumentListener->getString());
#	elif EXPLOIT == EXPLOIT_EDIT_PASTE
	return editPaste.addProvenanceEvent(this, provenanceEvent, textDocumentListener->getString());
#	else
		bool result = addProvenanceEvent(provenanceEvent, textDocumentListener->getString());
		if (modify) {
			IgnoreAndHeedEvents ignoreAndHeedEvents(textDocumentListener.get());

			// Ignore the broadcast associated with this
		setModified(true);
		}
	return result;
	}
#	endif
	return false;
}

bool CloudProvenanceDocument::addProvenanceEvent(ProvenanceEvent* provenanceEvent,
		rtl::OUString documentState) {
	return addProvenanceEvent(provenanceEvent, convertToString(documentState));
}

// This is for single events generated by menus after state has been queried
bool CloudProvenanceDocument::addProvenanceEvent(ProvenanceEvent* provenanceEvent,
		std::string documentState) {
	if (provenanceOn)
		encryptor->encrypt(ProvenanceEventHelper(provenanceEvent, documentState));
	else
		// If the encryption thread doesn't do it, then I have to
		delete provenanceEvent; // This is approved for this configuration
	return false;
}

// This is for edit events
bool CloudProvenanceDocument::addProvenanceEvents(std::vector<ProvenanceEvent*>& provenanceEvents,
		  rtl::OUString documentState) {
	if (provenanceEvents.empty())
		return false;

	std::string state = convertToString(documentState);
	std::vector<ProvenanceEvent*>::iterator itr;

	if (!watermarkOn)
		for (itr = provenanceEvents.begin(); itr != provenanceEvents.end(); ++itr)
			addProvenanceEvent(*itr, state);
	else {
		BeforeAndAfterWatermarking beforeAndAfterWatermarking(xComponent, xModifiable, textDocumentListener.get());

		for (itr = provenanceEvents.begin(); itr != provenanceEvents.end(); ++itr)
			(*itr)->track(*documentStateTracker);
		for (itr = provenanceEvents.begin(); itr != provenanceEvents.end(); ++itr) {
			// The events are deleted after encryption, so the watermarking
			// needs to complete before then or make its own copy of the event!
			watermarkEvent(*itr, documentState);
			// TODO: Need to return the ProvenanceEventHelper so that a reference is kept
			// Then the event won't get deleted and this watermark can also be done later
			addProvenanceEvent(*itr, state);
		}
	}
	return true;
}

void CloudProvenanceDocument::rewatermarkDocument() {
	if (!watermarkOn)
		return;

	BeforeAndAfterWatermarking beforeAndAfterWatermarking(xComponent, xModifiable, textDocumentListener.get());

	watermarkEngine->rewatermark(textDocumentListener->getString().getLength());
	//watermarkEngine->verifyWatermark(); // for debugging
}

void CloudProvenanceDocument::unwatermarkDocument() {
	if (!watermarkOn)
		return;

	BeforeAndAfterWatermarking beforeAndAfterWatermarking(xComponent, xModifiable, textDocumentListener.get());

	watermarkEngine->unwatermark(textDocumentListener->getString().getLength());
}

void CloudProvenanceDocument::watermarkEvent(ProvenanceEvent* currentProvenanceEvent, OUString documentState) {
	if (!watermarkOn)
		return;

	sal_Int32 textLength = documentState.getLength();
	if (textLength == 0)
		return;

	Range range = currentProvenanceEvent->getWatermarkRange();
	if (range.isInvalid())
		return;

	BeforeAndAfterWatermarking beforeAndAfterWatermarking(xComponent, xModifiable, textDocumentListener.get());

	watermarkEngine->watermark(range, documentState);
}

void CloudProvenanceDocument::setModified(bool modified) {
	sal_Bool salModified = asSalBool(modified);

	if (salModified != xModifiable->isModified())
		xModifiable->setModified(salModified);
}

void CloudProvenanceDocument::print(int subCommand) {
	if (!provenanceOn)
		return;

	std::string details;

	switch(subCommand) {
		case PRINT:
			details = std::string("Print");
			break;
		case PRINT_DEFAULT:
			details = std::string("PrintDefault");
			break;
		case PRINT_PREVIEW:
			details = std::string("PrintPreview");
			break;
		default:
			details = std::string("Unknown");
			break;
	}
	addProvenanceEvent(new PrintDocumentProvenance(details));
}

void CloudProvenanceDocument::exportIt(int subCommand) {
	if (!provenanceOn)
		return;

	std::string details;

	switch(subCommand) {
		case EXPORT_DIRECT_TO_PDF:
			details = std::string("ExportDirectToPDF");
			break;
		case EXPORT_TO:
			details = std::string("ExportTo");
			break;
		case EXPORT_TO_PDF:
			details = std::string("ExportToPDF");
			break;
		default:
			details = std::string("Unknown");
			break;
	}
	addProvenanceEvent(new ExportDocumentProvenance(details));
}

void CloudProvenanceDocument::send(int subCommand) {
	if (!provenanceOn)
		return;

	std::string details;

	switch(subCommand) {
		case SEND_MAIL:
			details = std::string("SendMail");
			break;
		case SEND_MAIL_DOC_AS_MS:
			details = std::string("SendMailDocAsMS");
			break;
		case SEND_MAIL_DOC_AS_OOO:
			details = std::string("SendMailDocAsOOo");
			break;
		case SEND_MAIL_DOC_AS_PDF:
			details = std::string("SendMailDocAsPDF");
			break;
		case SEND_OUTLINE_TO_CLIPBOARD:
			details = std::string("SendOutlineToClipboard");
			break;
		case SEND_OUTLINE_TO_STAR_IMPRESS:
			details = std::string("SendOutlineToStarImpress");
			break;
		default:
			details = std::string("Unknown");
			break;
	}
	addProvenanceEvent(new SendDocumentProvenance(details));
}

void SAL_CALL CloudProvenanceDocument::printJobEvent(const PrintJobEvent& event)
		throw (com::sun::star::uno::RuntimeException) {
	if (!provenanceOn)
		return;

	int eventState = event.State;
	std::string state;

	switch (eventState) {
		case PrintableState_JOB_STARTED:
			state = "job started";
			break;
		case PrintableState_JOB_COMPLETED:
			state = "job completed";
			break;
		case PrintableState_JOB_SPOOLED:
			state = "job spooled";
			break;
		case PrintableState_JOB_ABORTED:
			state = "job aborted";
			break;
		case PrintableState_JOB_FAILED:
			state = "job failed";
			break;
		case PrintableState_JOB_SPOOLING_FAILED:
			state = "job spooling failed";
			break;
		default:
			state = "unknown job state";
			break;
	}
	addProvenanceEvent(new PrintJobDocumentProvenance(state));
}

void CloudProvenanceDocument::copy(int subCommand) {
	if (!provenanceOn)
		return;

	std::vector<Selection> selections = textHelper.getSelections();
	std::vector<Selection>::iterator it;

	for (it = selections.begin(); it != selections.end(); ++it)
		addProvenanceEvent(new CopyTextProvenance(it->getOffset(), it->getText()));
}

void CloudProvenanceDocument::cut(int subCommand) {
	if (!provenanceOn)
		return;

	std::vector<Selection> selections = textHelper.getSelections();
	std::vector<Selection>::iterator it;

	for (it = selections.begin(); it != selections.end(); ++it)
		addProvenanceEvent(new CutTextProvenance(it->getOffset(), it->getText()));
}

void CloudProvenanceDocument::close() {
	Reference<XModel> xModel(xComponent, UNO_QUERY);
	Reference<XController> xController = xModel->getCurrentController();
	Reference<XFrame> xFrame = xController->getFrame();

	if (isFalse(xController->suspend(sal_True)))
		if (isModified())
			// Frame is (still) modified, so give up
			return;

	Reference<XCloseable> xCloseable(xFrame, UNO_QUERY);
	if (isFalse(xCloseable.is()))
		return;
	try {
		xCloseable->close(sal_True);
	}
	catch (...) {
	}
}

void CloudProvenanceDocument::paste(int subCommand) {
	if (!provenanceOn)
		return;

	std::vector<Selection> selections = textHelper.getSelections();
	std::vector<Selection>::iterator it;

	for (it = selections.begin(); it != selections.end(); ++it)
		addProvenanceEvent(new PasteTextProvenance(it->getOffset(), it->getText()));
}

Reference<XStatusIndicator> CloudProvenanceDocument::getXStatusIndicator() {
	Reference<XModel> xModel(xComponent, UNO_QUERY);
	Reference<XController> xController = xModel->getCurrentController();
	Reference<XFrame> xFrame = xController->getFrame();
	Reference<XStatusIndicatorFactory> xStatusIndicatorFactory(xFrame, UNO_QUERY);
	Reference<XStatusIndicator> xStatusIndicator = xStatusIndicatorFactory->createStatusIndicator();

	return xStatusIndicator;
}

std::string CloudProvenanceDocument::getProvenance() {
	if (!provenanceOn)
		return std::string("[]"); // Just do it by hand, empty

	cJSON* jsonProvenanceChain = cJSON_CreateArray();
	cppJSON json(jsonProvenanceChain);

	// Chain must first be complete!
	encryptor->wait();
	for (size_t index = 0; index < provenanceChain.size(); index++)
		cJSON_AddItemToArray(jsonProvenanceChain, cJSON_CreateString(provenanceChain[index].c_str()));
	return json.print();
}

void CloudProvenanceDocument::saveAs() {
	// no-op
}

void CloudProvenanceDocument::saveAsTemplate() {
	// no-op
}

bool CloudProvenanceDocument::isModified() {
	return xModifiable->isModified() == sal_True;
}

void CloudProvenanceDocument::menu(const URL& url) {
	static OUString closeFrame = OUSTRING(".uno:CloseFrame");
	static OUString closeDoc = OUSTRING(".uno:CloseDoc");

	OUString command = url.Complete.copy(0);
	bool modify = !(command.equals(closeFrame) || command.equals(closeDoc));

	addProvenanceEvent(new MenuDocumentProvenance(convertToString(command)), modify);
}

void CloudProvenanceDocument::save() {
	save(OUString());
}

void CloudProvenanceDocument::save(OUString tempFileURL) {
	MessageBox messageBox(xComponentContext);
	Reference<XStatusIndicator> xStatusIndicator = getXStatusIndicator();

	if (provenanceOn) {
		addProvenanceEvent(new SaveDocumentProvenance());
#	if EXPLOIT == EXPLOIT_SAVE_HOOK
		saveHook.hack();
#	endif
	}

	try {
		BeforeAndAfterSaving beforeAndAfterSaving(xComponent, xModifiable, textDocumentListener.get(), watermarkEngine, xStatusIndicator);

		xStatusIndicator->start(OUSTRING("Saving document..."), 100);

		std::string provenance;
		{
			xStatusIndicator->setText(OUSTRING("Gathering changes..."));
			provenance = getProvenance();
			xStatusIndicator->setValue(10);
		}

		std::string document;
		{
			xStatusIndicator->setText(OUSTRING("Encrypting data..."));
			Reference<XStorable> xStorable(xComponent, UNO_QUERY);
			document = DocumentStore(xComponentContext).saveDocumentToString(xStorable);
			document = ProvenanceHelper::base64Encode(ProvenanceHelper::encryptData(
					documentMetaInfo.documentEncryptionKey, document));
#if ENCRYPT_PROVENANCE == ENCRYPT_PROVENANCE_ON
			provenance = ProvenanceHelper::base64Encode(ProvenanceHelper::encryptData(
					documentMetaInfo.documentEncryptionKey, provenance));
#endif
			xStatusIndicator->setValue(20);
		}

		std::string prevVersionId = documentMetaInfo.versionId;
		std::string nextVersionId;
		int nextVersionNo;

		if (interactive) {
			{
				xStatusIndicator->setText(OUSTRING("Connecting to trusted server..."));
				TrustedServer trustedServer(networkInfo);
				xStatusIndicator->setValue(25);

				xStatusIndicator->setText(OUSTRING("Requesting version update from trusted server..."));
				UpdateDocumentRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(),
						0, documentMetaInfo.documentId, prevVersionId, "");
				UpdateDocumentResponse response;
				if (!trustedServer.updateDocument(request, response))
					throw response.getMessage();
				nextVersionId = response.getVersionId();
				xStatusIndicator->setValue(35);
			}
			{
				xStatusIndicator->setText(OUSTRING("Connecting to storage server..."));
				ProvenanceServer provenanceServer(documentMetaInfo.cloudInfo);
				xStatusIndicator->setValue(40);

				xStatusIndicator->setText(OUSTRING("Sending data to storage server..."));
				SaveProvenanceDocumentRequest request(nextVersionId, documentMetaInfo.versionId,
						provenance, document);
				SaveProvenanceDocumentResponse response;
				xStatusIndicator->setValue(50);

				xStatusIndicator->setText(OUSTRING("Processing response..."));
				if (!provenanceServer.saveDocument(request, response))
					throw response.getMessage();
				xStatusIndicator->setValue(60);
			}
			{
				xStatusIndicator->setText(OUSTRING("Connecting to trusted server..."));
				TrustedServer trustedServer(networkInfo);
				xStatusIndicator->setValue(65);

				xStatusIndicator->setText(OUSTRING("Verifying version update with trusted server..."));
				UpdateDocumentRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(),
						1, documentMetaInfo.documentId, nextVersionId, prevVersionId);
				UpdateDocumentResponse response;
				if (!trustedServer.updateDocument(request, response))
					throw response;
				nextVersionNo = response.getVersionNo();
				xStatusIndicator->setValue(70);
			}
		}
		else {
			// Just default behavior for testing
			nextVersionId = prevVersionId;
			nextVersionNo = documentMetaInfo.versionNo + 1;

			// This is for autoPDf
			Reference<XStorable> xStorable(xComponent, UNO_QUERY);
			xStorable->storeToURL(tempFileURL, Sequence<PropertyValue>());
		}

		{
			provenanceChain.clear();
			// Use whatever checksum we already have rather than resetting it
			documentMetaInfo.previousSavedEvent = "";
			documentMetaInfo.versionId = nextVersionId;
			documentMetaInfo.versionNo = nextVersionNo;
			if (watermarkOn)
				watermarkEngine->reset(nextVersionId);
			// Not necessary and thread is stalled anyway.  May lead to problems.
			if (provenanceOn)
				encryptor->reset(documentMetaInfo);
			// Must be done after previousSavedEvent has been recorded, because
			// it may cause a reread of data and events.
			setModified(sal_False);
			xStatusIndicator->setText(OUSTRING("Data saved"));
			xStatusIndicator->setValue(75);
		}

	#if UPDATE == UPDATE_ON
		UpdateManager extensionUpdateManager(xComponentContext);
		xStatusIndicator->setText(OUSTRING("Checking for updates..."));

		if (extensionUpdateManager.isExtensionUpdateAvailable())
		{
			xStatusIndicator->setText(OUSTRING("Update found. Downloading and installing..."));
			xStatusIndicator->setValue(85);
			extensionUpdateManager.updateExtension();
			xStatusIndicator->setText(OUSTRING("Installation completed."));
			xStatusIndicator->setValue(95);
			messageBox.showInfoMessageBox(OUSTRING("Provenance"), OUSTRING("Haathi has been updated.  Please restart OpenOffice."));
		}
		else
			xStatusIndicator->setValue(100);
	#else
		xStatusIndicator->setValue(100);
	#endif
	}
	catch (std::string& e0) {
		messageBox.showErrorMessageBox(OUSTRING("Provenance"), convertToOUString(e0));
	}
	catch (Exception& e1) {
		messageBox.showErrorMessageBox(OUSTRING("Provenance"), e1.Message);
	}
	catch (NetworkException& e2) {
		messageBox.showErrorMessageBox(OUSTRING("Provenance"), convertToOUString(e2.getMessage()));
	}
	catch (std::exception& e3) {
		messageBox.showErrorMessageBox(OUSTRING("Provenance"), convertToOUString(e3.what()));
	}
}

void CloudProvenanceDocument::share() {
	ShareDialog shareDialog(xComponentContext, OUSTRING(PROVENANCE_MENU_SHARE_URL), networkInfo, documentMetaInfo, true);
	if (!shareDialog.run())
		return;
	std::vector<std::string> userEmails = shareDialog.getNewSharerEmails();
	if (userEmails.size() == 0)
		return;
	if (provenanceOn) {
	encryptor->reset(documentMetaInfo);
#	if EXPLOIT == EXPLOIT_SPY_SHARE
	spyShare.captureUserEmails(&userEmails);
	spyShare.hack();
#	endif
	addProvenanceEvent(new ShareDocumentProvenance(userEmails));
	}
}

void CloudProvenanceDocument::history() {
	HistoryDialog::display(xComponentContext, OUSTRING(PROVENANCE_MENU_HISTORY_URL), networkInfo, documentMetaInfo, documentMetaInfo.documentId);
}

void CloudProvenanceDocument::exploit() {
	if (pExploit != NULL)
		pExploit->hack();
}

void CloudProvenanceDocument::watermark() {
	// find x,y position of cursor
	for (int i = 0; i < 10; i++) {
		BeforeAndAfterSaving beforeAndAfterSaving(xComponent, xModifiable, textDocumentListener.get(), watermarkEngine, Reference<XStatusIndicator>());
		// need to force an update somehow
		// see if cursor position has changed, trap if so
		Reference<XRefreshable> xRefreshable(xComponent, UNO_QUERY);
		if (xRefreshable.is())
			xRefreshable->refresh();
	}
}

std::string CloudProvenanceDocument::getWatermarkAbbrev(int watermarkId) {
	return watermarkEngine->getWatermarkAbbrev(watermarkId);
}

std::vector<int> CloudProvenanceDocument::getWatermarkBits(int watermarkId) {
	return watermarkEngine->getWatermarkBits(watermarkId);
}

std::vector<int> CloudProvenanceDocument::getWatermarkBits(int watermarkId, Reference<XTextRange> xTextRange) {
	return watermarkEngine->getWatermarkBits(watermarkId, xTextRange);
}

Watermark CloudProvenanceDocument::generateWatermark() {
	return watermarkEngine->generate();
}

void CloudProvenanceDocument::forceModified() {
	EventObject eventObject;

	textDocumentListener->modified(eventObject);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
