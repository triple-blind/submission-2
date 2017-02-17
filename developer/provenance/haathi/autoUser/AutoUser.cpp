/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/autoRnd/AutoRnd.cpp#4 $
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/crypto/Key.hpp"
#include "com/haathi/document/CloudProvenanceDocument.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <cppuhelper/bootstrap.hxx>

#include <sal/main.h> // Required for this to be a free-standing executable.
#include "sdkEnd.hpp"

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::crypto;
using namespace com::haathi::document;
using namespace com::haathi::network;
using namespace com::haathi::utils;
using namespace com::haathi::watermark::utils;

using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Utility functions
******************************************************************************/
// http://stackoverflow.com/questions/4316442/stdofstream-check-if-file-exists-before-writing
bool fileExists(std::string& filename) {
	std::ifstream infile(filename.c_str());

	return infile.good();
}
/******************************************************************************
DesktopCloser
******************************************************************************/
class DesktopCloser {
	protected:
		Reference<XDesktop> xDesktop;
	public:
		DesktopCloser(Reference<XInterface> desktop) {
			Reference<XDesktop> xDesktop(desktop, UNO_QUERY);
			
			this->xDesktop = xDesktop;
		}

		~DesktopCloser() {
			if (xDesktop.is())
				xDesktop->terminate();
		}
};
/******************************************************************************
XTextDocumentCloser
******************************************************************************/
class XTextDocumentCloser {
	protected:
		Reference<XTextDocument> xTextDocument;
	public:
		XTextDocumentCloser(Reference<XTextDocument> xTextDocument):
				xTextDocument(xTextDocument) {
		}

		~XTextDocumentCloser() {
			if (xTextDocument.is())
				xTextDocument->dispose();
		}
};
/******************************************************************************
AutoRnd
******************************************************************************/
class AutoRnd {
	protected:
		int argc;
		char** argv;

		bool validSyntax;
		bool validFiles;

		int watermark;
		int count;
		std::string pathToRndWm;

		bool validateSyntax();
		bool validateFiles();

		DocumentMetaInfo getDocumentMetaInfo();
		NetworkInfo getNetworkInfo();

		void saveRndWm(CloudProvenanceDocument* cloudProvenanceDocument);

		Reference<XTextDocument> newProvenanceDocument(Reference<XComponentContext> xComponentContext,
				CloudProvenanceDocument** cloudProvenanceDocument);
		Reference<XComponentContext> newXComponentContext();
		Reference<XInterface> newDesktop(Reference<XComponentContext> xComponentContext);

	public:
		AutoRnd(int argc, char** argv): argc(argc), argv(argv) {
			validSyntax = validateSyntax();
			if (!validSyntax)
				printUsage();
			else
				validFiles = validateFiles();
		}

		bool isValid() {
			return validSyntax && validFiles;
		}

		void printUsage();
		void run();
};

void AutoRnd::printUsage() {
	std::cerr
			<< "Usage: " << argv[0] << " Ww <count> <wm.rnd>" << std::endl
			<< "Where: w = 0 for no watermarking," << std::endl
			<< "       w = 1 for foreground luminance," << std::endl
			<< "       w = 2 for background luminance," << std::endl
			<< "       w = 4 for synonym," << std::endl
			<< "       w = 8 for baseline," << std::endl
			<< "       w = 16 for word spacing, or" << std::endl
			<< "       w = 32 for character width watermarking." << std::endl
			<< "Multiple values for w can be combined by summing." << std::endl << std::endl

			<< "count  specifies the number of values to produce." << std::endl
			<< "rnd.wm is the file to contain the random watermark bits." << std::endl;
}

bool AutoRnd::validateSyntax() {
	int index = 1;

	if (argc != 4)
		return false;

	if (argv[index][0] != 'W')
		return false;

	watermark = 0;

	for (int i = 1; argv[index][i] != 0; i++) {
		char c = argv[index][i];

		if (c < '0' || '9' < c)
			return false;
		watermark = watermark * 10 + (c - '0');
	}
	if (63 < watermark)
		return false;
	index++;

	count = 0;
	for (int i = 0; argv[index][i] != 0; i++) {
		char c = argv[index][i];

		if (c < '0' || '9' < c)
			return false;
		count = count * 10 + (c - '0');
	}
	index++;

	pathToRndWm = std::string(argv[index++]);
	return true;
}

bool AutoRnd::validateFiles() {
	if (fileExists(pathToRndWm)) {
		std::cerr << "The <rnd.wm> file already exists.";
		return false;
	}
	return true;
}

Reference<XTextDocument> AutoRnd::newProvenanceDocument(Reference<XComponentContext> xComponentContext,
		CloudProvenanceDocument** cloudProvenanceDocument) {
	DocumentMetaInfo documentMetaInfo = getDocumentMetaInfo();
	NetworkInfo networkInfo = getNetworkInfo();;
	std::vector<std::string> userEmails;

	CloudProvenanceDocument* pCloudProvenanceDocument = CloudProvenanceDocument::newCloudProvenanceDocument(
			xComponentContext, documentMetaInfo, networkInfo, userEmails, false, watermark);
	Reference<XComponent> xComponent = pCloudProvenanceDocument->getXComponent();
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);

	*cloudProvenanceDocument = pCloudProvenanceDocument;
	return xTextDocument;
}

Reference<XComponentContext> AutoRnd::newXComponentContext() {
	Reference<XComponentContext> xComponentContext(cppu::bootstrap());

	return xComponentContext;
}

Reference<XInterface> AutoRnd::newDesktop(Reference<XComponentContext> xComponentContext) {
	Reference<XMultiComponentFactory> xMultiComponentFactory(xComponentContext->getServiceManager(), UNO_QUERY);
	Reference<XInterface> desktop = xMultiComponentFactory->createInstanceWithContext(OUSTRING("com.sun.star.frame.Desktop"), xComponentContext);

	return desktop;
}

NetworkInfo AutoRnd::getNetworkInfo() {
	NetworkInfo networkInfo;
	
	return networkInfo;
}

DocumentMetaInfo AutoRnd::getDocumentMetaInfo() {
	DocumentMetaInfo documentMetaInfo;
	CryptoInfo provenanceEncryption;
	CryptoInfo userEncryption;
	int userId = 4;

	// This is the private key of user@provenance.com
	std::string privateKey = std::string(
		"MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBAIDEOxlliPSdAGUWzVP9smDrTDDPbo62mwS1LVaXWtP+4KgzmzY/"
		"JCzsSanqKgOQpZW6yeRYvUSwUEjiFSW/a/slf1KzOHQBA+q/+1rHcWBLl9ujL/epgb505v5MA3IFRLZVdJBrNZ1A51pWCGwIacQF"
		"NJ1PEUuqX2Ay+DCPPOT9AgMBAAECgYBmRnaCh4kucX2czW2tJraGEJ5lrnyr0hJyvM0UGAfGlo0uTD8d6Sz8mXkxApL4Xe2HbeNq"
		"AanjYv+BxEDzyIUkx4nNj369V9lAnthepNskY7ib1dqvo3n4D39rY71hJe8GkxuvrxqIXuM1JR/m5ukAVgDIYSJHLhn+R4pEw3br"
		"6QJBALYq18EMeiSnJYpDXI8ttv+LJQl9XfNoQOvrP5VXrsPifb7tjRf64qFvwU80aFnb8hhzuemrDSDUEQRmihzewdcCQQC09Kw8"
		"wZihL7qx2m5h/86PqrXNKB9rmCFf/V+VoNSjjipf2R6YpoEERJknJNiTpzXVVqN5BNPdWbbxi0jrCV1LAkEAoopfsJTdd07cpB2u"
		"vOJhYmpZRbpi1gizxM1HEo4GFipFYNK9DiSBiuIYbf+pDGTAAylfPSuurxgJhw+nNFNeSQJAVABs4DtithipCx0CNt4ALrMpeBZT"
		"F6hkfeHp+FTTwSUiyKHK0UQiCjAqBIDXRRvPIrz4YBp8m3mlMlXdQLZ1JwJAdYBYvGBVu3UyaGScAZzFMtKMokfjI0tZbhbWkExb"
		"dBlESvvbEGUXSkq3Da7iTc87NvrTbWRJrkzjutuSLa9P7Q=="
	);
	provenanceEncryption.privateKey = privateKey;
	provenanceEncryption.privateKeySize = 634;

	std::string publicKey = std::string(
		"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCAxDsZZYj0nQBlFs1T/bJg60wwz26OtpsEtS1Wl1rT/uCoM5s2PyQs7Emp6ioD"
		"kKWVusnkWL1EsFBI4hUlv2v7JX9Sszh0AQPqv/tax3FgS5fboy/3qYG+dOb+TANyBUS2VXSQazWdQOdaVghsCGnEBTSdTxFLql9g"
		"Mvgwjzzk/QIDAQAB"
	);
	provenanceEncryption.publicKey = publicKey;
	provenanceEncryption.publicKeySize = 162;

	std::string sessionKey = std::string("qWhWmFHbREQNtpP3SwkH+A==");
	provenanceEncryption.setSessionKey(Key(sessionKey, 16));

	documentMetaInfo.provenanceEncryption = provenanceEncryption;

	documentMetaInfo.userInfo.userId = userId;
	documentMetaInfo.userInfo.username = std::string("user@provenance.com");

	documentMetaInfo.documentTitle = std::string("AutoRnd");
	documentMetaInfo.documentId = 1;
	documentMetaInfo.documentEncryptionKey = Key("uXUGW0O+cNHShuGGctXoew==", 16);
	documentMetaInfo.versionId = std::string("0d112846-9e9b-40b4-a772-0379f7c03a57");

	userEncryption.publicKey = publicKey;
	userEncryption.publicKeySize = 162;

	documentMetaInfo.auditorList[userId] = userEncryption;

	return documentMetaInfo;
}

void AutoRnd::saveRndWm(CloudProvenanceDocument* cloudProvenanceDocument) {
	std::ofstream out(pathToRndWm.c_str(), std::ios::out);

	if (cloudProvenanceDocument == NULL)
		return;

	if (!out) {
		throw std::exception("There was a problem with the <rnd.wm> file.");
		return;
	}

	std::vector<Watermark> watermarks;

	for (int i = 0; i < count; i++)
		watermarks.push_back(cloudProvenanceDocument->generateWatermark());

	out << "id" << "\t" << "abbrev" << "\t" << "bits" << std::endl;

	int remainingWatermark = watermark;
	int mask = 1;

	while (remainingWatermark != 0) {
		if (remainingWatermark & mask) {
			std::string watermarkAbbrev = cloudProvenanceDocument->getWatermarkAbbrev(mask);

			out << mask << "\t" << watermarkAbbrev << "\t";

			for (size_t outer = 0; outer < watermarks.size(); outer++) {
				Watermark watermark(watermarks[outer]);

				if (outer > 0)
					out << ",";
				for (size_t inner = 0; inner < watermark.size(); inner++)
					out << watermark[inner];
			}
			out << std::endl;

			remainingWatermark -= mask;
		}
		mask <<= 1;
	}
}

void AutoRnd::run() {
	if (!isValid())
		return;
		
	try {
		Reference<XComponentContext> xComponentContext = newXComponentContext();
		Reference<XInterface> desktop = newDesktop(xComponentContext);
		DesktopCloser desktopCloser(desktop);

		{
			CloudProvenanceDocument* cloudProvenanceDocument;
			Reference<XTextDocument> xTextDocument = newProvenanceDocument(xComponentContext, &cloudProvenanceDocument);
			XTextDocumentCloser xTextDocumentCloser(xTextDocument);

			saveRndWm(cloudProvenanceDocument);
		}
	}
	catch (Exception& e) {
		std::cerr << convertToString(e.Message) << std::endl;
	}
	catch (std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown Exception..." << std::endl;
	}
}

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv) {
	AutoRnd autoRnd(argc, argv);

	autoRnd.run();
	return 0;
}
/*****************************************************************************/
