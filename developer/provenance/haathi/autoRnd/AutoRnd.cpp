/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/autoRnd/AutoRnd.cpp#4 $
/******************************************************************************
Include self
******************************************************************************/

/******************************************************************************
Include others
******************************************************************************/
#include "AutoLib.hpp"

#include "com/haathi/document/CloudProvenanceDocument.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/scheme/SchemeConfig.hpp"
#include "com/haathi/watermark/scheme/WatermarkSchemes.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

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
AutoRnd
******************************************************************************/
class AutoRnd {
	protected:
		int argc;
		char** argv;

		bool validSyntax;
		bool validFiles;

		int scheme;
		int count;
		std::string pathToRndWm;
	
		AutoLib autoLib;

		bool validateSyntax();
		bool validateFiles();
		bool isValid();
		void printUsage();

		void saveRndWm(CloudProvenanceDocument* cloudProvenanceDocument);

		Reference<XTextDocument> newProvenanceDocument(Reference<XComponentContext> xComponentContext,
				CloudProvenanceDocument** cloudProvenanceDocument);

	public:
		AutoRnd(int argc, char** argv);
		int run();
};

AutoRnd::AutoRnd(int argc, char** argv): argc(argc), argv(argv) {
	validSyntax = validateSyntax();
	if (!validSyntax)
		printUsage();
	else
		validFiles = validateFiles();
}

bool AutoRnd::isValid() {
	return validSyntax && validFiles;
}

void AutoRnd::printUsage() {
	std::cerr
			<< "Usage: autornd Ss <count> <wm.rnd>" << std::endl
			<< "Where: s = " << RUN_WATERMARK          << " for watermarking overhead only," << std::endl
			<< "       s = " << FG_LUMINANCE_WATERMARK << " for foreground luminance," << std::endl
			<< "       s = " << BG_LUMINANCE_WATERMARK << " for background luminance," << std::endl
			<< "       s = " << BASELINE_WATERMARK     << " for baseline," << std::endl
			<< "       s = " << CHAR_WIDTH_WATERMARK   << " for character width," << std::endl
			<< "       s = " << WORD_SPACING_WATERMARK << " for word spacing, or" << std::endl
			<< "       s = " << SYNONYM_WATERMARK      << " for synonym watermarking." << std::endl
			<< "       Multiple values for s can be combined by summing." << std::endl << std::endl

			<< "count  specifies the number of values to produce." << std::endl
			<< "rnd.wm is the file to contain the random watermark bits." << std::endl;
}

bool AutoRnd::validateSyntax() {
	int index = 1;

	if (argc != 4)
		return false;

	if (argv[index][0] != 'S')
		return false;

	scheme = 0;

	for (int i = 1; argv[index][i] != 0; i++) {
		char c = argv[index][i];

		if (c < '0' || '9' < c)
			return false;
		scheme = scheme * 10 + (c - '0');
	}
	if (63 < scheme)
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
	bool valid = true;

	valid &= validateOutputFile(pathToRndWm, "<rnd.wm>");
	return valid;
}

Reference<XTextDocument> AutoRnd::newProvenanceDocument(Reference<XComponentContext> xComponentContext,
		CloudProvenanceDocument** cloudProvenanceDocument) {
	DocumentMetaInfo documentMetaInfo = autoLib.getDocumentMetaInfo();
	NetworkInfo networkInfo = autoLib.getNetworkInfo();;
	std::vector<std::string> userEmails;

	// Visibility doesn't matter in the generation of watermarks, so SCHEME_VIS_MD has been chosen.
	CloudProvenanceDocument* pCloudProvenanceDocument = CloudProvenanceDocument::newCloudProvenanceDocument(
			xComponentContext, documentMetaInfo, networkInfo, userEmails, false, true, scheme, SCHEME_VIS_MD, false);
	Reference<XComponent> xComponent = pCloudProvenanceDocument->getXComponent();
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);

	*cloudProvenanceDocument = pCloudProvenanceDocument;
	return xTextDocument;
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

	int remainingWatermark = scheme;
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

int AutoRnd::run() {
	if (!isValid())
		return -1;
		
	try {
		Reference<XComponentContext> xComponentContext = autoLib.newXComponentContext();
		Reference<XInterface> desktop = autoLib.newDesktop(xComponentContext);
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
	return AutoRnd(argc, argv).run();
}
/*****************************************************************************/
