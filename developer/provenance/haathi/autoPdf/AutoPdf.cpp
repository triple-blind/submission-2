/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/autoPdf/AutoPdf.cpp#14 $
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
#include "com/haathi/text/Range.hpp"
#include "com/haathi/text/TextHelper.hpp"
#include "com/haathi/utils/PropertySetHelper.cpp"
#include "com/haathi/utils/PropertyValuesHelper.cpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/scheme/SchemeConfig.hpp"
#include "com/haathi/watermark/scheme/WatermarkSchemes.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/lang/EventObject.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/text/XPageCursor.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/text/XTextViewCursorSupplier.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>

#include <cppuhelper/bootstrap.hxx>
#include <cppuhelper/implbase1.hxx>

#include <osl/file.hxx>
#include <sal/main.h> // Required for this to be a free-standing executable.
#include "sdkEnd.hpp"

#include <cassert>
#include <cerrno>
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
//int getPageCount(Reference<XTextDocument> xTextDocument) {
//	Reference<XModel> xModel(xTextDocument, UNO_QUERY);
//	Reference<XController> xController = xModel->getCurrentController();
//	Reference<XPropertySet> xPropertySet(xController, UNO_QUERY);
//	Any anyPageCount = xPropertySet->getPropertyValue(OUSTRING("PageCount"));
//	sal_Int16 pageCount;
//	anyPageCount >>= pageCount;
//
//	return pageCount;
//}

Reference<XTextViewCursor> getXTextViewCursor(Reference<XTextDocument> xTextDocument) {
	Reference<XModel> xModel(xTextDocument, UNO_QUERY);
	Reference<XController> xController = xModel->getCurrentController();
	Reference<XTextViewCursorSupplier> xViewCursorSupplier(xController, UNO_QUERY);
	Reference<XTextViewCursor> xTextViewCursor = xViewCursorSupplier->getViewCursor();

	return xTextViewCursor;
}

Reference<XTextRange> getXTextRange(Reference<XTextDocument> xTextDocument, Reference<XTextRange> startRange, Reference<XTextRange> endRange) {
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	// Check to see if one past end is CR or LF which
	// is not included in the jumpToEndOfPage.  Even spaces
	// are included now, potentially multiple ones.
	xTextCursor->gotoRange(endRange, sal_False);
	while (xTextCursor->goRight(1, true) == sal_True) {
		OUString nextChar = xTextCursor->getString();

		if (!(nextChar[0] == '\n' || nextChar[0] == '\r' || nextChar[0] == ' '))
			break;
		xTextCursor->collapseToEnd();
		endRange = xTextCursor->getEnd();
	}

	xTextCursor->gotoRange(startRange, sal_False);
	xTextCursor->gotoRange(endRange, sal_True);
	Reference<XTextRange> xTextRange(xTextCursor, UNO_QUERY);

	return xTextRange;
}

Reference<XTextRange> getSinglePageTextRange(Reference<XTextDocument> xTextDocument, int page) {
	Reference<XTextViewCursor> xTextViewCursor = getXTextViewCursor(xTextDocument);
	Reference<XPageCursor> xPageCursor(xTextViewCursor, UNO_QUERY);

	xPageCursor->jumpToPage(page);
	xPageCursor->jumpToStartOfPage();
	Reference<XTextRange> startRange(xPageCursor, UNO_QUERY);
	startRange = startRange->getStart();
	
	xPageCursor->jumpToEndOfPage();
	Reference<XTextRange> endRange(xPageCursor, UNO_QUERY);
	endRange = endRange->getEnd();

	return getXTextRange(xTextDocument, startRange, endRange);
}

Reference<XTextRange> getWholePageTextRange(Reference<XTextDocument> xTextDocument, int* pageCount) {
	Reference<XTextViewCursor> xTextViewCursor = getXTextViewCursor(xTextDocument);
	Reference<XPageCursor> xPageCursor(xTextViewCursor, UNO_QUERY);

	xPageCursor->jumpToFirstPage();
	xPageCursor->jumpToStartOfPage();
	Reference<XTextRange> startRange(xPageCursor, UNO_QUERY);
	startRange = startRange->getStart();

	int page = 1;
	while (xPageCursor->jumpToNextPage() == sal_True)
		page++;
	// This produces the wrong result!  There are timing problems.
	//xPageCursor->jumpToLastPage();
	//sal_Int16 page = xPageCursor->getPage();
	*pageCount = page;

	// It is assumed (later) that this last page is not completely full
	// because it is very difficult to tell.
	xPageCursor->jumpToPreviousPage();
	xPageCursor->jumpToEndOfPage();

	Reference<XTextRange> endRange(xPageCursor, UNO_QUERY);
	endRange = endRange->getEnd();

	return getXTextRange(xTextDocument, startRange, endRange);
}

Reference<XTextRange> getWholeTextRange(Reference<XTextDocument> xTextDocument) {
	Reference<XTextViewCursor> xTextViewCursor = getXTextViewCursor(xTextDocument);
	Reference<XPageCursor> xPageCursor(xTextViewCursor, UNO_QUERY);

	xPageCursor->jumpToFirstPage();
	xPageCursor->jumpToStartOfPage();
	Reference<XTextRange> startRange(xPageCursor, UNO_QUERY);
	startRange = startRange->getStart();
	
	xPageCursor->jumpToLastPage();
	xPageCursor->jumpToEndOfPage();
	Reference<XTextRange> endRange(xPageCursor, UNO_QUERY);
	endRange = endRange->getEnd();

	return getXTextRange(xTextDocument, startRange, endRange);
}

Reference<XTextRange> getPageTextRange(Reference<XTextDocument> xTextDocument, int page, int* pageCount) {
	if (page == -1)
		return getWholePageTextRange(xTextDocument, pageCount);
	else if (page == 0)
		return getWholeTextRange(xTextDocument);
	else if (page > 0)
		return getSinglePageTextRange(xTextDocument, page);
	return Reference<XTextRange>();
}

int countWatermarkBits(std::vector<int>& watermarkBits, int mask) {
	int count = 0;

	std::vector<int>::iterator itr;
	for (itr = watermarkBits.begin(); itr != watermarkBits.end(); ++itr)
		if (*itr == 0 || *itr == 1)
			count++;
	if (mask == CHAR_WIDTH_WATERMARK)
		count /= 2;
	return count;
}
/******************************************************************************
AutoPdf
******************************************************************************/
class AutoPdf {
	protected:
		int argc;
		char** argv;

		bool validSyntax;
		bool validFiles;

		int scheme;
		int vis;
		std::string pathToDocTxt;
		std::string pathToDocPdf;
		std::string pathToDocOdt;
		std::string pathToInWm;
		std::string pathToInTxt;
		int count;

		AutoLib autoLib;

		bool validateSyntax();
		bool validateFiles();
		bool isValid();
		void printUsage();

		CloudProvenanceDocument* pCloudProvenanceDocument;

		PropertyValuesHelper getAutoLoadPropertyValuesHelper();
		PropertyValuesHelper getSaveDocPdfPropertyValuesHelper(int page);
		PropertyValuesHelper getSaveDocOdtPropertyValuesHelper();

		Reference<XTextDocument> autoLoad(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xTextDocument);
		Reference<XTextDocument> autoLoadProvenance(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xTextDocument);
		void saveDocPdf(Reference<XTextDocument> xTextDocument, int page);
		void saveDocOdt(Reference<XTextDocument> xTextDocument);
		void saveInWm(Reference<XTextDocument> xTextDocument, int page); // Requires cloudProvenanceDoc
		void saveInTxt(Reference<XTextDocument> xTextDocument, int page);

	public:
		AutoPdf(int argc, char** argv);
		int run();
};

AutoPdf::AutoPdf(int argc, char** argv): argc(argc), argv(argv) {
	pCloudProvenanceDocument = NULL;
	validSyntax = validateSyntax();
	if (!validSyntax)
		printUsage();
	else
		validFiles = validateFiles();
}

bool AutoPdf::isValid() {
	return validSyntax && validFiles;
}

void AutoPdf::printUsage() {
	std::cerr
			<< "Usage: autopdf Ss Vv <doc.txt> <doc.pdf> <doc.odt> <in.wm> <in.txt>" << std::endl
			<< "Where: s = " << RUN_WATERMARK          << " for watermarking overhead only," << std::endl
			<< "       s = " << FG_LUMINANCE_WATERMARK << " for foreground luminance," << std::endl
			<< "       s = " << BG_LUMINANCE_WATERMARK << " for background luminance," << std::endl
			<< "       s = " << BASELINE_WATERMARK     << " for baseline," << std::endl
			<< "       s = " << CHAR_WIDTH_WATERMARK   << " for character width," << std::endl
			<< "       s = " << WORD_SPACING_WATERMARK << " for word spacing, or" << std::endl
			<< "       s = " << SYNONYM_WATERMARK      << " for synonym watermarking." << std::endl
			<< "       Multiple values for s can be combined by summing." << std::endl << std::endl

			<< "       v = " << SCHEME_VIS_LO << " for low visibility watermarks, " << std::endl
			<< "       v = " << SCHEME_VIS_MD << " for medium visibility, and " << std::endl
			<< "       v = " << SCHEME_VIS_HI << " for high visibility." << std::endl << std::endl

			<< "doc.txt is an example name of the incoming text document." << std::endl
			<< "doc.pdf is the name of the outgoing PDF document." << std::endl
			<< "doc.odt names to outgoing OpenOffice document for saved for posterity." << std::endl
			<< " in.wm  is a file that will record the watermarks in doc.pdf." << std::endl
			<< " in.txt is a file that records the characters in doc.pdf." << std::endl;
}

bool AutoPdf::validateSyntax() {
	int index = 1;

	if (argc != 8)
		return false;

	if (argv[index][0] != 'S')
		return false;
	scheme = stoi(&argv[index++][1]);
	if (63 < scheme)
		return false;

	if (argv[index][0] != 'V')
		return false;
	vis = stoi(&argv[index++][1]);
	if (vis != SCHEME_VIS_LO && vis != SCHEME_VIS_MD && vis != SCHEME_VIS_HI)
		return false;

	pathToDocTxt = std::string(argv[index++]);
	pathToDocPdf = std::string(argv[index++]);
	pathToDocOdt = std::string(argv[index++]);
	pathToInWm = std::string(argv[index++]);
	pathToInTxt = std::string(argv[index++]);
	return true;
}

bool AutoPdf::validateFiles() {
	bool valid = true;

	valid &= validateInputFile(pathToDocTxt, "<doc.txt>");
	valid &= validateOutputFile(pathToDocPdf, "<doc.pdf>");
	valid &= validateOutputFile(pathToDocOdt, "<doc.odt>");
	valid &= validateOutputFile(pathToInWm, "<in.wm>");
	valid &= validateOutputFile(pathToInTxt, "<in.txt>");
	return valid;
}

//Sub showFilterOptions
//Dim args(),i%
//   args() = thisComponent.getArgs
//   for i = 0 to uBound(Args())
//      if args(i).Name = "FilterOptions" then inputbox "","",args(i).value
//   next
//End Sub

PropertyValuesHelper AutoPdf::getAutoLoadPropertyValuesHelper() {
	PropertyValuesHelper propertyValuesHelper(5);

	propertyValuesHelper.add("SuggestedSaveAsName", Any(OUSTRING("AutoPdf")));
	propertyValuesHelper.add("DocumentTitle", Any(OUSTRING("AutoPdf")));
	propertyValuesHelper.add("FilterName", Any(OUSTRING("Text (encoded)")));
	propertyValuesHelper.add("MediaType", Any(OUSTRING("text/plain"))); // Force into text mode
	propertyValuesHelper.add("FilterOptions", Any(OUSTRING("MS_1252,CR," FONT ",en-US")));
	// Font size cannot be set here
//	propertyValuesHelper.add("CharHeight", Any(32.0));
	// Some of the above was too platform specific, although the character set might be useful
//	propertyValuesHelper.add("FilterOptions", Any(OUSTRING(",CR,,")));
	return propertyValuesHelper;
}

PropertyValuesHelper AutoPdf::getSaveDocPdfPropertyValuesHelper(int page) {
	if (page > 0) {
		sal_Unicode uniPage('0' + page);
		OUString pageRange = OUString(uniPage) + OUSTRING("-") + OUString(uniPage);

		PropertyValuesHelper filterData(1);
		filterData.add("PageRange", Any(OUString(uniPage) + OUSTRING("-") + OUString(uniPage)));

		PropertyValuesHelper aMediaDescriptor(2);

		aMediaDescriptor.add("FilterName", Any(OUSTRING("writer_pdf_Export")));
		aMediaDescriptor.add("FilterData", Any(filterData.get()));
		return aMediaDescriptor;
	}
	else {
		PropertyValuesHelper aMediaDescriptor(1);
	
		aMediaDescriptor.add("FilterName", Any(OUSTRING("writer_pdf_Export")));
		return aMediaDescriptor;
	}

	// These are some of the values we have to play with for the conversion to PDF.
	// See also https://wiki.openoffice.org/wiki/API/Tutorials/PDF_export
	/*
	args1(2).Name = "FilterData"
	args1(2).Value = Array(
		Array("UseLosslessCompression",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("Quality",0,90,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("ReduceImageResolution",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("MaxImageResolution",0,300,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("UseTaggedPDF",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("SelectPdfVersion",0,0,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("ExportNotes",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("ExportBookmarks",0,true,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("OpenBookmarkLevels",0,-1,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("UseTransitionEffects",0,true,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("IsSkipEmptyPages",0,true,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("IsAddStream",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("EmbedStandardFonts",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("FormsType",0,0,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("ExportFormFields",0,true,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("AllowDuplicateFieldNames",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("HideViewerToolbar",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("HideViewerMenubar",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("HideViewerWindowControls",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("ResizeWindowToInitialPage",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("CenterWindow",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("OpenInFullScreenMode",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("DisplayPDFDocumentTitle",0,true,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("InitialView",0,0,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("Magnification",0,0,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("Zoom",0,100,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("PageLayout",0,0,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("FirstPageOnLeft",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("InitialPage",0,1,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("Printing",0,2,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("Changes",0,4,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("EnableCopyingOfContent",0,true,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("EnableTextAccessForAccessibilityTools",0,true,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("ExportLinksRelativeFsys",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("PDFViewSelection",0,0,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("ConvertOOoTargetToPDFTarget",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("ExportBookmarksToPDFDestination",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("_OkButtonString",0,"",com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("EncryptFile",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("PreparedPasswords",0,,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("RestrictPermissions",0,false,com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("PreparedPermissionPassword",0,Array(),com.sun.star.beans.PropertyState.DIRECT_VALUE),
		Array("",0,,com.sun.star.beans.PropertyState.DIRECT_VALUE))
	*/
}

Reference<XTextDocument> AutoPdf::autoLoadProvenance(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xComponentLoader) {
	std::string document = get_file_contents(pathToDocTxt.c_str());

	DocumentMetaInfo documentMetaInfo = autoLib.getDocumentMetaInfo();
	NetworkInfo networkInfo = autoLib.getNetworkInfo();

#if 1 // The Provenance way
	pCloudProvenanceDocument = CloudProvenanceDocument::newCloudProvenanceDocument(
			xComponentContext, documentMetaInfo, networkInfo, document, false, true, scheme, vis, false, getAutoLoadPropertyValuesHelper().get());
	Reference<XComponent> xComponent = pCloudProvenanceDocument->getXComponent();
#else // The Standard way
	DocumentStore documentStore(xComponentContext);
	Reference<XComponent> xComponent = documentStore.loadDocumentFromString(document, getAutoLoadPropertyValuesHelper().get());
#endif
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);

	return xTextDocument;
}

Reference<XTextDocument> AutoPdf::autoLoad(Reference<XComponentContext> xComponentContext, Reference<XComponentLoader> xComponentLoader) {
	return autoLoadProvenance(xComponentContext, xComponentLoader);
}

void AutoPdf::saveDocPdf(Reference<XTextDocument> xTextDocument, int page) {
	// This is too slow after formatting is complete, so it was
	// moved to CloudProvenanceDocument when the scheme is specified.
	//Reference<XText> xText = xTextDocument->getText();
	//Reference<XTextCursor> xTextCursor = xText->createTextCursor();
	//xTextCursor->gotoStart(sal_False);
	//xTextCursor->gotoEnd(sal_True);
	//PropertySetHelper propertySetHelper(xTextCursor);
	//propertySetHelper.set("CharHeight", Any(12.0f));

	Reference<XStorable> xStorable(xTextDocument, UNO_QUERY);
	osl::FileBase::RC errorCode;
	OUString systemPath = convertToOUString(pathToDocPdf);
	OUString fileUrl;

	errorCode = osl::FileBase::getFileURLFromSystemPath(systemPath, fileUrl);
	if (errorCode != 0)
		throw Exception(OUSTRING("getFileURLFromSystemPath resulted in error code ") + convertToOUString(errorCode) + OUSTRING("."), xTextDocument);
	xStorable->storeToURL(fileUrl, getSaveDocPdfPropertyValuesHelper(page).get()); // Just save the first page by default
}

PropertyValuesHelper AutoPdf::getSaveDocOdtPropertyValuesHelper() {
	PropertyValuesHelper aMediaDescriptor(0);

	return aMediaDescriptor;
}

void AutoPdf::saveDocOdt(Reference<XTextDocument> xTextDocument) {
	Reference<XStorable> xStorable(xTextDocument, UNO_QUERY);
	osl::FileBase::RC errorCode;
	OUString systemPath = convertToOUString(pathToDocOdt);
	OUString fileUrl;

	errorCode = osl::FileBase::getFileURLFromSystemPath(systemPath, fileUrl);
	if (errorCode != 0)
		throw Exception(OUSTRING("getFileURLFromSystemPath resulted in error code ") + convertToOUString(errorCode) + OUSTRING("."), xTextDocument);
	xStorable->storeToURL(fileUrl, getSaveDocOdtPropertyValuesHelper().get());
}

void outputWatermark(std::vector<int> watermarkBits, std::ofstream& out) {
	std::vector<int>::iterator it;
	for (it = watermarkBits.begin(); it != watermarkBits.end(); ++it)
		if (*it == -2)
			out << ' ';
		else if (*it < 0)
			out << '-';
		else
			out << *it;
}

void AutoPdf::saveInWm(Reference<XTextDocument> xTextDocument, int page) {
	std::ofstream out(pathToInWm.c_str(), std::ios::out);

	if (pCloudProvenanceDocument == NULL)
		return;

	if (!out) {
		throw std::exception("There was a problem with the <in.wm> file.");
		return;
	}

	int remainingWatermark = scheme;
	int mask = 1;

	out
		<< "id"
		<< "\tabbrev"
		<< "\tvis"
		<< "\tpp"
		<< "\tppBits"
		<< "\tallBits" << std::endl;

	while (remainingWatermark != 0) {
		if (remainingWatermark & mask) {
			int pageCount = -1;
			std::string watermarkAbbrev = pCloudProvenanceDocument->getWatermarkAbbrev(mask);
			std::vector<int> watermarkBits = pCloudProvenanceDocument->getWatermarkBits(mask, getPageTextRange(xTextDocument, page, &pageCount));
			std::vector<int> ppWatermarkBits = pCloudProvenanceDocument->getWatermarkBits(mask, getPageTextRange(xTextDocument, -1, &pageCount));

			out
				<< mask
				<< "\t"
				<< watermarkAbbrev
				<< "\t"
				<< vis
				<< "\t"
				<< pageCount
				<< "\t"
				<< countWatermarkBits(ppWatermarkBits, mask)
				<< "\t";

			if (page == 0) {
				for (int i = 1; i <= pageCount; i++) {
					std::vector<int> watermarkBits = pCloudProvenanceDocument->getWatermarkBits(mask, getPageTextRange(xTextDocument, i, &pageCount));
					if (i > 1)
						out << "\t";
					outputWatermark(watermarkBits, out);
				}
				out << std::endl;
			}
			else {
				outputWatermark(watermarkBits, out);
				out << std::endl;
			}
			remainingWatermark -= mask;
		}
		mask <<= 1;
	}
}

void AutoPdf::saveInTxt(Reference<XTextDocument> xTextDocument, int page) {
	std::ofstream out(pathToInTxt.c_str(), std::ios::out | std::ios::binary);

	if (!out) {
		throw std::exception("There was a problem with the <in.txt> file.");
		return;
	}

	int pageCount;
	std::string ssText;

	if (page == 0) {
		TextHelper textHelper(Reference<XComponent>(xTextDocument, UNO_QUERY));
		Reference<XTextRange> xTextRange = getPageTextRange(xTextDocument, -1, &pageCount);

		for (int i = 1; i <= pageCount; i++) {
			Reference<XTextRange> xTextRange = getPageTextRange(xTextDocument, i, &pageCount);
			OUString ouText = textHelper.getString(xTextRange);
			ssText = convertToString(ouText); // This will be UTF-8 encoded.  Is that a problem?
			if (i > 1)
				out << "\t";
			out << ssText;
		}
	}
	else {
		TextHelper textHelper(Reference<XComponent>(xTextDocument, UNO_QUERY));
		Reference<XTextRange> xTextRange = getPageTextRange(xTextDocument, page, &pageCount);
		OUString ouText = textHelper.getString(xTextRange);
		ssText = convertToString(ouText); // This will be UTF-8 encoded.  Is that a problem?
		out << ssText;
	}
}

int AutoPdf::run() {
	if (!isValid())
		return -1;
		
	int page = 0; // For all pages
//	int page = 1; // For first page only

	try {
		Reference<XComponentContext> xComponentContext = autoLib.newXComponentContext();
		Reference<XInterface> desktop = autoLib.newDesktop(xComponentContext);
		DesktopCloser desktopCloser(desktop);

		{
			Reference<XComponentLoader> xComponentLoader(desktop, UNO_QUERY);
			Reference<XTextDocument> xTextDocument = autoLoad(xComponentContext, xComponentLoader);
			XTextDocumentCloser xTextDocumentCloser(xTextDocument);

			saveInWm(xTextDocument, page);
			saveInTxt(xTextDocument, page);
			saveDocPdf(xTextDocument, page);
			saveDocOdt(xTextDocument);
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
	return AutoPdf(argc, argv).run();
}
/*****************************************************************************/
