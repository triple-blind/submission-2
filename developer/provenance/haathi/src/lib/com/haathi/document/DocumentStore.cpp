/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/DocumentStore.cpp#2 $

//
// Author: Aaron Gibson
// Date: February 23, 2012
//
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/document/DocumentStore.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/utils/StringStream.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <osl/file.hxx>
#include "sdkEnd.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::extension::ExtensionHelper;
using com::haathi::utils::StringStream;

using com::sun::star::beans::PropertyValue;
using com::sun::star::beans::PropertyState_DIRECT_VALUE;
using com::sun::star::frame::XStorable;
using com::sun::star::frame::XComponentLoader;
using com::sun::star::io::XInputStream;
using com::sun::star::io::XOutputStream;
using com::sun::star::lang::XComponent;
using com::sun::star::lang::XMultiComponentFactory;
using com::sun::star::uno::Exception;
using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::Reference;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::XComponentContext;
using com::sun::star::uno::XInterface;

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
DocumentStore::DocumentStore(Reference<XComponentContext> xComponentContext) {
	xComponentLoader = getXComponentLoader(xComponentContext);
}

DocumentStore::DocumentStore() {
	xComponentLoader = getXComponentLoader(ExtensionHelper::getXComponentContext());
}
	
Reference<XComponentLoader> DocumentStore::getXComponentLoader(Reference<XComponentContext> xComponentContext) {
	Reference<XMultiComponentFactory> xMultiComponentFactory = xComponentContext->getServiceManager();
	Reference<XInterface> desktop = xMultiComponentFactory->createInstanceWithContext(
			OUSTRING("com.sun.star.frame.Desktop"), xComponentContext);
	Reference<XComponentLoader> xComponentLoader(desktop, UNO_QUERY);

	return xComponentLoader;
}

// See comments from version below
Reference<XComponent> DocumentStore::loadDocumentFromString(const std::string& str,
		Sequence<PropertyValue> propertyValues) throw (Exception, RuntimeException) {
	Sequence<PropertyValue> storeProps(propertyValues.getLength() + 1);
	StringStream* stringStream = new StringStream(str);
	Reference<XInputStream> xinput = static_cast<XInputStream*>(stringStream);

	sal_Int32 i;
	for (i = 0; i < propertyValues.getLength(); ++i)
		storeProps[i] = propertyValues[i];
	storeProps[i].Name = OUSTRING("InputStream");
	storeProps[i].Value <<= xinput;
	storeProps[i].Handle = -1;
	storeProps[i].State = PropertyState_DIRECT_VALUE;

	Reference<XComponent> xComponent = xComponentLoader->loadComponentFromURL(OUSTRING("private:stream"),
			OUString(RTL_CONSTASCII_USTRINGPARAM( "_blank" )), 0, storeProps);
	return xComponent;
}

/*
Reference<XComponent> DocumentStore::loadDocumentFromString(
		const std::string& str,
		Sequence<PropertyValue> properties)
		throw (Exception, RuntimeException) {
	// Apparently, OpenOffice and LibreOffice have inconsistent usage of the "private:stream"
	// URL in the XComponentLoader's loadComponentFromURL method. So, we use a hack by
	// creating a temporary file in the "temporary" (OS-dependent, for UNIX the "/tmp" folder)
	// directory with the document contents. Then, we load the contents, and once loaded, the 
	// file will delete itself. I don't really understand why "private:stream" doesn't work. 
	// I mean, the documentation (what little there is) says to do exactly that, but then it
	// throws an UNO exception. Whatever, the code below should work and is cross-platform
	// (hopefully).

	// kwa: No, there's a problem with this.  Under Windows, at least, the file is not
	// deleted at the end.  The call to remove() fails.  OpenOffice seems to keep ahold
	// of the file and the OS will not delete it.  That means anyone can find our top
	// secret file and copy it behind the scenes.  We're not trying for Libre Office
	// compatibility anymore, so we'll revert to the string version above.

	osl::FileBase::RC errorCode;
	OUString tempDirURL;
	OUString tempFileURL;

	errorCode = osl::FileBase::getTempDirURL(tempDirURL);
	errorCode = osl::FileBase::createTempFile(&tempDirURL, NULL, &tempFileURL);

	osl::File file(tempFileURL);
	file.open(osl_File_OpenFlag_Write);
	sal_uInt64 bytesWritten = 0;
	sal_uInt64 sizeStr = str.size();
	const void* dataPtr = str.c_str();
	file.write(dataPtr, sizeStr, bytesWritten);
	file.close();

	// Now, we have the URL of the file to load from.
	Reference<XComponent> xComponent(xComponentLoader->loadComponentFromURL(tempFileURL,
			OUString(RTL_CONSTASCII_USTRINGPARAM( "_blank" )), 0, properties));

	// Delete the temporary file (if there were no errors...)
	errorCode = osl::File::remove(tempFileURL);
	// TODO: Windows can't delete the file and complains of E_ACESS!
    //if (errorCode != 0)
    	//std::cerr << "Something unexpected!" << std::endl;
	return xComponent;
}
*/

std::string DocumentStore::saveDocumentToString(Reference<XStorable> xStorable,
		Sequence<PropertyValue> properties)
		throw (Exception, RuntimeException) {
	Sequence<PropertyValue> storeProps(properties.getLength() + 1);
	StringStream* stringStream = new StringStream();
	Reference<XOutputStream> xoutput = static_cast<XOutputStream*>(stringStream);

	sal_Int32 i;
	for (i = 0; i < properties.getLength(); ++i)
		storeProps[i] = properties[i];
	storeProps[i].Name = OUSTRING("OutputStream");
	storeProps[i].Value <<= xoutput;
	storeProps[i].Handle = -1;
	storeProps[i].State = PropertyState_DIRECT_VALUE;

	xStorable->storeToURL(OUSTRING("private:stream"), storeProps);
	return stringStream->str();
}

Reference<XComponent> DocumentStore::createDocument(Sequence<PropertyValue> properties) {
	return xComponentLoader->loadComponentFromURL(
			OUSTRING("private:factory/swriter"),
			OUString(RTL_CONSTASCII_USTRINGPARAM("_blank")), 0,
			properties);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
