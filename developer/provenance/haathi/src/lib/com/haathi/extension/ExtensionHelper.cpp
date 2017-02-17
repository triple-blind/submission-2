/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/extension/ExtensionHelper.cpp#1 $

//
// Author: Aaron Gibson
// Date: March 14, 2012
//
// This file implements all of the "helper" functions defined in "ExtensionHelper.hpp".
//
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/extension/ExtensionHelper.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/extension/ExtensionConfig.hpp"
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/PropertySetHelper.hpp"
#include "com/haathi/utils/PropertyValuesHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/deployment/PackageInformationProvider.hpp>
#include <com/sun/star/deployment/XPackageInformationProvider.hpp>
#include <com/sun/star/frame/XTitle.hpp>
#include <com/sun/star/graphic/XGraphicProvider.hpp>
#include <cppuhelper/bootstrap.hxx> // defaultBootstrap_InitialComponentContext()
#include "sdkEnd.hpp"

#include <iostream> // std::cout
/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::convertToOUString;
using com::haathi::utils::convertToString;
using com::haathi::utils::PropertySetHelper;
using com::haathi::utils::PropertyValuesHelper;

using com::sun::star::awt::Rectangle;
using com::sun::star::awt::WindowAttribute::BORDER;
using com::sun::star::awt::WindowAttribute::CLOSEABLE;
using com::sun::star::awt::WindowAttribute::MOVEABLE;
using com::sun::star::awt::WindowAttribute::SIZEABLE;
using com::sun::star::awt::WindowClass_TOP;
using com::sun::star::awt::WindowDescriptor;
using com::sun::star::awt::XWindow;
using com::sun::star::awt::XWindowPeer;
using com::sun::star::awt::XToolkit;
using com::sun::star::beans::PropertyValue;
using com::sun::star::deployment::PackageInformationProvider;
using com::sun::star::deployment::XExtensionManager;
using com::sun::star::deployment::XPackageInformationProvider;
using com::sun::star::frame::XDesktop;
using com::sun::star::frame::XDispatchHelper;
using com::sun::star::frame::XFrame;
using com::sun::star::frame::XModel;
using com::sun::star::frame::XTitle;
using com::sun::star::graphic::XGraphic;
using com::sun::star::graphic::XGraphicProvider;
using com::sun::star::lang::XComponent;
using com::sun::star::lang::XMultiComponentFactory;
using com::sun::star::lang::XMultiServiceFactory;
using com::sun::star::ucb::XFileIdentifierConverter;
using com::sun::star::uno::Any;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::uno::XComponentContext;
using com::sun::star::uno::XInterface;
using com::sun::star::uno::Exception;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace extension {
/******************************************************************************
Global functions
******************************************************************************/
/**
	kwa: The bootstrap functions are inappropriate.  Each component is provided
	with a context upon construction and that one should be used rather than one
	fetched with the bootstrap function.  Method signatures have been changed to
	use a context that is passed in.
*/

/**
 IMPORTANT NOTES THAT MAY HELP YOU DEBUG
 
 Something that is ultimately really confusing about OpenOffice is that practically every 
 operation requires an "XMultiComponentFactory" or an "XMultiServiceFactory" (they are the
 same thing, although XMultiComponentFactory is newer and should probably be used instead).
 Naturally, the question becomes, how do I acquire this factory? The short answer is that you
 have to "bootstrap()" it. Note also that I use factory interchangeably with service manager.
 They are all really the same thing.
 
 If you browse through the documentation, you will stumble across two ways of doing this. The
 first way is some function ::cppu::bootstrap(). The second way is some function (actually it
 might be a macro, I don't know or care) called defaultBootstrap_InitialComponentContext(). 
 There might be some other functions that look similar to this one, but you probably won't need
 to use it. Anyway, while these two functions look the same, they are NOT!
 
 ::cppu::bootstrap() is designed to start-up OpenOffice when there isn't an instance running.
 There are some complicated things that OpenOffice allows, such as running it on a server 
 across the internet, and displaying it on a remote machine. This isn't that surprising really.
 MatLab and some other software allows for the same thing. Anyway, this function is designed to
 connect to a remote instance of OpenOffice, or start a local instance. This function starts
 OpenOffice from scratch.
 
 In spite of this, there are many instances where we must acquire the service manager when we
 already have a running instance of OpenOffice. To do this, we CANNOT use the ::cppu::bootstrap()
 function because it will try and connect to OpenOffice again. Since there is already an instance
 running, OpenOffice will block, essentially causing wherever you called this function to stall
 indefinitely. Finding the source of this was a complete pain in the ass for me, so learn from
 my mistakes! When you can guarantee that you already have a running instance of the office, 
 (i.e. you are writing code pertaining to an OpenOffice extension, hint, hint), then you should
 use the function: defaultBootstrap_InitialComponentContext(). 
 */

// In case there is no context to use, which should only happen if this code
// is called external to the extension
Reference<XComponentContext> ExtensionHelper::getXComponentContext() {
	Reference<XComponentContext> xComponentContext = cppu::defaultBootstrap_InitialComponentContext();

	if (!xComponentContext.is())
		std::cout << "Error acquiring component context..." << std::endl;
	return xComponentContext;
}

ExtensionHelper::ExtensionHelper(Reference<XComponentContext> xComponentContext):
		xComponentContext(xComponentContext) {
}

OUString ExtensionHelper::getURLToExtensionRoot(OUString& name) {
	Reference<XPackageInformationProvider> xPackageInformationProvider = PackageInformationProvider::get(xComponentContext);
	OUString path = xPackageInformationProvider->getPackageLocation(name);

	return path;
}

OUString ExtensionHelper::getURLToExtensionRoot() {
	static OUString name = OUSTRING(PROVENANCE_REFERENCE_URL);

	return getURLToExtensionRoot(name);
}

Reference<XFileIdentifierConverter> ExtensionHelper::getXFileIdentifierConverter() {
	Reference<XMultiComponentFactory> xMultiComponentFactory = getXMultiComponentFactory();
	Reference<XInterface> fileContentProvider = xMultiComponentFactory->createInstanceWithContext(
			OUSTRING("com.sun.star.ucb.FileContentProvider"), xComponentContext);
	Reference<XFileIdentifierConverter> xFileIdentifierConverter(fileContentProvider, UNO_QUERY);

	return xFileIdentifierConverter;
}

OUString ExtensionHelper::getSystemFilePathFromURL(OUString url) {
	return getXFileIdentifierConverter()->getSystemPathFromFileURL(url);
}

OUString ExtensionHelper::getURLFromSystemFilePath(OUString filepath) {
	return getXFileIdentifierConverter()->getFileURLFromSystemPath(OUString(), filepath);
}

Reference<XGraphic> ExtensionHelper::getGraphicFromFile(OUString file, sal_Bool inExtension) {
	OUString path = isTrue(inExtension) ? getURLToExtensionRoot() + file : file;
	try {
		Reference<XMultiComponentFactory> xMultiComponentFactory = getXMultiComponentFactory();
		Reference<XInterface> graphicProvider =	xMultiComponentFactory->createInstanceWithContext(
				OUSTRING("com.sun.star.graphic.GraphicProvider"), xComponentContext);
		Reference<XGraphicProvider> xGraphicProvider(graphicProvider, UNO_QUERY);
		PropertyValuesHelper props(1);
		props.add("URL", Any(path));
		Reference<XGraphic> result = xGraphicProvider->queryGraphic(props.get());
		return result;
	}
	catch (Exception& ex) {
		std::cout << "Something went wrong." << ex.Message << std::endl;
	}
	return Reference<XGraphic>();
}

std::string ExtensionHelper::getSystemFilePathToExtensionFile(const char* file) {
	OUString filePath = getSystemFilePathFromURL(getURLToExtensionRoot() + convertToOUString(file));

	return convertToString(filePath);
}

Reference<XMultiComponentFactory> ExtensionHelper::getXMultiComponentFactory() {
	Reference<XMultiComponentFactory> xMultiComponentFactory(xComponentContext->getServiceManager(), UNO_QUERY);

	return xMultiComponentFactory;
}

Reference<XMultiServiceFactory> ExtensionHelper::getXMultiServiceFactory() {
	Reference<XMultiServiceFactory> xMultiServiceFactory(getXMultiComponentFactory(), UNO_QUERY);

	return xMultiServiceFactory;
}

Reference<XToolkit> ExtensionHelper::getXToolkit() {
	Reference<XMultiServiceFactory> xMultiServiceFactory = getXMultiServiceFactory();
	Reference<XToolkit> xToolkit(xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.Toolkit")), UNO_QUERY);

	return xToolkit;
}

Reference<XDesktop> ExtensionHelper::getXDesktop() {
	Reference<XMultiServiceFactory> xMultiServiceFactory = getXMultiServiceFactory();
	Reference<XDesktop> xDesktop(xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.frame.Desktop")), UNO_QUERY);

	return xDesktop;
}

Reference<XDispatchHelper> ExtensionHelper::getXDispatchHelper() {
	Reference<XMultiServiceFactory> xMultiServiceFactory = getXMultiServiceFactory();
	Reference<XDispatchHelper> xDispatchHelper(xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.frame.DispatchHelper")), UNO_QUERY);

	return xDispatchHelper;
}

void ExtensionHelper::setTitle(Reference<XComponent> xComponent, std::string documentName, bool isReadOnly) {
	Reference<XModel> xModel(xComponent, UNO_QUERY);
	Reference<XFrame> xFrame = xModel->getCurrentController()->getFrame();

	OUString title = convertToOUString(std::string("Provenance[")) +
			convertToOUString(documentName) +
			convertToOUString(
					std::string("] ") +
					(isReadOnly ? std::string("(read-only) ") : std::string("")) +
					std::string("- OpenOffice Writer"));

	Reference<XInterface> xInterface(xModel, UNO_QUERY);
	PropertySetHelper propertySetHelper(xInterface);
//	propertySetHelper.set("BackgroundColor", Any((sal_Int32) 0x00ffff00));
//	propertySetHelper.set("DisplayBackgroundColor", Any((sal_Int32) 0x00ffff00));
//	propertySetHelper.set("FillColor", Any((sal_Int32) 0x00ffff00));

	Reference<XTitle> xTitle;
	xTitle = Reference<XTitle>(xFrame, UNO_QUERY);
	if (xTitle.is())
		xTitle->setTitle(title);

	xTitle = Reference<XTitle>(xModel, UNO_QUERY);
	if (xTitle.is())
		xTitle->setTitle(convertToOUString(documentName));
/*
	Reference<XWindowPeer> xWindowPeer0(xFrame, UNO_QUERY);
	if (xWindowPeer0.is())
		xWindowPeer0->setBackground(0x00ffff00);

	Reference<XWindowPeer> xWindowPeer1(xFrame->getContainerWindow(), UNO_QUERY);
	if (xWindowPeer1.is())
		xWindowPeer1->setBackground(0x00ffff00);

	Reference<XWindowPeer> xWindowPeer2(xFrame->getComponentWindow(), UNO_QUERY);
	if (xWindowPeer2.is())
		xWindowPeer2->setBackground(0x00adff2f);
*/
}

Reference<XWindowPeer> ExtensionHelper::getHiddenXWindowPeer(Reference<XMultiServiceFactory> xMultiServiceFactory, Reference<XToolkit> xToolkit, bool visible) {
	Reference<XInterface> desktop = xMultiServiceFactory->createInstance(OUSTRING("com.sun.star.frame.Desktop"));	
	Reference<XDesktop> xDesktop(desktop, UNO_QUERY);
	Rectangle rectangle = xDesktop->getCurrentFrame()->getContainerWindow()->getPosSize();

	WindowDescriptor windowDescriptor;
	windowDescriptor.Type = WindowClass_TOP;
	windowDescriptor.WindowServiceName = OUString();
	windowDescriptor.ParentIndex = -1;
	windowDescriptor.Parent = xToolkit->getDesktopWindow();
	windowDescriptor.Bounds = Rectangle(rectangle.X, rectangle.Y, rectangle.Width, rectangle.Height);
	windowDescriptor.WindowAttributes = BORDER | MOVEABLE | SIZEABLE | CLOSEABLE;
	Reference<XWindowPeer> xWindowPeer = xToolkit->createWindow(windowDescriptor);
	Reference<XWindow> xWindow = Reference<XWindow>(xWindowPeer, UNO_QUERY);
	xWindow->setVisible(asSalBool(visible));
	return xWindowPeer;
}

Reference<XExtensionManager> ExtensionHelper::getXExtensionManager() {
	Reference<XExtensionManager> xExtensionManager(xComponentContext->getValueByName(
			OUSTRING("/singletons/com.sun.star.deployment.ExtensionManager")), UNO_QUERY);

	return xExtensionManager;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
