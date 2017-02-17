/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/autoRnd/AutoRnd.cpp#4 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef AUTO_LIB_HPP
#	define AUTO_LIB_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/NetworkInfo.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/uno/XInterface.hpp>
#include "sdkEnd.hpp"

#include <string>
/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
Utility functions
******************************************************************************/
bool fileExists(const std::string& filename);
std::string get_file_contents(const char *filename);

bool validateInputFile(const std::string& inputFile, const std::string& description);
bool validateOutputFile(const std::string& outputFile, const std::string& description);

int stoi(char *s);
/******************************************************************************
DesktopCloser
******************************************************************************/
class DesktopCloser {
	protected:
		Reference<com::sun::star::frame::XDesktop> xDesktop;
	public:
		DesktopCloser(Reference<com::sun::star::uno::XInterface> desktop);
		~DesktopCloser();
};
/******************************************************************************
XTextDocumentCloser
******************************************************************************/
class XTextDocumentCloser {
	protected:
		Reference<com::sun::star::text::XTextDocument> xTextDocument;
	public:
		XTextDocumentCloser(Reference<com::sun::star::text::XTextDocument> xTextDocument);
		~XTextDocumentCloser();
};
/******************************************************************************
FileRemover
******************************************************************************/
class FileRemover {
	protected:
		OUString tempFileURL;
	public:
		FileRemover(OUString tempFileURL);
		~FileRemover();
};
/******************************************************************************
AutoLib
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
			class CloudProvenanceDocument;
		};
	};
};

class AutoLib {
	public:
		AutoLib();

		com::haathi::document::DocumentMetaInfo getDocumentMetaInfo();
		com::haathi::network::NetworkInfo getNetworkInfo();

		Reference<com::sun::star::uno::XComponentContext> newXComponentContext();
		Reference<com::sun::star::uno::XInterface> newDesktop(Reference<com::sun::star::uno::XComponentContext> xComponentContext);
};
/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
