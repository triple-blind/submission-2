//
// File: ModelessWindow.cxx
//
// Author: Aaron Gibson
// Date: September 15, 2012
//
// This file implements the details of the header "ModelessWindow.hpp".
//
// For the licensing of this file, see the README.txt at the project head.

#include <com/sun/star/awt/InvalidateStyle.hpp>

// My includes
#include "com/haathi/window/ModelessWindow.hpp"

#include "com/haathi/utils/StringWrapper.hpp"

// These strings are used very commonly used. They aren't macros, but still.
static OUString ToolkitServiceStr = constOUString("com.sun.star.awt.Toolkit");
static OUString DesktopServiceStr = constOUString("com.sun.star.frame.Desktop");
static OUString FrameServiceStr = constOUString("com.sun.star.frame.Frame");

static OUString ButtonStr = constOUString("com.sun.star.awt.UnoControlButton");
static OUString ButtonModelStr =
		constOUString("com.sun.star.awt.UnoControlButtonModel");
static OUString CheckboxStr =
		constOUString("com.sun.star.awt.UnoControlCheckBox");
static OUString CheckboxModelStr =
		constOUString("com.sun.star.awt.UnoControlCheckBoxModel");
static OUString ImageControlStr =
		constOUString("com.sun.star.awt.UnoImageControl");
static OUString ImageControlModelStr =
		constOUString("com.sun.star.awt.UnoImageControlModel");

// Property Names.
static OUString BackgroundColorStr = constOUString("BackgroundColor");
static OUString BorderStr = constOUString("Border");
static OUString GraphicStr = constOUString("Graphic");
static OUString HeightStr = constOUString("Height");
static OUString LabelStr = constOUString("Label");
static OUString NameStr = constOUString("Name");
static OUString PositionXStr = constOUString("PositionX");
static OUString PositionYStr = constOUString("PositionY");
static OUString ScaleImageStr = constOUString("ScaleImage");
static OUString TristateStr = constOUString("Tristate");
static OUString WidthStr = constOUString("Width");

// Test includes
#include "com/haathi/utils/IntrospectionUtils.hpp"

namespace com {
namespace haathi {
namespace window {

// Implementation of Modeless Window
//=========================================================
// Constructors and Destructors
//=========================================================
ModelessWindow::ModelessWindow(Reference<XMultiComponentFactory> serviceFactory,
		Reference<XComponentContext> context, WindowDescriptor descriptor) :
		xMCF(serviceFactory, UNO_QUERY_THROW), xContext(context,
				UNO_QUERY_THROW) {
	// First, create the Toolkit.
	xToolkit = Reference<XToolkit>(
			xMCF->createInstanceWithContext(ToolkitServiceStr, context),
			UNO_QUERY_THROW);

	// Now, create the window.
	xWindowPeer = xToolkit->createWindow(descriptor);
	xWindow = Reference<XWindow>(xWindowPeer, UNO_QUERY_THROW);

	// Create a frame.
	xFrame = Reference<XFrame>(
			xMCF->createInstanceWithContext(FrameServiceStr, xContext),
			UNO_QUERY_THROW);

	// Link the frame and window.
	xFrame->initialize(xWindow);

	// Add the frame to the desktop hierarchy.
	Reference<XFramesSupplier> xSupplier = Reference<XFramesSupplier>(
			xMCF->createInstanceWithContext(DesktopServiceStr, xContext),
			UNO_QUERY_THROW);
	Reference<XFrames> framesContainer = xSupplier->getFrames();
	framesContainer->append(xFrame);

	// Now for the final intializations.
	xWindowPeer->setBackground(0x00000000);
	xWindow->setVisible(sal_True );
	xFrame->setName(OUString());

	// For the hell of it, let's print all available services and interfaces
	// that this window has.
	std::cout << "XWindow:\n";
	printTypesToConsole(xWindow);
	printPropertiesToConsole(xWindow);
	std::cout << "XFrame:\n";
	printTypesToConsole(xFrame);
	printPropertiesToConsole(xFrame);
	std::cout << "XToolkit:\n";
	printTypesToConsole(xToolkit);
	printPropertiesToConsole(xToolkit);
// Look into XPropertySetInfo...	
}

ModelessWindow::~ModelessWindow() {
	// Is there anything to do?
	xFrame->dispose();
}

//=========================================================
// Private Methods
//=========================================================
void ModelessWindow::linkAndAddToWindow(Reference<XInterface> model,
		Reference<XInterface> control) {
	// Query and verify the inputs.
	Reference<XControlModel> xModel(model, UNO_QUERY);
	Reference<XControl> xControl(control, UNO_QUERY);
	if (!xModel.is()) {
		// We don't have a proper model to set!
		std::cout
				<< "ERROR: xModel in linkAndAddToWindow() is not valid! Not linking..."
				<< std::endl;
		return;
	}
	if (!xControl.is()) {
		std::cout
				<< "ERROR: xControl in linkAndAddToWindow() is not valid! Not linking..."
				<< std::endl;
		return;
	}
	// Here, we link the model with the control.
	xControl->setModel(xModel);

	// Next, we add the control to the parent window.
	xControl->createPeer(xToolkit, xWindowPeer);

	// Later, we can also do some bookkeeping, like adding these to a list for future access.
}

//=========================================================
// Public Methods
//=========================================================

sal_Bool ModelessWindow::isVisible() const {
	return sal_False ;
}

void ModelessWindow::setVisible(sal_Bool visible) {
	xWindow->setVisible(visible);
}

void ModelessWindow::redraw() {
	sal_Int16 flags = InvalidateStyle::CHILDREN | InvalidateStyle::UPDATE
			| InvalidateStyle::TRANSPARENT;
	xWindowPeer->invalidate(flags);
}
//=========================================================
// Insert Methods
//=========================================================
Reference<XPropertySet> ModelessWindow::insertImage(sal_Int32 posx,
		sal_Int32 posy, sal_Int32 height, sal_Int32 width, OUString name,
		Reference<XGraphic> graphic, Sequence<OUString> additionalPropNames,
		Sequence<Any> additionalPropValues) {
	// Create the model.
	Reference<XInterface> imageModel = xMCF->createInstanceWithContext(
			ImageControlModelStr, xContext);
	Reference<XMultiPropertySet> xmultiProps(imageModel, UNO_QUERY);

	// Set all of the properties of the model.
	Sequence<OUString> names(8);
	Sequence<Any> values(8);

	// Set the "alpha" channel to 0.
	values[0] <<= ((com::sun::star::util::Color) 0xff000000);
	names[0] = BackgroundColorStr;
	values[1] <<= ((sal_Int16) 0);	// 0 indicates "No Border"
	names[1] = BorderStr;
	values[2] <<= height;
	names[2] = HeightStr;
	values[3] <<= name;
	names[3] = NameStr;
	values[4] <<= posx;
	names[4] = PositionXStr;
	values[5] <<= posy;
	names[5] = PositionYStr;
	values[6] <<= sal_True;
	names[6] = ScaleImageStr;
	values[7] <<= width;
	names[7] = WidthStr;

	// Add the properties
	xmultiProps->setPropertyValues(names, values);

	Reference<XPropertySet> imageProps(imageModel, UNO_QUERY);
	Any val;
	if (graphic.is()) {
		val <<= graphic;
		imageProps->setPropertyValue(GraphicStr, val);
	}
	sal_Int32 i;
	for (i = 0; i < additionalPropNames.getLength(); ++i) {
		imageProps->setPropertyValue(additionalPropNames[i],
				additionalPropValues[i]);
	}

	// Create the control.
	Reference<XInterface> imageControl = xMCF->createInstanceWithContext(
			ImageControlStr, xContext);

	// Link the model and control, then add to the window.
	linkAndAddToWindow(imageModel, imageControl);

	// Return the XPropertySet reference.
	return imageProps;
}

Reference<XCheckBox> ModelessWindow::insertCheckbox(sal_Int32 posx,
		sal_Int32 posy, sal_Int32 height, sal_Int32 width, OUString name,
		OUString label, sal_Bool enable_tristate,
		Sequence<OUString> additionalPropNames,
		Sequence<Any> additionalPropValues) {
	Reference<XInterface> checkboxModel = xMCF->createInstanceWithContext(
			ButtonModelStr, xContext);
	Reference<XMultiPropertySet> xmultiprops(checkboxModel, UNO_QUERY);

	Sequence<OUString> defaultNames(7);
	Sequence<Any> defaultValues(7);

	defaultValues[0] <<= height;
	defaultNames[0] = HeightStr;
	defaultValues[1] <<= label;
	defaultNames[1] = LabelStr;
	defaultValues[2] <<= name;
	defaultNames[2] = NameStr;
	defaultValues[3] <<= posx;
	defaultNames[3] = PositionXStr;
	defaultValues[4] <<= posy;
	defaultNames[4] = PositionYStr;
	defaultValues[5] <<= enable_tristate;
	defaultNames[5] = TristateStr;
	defaultValues[6] <<= width;
	defaultNames[6] = WidthStr;

	// Now, add any custom properties.
	Reference<XPropertySet> xprops(checkboxModel, UNO_QUERY);
	sal_Int32 i;
	for (i = 0; i < additionalPropNames.getLength(); ++i) {
		xprops->setPropertyValue(additionalPropNames[i],
				additionalPropValues[i]);
	}

	// Create the control.
	Reference<XInterface> checkboxControl = xMCF->createInstanceWithContext(
			CheckboxStr, xContext);

	// Link them together.
	linkAndAddToWindow(checkboxModel, checkboxControl);

	Reference<XCheckBox> xcheckbox(checkboxModel, UNO_QUERY);
	return xcheckbox;
}

Reference<XButton> ModelessWindow::insertButton(
		Reference<XActionListener> listener, sal_Int32 posx, sal_Int32 posy,
		sal_Int32 height, sal_Int32 width, OUString name, OUString label,
		Sequence<OUString> additionalPropNames,
		Sequence<Any> additionalPropValues) {
	Reference<XInterface> buttonModel = xMCF->createInstanceWithContext(
			ButtonModelStr, xContext);

	Reference<XMultiPropertySet> xmultiProps(buttonModel, UNO_QUERY_THROW);

	Sequence<OUString> names(6);
	Sequence<Any> values(6);
	values[0] <<= height;
	names[0] = HeightStr;
	values[1] <<= label;
	names[1] = LabelStr;
	values[2] <<= name;
	names[2] = NameStr;
	values[3] <<= posx;
	names[3] = PositionXStr;
	values[4] <<= posy;
	names[4] = PositionYStr;
	values[5] <<= width;
	names[5] = WidthStr;

	xmultiProps->setPropertyValues(names, values);

	Reference<XPropertySet> xprops(buttonModel, UNO_QUERY_THROW);
	sal_Int32 i;
	for (i = 0; i < additionalPropNames.getLength(); ++i) {
		xprops->setPropertyValue(additionalPropNames[i],
				additionalPropValues[i]);
	}

	// Create the control.
	Reference<XInterface> xcontrol = xMCF->createInstanceWithContext(ButtonStr,
			xContext);
	linkAndAddToWindow(buttonModel, xcontrol);

	// Add the listener.
	Reference<XButton> xButton(xcontrol, UNO_QUERY_THROW);
	if (listener.is()) {
		xButton->addActionListener(listener);
	}

	return xButton;
}

}
;
}
;
}
;
// namespace com::haathi::window
