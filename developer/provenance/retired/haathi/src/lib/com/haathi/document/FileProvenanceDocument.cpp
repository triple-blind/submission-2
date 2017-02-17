/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/document/FileProvenanceDocument.cpp#1 $

//
// Author: Aaron Gibson
// Date: March 22, 2012
//
// This file implements the details of the FileProvenanceDocument class.
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/document/FileProvenanceDocument.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/document/TextDocumentListener.hpp"
#include "com/haathi/utils/Serialization.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/document/XDocumentEventBroadcaster.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <com/sun/star/util/XModifyListener.hpp>

// Temporary!
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <cppuhelper/implbase1.hxx>
#include "sdkEnd.hpp"

#include <fstream>
#include <iostream>
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::provenance;
using namespace com::haathi::utils;

using namespace com::sun::star::beans;
using namespace com::sun::star::document;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;

using namespace cppu;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class
******************************************************************************/

//============================================================
// Constructors and Destructors
//============================================================
FileProvenanceDocument::FileProvenanceDocument(
		Reference<XComponentContext> _xContext,
		Reference<XMultiComponentFactory> _xMCF,
		Reference<XComponent> _xComponent, std::string _path):
		ProvenanceDocument(_xContext, _xMCF, _xComponent), path(_path) {
	// The vector of events should already be initialized.
}

FileProvenanceDocument::~FileProvenanceDocument() {
	// We should delete all of the provenance events...
	std::vector<ProvenanceEvent*>::iterator itr;
	for (itr = events.begin(); itr != events.end(); ++itr) {
		delete *itr;
	}
}

//============================================================
// Virtual Functions
//============================================================
bool FileProvenanceDocument::addProvenanceEvent(ProvenanceEvent* event,
		std::string document_state) {
	events.push_back(event);
	return true;
}

void FileProvenanceDocument::copy() {

}

void FileProvenanceDocument::print() {

}

void FileProvenanceDocument::watermark(com::haathi::provenance::ProvenanceEvent* current_event) {

}

void FileProvenanceDocument::save() {
	// Use the serializer...
	std::ofstream output;
	output.open(path.c_str());
	std::cout << "Path: " << path.c_str() << std::endl;
	if (output.good()) {
		HAATHI_OARCHIVE oa(output);
		oa << BOOST_SERIALIZATION_NVP(events);
	} else {
		std::cerr << "Provenance Not Saved!" << std::endl;
	}
	output.close();
}

// Temporary!!!
class PropertyListenerTemp: public WeakImplHelper1<XPropertyChangeListener> {
public:
	PropertyListenerTemp() {
	}
	~PropertyListenerTemp() throw () {
	}

	virtual void SAL_CALL propertyChange(const PropertyChangeEvent& event)
			throw (com::sun::star::uno::RuntimeException) {
		std::cout << "PropertyChangeEvent: ";
		std::cout << event.PropertyName << " " << std::endl;
	}

	virtual void SAL_CALL disposing(const EventObject& event)
			throw (com::sun::star::uno::RuntimeException) {
	}
private:
	// Don't bother with these for now...
	PropertyListenerTemp& operator=(const PropertyListenerTemp&);
	PropertyListenerTemp(const PropertyListenerTemp&);
};

//============================================================
// Static Functions
//============================================================
FileProvenanceDocument* FileProvenanceDocument::createFileProvenanceDocument(
		Reference<XComponentContext> xContext, std::string path) {
	Reference<XMultiComponentFactory> xMCF = xContext->getServiceManager();

	//get the desktop service using createInstance which returns an XInterface type
	Reference<XInterface> Desktop = xMCF->createInstanceWithContext(
			OUSTRING("com.sun.star.frame.Desktop"), xContext);

	// Query for the XComponentLoader interface
	Reference<XComponentLoader> xCompLoader(Desktop, UNO_QUERY);

	// Load the component (i.e. a blank document)
	Reference<XComponent> xComponent = xCompLoader->loadComponentFromURL(
			OUSTRING("private:factory/swriter"),
			OUString(RTL_CONSTASCII_USTRINGPARAM("_default")), 0,
			Sequence<PropertyValue>());

	FileProvenanceDocument* result = new FileProvenanceDocument(xContext, xMCF,
			xComponent, path);

	//-------------------------------------------------------
	// Construct and add the required ProvenanceListeners...
	//-------------------------------------------------------
	TextDocumentListener* listener = new TextDocumentListener(xComponent,
			result);
	// Add the customized listener to the document.
	Reference<XModifyListener> xProvListener =
			static_cast<XModifyListener*>(listener);

	Reference<XModifyBroadcaster> xModBroadcaster(xComponent, UNO_QUERY);

	xModBroadcaster->addModifyListener(xProvListener);

	Reference<XDocumentEventBroadcaster> xDocumentEventBroadcaster(xComponent,
			UNO_QUERY);
	if (!xDocumentEventBroadcaster.is()) {
		std::cout << "XDocumentEventBroadcaster was null..." << std::endl;
	}

	Reference<XModel> xModelTemp(xComponent, UNO_QUERY);
	if (!xModelTemp.is()) {
		std::cout << "XModel was null..." << std::endl;
	}

	Reference<XDocumentEventListener> xDocumentEventListener =
			static_cast<XDocumentEventListener*>(listener);
	xDocumentEventBroadcaster->addDocumentEventListener(xDocumentEventListener);

	// Now, create the instance of "XText"
	Reference<XTextDocument> xTextDoc(xModelTemp, UNO_QUERY);
	Reference<XText> mainText = xTextDoc->getText();
	Reference<XPropertySet> xSet(mainText, UNO_QUERY);
	Reference<XPropertySetInfo> xInfo = xSet->getPropertySetInfo();
	Sequence<Property> seqProps = xInfo->getProperties();
	sal_Int32 temp;
	PropertyListenerTemp* prop_listener = new PropertyListenerTemp();
	Reference<XPropertyChangeListener> x_prop_listener =
			static_cast<XPropertyChangeListener*>(prop_listener);
	for (temp = 0; temp < seqProps.getLength(); temp++) {
		std::cout << "Property: " << seqProps[temp].Name << std::endl;
		xSet->addPropertyChangeListener(seqProps[temp].Name, x_prop_listener);
	}
//	std::cout << xRange->getString() << "\n";

	return result;
}

// TODO -- finish this............
FileProvenanceDocument* FileProvenanceDocument::createFileProvenanceDocumentWithDialog(
		Reference<XComponentContext> xContext) {
	std::string path = "something.txt";

	Reference<XMultiComponentFactory> tempMCF = xContext->getServiceManager();

	//get the desktop service using createInstance which returns an XInterface type
	Reference<XInterface> Desktop = tempMCF->createInstanceWithContext(
			OUSTRING("com.sun.star.frame.Desktop"), xContext);

	// Query for the XComponentLoader interface
	Reference<XComponentLoader> tempCompLoader(Desktop, UNO_QUERY);

	Reference<XComponent> tempComponent = tempCompLoader->loadComponentFromURL(
			OUSTRING("private:factory/swriter"),
			OUString(RTL_CONSTASCII_USTRINGPARAM("_default")), 0,
			Sequence<PropertyValue>());

	return new FileProvenanceDocument(xContext, tempMCF, tempComponent, path);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
