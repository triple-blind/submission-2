/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/MessageBox.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/MessageBox.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/MessageBoxButtons.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/XMessageBox.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::extension;
using namespace com::haathi::network;
using namespace com::haathi::utils;

using namespace com::sun::star::awt;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
MessageBoxProvider
******************************************************************************/
OUString MessageBoxProvider::exceptionMessage(NetworkException& exception) {
	return convertToOUString(std::string("Exception: ") + exception.getMessage());
}

OUString MessageBoxProvider::exceptionMessage(std::exception& exception) {
	return OUString::createFromAscii(exception.what());
}

OUString MessageBoxProvider::exceptionMessage(Exception& exception) {
	return OUSTRING("Exception: ") + exception.Message;
}

OUString MessageBoxProvider::exceptionMessage(const char* message) {
	return OUString::createFromAscii(message);
}
			
sal_Int16 MessageBoxProvider::showErrorMessageBox(NetworkException& exception) {
	return showErrorMessageBox(exceptionMessage(exception));
}

sal_Int16 MessageBoxProvider::showErrorMessageBox(std::exception& exception) {
	return showErrorMessageBox(exceptionMessage(exception));
}

sal_Int16 MessageBoxProvider::showErrorMessageBox(Exception& exception) {
	return showErrorMessageBox(exceptionMessage(exception));
}

sal_Int16 MessageBoxProvider::showErrorMessageBox(const char* message) {
	return showErrorMessageBox(exceptionMessage(message));
}
/******************************************************************************
MessageBox
******************************************************************************/
sal_Int16 MessageBox::executeMessageBox(Reference<XMessageBoxFactory> xMessageBoxFactory,
		Reference<XWindowPeer> xWindowPeer, MessageBoxType messageBoxType,
		sal_Int32 buttons, const OUString& title, const OUString& message) {
	Reference<XMessageBox> box = xMessageBoxFactory->createMessageBox(xWindowPeer,
			messageBoxType, buttons, title, message);
	return box->execute();
}

sal_Int16 MessageBox::showErrorMessageBox(Reference<XToolkit> xToolkit,
		Reference<XWindowPeer> xWindowPeer,
		const OUString& title, const OUString& message) {
	Reference<XMessageBoxFactory> xMessageBoxFactory(xToolkit, UNO_QUERY);
	return executeMessageBox(xMessageBoxFactory, xWindowPeer, MessageBoxType_ERRORBOX,
			MessageBoxButtons::BUTTONS_OK | MessageBoxButtons::DEFAULT_BUTTON_OK, title, message);
}

sal_Int16 MessageBox::showInfoMessageBox(Reference<XToolkit> xToolkit,
		Reference<XWindowPeer> xWindowPeer,
		const OUString& title, const OUString& message) {
	Reference<XMessageBoxFactory> xMessageBoxFactory(xToolkit, UNO_QUERY);
	return executeMessageBox(xMessageBoxFactory, xWindowPeer, MessageBoxType_INFOBOX,
			MessageBoxButtons::BUTTONS_OK | MessageBoxButtons::DEFAULT_BUTTON_OK, title, message);
}

sal_Int16 MessageBox::showQueryMessageBox(Reference<XToolkit> xToolkit,
		Reference<XWindowPeer> xWindowPeer,
		const OUString& title, const OUString& message) {
	Reference<XMessageBoxFactory> xMessageBoxFactory(xToolkit, UNO_QUERY);
	return executeMessageBox(xMessageBoxFactory, xWindowPeer, MessageBoxType_QUERYBOX,
			MessageBoxButtons::BUTTONS_YES_NO | MessageBoxButtons::DEFAULT_BUTTON_YES, title, message);
}

MessageBox::MessageBox(Reference<XMultiServiceFactory> xMultiServiceFactory) {
	xToolkit = Reference<XToolkit>(xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.Toolkit")), UNO_QUERY);
	xWindowPeer = ExtensionHelper::getHiddenXWindowPeer(xMultiServiceFactory, xToolkit);
}

MessageBox::MessageBox(Reference<XComponentContext> xComponentContext) {
	Reference<XMultiComponentFactory> xMultiComponentFactory = xComponentContext->getServiceManager();
	Reference<XMultiServiceFactory> xMultiServiceFactory(xMultiComponentFactory, UNO_QUERY);

	xToolkit = Reference<XToolkit>(xMultiServiceFactory->createInstance(
			OUSTRING("com.sun.star.awt.Toolkit")), UNO_QUERY);
	xWindowPeer = ExtensionHelper::getHiddenXWindowPeer(xMultiServiceFactory, xToolkit);
}

sal_Int16 MessageBox::showErrorMessageBox(OUString title, OUString message) {
	return MessageBox::showErrorMessageBox(xToolkit, xWindowPeer,
			title, message);
}

sal_Int16 MessageBox::showInfoMessageBox(OUString title, OUString message) {
	return MessageBox::showInfoMessageBox(xToolkit, xWindowPeer,
			title, message);
}

sal_Int16 MessageBox::showQueryMessageBox(OUString title, OUString message) {
	return MessageBox::showQueryMessageBox(xToolkit, xWindowPeer,
			title, message);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
