/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/autoRnd/AutoRnd.cpp#4 $
/******************************************************************************
Include self
******************************************************************************/
#include "AutoLib.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/crypto/Key.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>

#include <cppuhelper/bootstrap.hxx>

#include <osl/file.hxx>
#include "sdkEnd.hpp"

#include <exception>
#include <fstream>
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::crypto;
using namespace com::haathi::document;
using namespace com::haathi::network;
using namespace com::haathi::utils;

using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
Utility functions
******************************************************************************/
// http://stackoverflow.com/questions/4316442/stdofstream-check-if-file-exists-before-writing
bool fileExists(const std::string& filename) {
	std::ifstream infile(filename.c_str());

	return infile.good();
}

// http://insanecoding.blogspot.in/2011/11/how-to-read-in-file-in-c.html
std::string get_file_contents(const char *filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);

	if (in) {
		std::string contents;
 
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

bool validateInputFile(const std::string& inputFile, const std::string& description) {
	if (!fileExists(inputFile)) {
		std::cerr
			<< "The "
			<< description
			<< " input file doesn't exist: "
			<< inputFile
			<< std::endl;
		return false;
	}
	return true;
}

bool validateOutputFile(const std::string& outputFile, const std::string& description) {
	if (fileExists(outputFile)) {
		std::cerr
			<< "The "
			<< description
			<< " output file already exists: "
			<< outputFile
			<< std::endl;
		return false;
	}
	return true;
}

int stoi(char *s) {
	int i = 0;
	char c;

	while ((c = *s++) != 0) {
		if (c < '0' || '9' < c)
			throw std::runtime_error("Bad numeric format.");
		i = i * 10 + (c - '0');
	}
	return i;
}
/******************************************************************************
DesktopCloser
******************************************************************************/
DesktopCloser::DesktopCloser(Reference<XInterface> desktop) {
	Reference<XDesktop> xDesktop(desktop, UNO_QUERY);
	
	this->xDesktop = xDesktop;
}

DesktopCloser::~DesktopCloser() {
	if (xDesktop.is())
		xDesktop->terminate();
}
/******************************************************************************
XTextDocumentCloser
******************************************************************************/
XTextDocumentCloser::XTextDocumentCloser(Reference<XTextDocument> xTextDocument):
		xTextDocument(xTextDocument) {
}

XTextDocumentCloser::~XTextDocumentCloser() {
	if (xTextDocument.is())
		xTextDocument->dispose();
}
/******************************************************************************
FileRemover
******************************************************************************/
FileRemover::FileRemover(OUString tempFileURL): tempFileURL(tempFileURL) {
}

FileRemover::~FileRemover() {
	osl::FileBase::RC errorCode = osl::File::remove(tempFileURL);
	if (errorCode != 0)
		throw Exception(OUSTRING("remove resulted in error code ") + convertToOUString(errorCode) + OUSTRING("."), Reference<XInterface>());
}
/******************************************************************************
AutoLib
******************************************************************************/
AutoLib::AutoLib() {
}

Reference<XComponentContext> AutoLib::newXComponentContext() {
	// This next line is the old style version which doesn't work.  It results in an exception with message
	// "loading component library failed: /program/fwk.dll"
	// for the createInstanceWithContext call below.
	//Reference<XComponentContext> xComponentContext = cppu::defaultBootstrap_InitialComponentContext();
	Reference<XComponentContext> xComponentContext(cppu::bootstrap());

	return xComponentContext;
}

Reference<XInterface> AutoLib::newDesktop(Reference<XComponentContext> xComponentContext) {
	Reference<XMultiComponentFactory> xMultiComponentFactory(xComponentContext->getServiceManager(), UNO_QUERY);
	Reference<XInterface> desktop = xMultiComponentFactory->createInstanceWithContext(OUSTRING("com.sun.star.frame.Desktop"), xComponentContext);

	return desktop;
}

NetworkInfo AutoLib::getNetworkInfo() {
	NetworkInfo networkInfo;
	
	return networkInfo;
}

DocumentMetaInfo AutoLib::getDocumentMetaInfo() {
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

	documentMetaInfo.documentTitle = std::string("AutoLib");
	documentMetaInfo.documentId = 1;
	documentMetaInfo.documentEncryptionKey = Key("uXUGW0O+cNHShuGGctXoew==", 16);
	documentMetaInfo.versionId = std::string("0d112846-9e9b-40b4-a772-0379f7c03a57");

	userEncryption.publicKey = publicKey;
	userEncryption.publicKeySize = 162;

	documentMetaInfo.auditorList[userId] = userEncryption;

	return documentMetaInfo;
}
/******************************************************************************
Namespace
******************************************************************************/

/*****************************************************************************/
