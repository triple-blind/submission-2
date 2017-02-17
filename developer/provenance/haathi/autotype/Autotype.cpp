/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/autotype/Autotype.cpp#15 $

/*!
 @file Autotype.cxx

 @author Aaron Gibson
 @date July 9, 2012

 @brief
 This file implements a basic executable that will automate the process of typing the text
 of some specified input file(s). The options allow for enabling provenance collection on
 this automation.
 */
/******************************************************************************
Include self
******************************************************************************/

/******************************************************************************
Include others
******************************************************************************/
#include "AutoLib.hpp"

#include "com/haathi/document/CloudProvenanceDocument.hpp"
#include "com/haathi/document/DocumentStore.hpp"
#include "com/haathi/document/ProvenanceDocument.hpp"
#include "com/haathi/text/TextHelper.hpp"
#include "com/haathi/utils/PropertyValuesHelper.cpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/utils/Timer.hpp"
#include "com/haathi/watermark/scheme/SchemeConfig.hpp"
#include "com/haathi/watermark/scheme/WatermarkSchemes.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <osl/file.hxx>

#include <sal/main.h> // Required for this to be a free-standing executable.
#include "sdkEnd.hpp"

#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::crypto;
using namespace com::haathi::document;
using namespace com::haathi::network;
using namespace com::haathi::text;
using namespace com::haathi::utils;

using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
using namespace com::sun::star::view;
/******************************************************************************
Defines
******************************************************************************/
#define FONT "Arial"
//#define FONT "Times New Roman"
/******************************************************************************
Utilities
******************************************************************************/
void peekAtFilters(Reference<XComponentContext> xComponentContext) {
	Reference<XMultiComponentFactory> xMultiComponentFactory(xComponentContext->getServiceManager(), UNO_QUERY);
	Reference<XInterface> filterFactory = xMultiComponentFactory->createInstanceWithContext(OUSTRING("com.sun.star.document.FilterFactory"), xComponentContext);
	Reference<XNameAccess> xNameAccess(filterFactory, UNO_QUERY);
	Sequence<OUString> elementNames = xNameAccess->getElementNames();
	int count = elementNames.getLength();
	std::vector<OUString> names;

	for (int i = 0; i < count; i++)
		names.push_back(elementNames[i]);
	// Look at names
	names.clear();

	Any any = xNameAccess->getByName(OUSTRING("Text (encoded)"));
	Sequence<PropertyValue> propertyValues;
	any >>= propertyValues;
	count = propertyValues.getLength();
				
	for (int i = 0; i < count; i++)
		names.push_back(propertyValues[i].Name);
	// Look at names
}
/******************************************************************************
Autotype
******************************************************************************/
class Autotype {
	protected:
		int argc;
		char** argv;
 
		bool validSyntax;
		bool validFiles;
 
		bool doLoadNotType;
		bool doSave;
		bool doProvenance;
		bool doWatermark;
		int scheme;
		int vis;
		std::string pathToDocTxt;
		std::string pathToDocOdt;
		std::string pathToLogDat;
		double loadOrTypeTime;
		double saveTime;
		int count;
	
		AutoLib autoLib;

		bool validateSyntax();
		bool validateFiles();
		bool isValid();
		void printUsage();

		void logTimes(CloudProvenanceDocument* cloudProvenanceDocument);
		int countCharacters(Reference<XTextDocument> xTextDocument);

		PropertyValuesHelper getAutoLoadPropertyValuesHelper();
		PropertyValuesHelper getAutoTypePropertyValuesHelper();
		PropertyValuesHelper getSaveDocOdtPropertyValuesHelper();

		Reference<XTextDocument> autoLoad(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xTextDocument, CloudProvenanceDocument** cloudProvenanceDocument);
		Reference<XTextDocument> autoLoadStandard(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xTextDocument);
		Reference<XTextDocument> autoLoadProvenance(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xTextDocument, CloudProvenanceDocument** cloudProvenanceDocument);
		void autoType(Reference<XTextDocument> xTextDocument, CloudProvenanceDocument* cloudProvenanceDocument);
		void autoSave(Reference<XTextDocument> xTextDocument, CloudProvenanceDocument* cloudProvenanceDocument);
		void saveDocOdt(Reference<XTextDocument> xTextDocument);

		Reference<XTextDocument> newStandardDocument(Reference<XInterface> desktop);
		Reference<XTextDocument> newProvenanceDocument(Reference<XComponentContext> xComponentContext, CloudProvenanceDocument** cloudProvenanceDocument);
		Reference<XTextDocument> newXTextDocument(Reference<XComponentContext> xComponentContext,
				Reference<XInterface> desktop, CloudProvenanceDocument** cloudProvenanceDocument);

	public:
		Autotype(int argc, char** argv);
		int run();
};

Autotype::Autotype(int argc, char** argv): argc(argc), argv(argv) {
	validSyntax = validateSyntax();
	if (!validSyntax)
		printUsage();
	else
		validFiles = validateFiles();
}

bool Autotype::isValid() {
	return validSyntax && validFiles;
}

void Autotype::printUsage() {
	std::cerr
		// TODO: Add a user.dat file
			<< "  Usage: autotype L|T|LS|TS Pp Ww Ss Vv <doc.txt> <doc.odt> <log.dat>" << std::endl
			<< "  Where: L means to load the input file all at once," << std::endl
			<< "         T means to instead type one character at a time," << std::endl
			<< "         S means to save (and rewatermark) the files as well.  Also use" << std::endl << std::endl

			<< "         p = 0 for no Provenance collection or" << std::endl
			<< "         p = 1 for Provenance collection, and" << std::endl << std::endl

			<< "         w = 0 for no watermarking or" << std::endl
			<< "         w = 1 for watermarking, and" << std::endl << std::endl

			<< "         s = " << RUN_WATERMARK          << " for watermarking overhead only," << std::endl
			<< "         s = " << FG_LUMINANCE_WATERMARK << " for foreground luminance," << std::endl
			<< "         s = " << BG_LUMINANCE_WATERMARK << " for background luminance," << std::endl
			<< "         s = " << BASELINE_WATERMARK     << " for baseline," << std::endl
			<< "         s = " << CHAR_WIDTH_WATERMARK   << " for character width," << std::endl
			<< "         s = " << WORD_SPACING_WATERMARK << " for word spacing, or" << std::endl
			<< "         s = " << SYNONYM_WATERMARK      << " for synonym watermarking." << std::endl
			<< "         Multiple values for s can be combined by summing." << std::endl << std::endl

			<< "         v = " << SCHEME_VIS_LO << " for low visibility watermarks, " << std::endl
			<< "         v = " << SCHEME_VIS_MD << " for medium visibility, and " << std::endl
			<< "         v = " << SCHEME_VIS_HI << " for high visibility." << std::endl << std::endl

			<< "         doc.txt is the input text file to load or type." << std::endl
			<< "         doc.odt is the resulting OpenOffice file to serve as a record." << std::endl
			<< "         log.dat is the output log file that records time and other statistics." << std::endl << std::endl

			<< "Example: autotype LS P0 W1 S3 doc.txt doc.odt log.dat" << std::endl;
}

bool Autotype::validateSyntax() {
	int index = 1;

	if (argc != 9)
		return false;

	if (strlen(argv[index]) > 2)
		return false;

	if (argv[index][0] == 'L')
		doLoadNotType = true;
	else if (argv[index][0] == 'T')
		doLoadNotType = false;
	else
		return false;

	if (strlen(argv[index]) == 1)
		doSave = false;
	else
		if (argv[index][1] == 'S')
			doSave = true;
		else
			return false;
	index++;

	if (strlen(argv[index]) != 2)
		return false;
	if (argv[index][0] != 'P')
		return false;
	if (argv[index][1] == '0')
		doProvenance = false;
	else if (argv[index][1] == '1')
		doProvenance = true;
	else
		return false;
	index++;

	if (strlen(argv[index]) != 2)
		return false;
	if (argv[index][0] != 'W')
		return false;
	if (argv[index][1] == '0')
		doWatermark = false;
	else if (argv[index][1] == '1')
		doWatermark = true;
	else
		return false;
	index++;

	if (argv[index][0] != 'S')
		return false;
	scheme = stoi(&argv[index++][1]);
	if (63 < scheme)
		return false;
	if (!doWatermark)
		scheme = 0;

	if (argv[index][0] != 'V')
		return false;
	vis = stoi(&argv[index++][1]);
	if (vis != SCHEME_VIS_LO && vis != SCHEME_VIS_MD && vis != SCHEME_VIS_HI)
		return false;

	pathToDocTxt = std::string(argv[index++]);
	pathToDocOdt = std::string(argv[index++]);
	pathToLogDat = std::string(argv[index++]);
	return true;
}

bool Autotype::validateFiles() {
	bool valid = true;

	valid &= validateInputFile(pathToDocTxt, "<doc.txt>");
	valid &= validateOutputFile(pathToDocOdt, "<log.odt>");
	valid &= validateOutputFile(pathToLogDat, "<log.dat>");
	return valid;
}

void Autotype::autoType(Reference<XTextDocument> xTextDocument, CloudProvenanceDocument* cloudProvenanceDocument) {
	bool lockControllers = false;
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();
	Reference<XModel> xModel(xTextDocument, UNO_QUERY);
	Timer timer;

	if (lockControllers)
		xModel->lockControllers();
	{
		std::string stdDocument = get_file_contents(pathToDocTxt.c_str());
		OUString ouDocument = convertToOUString(stdDocument);
		int length = ouDocument.getLength();
		int pos = 0;

		timer.start();
		if (cloudProvenanceDocument == NULL)
			while (pos < length) {
				xTextCursor->setString(OUString(ouDocument[pos++]));
				xTextCursor->goRight(1, sal_False);
			}
		else
			while (pos < length) {
				xTextCursor->setString(OUString(ouDocument[pos++]));
				xTextCursor->goRight(1, sal_False);
				cloudProvenanceDocument->forceModified();
			}
		timer.stop();
	}
	loadOrTypeTime = timer.getElapsedTime();
	if (lockControllers)
		xModel->unlockControllers();
}

PropertyValuesHelper Autotype::getAutoLoadPropertyValuesHelper() {
#if 0 // This is for the odt load
	PropertyValuesHelper propertyValuesHelper(0);
	return propertyValuesHelper;
#else // These are for a txt load
	PropertyValuesHelper propertyValuesHelper(5);

	propertyValuesHelper.add("SuggestedSaveAsName", Any(OUSTRING("Autotype")));
	propertyValuesHelper.add("DocumentTitle", Any(OUSTRING("Autotype")));
	propertyValuesHelper.add("FilterName", Any(OUSTRING("Text (encoded)")));
	propertyValuesHelper.add("MediaType", Any(OUSTRING("text/plain"))); // Force into text mode
	propertyValuesHelper.add("FilterOptions", Any(OUSTRING("MS_1252,CR," FONT ",en-US")));
	// Font size cannot be set here
//	propertyValuesHelper.add("CharHeight", Any(32.0));
	// Some of the above was too platform specific, although the character set might be useful
//	propertyValuesHelper.add("FilterOptions", Any(OUSTRING(",CR,,")));
	return propertyValuesHelper;
#endif
}

PropertyValuesHelper Autotype::getAutoTypePropertyValuesHelper() {
	PropertyValuesHelper propertyValuesHelper(2);

	propertyValuesHelper.add("SuggestedSaveAsName", Any(OUSTRING("Autotype")));
	propertyValuesHelper.add("DocumentTitle", Any(OUSTRING("Autotype")));
	return propertyValuesHelper;
}

Reference<XTextDocument> Autotype::autoLoadStandard(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xComponentLoader) {
	DocumentStore documentStore(xComponentContext);
	std::string document = get_file_contents(pathToDocTxt.c_str());
	Timer timer;
		
	timer.start();
	Reference<XComponent> xComponent = documentStore.loadDocumentFromString(document, getAutoLoadPropertyValuesHelper().get());
	timer.stop();
	loadOrTypeTime = timer.getElapsedTime();

	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
		
	return xTextDocument;
}
		
Reference<XTextDocument> Autotype::autoLoadProvenance(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xComponentLoader, CloudProvenanceDocument** cloudProvenanceDocument) {
	std::string document = get_file_contents(pathToDocTxt.c_str());
	Timer timer;
	
	DocumentMetaInfo documentMetaInfo = autoLib.getDocumentMetaInfo();
	NetworkInfo networkInfo = autoLib.getNetworkInfo();
		
	// These show different stages of testing and might be useful again
//	Reference<XTextDocument> xTextDocument = autoLoadStandard(xComponentContext, xComponentLoader);
//	Reference<XComponent> xComponent(xTextDocument, UNO_QUERY);
//	DocumentMenu* documentMenu = new DocumentMenu(xComponentContext, xComponent);
//	ProvenanceDocument* provenanceDocument = new ProvenanceDocument(xComponentContext, xComponent);
//	CloudProvenanceDocument* cloudProvenanceDocument = new CloudProvenanceDocument(xComponentContext, xComponent,
//			documentMetaInfo, networkInfo, scheme);
		
	timer.start();
	CloudProvenanceDocument* pCloudProvenanceDocument = CloudProvenanceDocument::newCloudProvenanceDocument(
			xComponentContext, documentMetaInfo, networkInfo, document, doProvenance, doWatermark, scheme, vis, false, getAutoLoadPropertyValuesHelper().get());
	*cloudProvenanceDocument = pCloudProvenanceDocument;
	Reference<XComponent> xComponent = pCloudProvenanceDocument->getXComponent();
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	timer.stop();
	loadOrTypeTime = timer.getElapsedTime();
	return xTextDocument;
} 

Reference<XTextDocument> Autotype::autoLoad(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xComponentLoader, CloudProvenanceDocument** cloudProvenanceDocument) {
	*cloudProvenanceDocument = NULL;
	if (doProvenance || doWatermark)
		return autoLoadProvenance(xComponentContext, xComponentLoader, cloudProvenanceDocument);
	return autoLoadStandard(xComponentContext, xComponentLoader);
}

void Autotype::autoSave(Reference<XTextDocument> xTextDocument, CloudProvenanceDocument* cloudProvenanceDocument) {
	Reference<XText> xText = xTextDocument->getText();
	Timer timer;

	{
		Reference<XStorable> xStorable(xTextDocument, UNO_QUERY);
		OUString tempDirURL;
		OUString tempFileURL;
		osl::FileBase::RC errorCode;

		errorCode = osl::FileBase::getTempDirURL(tempDirURL);
		if (errorCode != 0)
			throw Exception(OUSTRING("getTempDirURL resulted in error code ") + convertToOUString(errorCode) + OUSTRING("."), xTextDocument);

		errorCode = osl::FileBase::createTempFile(&tempDirURL, NULL, &tempFileURL);
		if (errorCode != 0)
			throw Exception(OUSTRING("createTempFile resulted in error code ") + convertToOUString(errorCode) + OUSTRING("."), xTextDocument);
		
		FileRemover fileRemover(tempFileURL);
				
		timer.start();
		if (cloudProvenanceDocument == NULL)
			xStorable->storeToURL(tempFileURL, Sequence<PropertyValue>());
		else
			cloudProvenanceDocument->save(tempFileURL);
		timer.stop();
	}
	saveTime = timer.getElapsedTime();
}

PropertyValuesHelper Autotype::getSaveDocOdtPropertyValuesHelper() {
	PropertyValuesHelper aMediaDescriptor(0);

	return aMediaDescriptor;
}

void Autotype::saveDocOdt(Reference<XTextDocument> xTextDocument) {
	Reference<XStorable> xStorable(xTextDocument, UNO_QUERY);
	osl::FileBase::RC errorCode;
	OUString systemPath = convertToOUString(pathToDocOdt);
	OUString fileUrl;

	errorCode = osl::FileBase::getFileURLFromSystemPath(systemPath, fileUrl);
	if (errorCode != 0)
		throw Exception(OUSTRING("getFileURLFromSystemPath resulted in error code ") + convertToOUString(errorCode) + OUSTRING("."), xTextDocument);
	xStorable->storeToURL(fileUrl, getSaveDocOdtPropertyValuesHelper().get());
}

Reference<XTextDocument> Autotype::newStandardDocument(Reference<XInterface> desktop) {
	Reference<XComponentLoader> xComponentLoader(desktop, UNO_QUERY);
	Reference<XComponent> document = xComponentLoader->loadComponentFromURL(OUSTRING("private:factory/swriter"),
			OUSTRING("_blank"), 0, getAutoTypePropertyValuesHelper().get());
	Reference<XTextDocument> xTextDocument(document, UNO_QUERY);
				
	return xTextDocument;
}

Reference<XTextDocument> Autotype::newProvenanceDocument(Reference<XComponentContext> xComponentContext,
		CloudProvenanceDocument** cloudProvenanceDocument) {
	DocumentMetaInfo documentMetaInfo = autoLib.getDocumentMetaInfo();
	NetworkInfo networkInfo = autoLib.getNetworkInfo();;
	std::vector<std::string> userEmails;

	// Need to make this not interactive
	CloudProvenanceDocument* pCloudProvenanceDocument = CloudProvenanceDocument::newCloudProvenanceDocument(
			xComponentContext, documentMetaInfo, networkInfo, userEmails, doProvenance, doWatermark, scheme, vis, false);
	*cloudProvenanceDocument = pCloudProvenanceDocument;
	// Try Provenance document with built-in autotyping so it can be done there
	//CloudProvenanceDocument* pCloudProvenanceDocument = CloudProvenanceDocument::newCloudProvenanceDocument(
	//		xComponentContext, documentMetaInfo, networkInfo, userEmails, doProvenance, scheme, pathToDocTxt, pathToLogDat);
	Reference<XComponent> xComponent = pCloudProvenanceDocument->getXComponent();
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);

	return xTextDocument;
}

Reference<XTextDocument> Autotype::newXTextDocument(Reference<XComponentContext> xComponentContext,
		Reference<XInterface> desktop, CloudProvenanceDocument** cloudProvenanceDocument) {
	*cloudProvenanceDocument = NULL;
	return (doProvenance || doWatermark) ? newProvenanceDocument(xComponentContext, cloudProvenanceDocument) : newStandardDocument(desktop);
}

void Autotype::logTimes(CloudProvenanceDocument* cloudProvenanceDocument) {
	std::ofstream logFile(pathToLogDat.c_str(), std::fstream::app);

	logFile
		<< "P"
		<< "\tW"
		<< "\tS"
		<< "\tAbbrev"
		<< "\tCharCount" 
		<< "\tLoadTime" 
		<< "\tTypeTime" 
		<< "\tSaveTime"
		<< "\tTextHelperCount" << "\tTextHelperTime" 
		<< "\tDifferencerCount" << "\tDifferencerTime" 
		<< "\tEncryptorCount" << "\tEncryptorTime" 
		<< std::endl;
		
	std::string abbrev = cloudProvenanceDocument != NULL ?
			cloudProvenanceDocument->getWatermarkAbbrev(scheme) : "-";

	logFile
		<< (doProvenance ? "1" : "0")
		<< "\t" << (doWatermark ? "1" : "0")
		<< "\t" << scheme
		<< "\t" << abbrev
		<< "\t" << count;
	if (doLoadNotType)
		logFile << "\t" << loadOrTypeTime;
	else
		logFile << "\t-";
	if (!doLoadNotType)
		logFile << "\t" << loadOrTypeTime;
	else
		logFile << "\t-";
	if (doSave)
		logFile << "\t" << saveTime;
	else
		logFile << "\t-";

	logFile
			<< "\t" << textHelperTimer.getCount() << "\t" << textHelperTimer.getElapsedTime()
			<< "\t" << differencerTimer.getCount() << "\t" << differencerTimer.getElapsedTime()
			<< "\t" << encryptorTimer.getCount() << "\t" << encryptorTimer.getElapsedTime()
			<< std::endl;
}

int Autotype::countCharacters(Reference<XTextDocument> xTextDocument) {
	Reference<XComponent> xComponent(xTextDocument, UNO_QUERY);
	TextHelper textHelper(xComponent);
	OUString content = textHelper.getString();
		
	return content.getLength();
}
		
int Autotype::run() {
	if (!isValid())
		return -1;
		
	try {
		CloudProvenanceDocument* cloudProvenanceDocument = NULL;
		Reference<XComponentContext> xComponentContext = autoLib.newXComponentContext();
		Reference<XInterface> desktop = autoLib.newDesktop(xComponentContext);
		DesktopCloser desktopCloser(desktop);
		
		{
			Reference<XTextDocument> xTextDocument;

			if (doLoadNotType) {
				Reference<XComponentLoader> xComponentLoader(desktop, UNO_QUERY);
				xTextDocument = autoLoad(xComponentContext, xComponentLoader, &cloudProvenanceDocument);
			}
			else
				xTextDocument = newXTextDocument(xComponentContext, desktop, &cloudProvenanceDocument);

			XTextDocumentCloser xTextDocumentCloser(xTextDocument);

			if (!doLoadNotType)
				autoType(xTextDocument, cloudProvenanceDocument);
			count = countCharacters(xTextDocument);
			saveDocOdt(xTextDocument);
			if (doSave)
				autoSave(xTextDocument, cloudProvenanceDocument);
			logTimes(cloudProvenanceDocument);
		}
	}
	catch (Exception& e) {
		std::cerr << convertToString(e.Message) << std::endl;
		return -1;
	}
	catch (std::exception& e){
		std::cerr << e.what() << std::endl;
		return -1;
	}
	catch (...) {
		std::cerr << "Unknown Exception..." << std::endl;
		return -1;
	}
	return 0;
}

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv) {
	return Autotype(argc, argv).run();
}
/*****************************************************************************/
