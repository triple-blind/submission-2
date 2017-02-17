// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/dialog/BaseDialog.cpp#1 $

// The obvious include...
#include "Config.hpp"
#include "com/haathi/dialog/BaseDialog.hpp"

// Other includes that aren't already included in the header.
// In com::sun::star::awt
#include <com/sun/star/awt/MessageBoxButtons.hpp>
#include <com/sun/star/awt/Rectangle.hpp>
#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <com/sun/star/awt/WindowClass.hpp>
#include <com/sun/star/awt/XMessageBox.hpp>
#include <com/sun/star/awt/XMessageBoxFactory.hpp>
// In com::sun::star::util
#include <com/sun/star/util/Color.hpp>
#include <com/sun/star/util/XMacroExpander.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <string>
// Exceptions
// In com::sun::star::io
#include <com/sun/star/io/IOException.hpp>
// In com::sun::star::lang
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/lang/WrappedTargetException.hpp>

// Helper functions
#include "com/haathi/utils/ExtensionHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

// Configuration for registering with OpenOffice (or Libreoffice).
#include "com/haathi/utils/Configuration.hpp"

// Includes and namespaces for a test.
#include <com/sun/star/uno/XComponentContext.hpp>

#include <com/sun/star/lang/XTypeProvider.hpp>

#include "com/haathi/utils/IntrospectionUtils.hpp"

// namespaces
using namespace com::sun::star::io;
using namespace com::sun::star::uno;

namespace com {
namespace haathi {
namespace dialog {

//==================================================================================
// Helper class for page-loading listeners.
//==================================================================================	

//==================================================================================
// Constructors and Destructors
//==================================================================================
Reference<XMultiServiceFactory> asMSF(Reference<XComponentContext> xContext) {
	Reference<XMultiComponentFactory> xMCF(xContext->getServiceManager(), UNO_QUERY);
	Reference<XMultiServiceFactory> xMSF(xMCF, UNO_QUERY);
	
	return xMSF;
}

BaseDialog::BaseDialog(Reference<XComponentContext> xContext,
		sal_Int32 height, sal_Int32 width, OUString title):
		isModal(sal_False) {
	Reference<XMultiServiceFactory> remoteMSF = asMSF(xContext);
	Reference<XInterface> Desktop = remoteMSF->createInstance(
			OUSTRING("com.sun.star.frame.Desktop"));
	Reference<XComponentLoader> rComponentLoader(Desktop, UNO_QUERY);

	baseModel = remoteMSF->createInstance(OUSTRING("com.sun.star.awt.UnoControlDialogModel"));
	windowProps = Reference<XPropertySet>(baseModel, UNO_QUERY);
	setWindowProperties(height, width, title);

	// The dialog model is also a service factory that creates dialog-specific
	// components such as listboxes, buttons, and textfields.
	xMSF = Reference<XMultiServiceFactory>(baseModel, UNO_QUERY);
	base = remoteMSF->createInstance(
			OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.awt.UnoControlDialog")));
	xControl = Reference<XControl>(base, UNO_QUERY);

	Reference<XControlModel> xControlModel(baseModel, UNO_QUERY);
	xControl->setModel(xControlModel);

	// The "xNameContainer" is a list of all of the components that are in this
	// dialog. Each component is referenced by a unique name. 
	xNameContainer = Reference<XNameContainer>(baseModel, UNO_QUERY);

	// The "xControlContainer" is a list of all of the controls for the components
	// that are in this dialog. Each component is referenced by a unique name. In
	// fact, this name is the SAME unique name that is used to reference the components
	// themselves in the xNameContainer.
	xControlContainer = Reference<XControlContainer>(base, UNO_QUERY);

	// Create the Toolkit.
	Reference<XInterface> toolkit = remoteMSF->createInstance(
			OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.awt.Toolkit")));
	xToolkit = Reference<XToolkit>(toolkit, UNO_QUERY);

	// Now, finish the creation, depending on the type of window.
	if (true) {
		// If a modal dialog is required, then we already have everything set up.
		// Just link the controller, the window peer and the toolkit.
		if (!xWindowPeer.is()) {
			xWindowPeer = xToolkit->getDesktopWindow();
		}
		xControl->createPeer(xToolkit, xWindowPeer);
		xWindowPeer = xControl->getPeer();
		xWindow = Reference<XWindow>(base, UNO_QUERY);
	} 
}

BaseDialog::BaseDialog(Reference<XComponentContext> _xContext, sal_Bool modal):
		dialog_result(0), page(1), isModal(modal), xContext(_xContext) {
	Reference<XMultiServiceFactory> tmpMSF = asMSF(xContext);

	// A generic type for all UNO objects; we use it to pass parameters.
	Any val;

	Reference<XInterface> tmpDesktop = tmpMSF->createInstance(
			OUSTRING("com.sun.star.frame.Desktop"));

	// Get the component loader.
	Reference<XComponentLoader> tmpComponentLoader(tmpDesktop, UNO_QUERY);

	// Create the model of the dialog.
	baseModel = tmpMSF->createInstance(
			OUSTRING(
					"com.sun.star.awt.UnoControlDialogModel"));
	windowProps = Reference<XPropertySet>(baseModel, UNO_QUERY);
	// kwa: can set size here already!


	// The dialog model is also a service factory that creates dialog-specific
	// components such as listboxes, buttons, and textfields.
	xMSF = Reference<XMultiServiceFactory>(baseModel, UNO_QUERY);

	// Create the controls for the dialog.
	base =
			tmpMSF->createInstance(
					OUString(
							RTL_CONSTASCII_USTRINGPARAM("com.sun.star.awt.UnoControlDialog")
							));

	// Now that we have created the baseWindow, we get all of the interfaces that it implements
	// and store them into their own location, since these are commonly used.
	xControl = Reference<XControl>(base, UNO_QUERY);

	Reference<XControlModel> xControlModel(baseModel, UNO_QUERY);
	xControl->setModel(xControlModel);

	// The "xNameContainer" is a list of all of the components that are in this
	// dialog. Each component is referenced by a unique name. 
	xNameContainer = Reference<XNameContainer>(baseModel, UNO_QUERY);

	// The "xControlContainer" is a list of all of the controls for the components
	// that are in this dialog. Each component is referenced by a unique name. In
	// fact, this name is the SAME unique name that is used to reference the components
	// themselves in the xNameContainer.
	xControlContainer = Reference<XControlContainer>(base, UNO_QUERY);

	// Create the Toolkit.
	Reference<XInterface> toolkit = tmpMSF->createInstance(
			OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.awt.Toolkit")));
	xToolkit = Reference<XToolkit>(toolkit, UNO_QUERY);

	// Now, finish the creation, depending on the type of window.
	if (isModal == sal_True ) {
		// If a modal dialog is required, then we already have everything set up.
		// Just link the controller, the window peer and the toolkit.
		if (!xWindowPeer.is()) {
			xWindowPeer = xToolkit->getDesktopWindow();
		}
		xControl->createPeer(xToolkit, xWindowPeer);
		xWindowPeer = xControl->getPeer();
		xWindow = Reference<XWindow>(base, UNO_QUERY);

	} else {
		// Technically, modeless dialogs are not really supported directly with the 
		// UnoControlDialogModel service, so we use a hack; create an invisible window
		// to serve as the "parent". This means that any other window can take focus
		// while this window is active, thus giving the illusion that it is modeless.
		WindowDescriptor aDescriptor;
		aDescriptor.Type = WindowClass_TOP;
		aDescriptor.WindowServiceName = OUString();
		aDescriptor.ParentIndex = -1;
		aDescriptor.Parent = xToolkit->getDesktopWindow();
		aDescriptor.Bounds = Rectangle(1, 1, 1, 1);
		aDescriptor.WindowAttributes = WindowAttribute::BORDER
				| WindowAttribute::MOVEABLE | WindowAttribute::SIZEABLE
				| WindowAttribute::CLOSEABLE;
		xWindowPeer = xToolkit->createWindow(aDescriptor);

		xControl->createPeer(xToolkit, xWindowPeer);

		xWindow = Reference<XWindow>(xWindowPeer, UNO_QUERY);
		xWindow->setVisible(sal_False );

		// Now we've created the invisible window. Reset the other components to reference
		// the dialog itself.
		xWindowPeer = xControl->getPeer();
		xWindow = Reference<XWindow>(base, UNO_QUERY);
	}
#define DEBUG
#ifdef DEBUG
	std::cout << "Base Dialog: xWindow\n";
	inspectInterface(xWindow, xContext);
	std::cout << "Base Dialog: xControl\n";
	inspectInterface(xControl, xContext);
	std::cout << "Base Dialog: baseModel\n";
	inspectInterface(baseModel, xContext);
#endif
}

BaseDialog::~BaseDialog() {
	// The documentation of OpenOffice suggests to always call "dispose()" when closing to 
	// make sure that everything is actually destroyed. This seems to solve a bug where the
	// dialog will crash OpenOffice if it is opened twice.
	xWindow->dispose();
}

BaseDialog& BaseDialog::operator=(const BaseDialog& other) {
	xMCF = other.xMCF;
	xContext = other.xContext;

	xMSF = other.xMSF;
	baseModel = other.baseModel;
	base = other.base;
	windowProps = other.windowProps;
	xNameContainer = other.xNameContainer;
	xControlContainer = other.xControlContainer;

	xControl = other.xControl;
	xWindow = other.xWindow;

	return *this;
}

BaseDialog::BaseDialog(const BaseDialog& other) :
		xMCF(other.xMCF), xContext(other.xContext), xMSF(other.xMSF), baseModel(
				other.baseModel), base(other.base), windowProps(
				other.windowProps), xNameContainer(other.xNameContainer), xControlContainer(
				other.xControlContainer), xControl(other.xControl), xWindow(
				other.xWindow), isModal(other.isModal), dialog_result(other.dialog_result) {
	// Everything should already be moved over.
}

std::string BaseDialog::getSystemFilePathToExtensionFile(const char* file) {
	return com::haathi::utils::getSystemFilePathToExtensionFile(xContext, file);
}

Reference<XGraphic> BaseDialog::getGraphicFromFile(const char* file) {
	return com::haathi::utils::getGraphicFromFile(xContext, convertToOUString(file), sal_True);
}
//==================================================================================
// Initialization-esque functions
//==================================================================================
void BaseDialog::setWindowProperties(sal_Int32 height, sal_Int32 width,
		OUString title, sal_Bool closeable) {
	Any val;

	val <<= height;
	windowProps->setPropertyValue(OUSTRING("Height"), val);
	val <<= width;
	windowProps->setPropertyValue(OUSTRING("Width"), val);
	val <<= title;
	windowProps->setPropertyValue(OUSTRING("Title"), val);
}

void BaseDialog::setWindowProperties(sal_Int32 xpos, sal_Int32 ypos,
		sal_Int32 height, sal_Int32 width, OUString title, sal_Bool closeable) {
	// We use the XWindow interface to set the size.
//std::cout << "Setting Properties";
//	xWindow->setPosSize(xpos, ypos, width, height, PosSize::POSSIZE);
	Any val;
	val <<= xpos;
	windowProps->setPropertyValue(OUSTRING("PositionX"), val);
	val <<= ypos;
	windowProps->setPropertyValue(OUSTRING("PositionY"), val);
	val <<= height;
	windowProps->setPropertyValue(OUSTRING("Height"), val);
	val <<= width;
	windowProps->setPropertyValue(OUSTRING("Width"), val);
	val <<= title;
	windowProps->setPropertyValue(OUSTRING("Title"), val);
}

sal_Int16 BaseDialog::run(sal_Bool visible) {
	// Load the current page.
	loadPage(page);

	// Execute the dialog (This is so that the window actually closes when expected!)
	Reference<XDialog> xDialog(base, UNO_QUERY);
	sal_Int16 result = xDialog->execute();
	return dialog_result;
}

void BaseDialog::close() {
	Reference<XDialog> xDialog(base, UNO_QUERY);
	xDialog->endExecute();
}

void BaseDialog::endExecute() {
	close();
}


void BaseDialog::loadPage(sal_Int16 _page) {
	Any val;
	page = _page;
	val <<= this->page;
	windowProps->setPropertyValue(OUSTRING("Step"), val);
	repaint();
}

//==================================================================================
// Insertion Functions
//==================================================================================
//--------------------------
// Insert Image function
//--------------------------
Reference<XPropertySet> BaseDialog::insertImageOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, Reference<XGraphic> graphic,
		Sequence<OUString> propNames, Sequence<Any> propValues) {
	Reference<XInterface> imageModel = xMSF->createInstance(
			OUSTRING(
					"com.sun.star.awt.UnoControlImageControlModel"));

	// Use "XMultiPropertySet" because it is supposed to be faster...
	Reference<XMultiPropertySet> xmultiprops(imageModel, UNO_QUERY);

	Sequence<OUString> names(9);
	Sequence<Any> values(9);

	// Set the "alpha" channel to 0.
	values[0] <<= ((com::sun::star::util::Color) 0xff000000);
	names[0] = OUSTRING("BackgroundColor");
	values[1] <<= ((sal_Int16) 0);	// 0 indicates "No Border"
	names[1] = OUSTRING("Border");
	values[2] <<= height;
	names[2] = OUSTRING("Height");
	values[3] <<= name;
	names[3] = OUSTRING("Name");
	values[4] <<= posx;
	names[4] = OUSTRING("PositionX");
	values[5] <<= posy;
	names[5] = OUSTRING("PositionY");
	values[6] <<= sal_True;
	names[6] = OUSTRING("ScaleImage");
	values[7] <<= page;
	names[7] = OUSTRING("Step");
	values[8] <<= width;
	names[8] = OUSTRING("Width");

	// Add the properties
	xmultiprops->setPropertyValues(names, values);

	// Now, add any additional properties
	Reference<XPropertySet> imageProps(imageModel, UNO_QUERY);
	Any val;
	if (graphic.is()) {
		val <<= graphic;
		imageProps->setPropertyValue(OUSTRING("Graphic"), val);
	}

	sal_Int32 i;
	for (i = 0; i < propNames.getLength(); i++) {
		imageProps->setPropertyValue(propNames[i], propValues[i]);
	}

	// Add the image control to the window.
	val <<= imageModel;
	xNameContainer->insertByName(name, val);

// FOR DEBUGGING
//	Reference< XInterface > imageControl = xControlContainer->getControl(name);
//	Reference< XTypeProvider > xinfo(imageControl, UNO_QUERY);
//	Sequence< Type > list = xinfo->getTypes();
//	for(i = 0; i < list.getLength(); ++i){
//		std::cout << list[i].getTypeName() << std::endl;
//	}

	return imageProps;
}

//--------------------------
// insertButton functions
//--------------------------
Reference<XButton> BaseDialog::insertButtonOnPage(sal_Int16 page,
		Reference<XActionListener> _listener, sal_Int32 posx, sal_Int32 posy,
		sal_Int32 height, sal_Int32 width, OUString name, OUString label,
		Sequence<OUString> propNames, Sequence<Any> propValues) {

	// Use the service factory for this window.
	Reference<XInterface> buttonModel =
			xMSF->createInstance(
					OUString(
							RTL_CONSTASCII_USTRINGPARAM("com.sun.star.awt.UnoControlButtonModel")
							));

	Reference<XMultiPropertySet> xmultiprops(buttonModel, UNO_QUERY);

	// Now, we add all of the properties of the button...
	Sequence<OUString> names(7);
	Sequence<Any> values(7);

	values[0] <<= height;
	names[0] = OUSTRING("Height");
	values[1] <<= label;
	names[1] = OUSTRING("Label");
	values[2] <<= name;
	names[2] = OUSTRING("Name");
	values[3] <<= posx;
	names[3] = OUSTRING("PositionX");
	values[4] <<= posy;
	names[4] = OUSTRING("PositionY");
	values[5] <<= page;
	names[5] = OUSTRING("Step");
	values[6] <<= width;
	names[6] = OUSTRING("Width");

	xmultiprops->setPropertyValues(names, values);

	// Now, we pass any additional properties.
	Reference<XPropertySet> buttonProps(buttonModel, UNO_QUERY);
	sal_Int32 i;
	for (i = 0; i < propNames.getLength(); i++) {
		buttonProps->setPropertyValue(propNames[i], propValues[i]);
	}

	// Add the button to the window.
	Any val;
	val <<= buttonModel;
	xNameContainer->insertByName(name, val);

	// Add the listener to the button if it exists, and return the result.
	Reference<XInterface> objectButton = xControlContainer->getControl(name);
	Reference<XButton> xButton(objectButton, UNO_QUERY);
	if (_listener.is()) {
		xButton->addActionListener(_listener);
	}
	return xButton;
}

Reference<XCheckBox> BaseDialog::insertCheckboxOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, OUString label, sal_Bool enable_tristate,
		Sequence<OUString> names, Sequence<Any> values) {
	// Needed to pass parameters, as always.
	Any val;

	// Create the checkbox service
	Reference<XInterface> xcheckboxModel = xMSF->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlCheckBoxModel"));

	Sequence<OUString> defaultNames(8);
	Sequence<Any> defaultValues(8);

	defaultValues[0] <<= height;
	defaultNames[0] = OUSTRING("Height");
	defaultValues[1] <<= label;
	defaultNames[1] = OUSTRING("Label");
	defaultValues[2] <<= name;
	defaultNames[2] = OUSTRING("Name");
	defaultValues[3] <<= posx;
	defaultNames[3] = OUSTRING("PositionX");
	defaultValues[4] <<= posy;
	defaultNames[4] = OUSTRING("PositionY");
	defaultValues[5] <<= page;
	defaultNames[5] = OUSTRING("Step");
	defaultValues[6] <<= enable_tristate;
	defaultNames[6] = OUSTRING("Tristate");
	defaultValues[7] <<= width;
	defaultNames[7] = OUSTRING("Width");

	// Set the constructed properties
	Reference<XMultiPropertySet> xmultiprops(xcheckboxModel, UNO_QUERY);
	xmultiprops->setPropertyValues(defaultNames, defaultValues);

	// Set any additional properties
	Reference<XPropertySet> xprops(xcheckboxModel, UNO_QUERY);
	sal_Int32 i;
	for (i = 0; i < names.getLength(); ++i) {
		val <<= values[i];
		xprops->setPropertyValue(names[i], val);
	}

	// Now, add the control to the dialog.
	// Add the listbox to the current window.
	val <<= xcheckboxModel;
	xNameContainer->insertByName(name, val);

	// Add the data to the listbox, as well as listeners.
	Reference<XInterface> objectCheckbox = xControlContainer->getControl(name);
	Reference<XCheckBox> xcheckbox(objectCheckbox, UNO_QUERY);
	return xcheckbox;
}

//--------------------------
// insertListbox (one-column table, with ability to add listeners)
// and related functions
//--------------------------
Reference<XListBox> BaseDialog::insertListBoxOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, Sequence<OUString> list_items, sal_Bool multi_select,
		Sequence<OUString> propNames, Sequence<Any> propVals) {
	// Needed to pass properties.
	Any val;

	// Create the listbox service
	Reference<XInterface> xlistBoxModel = xMSF->createInstance(
			OUSTRING(
					"com.sun.star.awt.UnoControlListBoxModel"));

	Sequence<OUString> defaultNames(6);
	Sequence<Any> defaultValues(6);

	defaultValues[0] <<= height;
	defaultNames[0] = OUSTRING("Height");
	defaultValues[1] <<= name;
	defaultNames[1] = OUSTRING("Name");
	defaultValues[2] <<= posx;
	defaultNames[2] = OUSTRING("PositionX");
	defaultValues[3] <<= posy;
	defaultNames[3] = OUSTRING("PositionY");
	defaultValues[4] <<= page;
	defaultNames[4] = OUSTRING("Step");
	defaultValues[5] <<= width;
	defaultNames[5] = OUSTRING("Width");

	// Set the specified properties
	Reference<XMultiPropertySet> xmultiprops(xlistBoxModel, UNO_QUERY);
	xmultiprops->setPropertyValues(defaultNames, defaultValues);

	// Set any additional properties.
	Reference<XPropertySet> listBoxProps(xlistBoxModel, UNO_QUERY);

	sal_Int32 i;
	for (i = 0; i < propNames.getLength(); ++i) {
		val <<= propVals[i];
		listBoxProps->setPropertyValue(propNames[i], val);
	}

	// Add the listbox to the current window.
	val <<= xlistBoxModel;
	xNameContainer->insertByName(name, val);

	// Add the data to the listbox, as well as listeners.
	Reference<XInterface> objectListBox = xControlContainer->getControl(name);
	Reference<XListBox> xListBox(objectListBox, UNO_QUERY);

	// Remove everything first, just in case; then add everything.
	xListBox->removeItems(0, xListBox->getItemCount());
	xListBox->addItems(list_items, 0);
	xListBox->setMultipleMode(multi_select);
	xListBox->makeVisible(sal_Int16(0));
	return xListBox;
}

//-----------------------------------
// Insert ComboBox
//-----------------------------------
Reference<XComboBox> BaseDialog::insertComboBoxOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, Sequence<OUString> item_list, Sequence<OUString> names,
		Sequence<Any> values) {
	Reference<XInterface> ocombo =
			xMSF->createInstance(
					OUString(
							RTL_CONSTASCII_USTRINGPARAM("com.sun.star.awt.UnoControlComboBoxModel")
							));
	if (!ocombo.is()) {
		// Error
	}

	Reference<XMultiPropertySet> xmultiprops(ocombo, UNO_QUERY);
	Sequence<OUString> propNames(8);
	Sequence<Any> propValues(8);

	propNames[0] = OUSTRING("Dropdown");
	propValues[0] <<= ((sal_Bool) sal_True );
	propNames[1] = OUSTRING("Height");
	propValues[1] <<= height;
	propNames[2] = OUSTRING("Name");
	propValues[2] <<= name;
	propNames[3] = OUSTRING("PositionX");
	propValues[3] <<= posx;
	propNames[4] = OUSTRING("PositionY");
	propValues[4] <<= posy;
	propNames[5] = OUSTRING("Step");
	propValues[5] <<= page;
	propNames[6] = OUSTRING("StringItemList");
	propValues[6] <<= item_list;
	propNames[7] = OUSTRING("Width");
	propValues[7] <<= width;

	xmultiprops->setPropertyValues(propNames, propValues);

	// Set any additional properties.
	Reference<XPropertySet> xprops(ocombo, UNO_QUERY);
	sal_Int32 i;
	for (i = 0; i < names.getLength(); ++i) {
		xprops->setPropertyValue(names[i], values[i]);
	}

	// Add the combobox to the dialog.
	Any val;
	val <<= ocombo;
	xNameContainer->insertByName(name, val);

	Reference<XInterface> objectListBox = xControlContainer->getControl(name);
	Reference<XComboBox> xComboBox(objectListBox, UNO_QUERY);

	return xComboBox;
}

//--------------------------
// insertFixedText functions
//--------------------------
Reference<XPropertySet> BaseDialog::insertFixedTextOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, OUString label, sal_Int16 alignment,
		Sequence<OUString> names, Sequence<Any> values) {
	Reference<XInterface> labelModel = xMSF->createInstance(
			OUSTRING(
					"com.sun.star.awt.UnoControlFixedTextModel"));
	if (!labelModel.is()) {
		// Error.
		// std::cout << "BaseDialog::insertFixedText: labelModel was not initialized!"; 
	}

	Reference<XMultiPropertySet> xmultiprops(labelModel, UNO_QUERY);

	Sequence<OUString> propNames(8);
	Sequence<Any> propValues(8);

	propNames[0] = OUSTRING("Align");
	propValues[0] <<= alignment;

	propNames[1] = OUSTRING("Height");
	propValues[1] <<= height;

	propNames[2] = OUSTRING("Label");
	propValues[2] <<= label;

	propNames[3] = OUSTRING("Name");
	propValues[3] <<= name;

	propNames[4] = OUSTRING("PositionX");
	propValues[4] <<= posx;

	propNames[5] = OUSTRING("PositionY");
	propValues[5] <<= posy;

	propNames[6] = OUSTRING("Step");
	propValues[6] <<= page;

	propNames[7] = OUSTRING("Width");
	propValues[7] <<= width;

	// Add the default properties.
	xmultiprops->setPropertyValues(propNames, propValues);

	// We will return an instance of this.
	Reference<XPropertySet> xtextProps(labelModel, UNO_QUERY);

	// Add any additional properties, if they are available.
	sal_Int32 i;
	for (i = 0; i < names.getLength(); i++) {
		xtextProps->setPropertyValue(names[i], values[i]);
	}

	// Add the component back into the model.
	Any val;
	val <<= labelModel;
	xNameContainer->insertByName(name, val);

	return xtextProps;
}

//--------------------------
// insertTextField functions
//--------------------------
Reference<XTextComponent> BaseDialog::insertTextFieldOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, OUString initialText, Sequence<OUString> names,
		Sequence<Any> values) {
	// Create the text model.
	Reference<XInterface> textModel = xMSF->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlEditModel"));

	// Get the property list.
	Reference<XMultiPropertySet> xmultiProps(textModel, UNO_QUERY);

	// Initialize all of the properties.
	Sequence<OUString> propNames(7);
	Sequence<Any> propValues(7);

	propNames[0] = OUSTRING("Height");
	propValues[0] <<= height;
	propNames[1] = OUSTRING("Name");
	propValues[1] <<= name;
	propNames[2] = OUSTRING("PositionX");
	propValues[2] <<= posx;
	propNames[3] = OUSTRING("PositionY");
	propValues[3] <<= posy;
	propNames[4] = OUSTRING("Step");
	propValues[4] <<= page;
	propNames[5] = OUSTRING("Text");
	propValues[5] <<= initialText;
	propNames[6] = OUSTRING("Width");
	propValues[6] <<= width;
	xmultiProps->setPropertyValues(propNames, propValues);

	if (names.getLength() != 0) {
		Reference<XPropertySet> xPropSet(textModel, UNO_QUERY);
		sal_Int32 i;
		for (i = 0; i < names.getLength(); i++) {
			xPropSet->setPropertyValue(names[i], values[i]);
		}
	}

	// Insert the component into the window.
	Any val;
	val <<= textModel;
	xNameContainer->insertByName(name, val);

	Reference<XControl> xControl = xControlContainer->getControl(name);
	Reference<XTextComponent> result(xControl, UNO_QUERY);
	return result;
}

//--------------------------
// insertPasswordField functions
//--------------------------
Reference<XTextComponent> BaseDialog::insertPasswordFieldOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
		Sequence<OUString> names, Sequence<Any> values) {
	// Create the text model.
	Reference<XInterface> textModel = xMSF->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlEditModel"));

	// Get the property list.
	Reference<XMultiPropertySet> xmultiProps(textModel, UNO_QUERY);

	// Initialize all of the properties.
	Sequence<OUString> propNames(7);
	Sequence<Any> propValues(7);

	sal_Int16 echoChar = '*'; // 0x25CF; // Unicode character for "black circle".

	propNames[0] = OUSTRING("EchoChar");
	propValues[0] <<= (sal_Int16) echoChar;

	propNames[1] = OUSTRING("Height");
	propValues[1] <<= (sal_Int32) 12;

	propNames[2] = OUSTRING("Name");
	propValues[2] <<= name;

	propNames[3] = OUSTRING("PositionX");
	propValues[3] <<= posx;

	propNames[4] = OUSTRING("PositionY");
	propValues[4] <<= posy;

	propNames[5] = OUSTRING("Step");
	propValues[5] <<= page;

	propNames[6] = OUSTRING("Width");
	propValues[6] <<= width;
	xmultiProps->setPropertyValues(propNames, propValues);

	Reference<XPropertySet> xPropSet(textModel, UNO_QUERY);

	// Add any additional properties
	if (names.getLength() != 0) {
		sal_Int16 i;
		for (i = 0; i < names.getLength(); i++) {
			xPropSet->setPropertyValue(names[i], values[i]);
		}
	}

	// Insert the component into the window.
	Any val;
	val <<= textModel;
	xNameContainer->insertByName(name, val);

	Reference<XControl> xControl = xControlContainer->getControl(name);
	Reference<XTextComponent> result(xControl, UNO_QUERY);
	return result;
}

//--------------------------
// insertFileControl functions
//--------------------------
Reference<XTextComponent> BaseDialog::insertFileControlOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, Sequence<OUString> names, Sequence<Any> values) {
	// Set properties for the FilePicker that may appear???

	// Create the control.
	Reference<XInterface> fileModel = xMSF->createInstance(
			OUSTRING(
					"com.sun.star.awt.UnoControlFileControlModel"));

	Reference<XMultiPropertySet> xmultiProps(fileModel, UNO_QUERY);

	Sequence<OUString> propNames(6);
	Sequence<Any> propValues(6);

	propNames[0] = OUSTRING("Height");
	propValues[0] <<= height;

	propNames[1] = OUSTRING("Name");
	propValues[1] <<= name;

	propNames[2] = OUSTRING("PositionX");
	propValues[2] <<= posx;

	propNames[3] = OUSTRING("PositionY");
	propValues[3] <<= posy;

	propNames[4] = OUSTRING("Step");
	propValues[4] <<= page;

	propNames[5] = OUSTRING("Width");
	propValues[5] <<= width;
	xmultiProps->setPropertyValues(propNames, propValues);

	// Insert any additional properties (if applicable)
	sal_Int32 i;
	Reference<XPropertySet> xPropSet(fileModel, UNO_QUERY);
	for (i = 0; i < names.getLength(); i++) {
		xPropSet->setPropertyValue(names[i], values[i]);
	}

	// Add the component to the dialog.
	Any val;
	val <<= fileModel;
	xNameContainer->insertByName(name, val);

	// Extract the "control" for the dialog to obtain the XTextComponent reference.
	Reference<XControl> fileControl = xControlContainer->getControl(name);
	Reference<XTextComponent> result(fileControl, UNO_QUERY);
	return result;
}

//--------------------------
// insertScrollbar functions
//--------------------------
Reference<XScrollBar> BaseDialog::insertScrollBar(
		Reference<XAdjustmentListener> _listener, sal_Int32 posx,
		sal_Int32 posy, sal_Int32 length, OUString name, sal_Bool isHorizontal,
		sal_Int32 minScrollValue, sal_Int32 maxScrollValue,
		sal_Int32 initScrollValue, sal_Int32 lineIncrement,
		sal_Int32 blockIncrement, sal_Int32 thickness, Sequence<OUString> names,
		Sequence<Any> values) {
	try {
		// Create the model.
		Reference<XInterface> scrollbar = xMSF->createInstance(
				OUSTRING(
						"com.sun.star.awt.UnoControlScrollBarModel"));

		// Get the XPropertySet for the created scrollbar model.
		// Then, we can set the properties.
		Reference<XPropertySet> xScrollProps(scrollbar, UNO_QUERY);

		// Now, set each property.
		Any val;

		// First, we set the properties that do not depend on Orientation.
		val <<= posx;
		xScrollProps->setPropertyValue(OUSTRING("PositionX"),
				val);
		val <<= posy;
		xScrollProps->setPropertyValue(OUSTRING("PositionY"),
				val);
		val <<= name;
		xScrollProps->setPropertyValue(OUSTRING("Name"), val);
		val <<= minScrollValue;
		xScrollProps->setPropertyValue(
				OUSTRING("ScrollValueMin"), val);
		val <<= maxScrollValue;
		xScrollProps->setPropertyValue(
				OUSTRING("ScrollValueMax"), val);
		val <<= initScrollValue;
		xScrollProps->setPropertyValue(OUSTRING("ScrollValue"),
				val);
		val <<= lineIncrement;
		xScrollProps->setPropertyValue(
				OUSTRING("LineIncrement"), val);
		val <<= blockIncrement;
		xScrollProps->setPropertyValue(
				OUSTRING("BlockIncrement"), val);

		// Now, we set the properties that do depend on Orientation.
		if (isHorizontal) {
			val <<= ScrollBarOrientation::HORIZONTAL;
			xScrollProps->setPropertyValue(
					OUSTRING("Orientation"), val);
			val <<= thickness;
			xScrollProps->setPropertyValue(OUSTRING("Height"),
					val);
			val <<= length;
			xScrollProps->setPropertyValue(OUSTRING("Width"),
					val);
		} else {
			val <<= (sal_Int32) ScrollBarOrientation::VERTICAL;
			xScrollProps->setPropertyValue(
					OUSTRING("Orientation"), val);
			val <<= thickness;
			xScrollProps->setPropertyValue(OUSTRING("Width"),
					val);
			val <<= length;
			xScrollProps->setPropertyValue(OUSTRING("Height"),
					val);
		}

		// Insert the Scrollbar into the main frame.
		val <<= scrollbar;
		xNameContainer->insertByName(name, val);

		// Add the adjustment listener, if possible.
		Reference<XControl> xScrollControl = xControlContainer->getControl(
				name);
		Reference<XScrollBar> xScrollBar(xScrollControl, UNO_QUERY);
		if (_listener.is()) {
			xScrollBar->addAdjustmentListener(_listener);
		}

		return xScrollBar;
	} catch (IllegalArgumentException & e) {
#ifdef DEBUG_PRINT_TOOLBAR
		std::cout << "In initializeSlider(): " << e.Message;
#endif
	} catch (WrappedTargetException & e) {
#ifdef DEBUG_PRINT_TOOLBAR
		std::cout << "In initializeSlider(): " << e.Message;
#endif		
	} catch (ElementExistException& e) {
#ifdef DEBUG_PRINT_TOOLBAR
		std::cout << "In initializeSlider(): " << e.Message;
#endif
	} catch (PropertyVetoException& e) {
#ifdef DEBUG_PRINT_TOOLBAR
		std::cout << "In initializeSlider(): " << e.Message;
#endif
	} catch (UnknownPropertyException & e) {
#ifdef DEBUG_PRINT_TOOLBAR
		std::cout << "In initializeSlider(): " << e.Message;
#endif
	} catch (...) {
#ifdef DEBUG_PRINT_TOOLBAR
		std::cout << "In initializeSlider(): Ambiguous Error.\n";
#endif
	}
	return NULL; // kwa
}

// Insert Line
// TODO -- Redo this...
Reference<XPropertySet> BaseDialog::insertLine(sal_Int32 posx, sal_Int32 posy,
		sal_Int32 thickness, sal_Int32 length, OUString name,
		sal_Bool isHorizontal, Color color) {
	// To insert a line, we simply insert a textfield with no text. The 'color' is set with the
	// background property.
	Sequence<OUString> propNames(1);
	Sequence<Any> propValues(1);
	propNames[0] = OUSTRING("BackgroundColor");
	propValues[0] <<= color;

	if (isHorizontal) {
		return insertImage(posx, posy, thickness, length, name,
				Reference<XGraphic>(), propNames, propValues);
	} else {
		return insertImage(posx, posy, length, thickness, name,
				Reference<XGraphic>(), propNames, propValues);
	}
}

/*	// Insert DateFields.
 Reference< XPropertySet > BaseDialog::insertDateField(sal_Int32 posx, sal_Int32 posy, sal_Int32 height,
 sal_Int32 width, OUString name, sal_Int32 date, sal_Int16 dateFormat, sal_Int32 dateMin, 
 sal_Int32 dateMax)
 {
 Reference< XInterface > dateModel = xMSF->createInstance(
 OUSTRING("com.sun.star.awt.UnoControlDateFieldModel"));
 Reference< XPropertySet > dateProps(dateModel, UNO_QUERY);
 // Pass in the usual properties
 Any val;
 val <<= posx;
 dateProps->setPropertyValue(OUSTRING("PositionX"), val);
 val <<= posy;
 dateProps->setPropertyValue(OUSTRING("PositionY"), val);
 val <<= height;
 dateProps->setPropertyValue(OUSTRING("Height"), val);
 val <<= width;
 dateProps->setPropertyValue(OUSTRING("Width"), val);
 val <<= name;
 dateProps->setPropertyValue(OUSTRING("Name"), val);
 // Set the date properties -- 
 val <<= dateFormat;
 dateProps->setPropertyValue(OUSTRING("DateFormat"), val);
 val <<= dateMin;
 dateProps->setPropertyValue(OUSTRING("DateMin"), val);
 val <<= dateMax;
 dateProps->setPropertyValue(OUSTRING("DateMax"), val);
 val <<= date;
 dateProps->setPropertyValue(OUSTRING("Date"), val);
 
 val <<= ((sal_Bool) true);
 dateProps->setPropertyValue(OUSTRING("Dropdown"), val);
 // Insert the date field into the model.
 val <<= dateModel;
 xNameContainer->insertByName(name, val);
 
 // Return the XPropertySet...
 return dateProps;
 }
 */
// Pattern field.
Reference<XPatternField> BaseDialog::insertPatternFieldOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, OUString literal_mask, OUString edit_mask,
		sal_Bool strict, Sequence<OUString> names, Sequence<Any> values) {
	Reference<XInterface> patternModel = xMSF->createInstance(
			OUSTRING(
					"com.sun.star.awt.UnoControlPatternFieldModel"));
	Reference<XMultiPropertySet> xmultiprops(patternModel, UNO_QUERY);

	// Set the properties.
	Sequence<OUString> propNames(9);
	Sequence<Any> propValues(9);

	propValues[0] <<= edit_mask;
	propNames[0] = OUSTRING("EditMask");
	propValues[1] <<= height;
	propNames[1] = OUSTRING("Height");
	propValues[2] <<= literal_mask;
	propNames[2] = OUSTRING("LiteralMask");
	propValues[3] <<= name;
	propNames[3] = OUSTRING("Name");
	propValues[4] <<= posx;
	propNames[4] = OUSTRING("PositionX");
	propValues[5] <<= posy;
	propNames[5] = OUSTRING("PositionY");
	propValues[6] <<= page;
	propNames[6] = OUSTRING("Step");
	propValues[7] <<= strict;
	propNames[7] = OUSTRING("StrictFormat");
	propValues[8] <<= width;
	propNames[8] = OUSTRING("Width");

	xmultiprops->setPropertyValues(propNames, propValues);

	// Add any additional properties
	Reference<XPropertySet> xProps(patternModel, UNO_QUERY);
	int i;
	for (i = 0; i < names.getLength(); ++i) {
		xProps->setPropertyValue(names[i], values[i]);
	}

	// Add the component to the dialog.
	Any val;
	val <<= patternModel;
	xNameContainer->insertByName(name, val);

	// Extract the "control" for the dialog to obtain the XTextComponent reference.
	Reference<XControl> xPatternControl = xControlContainer->getControl(name);
	Reference<XPatternField> result(xPatternControl, UNO_QUERY);
	return result;
}

// Numeric Field
Reference<XNumericField> BaseDialog::insertNumericFieldOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, sal_Bool spin_enable, double minVal, double maxVal,
		double initVal, double stepVal, sal_Int16 decimal_count,
		Sequence<OUString> names, Sequence<Any> values) {
	Any val;

	Reference<XInterface> numericModel = xMSF->createInstance(
			OUSTRING(
					"com.sun.star.awt.UnoControlNumericFieldModel"));

	Reference<XMultiPropertySet> xmultiprops(numericModel, UNO_QUERY);

	Sequence<OUString> propNames(13);
	Sequence<Any> propValues(13);

	propValues[0] <<= decimal_count;
	propNames[0] = OUSTRING("DecimalAccuracy");
	propValues[1] <<= height;
	propNames[1] = OUSTRING("Height");
	propValues[2] <<= name;
	propNames[2] = OUSTRING("Name");
	propValues[3] <<= posx;
	propNames[3] = OUSTRING("PositionX");
	propValues[4] <<= posy;
	propNames[4] = OUSTRING("PositionY");
	propValues[5] <<= sal_False;
	propNames[5] = OUSTRING("ShowThousandsSeparator");
	propValues[6] <<= spin_enable;
	propNames[6] = OUSTRING("Spin");
	propValues[7] <<= page;
	propNames[7] = OUSTRING("Step");
	propValues[8] <<= initVal;
	propNames[8] = OUSTRING("Value");
	propValues[9] <<= maxVal;
	propNames[9] = OUSTRING("ValueMax");
	propValues[10] <<= minVal;
	propNames[10] = OUSTRING("ValueMin");
	propValues[11] <<= stepVal;
	propNames[11] = OUSTRING("ValueStep");
	propValues[12] <<= width;
	propNames[12] = OUSTRING("Width");

	xmultiprops->setPropertyValues(propNames, propValues);

	// Add any additional properties.
	Reference<XPropertySet> xprops(numericModel, UNO_QUERY);
	int i;
	for (i = 0; i < names.getLength(); i++) {
		xprops->setPropertyValue(names[i], values[i]);
	}

	// Add the control to the dialog.	
	val <<= numericModel;
	xNameContainer->insertByName(name, val);

	// Extract the "control" for the dialog to obtain the XTextComponent reference.
	Reference<XControl> fileControl = xControlContainer->getControl(name);
	Reference<XNumericField> result(fileControl, UNO_QUERY);
	return result;

}

Reference<XDateField> BaseDialog::insertFixedDateFieldOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, sal_Int16 date_format, sal_Int32 date,
		Sequence<OUString> names, Sequence<Any> values) {
	Any val;
	Reference<XInterface> date_model = xMSF->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlDateFieldModel"));

	Reference<XMultiPropertySet> xmultiprops(date_model, UNO_QUERY);

	Sequence<OUString> propNames(9);
	Sequence<Any> propValues(9);

	propValues[0] <<= date;
	propNames[0] = OUSTRING("Date");
	propValues[1] <<= date_format;
	propNames[1] = OUSTRING("DateFormat");
	propValues[2] <<= height;
	propNames[2] = OUSTRING("Height");
	propValues[3] <<= name;
	propNames[3] = OUSTRING("Name");
	propValues[4] <<= posx;
	propNames[4] = OUSTRING("PositionX");
	propValues[5] <<= posy;
	propNames[5] = OUSTRING("PositionY");
	propValues[6] <<= ((sal_Bool) sal_True );
	propNames[6] = OUSTRING("ReadOnly");
	propValues[7] <<= page;
	propNames[7] = OUSTRING("Step");
	propValues[8] <<= width;
	propNames[8] = OUSTRING("Width");

	xmultiprops->setPropertyValues(propNames, propValues);

	// Add any additional properties...
	sal_Int32 i;
	Reference<XPropertySet> xprops(date_model, UNO_QUERY);
	for (i = 0; i < names.getLength(); i++) {
		xprops->setPropertyValue(names[i], values[i]);
	}

	// Add the control to the dialog.	
	val <<= date_model;
	xNameContainer->insertByName(name, val);

	// Extract the "control" for the dialog to obtain the XTextComponent reference.
	Reference<XControl> dateControl = xControlContainer->getControl(name);
	Reference<XDateField> result(dateControl, UNO_QUERY);
	return result;
}

// insertFixedTimeFieldOnPage
Reference<XTimeField> BaseDialog::insertFixedTimeFieldOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, sal_Int16 time_format, sal_Int32 time,
		Sequence<OUString> names, Sequence<Any> values) {
	Any val;
	Reference<XInterface> date_model = xMSF->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlTimeFieldModel"));

	Reference<XMultiPropertySet> xmultiprops(date_model, UNO_QUERY);

	Sequence<OUString> propNames(9);
	Sequence<Any> propValues(9);

	propValues[0] <<= height;
	propNames[0] = OUSTRING("Height");
	propValues[1] <<= name;
	propNames[1] = OUSTRING("Name");
	propValues[2] <<= posx;
	propNames[2] = OUSTRING("PositionX");
	propValues[3] <<= posy;
	propNames[3] = OUSTRING("PositionY");
	propValues[4] <<= ((sal_Bool) sal_True );
	propNames[4] = OUSTRING("ReadOnly");
	propValues[5] <<= page;
	propNames[5] = OUSTRING("Step");
	propValues[6] <<= time;
	propNames[6] = OUSTRING("Time");
	propValues[7] <<= time_format;
	propNames[7] = OUSTRING("TimeFormat");
	propValues[8] <<= width;
	propNames[8] = OUSTRING("Width");

	xmultiprops->setPropertyValues(propNames, propValues);

	// Add any additional properties...
	sal_Int32 i;
	Reference<XPropertySet> xprops(date_model, UNO_QUERY);
	for (i = 0; i < names.getLength(); i++) {
		xprops->setPropertyValue(names[i], values[i]);
	}

	// Add the control to the dialog.	
	val <<= date_model;
	xNameContainer->insertByName(name, val);

	// Extract the "control" for the dialog to obtain the XTextComponent reference.
	Reference<XControl> dateControl = xControlContainer->getControl(name);
	Reference<XTimeField> result(dateControl, UNO_QUERY);
	return result;
}

Reference<XProgressBar> BaseDialog::insertProgressbarOnPage(sal_Int16 page,
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, sal_Int32 begin, sal_Int32 end, Sequence<OUString> names,
		Sequence<Any> values) {
	Any val;
	Reference<XInterface> progress_model = xMSF->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlProgressBarModel"));

	Reference<XMultiPropertySet> xmultiprops(progress_model, UNO_QUERY);

	Sequence<OUString> propNames(9);
	Sequence<Any> propValues(9);

	propValues[0] <<= height;
	propNames[0] = OUSTRING("Height");
	propValues[1] <<= name;
	propNames[1] = OUSTRING("Name");
	propValues[2] <<= posx;
	propNames[2] = OUSTRING("PositionX");
	propValues[3] <<= posy;
	propNames[3] = OUSTRING("PositionY");

	propNames[4] = OUSTRING("ProgressValue");
	propValues[4] <<= begin;
	propNames[5] = OUSTRING("ProgressValueMax");
	propValues[5] <<= end;
	propNames[6] = OUSTRING("ProgressValueMin");
	propValues[6] <<= begin;

	propValues[7] <<= page;
	propNames[7] = OUSTRING("Step");
	propValues[8] <<= width;
	propNames[8] = OUSTRING("Width");

	xmultiprops->setPropertyValues(propNames, propValues);

	// Add any additional properties...
	sal_Int32 i;
	Reference<XPropertySet> xprops(progress_model, UNO_QUERY);
	for (i = 0; i < names.getLength(); i++) {
		xprops->setPropertyValue(names[i], values[i]);
	}

	// Add the control to the dialog.	
	val <<= progress_model;
	xNameContainer->insertByName(name, val);

	// Extract the "control" for the dialog to obtain the XTextComponent reference.
	Reference<XControl> progressControl = xControlContainer->getControl(name);
	Reference<XProgressBar> result(progressControl, UNO_QUERY);
	return result;
}

//-----------------------------------------------------
// MessageBox Functions
//-----------------------------------------------------

void BaseDialog::showErrorMessageBox(OUString title, OUString message) {
	Reference<XMessageBoxFactory> xFactory(xToolkit, UNO_QUERY);
#if OFFICE == OFFICE_LIBRE
	Rectangle rect;
//		rect.X = 0;
//		rect.Y = 0;
//		rect.Width = 200;
//		rect.Height = 100;
	Reference<XMessageBox> box = xFactory->createMessageBox(xWindowPeer, rect,
			OUSTRING("errorbox"), 1, title, message);
#elif OFFICE == OFFICE_OPEN
	Reference<XMessageBox> box = xFactory->createMessageBox(xWindowPeer,
			MessageBoxType_ERRORBOX, 1, title, message);
#endif
	box->execute();
}

void BaseDialog::showInfoMessageBox(OUString title, OUString message) {
	Reference<XMessageBoxFactory> xFactory(xToolkit, UNO_QUERY);
#if OFFICE == OFFICE_LIBRE
	Rectangle rect;
//		rect.X = 0;
//		rect.Y = 0;
//		rect.Width = 200;
//		rect.Height = 100;
	Reference<XMessageBox> box = xFactory->createMessageBox(xWindowPeer, rect,
			OUSTRING("infobox"), 1, title, message);
#elif OFFICE == OFFICE_OPEN
	Reference<XMessageBox> box = xFactory->createMessageBox(xWindowPeer,
			MessageBoxType_INFOBOX, 1, title, message);
#endif
	box->execute();
}

//-----------------------------------------------------
// Static Functions
//-----------------------------------------------------
Reference<XActionListener> BaseDialog::getLoadPageListener(BaseDialog* dialog,
		sal_Int16 page_to_load) {
	LoadPageListener* listener = new LoadPageListener(dialog, page_to_load);
	Reference<XActionListener> xlistener =
			static_cast<XActionListener*>(listener);
	return xlistener;
}

}
;
}
;
}
;
// namespace org::provenance::dialog

