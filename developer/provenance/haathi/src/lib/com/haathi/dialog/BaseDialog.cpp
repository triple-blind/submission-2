/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/BaseDialog.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/BaseDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/dialog/MessageBox.hpp"
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/menu/MenuConfig.hpp"
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/PropertySetHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/awt/Rectangle.hpp>
#include <com/sun/star/awt/XDialog.hpp>
#include <com/sun/star/awt/XTopWindow.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/ui/ItemStyle.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include "sdkEnd.hpp"

#include <algorithm>
#include <string>
/******************************************************************************
Define
******************************************************************************/
#define STRATEGY_HIDDEN 0
#define STRATEGY_LISTEN 1

#define STRATEGY STRATEGY_HIDDEN
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::extension;
using namespace com::haathi::network;
using namespace com::haathi::utils;

using namespace com::sun::star::awt;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::ui;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;

using namespace rtl;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
DialogMessageBoxProvider
******************************************************************************/
DialogMessageBoxProvider::DialogMessageBoxProvider() {
	xToolkit = 0;
	xWindowPeer = 0;
	title = 0;
}

DialogMessageBoxProvider::~DialogMessageBoxProvider() {
}

void DialogMessageBoxProvider::initializeDialogMessageBoxProvider(Reference<XToolkit>* xToolkit,
		Reference<XWindowPeer>* xWindowPeer, OUString* title) {
	this->xToolkit = xToolkit;
	this->xWindowPeer = xWindowPeer;
	this->title = title;
}

OUString DialogMessageBoxProvider::exceptionMessage(ResponsePacket& responsePacket) {
	return OUSTRING("Server error: ") + convertToOUString(responsePacket.getMessage());
}

sal_Int16 DialogMessageBoxProvider::showErrorMessageBox(ResponsePacket& responsePacket) {
	return showErrorMessageBox(exceptionMessage(responsePacket));
}

sal_Int16 DialogMessageBoxProvider::showErrorMessageBox(NetworkException& exception) {
	return this->MessageBoxProvider::showErrorMessageBox(exception);
}

sal_Int16 DialogMessageBoxProvider::showErrorMessageBox(std::exception& exception) {
	return this->MessageBoxProvider::showErrorMessageBox(exception);
}

sal_Int16 DialogMessageBoxProvider::showErrorMessageBox(Exception& exception) {
	return this->MessageBoxProvider::showErrorMessageBox(exception);
}

sal_Int16 DialogMessageBoxProvider::showErrorMessageBox(const char* message) {
	return this->MessageBoxProvider::showErrorMessageBox(message);
}

sal_Int16 DialogMessageBoxProvider::showErrorMessageBox(OUString message) {
	return MessageBox::showErrorMessageBox(*xToolkit, *xWindowPeer, *title, message);
}

sal_Int16 DialogMessageBoxProvider::showInfoMessageBox(OUString message) {
	return MessageBox::showInfoMessageBox(*xToolkit, *xWindowPeer, *title, message);
}

sal_Int16 DialogMessageBoxProvider::showQueryMessageBox(OUString message) {
	return MessageBox::showQueryMessageBox(*xToolkit, *xWindowPeer, *title, message);
}		
/******************************************************************************
DocumentData
******************************************************************************/
DocumentData::DocumentData(int id, std::string name) {
	this->id = id;
	this->name = name;
	std::transform(name.begin(), name.end(), std::back_inserter(uname), ::toupper);
}

int DocumentData::compare(DocumentData& other) {
	int comparison;

	comparison = uname.compare(other.uname);
	if (comparison != 0)
		return comparison;

	comparison = name.compare(other.name);
	if (comparison != 0)
		return comparison;

	return comparison = id - other.id;
}
/******************************************************************************
Listeners
******************************************************************************/	
CloseListener::~CloseListener() {
}

ListSelectListener::~ListSelectListener() {
}

TextEditListener::~TextEditListener() {
}

ButtonClickListener::~ButtonClickListener() {
}

WindowListener::~WindowListener() {
}
/******************************************************************************
ButtonListener
******************************************************************************/
ButtonListener::ButtonListener(): 
	listener(0), id(0) {
}

ButtonListener::ButtonListener(ButtonClickListener* listener, int id):
	listener(listener), id(id) {
}

ButtonListener& ButtonListener::operator=(const ButtonListener& other) {
	this->listener = other.listener;
	this->id = other.id;
	return *this;
}

ButtonListener::ButtonListener(const ButtonListener& other):
	listener(other.listener), id(other.id) {
}

ButtonListener::~ButtonListener() {
}

void SAL_CALL ButtonListener::actionPerformed(const ActionEvent& event)
		throw (RuntimeException) {
	if (listener)
		listener->onButtonClicked(id);
}

void SAL_CALL ButtonListener::disposing(const EventObject& object)
		throw (RuntimeException) {
}
/******************************************************************************
TextListener
******************************************************************************/
TextListener::TextListener(): 
	listener(0), id(0) {
}

TextListener::TextListener(TextEditListener* listener, int id):
	listener(listener), id(id) {
}

TextListener& TextListener::operator=(const TextListener& other) {
	this->listener = other.listener;
	this->id = other.id;
	return *this;
}

TextListener::TextListener(const TextListener& other):
	listener(other.listener), id(other.id) {
}

TextListener::~TextListener() {
}

void SAL_CALL TextListener::textChanged(const TextEvent& event)
		throw (RuntimeException) {
	if (listener)
		listener->onTextEdited(id);
}

void SAL_CALL TextListener::disposing(const EventObject& object)
		throw (RuntimeException) {
}
/******************************************************************************
ListListener
******************************************************************************/
ListListener::ListListener(): 
	listener(0), id(0) {
}

ListListener::ListListener(ListSelectListener* listener, int id):
	listener(listener), id(id) {
}

ListListener& ListListener::operator=(const ListListener& other) {
	this->listener = other.listener;
	this->id = other.id;
	return *this;
}

ListListener::ListListener(const ListListener& other):
	listener(other.listener), id(other.id) {
}

ListListener::~ListListener() {
}

void SAL_CALL ListListener::itemStateChanged(const ItemEvent& event)
		throw (RuntimeException) {
	if (listener)
		listener->onListSelected(id, event.Selected);
}

void SAL_CALL ListListener::actionPerformed(const ActionEvent& event)
		throw (RuntimeException) {
	if (listener)
		listener->onListDoubleSelected(id);
}

void SAL_CALL ListListener::disposing(const EventObject& object)
		throw (RuntimeException) {
}
/******************************************************************************
ClosingListener
******************************************************************************/
ClosingListener::ClosingListener(): 
	listener(0), id(0) {
}

ClosingListener::ClosingListener(CloseListener* listener, int id):
	listener(listener), id(id) {
}

ClosingListener& ClosingListener::operator=(const ClosingListener& other) {
	this->listener = other.listener;
	this->id = other.id;
	return *this;
}

ClosingListener::ClosingListener(const ClosingListener& other):
	listener(other.listener), id(other.id) {
}

ClosingListener::~ClosingListener() {
}

void SAL_CALL ClosingListener::queryClosing(const EventObject& object, sal_Bool getsOwnership)
		throw (CloseVetoException, RuntimeException) {
}

void SAL_CALL ClosingListener::notifyClosing(const EventObject& object)
		throw (RuntimeException) {
	if (listener)
		listener->onClosed(id);
}

void SAL_CALL ClosingListener::disposing(const EventObject& object)
		throw (RuntimeException) {
}
/******************************************************************************
TopWindowListener
******************************************************************************/
TopWindowListener::TopWindowListener(): 
	listener(0), id(0) {
}

TopWindowListener::TopWindowListener(WindowListener* listener, int id):
	listener(listener), id(id) {
}

TopWindowListener& TopWindowListener::operator=(const TopWindowListener& other) {
	this->listener = other.listener;
	this->id = other.id;
	return *this;
}

TopWindowListener::TopWindowListener(const TopWindowListener& other):
	listener(other.listener), id(other.id) {
}

TopWindowListener::~TopWindowListener() {
}

void SAL_CALL TopWindowListener::windowActivated(const com::sun::star::lang::EventObject& eventObject)
		throw (com::sun::star::uno::RuntimeException) {
}

void SAL_CALL TopWindowListener::windowClosed(const com::sun::star::lang::EventObject& eventObject)
		throw (com::sun::star::uno::RuntimeException) {
}

void SAL_CALL TopWindowListener::windowClosing(const com::sun::star::lang::EventObject& eventObject)
		throw (com::sun::star::uno::RuntimeException) {
	if (listener)
		listener->onWindowClosed(id);
}

void SAL_CALL TopWindowListener::windowDeactivated(const com::sun::star::lang::EventObject& eventObject)
		throw (com::sun::star::uno::RuntimeException) {
}

void SAL_CALL TopWindowListener::windowMinimized(const com::sun::star::lang::EventObject& eventObject)
		throw (com::sun::star::uno::RuntimeException) {
}

void SAL_CALL TopWindowListener::windowNormalized(const com::sun::star::lang::EventObject& eventObject)
		throw (com::sun::star::uno::RuntimeException) {
}

void SAL_CALL TopWindowListener::windowOpened(const com::sun::star::lang::EventObject& eventObject)
		throw (com::sun::star::uno::RuntimeException) {
}

void SAL_CALL TopWindowListener::disposing(const com::sun::star::lang::EventObject& object)
		throw (com::sun::star::uno::RuntimeException) {
}
/******************************************************************************
BaseDialog
******************************************************************************/
const std::string BaseDialog::emptyString;
const OUString BaseDialog::emptyOUString;
const NamesValuesHelper BaseDialog::emptyNamesValuesHelper;

const sal_Int32 BaseDialog::margin = 6;
const sal_Int32 BaseDialog::btnMargin = margin;
const sal_Int32 BaseDialog::dlgMargin = margin;
const sal_Int32 BaseDialog::lstMargin = 4;

const sal_Int32 BaseDialog::lblSpace = 2;
const sal_Int32 BaseDialog::btnSpace = 3;
const sal_Int32 BaseDialog::txtSpace = 4;

const sal_Int32 BaseDialog::lblHeight = 8;
const sal_Int32 BaseDialog::txtHeight = 12;
const sal_Int32 BaseDialog::btnHeight = 14;

const sal_Int32 BaseDialog::lblWidth = 32;
const sal_Int32 BaseDialog::txtWidth = 108;
const sal_Int32 BaseDialog::btnWidth = 32;

const sal_Int32 BaseDialog::lblAdjust = (txtHeight - lblHeight) / 2;

BaseDialog::BaseDialog(Reference<XComponentContext> xComponentContext,
		sal_Int32 height, sal_Int32 width, OUString title, OUString helpUrl, bool isModal):
		xComponentContext(xComponentContext), title(title), isModal(isModal) {
	Reference<XDesktop> xDesktop = ExtensionHelper(xComponentContext).getXDesktop();
	Rectangle containerRectangle = xDesktop->getCurrentFrame()->getContainerWindow()->getPosSize();

	parentWidth = containerRectangle.Width;
	parentHeight = containerRectangle.Height;
	init(height, width, helpUrl);
}

BaseDialog::BaseDialog(Reference<XComponentContext> xComponentContext,
		sal_Int32 height, sal_Int32 width, OUString title, OUString helpUrl,
		sal_Int32 parentHeight, sal_Int32 parentWidth, bool isModal):
		xComponentContext(xComponentContext), title(title),
		parentHeight(parentHeight), parentWidth(parentWidth), isModal(isModal) {
	init(height, width, helpUrl);
}

Reference<XInterface> BaseDialog::newUnoControlDialogModel(Reference<XMultiServiceFactory> xMultiServiceFactory,
		sal_Int32 height, sal_Int32 width, rtl::OUString title, OUString helpUrl) {
	Reference<XInterface> unoControlDialogModel = xMultiServiceFactory->createInstance(OUSTRING("com.sun.star.awt.UnoControlDialogModel"));
	PropertySetHelper(unoControlDialogModel)
		.set("Height", Any(height))
		.set("Width", Any(width))
		.set("Title", Any(title))
		.set("HelpURL", Any(helpUrl));	

	return unoControlDialogModel;
}

void BaseDialog::init(sal_Int32 height, sal_Int32 width, OUString helpUrl) {
	Reference<XMultiServiceFactory> tmpXMultiServiceFactory = ExtensionHelper(xComponentContext).getXMultiServiceFactory();
	Reference<XInterface> toolkit = tmpXMultiServiceFactory->createInstance(OUSTRING("com.sun.star.awt.Toolkit"));
	xToolkit = Reference<XToolkit>(toolkit, UNO_QUERY);

	unoControlDialog = tmpXMultiServiceFactory->createInstance(OUSTRING("com.sun.star.awt.UnoControlDialog"));
	unoControlDialogModel = newUnoControlDialogModel(tmpXMultiServiceFactory, height, width, title, helpUrl);

	xControl = Reference<XControl>(unoControlDialog, UNO_QUERY);
	xControl->setModel(Reference<XControlModel>(unoControlDialogModel, UNO_QUERY));
	xWindowPeer = xToolkit->getDesktopWindow();
#if STRATEGY == STRATEGY_HIDDEN	
	if (!isModal)
		// To see the window on screen, use false
		xWindowPeer = ExtensionHelper::getHiddenXWindowPeer(tmpXMultiServiceFactory, xToolkit, false);
#endif 
	xControl->createPeer(xToolkit, xWindowPeer);

	xMultiServiceFactory = Reference<XMultiServiceFactory>(unoControlDialogModel, UNO_QUERY);
	xControlContainer = Reference<XControlContainer>(unoControlDialog, UNO_QUERY);
	xNameContainer = Reference<XNameContainer>(unoControlDialogModel, UNO_QUERY);

	center();
	initializeDialogMessageBoxProvider(&xToolkit, &xWindowPeer, &title);

#if STRATEGY == STRATEGY_LISTEN	
	if (!isModal) {
		topWindowListener = TopWindowListener(this, 0);
		connectTopWindowListener(xControl, &topWindowListener);
		setVisible(true);
	}
#endif 
}

void BaseDialog::setVisible(bool visible) {
	Reference<XWindow> xWindow(unoControlDialog, UNO_QUERY);
	xWindow->setVisible(asSalBool(visible));
}

BaseDialog::~BaseDialog() {
	if (!isModal && isTrue(xWindowPeer.is())) {
		// Whether this is necessary or not, it causes the program to hang
		// especially when called a second time for a new dialog
		//xWindowPeer->dispose();
		// Doesn't work
		Reference<XCloseable> xCloseable(xWindowPeer, UNO_QUERY);
		if (isTrue(xCloseable.is()))
			xCloseable->close(sal_False);
	}
}

std::string BaseDialog::getSystemFilePathToExtensionFile(const char* file) {
	return ExtensionHelper(xComponentContext).getSystemFilePathToExtensionFile(file);
}
//==================================================================================
// Initialization-esque functions
//==================================================================================
void BaseDialog::center() {
	Reference<XWindow> xWindow(unoControlDialog, UNO_QUERY);
	Rectangle dialogRectangle = xWindow->getPosSize();

	// Record the dialog's width and height here for later use as parentWidth and parentHeight
	sal_Int32 positionX = (parentWidth - (width = dialogRectangle.Width)) / 2;
	sal_Int32 positionY = (parentHeight - (height = dialogRectangle.Height)) / 2;

	xWindow->setPosSize(positionX, positionY, 0, 0, PosSize::POS);
}

sal_Int16 BaseDialog::run(sal_Bool dummy) {
	Reference<XDialog> xDialog(unoControlDialog, UNO_QUERY);
	sal_Int16 result = xDialog->execute();

	return dialogResult = result;
}

void BaseDialog::close() {
	Reference<XDialog> xDialog(unoControlDialog, UNO_QUERY);
	xDialog->endExecute();
}

sal_Int16 BaseDialog::execute() {
	return run();
}

void BaseDialog::endExecute() {
	close();
}
//==================================================================================
// Insertion Functions
//==================================================================================

Reference<XControl> BaseDialog::insertByName(OUString& name, Reference<XInterface>& model) {
	xNameContainer->insertByName(name, Any(model));
	return xControlContainer->getControl(name);
}

Reference<XFixedText> BaseDialog::insertFixedText(
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, OUString label,
		sal_Bool tabstop, sal_Int16 tabIndex,
		OUString helpText,
		NamesValuesHelper namesValuesHelper) {
	Reference<XInterface> model = xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlFixedTextModel"));
	PropertySetHelper(model)
		.set("Align",     Any((sal_Int16) (ItemStyle::ALIGN_LEFT -  1)))
		.set("Height",    Any(height))
		.set("HelpText",  Any(helpText))
		.set("Label",     Any(label))
		.set("Name",      Any(name))
		.set("PositionX", Any(posx))
		.set("PositionY", Any(posy))
		.set("TabIndex",  Any(tabIndex))
		.set("Tabstop",   Any(tabstop))
		.set("Width",     Any(width))
		.set(namesValuesHelper);
	Reference<XFixedText> result(insertByName(name, model), UNO_QUERY);
	return result;
}

Reference<XTextComponent> BaseDialog::insertTextComponent(
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, OUString initialText,
		sal_Bool tabstop, sal_Int16 tabIndex,
		NamesValuesHelper namesValuesHelper) {
	Reference<XInterface> model = xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlEditModel"));
	PropertySetHelper(model)
		.set("Height",    Any(height))
		.set("Name",      Any(name))
		.set("PositionX", Any(posx))
		.set("PositionY", Any(posy))
		.set("TabIndex",  Any(tabIndex))
		.set("Tabstop",   Any(tabstop))
		.set("Text",      Any(initialText))
		.set("Width",     Any(width))
//		.set("HelpURL",   Any(OUSTRING("vnd.com.provenance.haathi:login01")))
		.set(namesValuesHelper);
	Reference<XTextComponent> result(insertByName(name, model), UNO_QUERY);
	return result;
}

Reference<XTextComponent> BaseDialog::insertReadOnlyTextComponent(
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, OUString initialText,
		sal_Bool tabstop, sal_Int16 tabIndex,
		NamesValuesHelper namesValuesHelper) {
	Reference<XInterface> model = xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlEditModel"));

	PropertySetHelper(model)
		.set("Height",    Any(height))
		.set("Name",      Any(name))
		.set("PositionX", Any(posx))
		.set("PositionY", Any(posy))
		.set("ReadOnly",  Any(sal_True))
		.set("TextColor", Any((sal_Int32) 0x808080)) // color picked from screen
		//.set("Enabled",   Any(sal_False))
		.set("TabIndex",  Any(tabIndex))
		.set("Tabstop",   Any(tabstop))
		.set("Text",      Any(initialText))
		.set("Width",     Any(width))
		.set(namesValuesHelper);
	Reference<XTextComponent> result(insertByName(name, model), UNO_QUERY);

	// It would be nice to read the disabled color and then use it for
	// the enabled case, but this isn't working.  The reason to keep the
	// control enabled is so that people can scroll through long texts.

	/*
	Reference<XPropertySet> xPropertySet(model, UNO_QUERY);
	Any any = xPropertySet->getPropertyValue(OUString::createFromAscii("TextColor"));
	sal_Int32 textColor;
	any >>= textColor;

	PropertySetHelper(model)
		.set("Enabled",   Any(sal_True))
		.set("TextColor", Any(textColor);
	*/
	return result;
}

Reference<XTextComponent> BaseDialog::insertPasswordComponent(
		sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
		OUString name, OUString initialText,
		sal_Bool tabstop, sal_Int16 tabIndex,
		NamesValuesHelper namesValuesHelper) {
	Reference<XInterface> model = xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlEditModel"));
	PropertySetHelper(model)
		.set("EchoChar",  Any((sal_Int16) 0x2022))
		.set("Height",    Any(height))
		.set("Name",      Any(name))
		.set("PositionX", Any(posx))
		.set("PositionY", Any(posy))
		.set("TabIndex",  Any(tabIndex))
		.set("Tabstop",   Any(tabstop))
		.set("Text",      Any(initialText))
		.set("Width",     Any(width))
		.set(namesValuesHelper);
	Reference<XTextComponent> result(insertByName(name, model), UNO_QUERY);
	return result;
}

Reference<XButton> BaseDialog::insertButton(
		sal_Int32 posx, sal_Int32 posy,	sal_Int32 height, sal_Int32 width,
		OUString name, OUString label,
		sal_Bool tabstop, sal_Int16 tabIndex,
		sal_Bool defaultButton, PushButtonType pushButtonType,
		OUString helpText,
		NamesValuesHelper namesValuesHelper) {
	Reference<XInterface> model = xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlButtonModel"));
	PropertySetHelper(model)
		.set("DefaultButton",  Any(defaultButton))
		.set("Height",         Any(height))
		.set("HelpText",       Any(helpText))
		.set("Label",          Any(label))
		.set("Name",           Any(name))
		.set("PositionX",      Any(posx))
		.set("PositionY",      Any(posy))
		.set("PushButtonType", Any((sal_Int16) pushButtonType))
		.set("TabIndex",       Any(tabIndex))
		.set("Tabstop",        Any(tabstop))
		.set("Width",          Any(width))
		.set(namesValuesHelper);
	Reference<XButton> result(insertByName(name, model), UNO_QUERY);
	return result;
}

Reference<XButton> BaseDialog::insertOkButton(
		sal_Int32 posx, sal_Int32 posy,	sal_Int32 height, sal_Int32 width,	
		sal_Bool tabstop, sal_Int16 tabIndex,
		OUString helpText,
		NamesValuesHelper namesValuesHelper) {
	return insertButton(posx, posy, height, width,
			OUSTRING("btnOk"), OUSTRING("~OK"),
			tabstop, tabIndex,
			sal_True, PushButtonType_OK,
			helpText,
			namesValuesHelper);
}

Reference<XButton> BaseDialog::insertCloseButton(
		sal_Int32 posx, sal_Int32 posy,	sal_Int32 height, sal_Int32 width,	
		sal_Bool tabstop, sal_Int16 tabIndex,
		OUString helpText,
		NamesValuesHelper namesValuesHelper) {
	return insertButton(posx, posy, height, width,
			OUSTRING("btnClose"), OUSTRING("~Close"),
			tabstop, tabIndex,
			sal_True, PushButtonType_OK,
			helpText,
			namesValuesHelper);
}

Reference<XButton> BaseDialog::insertCancelButton(
		sal_Int32 posx, sal_Int32 posy,	sal_Int32 height, sal_Int32 width,	
		sal_Bool tabstop, sal_Int16 tabIndex,
		OUString helpText,
		NamesValuesHelper namesValuesHelper) {
	return insertButton(posx, posy, height, width,
			OUSTRING("btnCancel"), OUSTRING("~Cancel"),
			tabstop, tabIndex,
			sal_False, PushButtonType_CANCEL,
			helpText,
			namesValuesHelper);
}

Reference<XButton> BaseDialog::insertStandardButton(
		sal_Int32 posx, sal_Int32 posy,	sal_Int32 height, sal_Int32 width,
		OUString name, OUString label,
		sal_Bool tabstop, sal_Int16 tabIndex,
		OUString helpText, ButtonListener& listener,
		NamesValuesHelper namesValuesHelper) {
	Reference<XButton> xButton = insertButton(posx, posy, height, width,
			name, label,
			tabstop, tabIndex,
			sal_False, PushButtonType_STANDARD,
			helpText,
			namesValuesHelper);
	Reference<XActionListener> xActionListener = static_cast<XActionListener*>(&listener);
	xButton->addActionListener(xActionListener);
	return xButton;
}

Reference<XListBox> BaseDialog::insertListBox(sal_Int32 posx, sal_Int32 posy,
		sal_Int32 height, sal_Int32 width, OUString name,
		sal_Bool tabstop, sal_Int16 tabIndex,
		ListListener& listener, sal_Bool multiSelection,
		NamesValuesHelper namesValuesHelper) {
	Reference<XInterface> model = xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlListBoxModel"));
	PropertySetHelper(model)
		.set("Height",         Any(height))
		.set("Name",           Any(name))
		.set("PositionX",      Any(posx))
		.set("PositionY",      Any(posy))
		.set("TabIndex",       Any(tabIndex))
		.set("Tabstop",        Any(tabstop))
		.set("Width",          Any(width))
		.set("MultiSelection", Any(multiSelection))
		.set(namesValuesHelper);
	Reference<XListBox> result(insertByName(name, model), UNO_QUERY);
	Reference<XItemListener> xItemListener = static_cast<XItemListener*>(&listener);
	result->addItemListener(xItemListener);
	Reference<XActionListener> xActionListener = static_cast<XActionListener*>(&listener);
	result->addActionListener(xActionListener);
	return result;
}

void BaseDialog::insertLine(sal_Int32 posx, sal_Int32 posy,
		sal_Int32 length, OUString name, sal_Bool isHorizontal,
		NamesValuesHelper namesValuesHelper) {
	Reference<XInterface> model = xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.UnoControlFixedLineModel"));
	PropertySetHelper(model)
		.set("Height",         Any(sal_Int32(1)))
		.set("Name",           Any(name))
		.set("Orientation",    Any((sal_Int32) (isTrue(isHorizontal) ? 0 : 1)))
		.set("PositionX",      Any(posx))
		.set("PositionY",      Any(posy))
		.set("Width",          Any(length))
		.set(namesValuesHelper);
	insertByName(name, model);
	// Todo figure out how to return something reasonable
}

void BaseDialog::setFocus(Reference<XButton> xButton) {
	Reference<XWindow> xWindow(xButton, UNO_QUERY);
	xWindow->setFocus();
}

void BaseDialog::setFocus(Reference<XTextComponent> xTextComponent) {
	Reference<XWindow> xWindow(xTextComponent, UNO_QUERY);
	xWindow->setFocus();
}

void BaseDialog::setFocus(Reference<XListBox> xListBox) {
	Reference<XWindow> xWindow(xListBox, UNO_QUERY);
	xWindow->setFocus();
}

void BaseDialog::enable(Reference<XControl> xControl, bool enabled) {
	Reference<XPropertySet> xPropertySet(xControl->getModel(), UNO_QUERY);
	PropertySetHelper propertySetHelper(xPropertySet);
	propertySetHelper.set("Enabled", Any(asSalBool(enabled)));
}

void BaseDialog::enable(Reference<XButton> xButton, bool enabled) {
	Reference<XControl> xControl(xButton, UNO_QUERY);
	enable(xControl, enabled);
}

void BaseDialog::toFront() {
	Reference<XTopWindow> xTopWindow(unoControlDialog, UNO_QUERY);

	if (xTopWindow.is())
		xTopWindow->toFront();
}

void BaseDialog::connectTextListener(Reference<XTextComponent>& xTextComponent, TextListener* textListener) {
	Reference<XTextListener> listener = static_cast<XTextListener*>(textListener);
	xTextComponent->addTextListener(listener);
}

void BaseDialog::connectClosingListener(Reference<XComponent>& xComponent, ClosingListener* closingListener) {
	Reference<XCloseListener> listener = static_cast<XCloseListener*>(closingListener);
	Reference<XCloseable> xCloseable(xComponent, UNO_QUERY);
	if (xCloseable.is())
		xCloseable->addCloseListener(listener);
}

void BaseDialog::connectTopWindowListener(Reference<XControl>& xControl, TopWindowListener* topWindowListener) {
	Reference<XTopWindowListener> listener = static_cast<XTopWindowListener*>(topWindowListener);
	Reference<XTopWindow> xTopWindow(xControl, UNO_QUERY);
	if (xTopWindow.is())
		xTopWindow->addTopWindowListener(listener);
}

void BaseDialog::disconnectClosingListener(Reference<XComponent>& xComponent, ClosingListener* closingListener) {
	Reference<XCloseListener> listener = static_cast<XCloseListener*>(closingListener);
	Reference<XCloseable> xCloseable(xComponent, UNO_QUERY);
	if (xCloseable.is())
		xCloseable->removeCloseListener(listener);
}

void BaseDialog::disconnectTopWindowListener(Reference<XControl>& xControl, TopWindowListener* topWindowListener) {
	Reference<XTopWindowListener> listener = static_cast<XTopWindowListener*>(topWindowListener);
	Reference<XTopWindow> xTopWindow(xControl, UNO_QUERY);
	if (xTopWindow.is())
		xTopWindow->removeTopWindowListener(listener);
}
//-----------------------------------------------------
// MessageBox Functions
//-----------------------------------------------------

void BaseDialog::onListSelected(int id, sal_Int32 selected) {
}

void BaseDialog::onListDoubleSelected(int id) {
}

void BaseDialog::onTextEdited(int id) {
}

void BaseDialog::onButtonClicked(int id) {
}

void BaseDialog::onClosed(int id) {
}

void BaseDialog::onWindowClosed(int id) {
}

static bool compareDocuments(DocumentData left, DocumentData right) {
	return left.compare(right) < 0;
}

void BaseDialog::sortDocumentList(std::vector<DocumentData>& documentList) {
	std::sort(documentList.begin(), documentList.end(), compareDocuments);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
