/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/document/SimulatedProvenanceDocument.cpp#1 $

//
// Author: Nitin Shinde
// Date: July 23, 2012
//
// This file implements the class CloudProvenanceDocument that will send the collected provenance
// to the cloud on a save operation.
//
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/document/SimulatedProvenanceDocument.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/document/TextDocumentListener.hpp"

// TEMPORARY
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/NetworkInfo.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/document/XDocumentEventBroadcaster.hpp>
#include <com/sun/star/document/XDocumentEventListener.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
#include "sdkEnd.hpp"

#include <ctime>
#include <fstream>

#include "Config.hpp"
#if OSYS != OSYS_WIN
#	include <unistd.h>
#else
#	include <windows.h>
#	define usleep(x) Sleep((x)/1000)
#endif
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::crypto;
using namespace com::haathi::dialog;
using namespace com::haathi::network;
using namespace com::haathi::provenance;
using namespace com::haathi::thread;
using namespace com::haathi::utils;

using namespace com::sun::star::beans;
using namespace com::sun::star::document;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class
******************************************************************************/
static NetworkInfo emptyNetworkInfo;
static std::string emptyPassword;

SimulatedProvenanceDocument::SimulatedProvenanceDocument( Reference< XComponentContext > _xContext, 
		Reference< XMultiComponentFactory > _xMCF, Reference< XComponent > _xComponent, DocumentMetaInfo info):
		CloudProvenanceDocument( _xContext, _xMCF, _xComponent,info, emptyNetworkInfo, emptyPassword)
{
}

SimulatedProvenanceDocument::~SimulatedProvenanceDocument()
{
	
}

//--------------------------------------------------------
// Member Functions
//--------------------------------------------------------

void SimulatedProvenanceDocument::save()
{
	CloudProvenanceDocument::save();
}

//--------------------------------------------------------
// Static Functions
//--------------------------------------------------------
SimulatedProvenanceDocument* SimulatedProvenanceDocument::createCloudProvenanceDocument(
		Reference<XComponentContext> xContext, DocumentMetaInfo info) {

	Reference<XMultiComponentFactory> tempMCF = xContext->getServiceManager();

	//get the desktop service using createInstance which returns an XInterface type
	Reference<XInterface> Desktop = tempMCF->createInstanceWithContext(
			OUSTRING("com.sun.star.frame.Desktop"), xContext);

	// Query for the XComponentLoader interface
	Reference<com::sun::star::frame::XComponentLoader> tempCompLoader(Desktop, UNO_QUERY);

	// Load the component (i.e. a blank document)
	Reference<XComponent> tempComponent = tempCompLoader->loadComponentFromURL(
			OUSTRING("private:factory/swriter"),
			OUString(RTL_CONSTASCII_USTRINGPARAM("_blank")), 0,
			Sequence<PropertyValue>());

	SimulatedProvenanceDocument* result = new SimulatedProvenanceDocument(xContext,
			tempMCF, tempComponent, info);


	//-------------------------------------------------------
	// Construct and add the required ProvenanceListeners...
	//-------------------------------------------------------
	TextDocumentListener* listener = new TextDocumentListener(tempComponent,
			result);


	// Add the customized listener to the document.
	Reference<com::sun::star::util::XModifyListener> xProvListener =
			static_cast<com::sun::star::util::XModifyListener*>(listener);

	Reference<com::sun::star::util::XModifyBroadcaster> xModBroadcaster(tempComponent, UNO_QUERY);

	xModBroadcaster->addModifyListener(xProvListener);

	Reference<XDocumentEventBroadcaster> xDocumentEventBroadcaster(
			tempComponent, UNO_QUERY);
	if (!xDocumentEventBroadcaster.is()) {
		std::cerr << "XDocumentEventBroadcaster was null..." << std::endl;
	}

	// Do we even care about the XModel interface in this context?	
	Reference<XModel> xModelTemp(tempComponent, UNO_QUERY);
	if (!xModelTemp.is()) {
		std::cerr << "XModel was null..." << std::endl;
	}

	Reference<XDocumentEventListener> xDocumentEventListener =
			static_cast<XDocumentEventListener*>(listener);
	xDocumentEventBroadcaster->addDocumentEventListener(xDocumentEventListener);

	result->autoType("/home/nitin/Downloads/hello.spim","/home/nitin/Downloads/sts.txt");

	return result;
	
}
	
CloudProvenanceDocument* SimulatedProvenanceDocument::loadProvenanceDocumentFromStream(
	Reference<XComponentContext> xContext, std::string str, com::haathi::document::DocumentMetaInfo info,
	NetworkInfo& networkInfo, std::string& password)
{
	try {
		return CloudProvenanceDocument::loadProvenanceDocumentFromStream(xContext, str,info, networkInfo, password);
		
	
	} catch (com::sun::star::uno::Exception & e1) {
		std::cerr << "Uno Exception: " << e1.Message << std::endl;
	} catch (std::exception & e2) {
		std::cerr << "std::exception: " << e2.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown Exception: " << std::endl;
	}
	return NULL;
}


void SimulatedProvenanceDocument::autoType(std::string pathToInputFile, std::string pathToLogFile)
{
	
	// Auto type
	
	std::fstream infile;

	infile.open (pathToInputFile.c_str(), std::fstream::in);
	if(infile.is_open())
	std::cout << "File openedd" << std::endl;
	else
	std::cout << "File open failed" << std::endl;

	// Now, acquire the instance of XText.
	// Start typing!
	char ch,prev ;
	time_t startTime, endTime;
	std::cout << "came here" << std::endl;
	ch = prev = 0;	
	if(xComponent!= NULL && !xComponent.is())
	{
		std::cout << "xComponent null" << std::endl;
	}
	else
	{
		std::cout << "xComponent ok" << std::endl;
	}
		std::cout << "came here" << std::endl;
	Reference< XTextDocument > text_doc(xComponent, UNO_QUERY);
	if(!text_doc.is())
	{
		std::cout << "text_doc null" << std::endl;
	}
	else
	{
		std::cout << "text_doc ok" << std::endl;
	}
		std::cout << "came here" << std::endl;
	startTime = time(NULL);
	while(infile.good())
	{
		infile >> std::noskipws >> ch;
		Reference< XText > xText = text_doc->getText();
		if(!xText.is())
		{
			std::cerr << "xText null" << std::endl;
		}
		Reference< XTextRange > xEnd = xText->getEnd();
		if(!xEnd.is())
		{
			std::cerr << "xEnd null" << std::endl;
		}
		xEnd->setString(convertToOUString(ch));
		usleep(25000);
		
		//if( ch == '\n' && prev=='\n')
		//	save();
		//prev = ch;
	}

	infile.close();

	endTime = time(NULL);

	std::fstream filestr;

	filestr.open (pathToLogFile.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);

	filestr << " Total time taken to type : " << (endTime - startTime) << " seconds " << std::endl;
  		
	filestr.close();

}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/

