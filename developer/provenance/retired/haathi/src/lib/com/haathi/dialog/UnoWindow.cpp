//
// Implementation of UnoWindow
//
// Author: Aaron Gibson
// Date: September 10, 2011
//
// This is the implementation of an UnoWindow. This is different from UnoBaseWindow because
// it is not implemented as a dialog. This gives us more freedom with the initialization and
// creation of components because we don't have to run the cryptic 'execute()' method to be
// able to close the window. Furthermore, we can call dialogs from this function if we so 
// desire; the XMultiServiceFactory should be more varied in this class, unlike Dialogs where
// only the "UnoControl" and "UnoControlModel" services can actually be created.
// 

// Includes
// The obvious Include
#include "UnoWindow.hpp"

// System includes -- most should already be handled by the header file.
// In com::sun::star::awt
#include <com/sun/star/awt/WindowDescriptor.hpp>

// Namespaces

namespace com {
namespace haathi {
namespace window {

//======================================================================
// Constructors and Destructors
//======================================================================
UnoWindow::UnoWindow(Reference<XMultiServiceFactory> remoteMSF) {
//get the desktop service using createInstance returns an XInterface type
	Reference < XInterface > Desktop = remoteMSF->createInstance(
			OUString::createFromAscii("com.sun.star.frame.Desktop"));

//query for the XComponentLoader interface
	Reference < XComponentLoader > xComponentLoader(Desktop, UNO_QUERY);

	// We create the Toolkit for the window.
	xToolkit = Reference < XToolkit
			> (remoteMSF->createInstance(OUString(RTL_CONSTASCII_USTRINGPARAM(
					"com.sun.star.awt.Toolkit" ))), UNO_QUERY);
	WindowDescriptor aDescriptor;
	aDescriptor.Type = WindowClass_TOP;
	aDescriptor.WindowServiceName = OUString(RTL_CONSTASCII_USTRINGPARAM(""));
	aDescriptor.ParentIndex = -1;
	aDescriptor.Parent = xToolkit->getDesktopWindow();
	aDescriptor.Bounds = Rectangle(100, 200, 300, 400);
	aDescriptor.WindowAttributes = WindowAttribute::BORDER
			| WindowAttribute::MOVEABLE | WindowAttribute::SHOW
			| WindowAttribute::CLOSEABLE | WindowAttribute::SIZEABLE;
	Reference < XDesktop > rDesktop(Desktop, UNO_QUERY);
	Reference < XFramesSupplier > frameSupplier(Desktop, UNO_QUERY);

	// Call the helper function to finish the initialization of the Window.
	// Note that the xToolkit instance variable MUST be properly initialized
	// before calling this function!
	initializeUnoWindow(remoteMSF, frameSupplier, aDescriptor,
			OUString::createFromAscii("Window"), 0x00111111);
}

UnoWindow::UnoWindow(Reference<XMultiServiceFactory> remoteMSF,
		sal_Int32 attributes, sal_Int32 posx, sal_Int32 posy, sal_Int32 height,
		sal_Int32 width, OUString name) {
//get the desktop service using createInstance returns an XInterface type
	Reference < XInterface > Desktop = remoteMSF->createInstance(
			OUString::createFromAscii("com.sun.star.frame.Desktop"));

//query for the XComponentLoader interface
	Reference < XComponentLoader > xComponentLoader(Desktop, UNO_QUERY);

	// We create the Toolkit for the window.
	xToolkit = Reference < XToolkit
			> (remoteMSF->createInstance(OUString(RTL_CONSTASCII_USTRINGPARAM(
					"com.sun.star.awt.Toolkit" ))), UNO_QUERY);
	WindowDescriptor aDescriptor;
	aDescriptor.Type = WindowClass_TOP;
	aDescriptor.WindowServiceName = OUString(RTL_CONSTASCII_USTRINGPARAM(""));
	aDescriptor.ParentIndex = -1;
	aDescriptor.Parent = xToolkit->getDesktopWindow();
	aDescriptor.Bounds = Rectangle(posx, posy, width, height);
	aDescriptor.WindowAttributes = WindowAttribute::BORDER
			| WindowAttribute::MOVEABLE | WindowAttribute::SHOW
			| WindowAttribute::CLOSEABLE | WindowAttribute::SIZEABLE;
	Reference < XDesktop > rDesktop(Desktop, UNO_QUERY);
	Reference < XFramesSupplier > frameSupplier(Desktop, UNO_QUERY);

	// Call the helper function to finish the initialization of the Window.
	// Note that the xToolkit instance variable MUST be properly initialized
	// before calling this function!
	initializeUnoWindow(remoteMSF, frameSupplier, aDescriptor, name,
			(Color) 0x00111111);
}

UnoWindow::UnoWindow(Reference<XMultiServiceFactory> remoteMSF,
		sal_Int32 attributes, sal_Int32 posx, sal_Int32 posy, sal_Int32 height,
		sal_Int32 width, OUString name, Color color) {
//get the desktop service using createInstance returns an XInterface type
	Reference < XInterface > Desktop = remoteMSF->createInstance(
			OUString::createFromAscii("com.sun.star.frame.Desktop"));

//query for the XComponentLoader interface
	Reference < XComponentLoader > xComponentLoader(Desktop, UNO_QUERY);

	// We create the Toolkit for the window.
	xToolkit = Reference < XToolkit
			> (remoteMSF->createInstance(OUString(RTL_CONSTASCII_USTRINGPARAM(
					"com.sun.star.awt.Toolkit" ))), UNO_QUERY);
	WindowDescriptor aDescriptor;
	aDescriptor.Type = WindowClass_TOP;
	aDescriptor.WindowServiceName = OUString(RTL_CONSTASCII_USTRINGPARAM(""));
	aDescriptor.ParentIndex = -1;
	aDescriptor.Parent = xToolkit->getDesktopWindow();
	aDescriptor.Bounds = Rectangle(posx, posy, width, height);
	aDescriptor.WindowAttributes = WindowAttribute::BORDER
			| WindowAttribute::MOVEABLE | WindowAttribute::SHOW
			| WindowAttribute::CLOSEABLE | WindowAttribute::SIZEABLE;
	Reference < XDesktop > rDesktop(Desktop, UNO_QUERY);
	Reference < XFramesSupplier > frameSupplier(Desktop, UNO_QUERY);

	// Call the helper function to finish the initialization of the Window.
	// Note that the xToolkit instance variable MUST be properly initialized
	// before calling this function!
	initializeUnoWindow(remoteMSF, frameSupplier, aDescriptor, name, color);
}

UnoWindow::UnoWindow(Reference<XMultiServiceFactory> remoteMSF,
		WindowDescriptor aDescriptor) {
	//get the desktop service using createInstance returns an XInterface type
	Reference < XInterface > Desktop = remoteMSF->createInstance(
			OUString::createFromAscii("com.sun.star.frame.Desktop"));

//query for the XComponentLoader interface
	Reference < XComponentLoader > xComponentLoader(Desktop, UNO_QUERY);

	// We create the Toolkit for the window.
	xToolkit = Reference < XToolkit
			> (remoteMSF->createInstance(OUString(RTL_CONSTASCII_USTRINGPARAM(
					"com.sun.star.awt.Toolkit" ))), UNO_QUERY);
	Reference < XDesktop > rDesktop(Desktop, UNO_QUERY);
	Reference < XFramesSupplier > frameSupplier(Desktop, UNO_QUERY);
	initializeUnoWindow(remoteMSF, frameSupplier, aDescriptor,
			OUString::createFromAscii("Window"), 0x00111111);
}

UnoWindow::UnoWindow(const UnoWindow& other) {
	// TODO -- finish this.
}

UnoWindow& UnoWindow::operator=(const UnoWindow& other) {
	// TODO -- finish this.
	return *this;
}
UnoWindow::~UnoWindow() {

}

//======================================================================
// Initialization Functions
//======================================================================
void UnoWindow::initializeUnoWindow(Reference<XMultiServiceFactory> remoteMSF,
		Reference<XFramesSupplier> frameSupplier, WindowDescriptor descriptor,
		OUString name, Color backgroundColor) {
	// At this point, if you stop the program, you will have a new OOo window on the screen, 
	// but you cannot do anything  with it.  You cannot even close it!
	xWindowPeer = xToolkit->createWindow(descriptor);
	xWindow = Reference < XWindow > (xWindowPeer, UNO_QUERY);
	xWindowPeer->setBackground(backgroundColor);

	// create a new frame
	xFrame =
			Reference < XFrame
					> (remoteMSF->createInstance(
							OUString(
									RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Frame" )
									)), UNO_QUERY);
	xFrame->initialize(xWindow);
	xFrame->setCreator(frameSupplier);
	xFrame->setName(name);
}

void UnoWindow::setWindowProperties(sal_Int32 posx, sal_Int32 posy,
		sal_Int32 height, sal_Int32 width) {
	WindowDescriptor aDescriptor;
	aDescriptor.Type = WindowClass_TOP;
	aDescriptor.WindowServiceName = OUString(RTL_CONSTASCII_USTRINGPARAM(""));
}

//======================================================================
// Constructors and Destructors
//======================================================================

//======================================================================
// Static Functions
//======================================================================
UnoWindow* UnoWindow::createUnoWindow(
		Reference<XMultiServiceFactory> remoteMSF) {
	return new UnoWindow(remoteMSF);
}
/*
 protected:
 // Instance variables that are required to create a window.
 Reference< XMultiServiceFactory xMSF;
 
 Reference< XToolkit > xToolkit;
 Reference< XWindow > xWindow;
 Reference< XWindowPeer > xWindowPeer;
 Reference< XFrame > xFrame;

 public:
 UnoWindow();
 virtual ~UnoWindow();

 // Initialization Functions
 void setWindowProperties(sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width, 	
 WindowAttribute props);
 // Insertion-of-Component functions.
 };
 */

}
;
}
;
}
;
// namespace com::haathi::window

