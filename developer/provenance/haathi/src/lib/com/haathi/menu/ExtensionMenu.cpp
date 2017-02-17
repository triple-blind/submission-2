/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/menu/ExtensionMenu.cpp#1 $

/*
 *  Created by Aaron Gibson on 7/28/11.
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/menu/ExtensionMenu.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/auditor/AuditorRunner.hpp"
#include "com/haathi/dialog/AboutDialog.hpp"
#include "com/haathi/dialog/AuditDialog.hpp"
#include "com/haathi/dialog/LoggerDialog.hpp"
#include "com/haathi/dialog/LoginDialog.hpp"
#include "com/haathi/dialog/MessageBox.hpp"
#include "com/haathi/dialog/MessageBoxResults.hpp"
#include "com/haathi/dialog/NewDialog.hpp"
#include "com/haathi/dialog/OpenDialog.hpp"
#include "com/haathi/document/DocumentTracker.hpp"
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/menu/MenuConfig.hpp"
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/Debug.hpp"
#include "com/haathi/utils/Guard.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/frame/DispatchResultEvent.hpp>
#include <com/sun/star/frame/DispatchResultState.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
#include "sdkEnd.hpp"

#include <fstream> // diagnostic
#include <sstream>
#include <string>
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::auditor;
using namespace com::haathi::dialog;
using namespace com::haathi::document;
using namespace com::haathi::extension;
using namespace com::haathi::utils;

using namespace com::sun::star::awt;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;

using namespace cppu;
using namespace rtl;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace menu {
/******************************************************************************
Class
******************************************************************************/
DispatchData<ExtensionMenu> ExtensionMenu::dispatchData[] = {
	{OUSTRING(PROVENANCE_MENU_ABOUT_URL),  &ExtensionMenu::about,                &ExtensionMenu::alwaysEnabled},
	{OUSTRING(PROVENANCE_MENU_LOGIN_URL),  &ExtensionMenu::login,                &ExtensionMenu::isLoginEnabled},
	{OUSTRING(PROVENANCE_MENU_NEW_URL),    &ExtensionMenu::anew,                 &ExtensionMenu::alwaysEnabled},
	{OUSTRING(PROVENANCE_MENU_OPEN_URL),   &ExtensionMenu::open,                 &ExtensionMenu::alwaysEnabled},
	{OUSTRING(PROVENANCE_MENU_AUDIT_URL),  &ExtensionMenu::audit,                &ExtensionMenu::alwaysEnabled},
	{OUSTRING(PROVENANCE_MENU_LOGGER_URL), &ExtensionMenu::showProvenanceLogger, &ExtensionMenu::alwaysEnabled},
	{OUSTRING(PROVENANCE_MENU_LOGOUT_URL), &ExtensionMenu::logout,               &ExtensionMenu::isLogoutEnabled},
	{OUSTRING("")}
};

DispatchHelper<ExtensionMenu> ExtensionMenu::dispatchHelper(dispatchData);

ExtensionMenu::ExtensionMenu(const Reference<XComponentContext>& xComponentContext):
		xComponentContext(xComponentContext) {
}

ExtensionMenu::~ExtensionMenu() {
	if (xFrame.is())
		xFrame.clear();
}
/******************************************************************************
XServiceInfo
******************************************************************************/
static OUString provenanceMenuServiceName = OUSTRING(PROVENANCE_MENU_SERVICENAME);
static OUString implementationname = OUSTRING(PROVENANCE_MENU_IMPLEMENTATIONNAME);
static OUString protocolHandler = OUSTRING("com.sun.star.frame.ProtocolHandler");

OUString ExtensionMenu_getImplementationName() throw (RuntimeException) {
	return implementationname;
}

sal_Bool SAL_CALL ExtensionMenu_supportsService(const OUString& serviceName)
		throw (RuntimeException) {
	return (serviceName.equals(provenanceMenuServiceName) ||
			serviceName.equals(protocolHandler));
}

Sequence<OUString> SAL_CALL ExtensionMenu_getSupportedServiceNames()
		throw (RuntimeException) {
	Sequence<OUString> supportedServiceNames(1);

	supportedServiceNames[0] = provenanceMenuServiceName;
	return supportedServiceNames;
}

Reference<XInterface> SAL_CALL ExtensionMenu_createInstance(
		const Reference<XComponentContext>& xComponentContext)
		throw (Exception) {
	return (OWeakObject *) new ExtensionMenu(xComponentContext);
}

OUString SAL_CALL ExtensionMenu::getImplementationName()
		throw (RuntimeException) {
	return ExtensionMenu_getImplementationName();
}

sal_Bool SAL_CALL ExtensionMenu::supportsService(const OUString& rServiceName)
		throw (RuntimeException) {
	return ExtensionMenu_supportsService(rServiceName);
}

Sequence<OUString> SAL_CALL ExtensionMenu::getSupportedServiceNames()
		throw (RuntimeException) {
	return ExtensionMenu_getSupportedServiceNames();
}
/******************************************************************************
XInitialization
******************************************************************************/
void SAL_CALL ExtensionMenu::initialize(const Sequence<Any>& aArguments)
		throw (Exception, RuntimeException) {
	if (aArguments.getLength()) {
		Reference<XFrame> tempFrame;
		// The first argument should be the xFrame that this is being initialized in.
		aArguments[0] >>= tempFrame;
		xFrame = tempFrame;
	}
}
/******************************************************************************
XDispatchProvider
******************************************************************************/
Reference<XDispatch> SAL_CALL ExtensionMenu::queryDispatch(const URL& url,
		const OUString& sTargetFrameName, sal_Int32 nSearchFlags)
		throw (RuntimeException) {

#if 0
	std::ofstream file("C:\\Users\\Keith Alcock\\Documents\\My Data\\menu_urls.txt",
			std::ios::out | std::ios::app | std::ios::binary);
	if (file.is_open()) {
		file << url.Complete << std::endl;
		file.close();
	}
#endif

	if (!xFrame.is())
		return 0;
	return dispatchHelper.queryDispatch(url, this);
}

Sequence<Reference<XDispatch> > SAL_CALL ExtensionMenu::queryDispatches(
		const Sequence<DispatchDescriptor>& seqDescripts)
		throw (RuntimeException) {
	sal_Int32 nCount = seqDescripts.getLength();
	Sequence<Reference<XDispatch> > lDispatcher(nCount);

	for (sal_Int32 i = 0; i < nCount; ++i)
		lDispatcher[i] = queryDispatch(seqDescripts[i].FeatureURL,
				seqDescripts[i].FrameName, seqDescripts[i].SearchFlags);
	return lDispatcher;
}
/******************************************************************************
XDispatch
******************************************************************************/
void ExtensionMenu::displayFatalError(const OUString name) {
	Reference<XToolkit> xToolkit = ExtensionHelper(xComponentContext).getXToolkit();
	Reference<XWindowPeer> xWindowPeer = xToolkit->getDesktopWindow();
	OUString message = OUSTRING("A fatal error occurred during execution of \"") + name + OUSTRING("\".");

	MessageBox::showErrorMessageBox(xToolkit, xWindowPeer,
			OUSTRING("Provenance"), message);
}

void SAL_CALL ExtensionMenu::dispatch(const URL& url,
		const Sequence<PropertyValue>& lArgs) throw (RuntimeException) {
	try {
		if (dispatchHelper.dispatch(this, url, getLog()))
			return;
	}
	catch (...) {
		displayFatalError(url.Path);
		return;
	}
}

void SAL_CALL ExtensionMenu::addStatusListener(const Reference<XStatusListener>& xStatusListener,
		const URL& url) throw (RuntimeException) {
	FeatureStateEvent featureStateEvent;
	if (!dispatchHelper.addStatusListener(this, url, featureStateEvent))
		return;
	featureStateEvent.Source = (XDispatch*) this;
	if (url.Complete.equalsAscii(PROVENANCE_MENU_LOGOUT_URL)) {
		OUString state = OUSTRING("Logout ~<");
		if (isLogoutEnabled())
			// Tildas in the user name are removed, so how to include?
			state += OUSTRING(": ") + LoginDialog::getFirstAndLast();
		featureStateEvent.State = Any(state);
	}
	xStatusListener->statusChanged(featureStateEvent);
}

void SAL_CALL ExtensionMenu::removeStatusListener(
		const Reference<XStatusListener>& xStatusListener, const URL& aURL)
		throw (RuntimeException) {
}
/******************************************************************************
Helper Functions
******************************************************************************/
bool ExtensionMenu::alwaysEnabled() {
	return true;
}

bool ExtensionMenu::alwaysDisabled() {
	return false;
}

bool ExtensionMenu::isLoginEnabled() {
	return !LoginDialog::isLoggedIn();
}

bool ExtensionMenu::isLogoutEnabled() {
	return LoginDialog::isLoggedIn();
}

void ExtensionMenu::about() {
	AboutDialog::display(xComponentContext, OUSTRING(PROVENANCE_MENU_ABOUT_URL));
}

void ExtensionMenu::login() {
	if (!isLoginEnabled())
		return;
	LoginDialog::display(xComponentContext, OUSTRING(PROVENANCE_MENU_LOGIN_URL));
}

void ExtensionMenu::anew() {
	if (isLoginEnabled())
		login();
	if (!isLoginEnabled())
		NewDialog::display(xComponentContext, OUSTRING(PROVENANCE_MENU_NEW_URL), OUSTRING(PROVENANCE_MENU_SHARE_URL),
				LoginDialog::getNetworkInfo(), LoginDialog::getDocumentMetaInfo());
}

void ExtensionMenu::open() {
	if (isLoginEnabled())
		login();
	if (!isLoginEnabled())
		OpenDialog::display(xComponentContext, OUSTRING(PROVENANCE_MENU_OPEN_URL), OUSTRING(PROVENANCE_MENU_HISTORY_URL),
				LoginDialog::getNetworkInfo(), LoginDialog::getDocumentMetaInfo());
}

void ExtensionMenu::audit() {
	if (AuditorRunner::toFront())
		return;
	if (isLoginEnabled())
		login();
	if (!isLoginEnabled())
		AuditDialog::display(xComponentContext, OUSTRING(PROVENANCE_MENU_AUDIT_URL), OUSTRING(PROVENANCE_MENU_HISTORY_URL),
				OUSTRING(PROVENANCE_MENU_AUDITOR_URL), LoginDialog::getNetworkInfo(), LoginDialog::getDocumentMetaInfo());
}

bool ExtensionMenu::isModified(Reference<XFrame>& xFrame) {
	if (isFalse(xFrame.is()))
		return false;

	Reference<XModel> xModel = xFrame->getController()->getModel();
	if (isFalse(xModel.is()))
		return false;

	Reference<XModifiable> xModifiable(xModel, UNO_QUERY);
	if (isFalse(xModifiable.is()))
		return false;
	return isTrue(xModifiable->isModified());
}

int ExtensionMenu::countFrames(Reference<XDesktop> xDesktop) {
	Reference<XEnumerationAccess> xEnumerationAccess = xDesktop->getComponents();
	Reference<XEnumeration> xEnumeration = xEnumerationAccess->createEnumeration();
	int count = 0;

	for (; isTrue(xEnumeration->hasMoreElements()); xEnumeration->nextElement())
		count++;
	return count;
}

bool ExtensionMenu::closeAll() {
	// See https://wiki.openoffice.org/wiki/Documentation/DevGuide/OfficeDev/Closing_Documents
	while (true) {
		try {

			Reference<XComponent> xComponent = DocumentTracker::instance.getXComponent();
			if (!xComponent.is())
				break;

			Reference<XModel> xModel(xComponent, UNO_QUERY);
			Reference<XFrame> xFrame = xModel->getCurrentController()->getFrame();

			Reference<XController> xController = xFrame->getController();
			if (isFalse(xController.is()))
				return false;
			if (isFalse(xController->suspend(sal_True)))
				if (isModified(xFrame))
					// Frame is (still) modified, so give up
					return false;

			Reference<XCloseable> xCloseable(xFrame, UNO_QUERY);
			if (isFalse(xCloseable.is()))
				return false;
			try {
				xCloseable->close(sal_True);
			}
			catch (...) {
			}
		}
		catch (...) {
			return false;
		}
	}
	return true;
}

bool ExtensionMenu::closeAnyway(int count) {
	Reference<XToolkit> xToolkit = ExtensionHelper(xComponentContext).getXToolkit();
	Reference<XWindowPeer> xWindowPeer = xToolkit->getDesktopWindow();

	std::ostringstream ss;
	ss << "The " << count << " Provenance document" << (count == 1 ? "" : "s" ) <<
			" you are editing must be closed before logout.\n"
			"You will be given the opportunity to save modified documents.\n\n"
			"Continue with logout?";
	std::string message = ss.str();

	return MessageBox::showQueryMessageBox(xToolkit, xWindowPeer,
			OUSTRING("Provenance - Logout"), convertToOUString(message)) == MessageBoxResults::YES;
}

void ExtensionMenu::logout() {
	if (!isLogoutEnabled())
		return;
	int count = DocumentTracker::instance.getCount();
	if (count == 0) {
		LoginDialog::logout();
		return;
	}
	if (!closeAnyway(count))
		return;
	if (!closeAll())
		return;
	LoginDialog::logout();

	Reference<XDesktop> xDesktop = ExtensionHelper(xComponentContext).getXDesktop();
	if (countFrames(xDesktop) == 0)
		xDesktop->terminate();
}

void ExtensionMenu::showProvenanceLogger() {
	LoggerDialog::display(xComponentContext, OUSTRING(PROVENANCE_MENU_LOGGER_URL));
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
