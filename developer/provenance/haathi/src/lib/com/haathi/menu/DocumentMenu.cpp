/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/menu/DocumentMenu.cpp#1 $

//
// Implementation of DocumentMenu.hpp
//
// Author: Aaron Gibson
// Date: April 1, 2012
//
// This source file will implement the details of DocumentMenu. This includes
// registering a document with the proper XDispatchProviderInterceptor stuff so that
// we "intercept" save operations (and any other operation for that matter). This allows
// us to change the default behaviour of certain actions such as "save", "load", "print",
// etc., depending on the nature of the provenance collection scheme.
//
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/menu/DocumentMenu.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/exploit/Exploit.hpp"
#include "com/haathi/menu/MenuConfig.hpp"
#include "com/haathi/menu/SubCommand.hpp"
#include "com/haathi/utils/Logger.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/view/XPrintJobBroadcaster.hpp>
#include "sdkEnd.hpp"

#include <fstream> // diagnostic
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::menu;
using namespace com::haathi::utils;

using com::haathi::exploit::pExploit;

using namespace com::sun::star::beans;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
using namespace com::sun::star::view;

using namespace cppu;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace menu {
/******************************************************************************
DocumentMenu
******************************************************************************/	
int DispatchTunneler::count = 0;

DispatchData<DocumentMenu> DocumentMenu::dispatchData[] = {
	{OUSTRING(PROVENANCE_MENU_HISTORY_URL),   &DocumentMenu::history,   &DocumentMenu::alwaysEnabled},
	{OUSTRING(PROVENANCE_MENU_SHARE_URL),     &DocumentMenu::share,     &DocumentMenu::alwaysEnabled},
	{OUSTRING(PROVENANCE_MENU_EXPLOIT_URL),   &DocumentMenu::exploit,   &DocumentMenu::exploitEnabled},
	{OUSTRING(PROVENANCE_MENU_WATERMARK_URL), &DocumentMenu::watermark, &DocumentMenu::alwaysEnabled},
	{OUSTRING("slot:"),                       &DocumentMenu::save,      &DocumentMenu::alwaysEnabled},
	{OUSTRING(".uno:Save"),                   &DocumentMenu::save,      &DocumentMenu::alwaysEnabled},
	{OUSTRING(".uno:SaveAs"),                 0,                        &DocumentMenu::alwaysDisabled},
	{OUSTRING(".uno:SaveAsTemplate"),         0,                        &DocumentMenu::alwaysDisabled},

	{OUSTRING(".uno:PrintPreview"),           0,                        &DocumentMenu::alwaysDisabled},
	{OUSTRING(".uno:NewHtmlDoc"),             0,                        &DocumentMenu::alwaysDisabled},
	{OUSTRING(".uno:NewGlobalDoc"),           0,                        &DocumentMenu::alwaysDisabled},
	{OUSTRING(".uno:WebHtml"),                0,                        &DocumentMenu::alwaysDisabled},

	{OUSTRING(".uno:CloseFrame"),             &DocumentMenu::close,     &DocumentMenu::alwaysEnabled},
	{OUSTRING(".uno:CloseDoc"),               &DocumentMenu::close,     &DocumentMenu::alwaysEnabled},

	{OUSTRING("")}
};

DispatchHelper<DocumentMenu> DocumentMenu::dispatchHelper(dispatchData);

TunnelerData<DocumentMenu> DocumentMenu::tunnelerData[] = {
	{OUSTRING(".uno:Copy"),                     &DocumentMenu::copy,     DEFAULT},
	{OUSTRING(".uno:Cut"),                      &DocumentMenu::cut,      DEFAULT},
	{OUSTRING(".uno:Paste"),                    &DocumentMenu::paste,    DEFAULT},

	{OUSTRING(".uno:Print"),                    &DocumentMenu::print,    PRINT},
	{OUSTRING(".uno:PrintDefault"),             &DocumentMenu::print,    PRINT_DEFAULT},
	{OUSTRING(".uno:PrintPreview"),             &DocumentMenu::print,    PRINT_PREVIEW},

//	{OUSTRING(".uno:ExportDirectToPDF"),        &DocumentMenu::exportIt, EXPORT_DIRECT_TO_PDF},
	{OUSTRING(".uno:ExportTo"),                 &DocumentMenu::exportIt, EXPORT_TO},
	{OUSTRING(".uno:ExportToPDF"),              &DocumentMenu::exportIt, EXPORT_TO_PDF},

	{OUSTRING(".uno:SendMail"),                 &DocumentMenu::send,     SEND_MAIL},
	{OUSTRING(".uno:SendMailDocAsMS"),          &DocumentMenu::send,     SEND_MAIL_DOC_AS_MS},
	{OUSTRING(".uno:SendMailDocAsOOo"),         &DocumentMenu::send,     SEND_MAIL_DOC_AS_OOO},
	{OUSTRING(".uno:SendMailDocAsPDF"),         &DocumentMenu::send,     SEND_MAIL_DOC_AS_PDF},
	{OUSTRING(".uno:SendOutlineToClipboard"),   &DocumentMenu::send,     SEND_OUTLINE_TO_CLIPBOARD},
	{OUSTRING(".uno:SendOutlineToStarImpress"), &DocumentMenu::send,     SEND_OUTLINE_TO_STAR_IMPRESS},
	{OUSTRING("")}
};

TunnelerHelper<DocumentMenu> DocumentMenu::tunnelerHelper(tunnelerData);

bool DocumentMenu::alwaysEnabled() {
	return true;
}

bool DocumentMenu::alwaysDisabled() {
	return false;
}

bool DocumentMenu::exploitEnabled() {
	return pExploit != NULL && pExploit->isEnabled(); // Pass something here for analysis
}

DocumentMenu::DocumentMenu(Reference<XComponentContext> xComponentContext,
		Reference<XComponent> xComponent):
		xComponent(xComponent), xComponentContext(xComponentContext) {
	// Add this to the current component so that we actually intercept these actions.
	Reference<XDispatchProviderInterceptor> xDispatchProviderInterceptor =
			static_cast<XDispatchProviderInterceptor*>(this);
	Reference<XModel> xModel(xComponent, UNO_QUERY);
	Reference<XFrame> xFrame = xModel->getCurrentController()->getFrame();
	Reference<XDispatchProviderInterception> xInterception(xFrame, UNO_QUERY);

	xInterception->registerDispatchProviderInterceptor(xDispatchProviderInterceptor);
}

Reference<XDispatch> SAL_CALL DocumentMenu::queryDispatch(
		const URL& url, const OUString& sTargetFrameName, sal_Int32 nSearchFlags)
		throw(RuntimeException) {
#if 0
	std::ofstream file("C:\\Users\\Keith Alcock\\Documents\\My Data\\document_urls.txt",
			std::ios::out | std::ios::app | std::ios::binary);
	if (file.is_open()) {
		file << url.Complete << std::endl;
		file.close();
	}
#endif

	Reference<XDispatch> xDispatch;

	// Commands coming from the Provenance menu are handled here.
	xDispatch = dispatchHelper.queryDispatch(url, this);
	if (xDispatch != 0)
		return xDispatch;
	
	// Move on to tunneling options
	xDispatch = tunnelerHelper.queryDispatch(url, this);
	// We'll do it here anyway in order to log menu selection
	if (!xDispatch.is())
		xDispatch = static_cast<XDispatch*>(this);

	Reference<XDispatch> above = xDispatch;
	osl::MutexGuard guard(lock);
	Reference<XDispatch> below = mxSlave.is() ?
			mxSlave->queryDispatch(url, sTargetFrameName, nSearchFlags) :
			Reference<XDispatch>();
	DispatchTunneler* pDispatchTunneler = new DispatchTunneler(above, below, lock);
	return static_cast<XDispatch*>(pDispatchTunneler);
}

Sequence <Reference<XDispatch> > SAL_CALL DocumentMenu::queryDispatches(
		const Sequence<DispatchDescriptor>& seqDescriptor)
		throw(RuntimeException) {
	sal_Int32 count = seqDescriptor.getLength();
	Sequence<Reference<XDispatch> > dispatches(count);
	for (sal_Int32 i = 0; i < count; ++i)
		dispatches[i] = queryDispatch(seqDescriptor[i].FeatureURL,
				seqDescriptor[i].FrameName, seqDescriptor[i].SearchFlags);
	return dispatches;
}

void SAL_CALL DocumentMenu::dispatch(const URL& url, const Sequence<PropertyValue>& lArgs)
		throw (RuntimeException) {
	try {
		if (dispatchHelper.dispatch(this, url, getLog()))
			return;
		if (tunnelerHelper.dispatch(this, url, getLog()))
			return;
		menu(url);
	}
	catch (...) {
		return;
	}
}

void SAL_CALL DocumentMenu::addStatusListener(const Reference<XStatusListener>& xListener,
		const URL& url) throw (RuntimeException) {
	FeatureStateEvent featureStateEvent;
	if (!dispatchHelper.addStatusListener(this, url, featureStateEvent))
		return;
	featureStateEvent.Source = (XDispatch*) this;
	if (url.Complete.equalsAscii(PROVENANCE_MENU_EXPLOIT_URL)) {
		OUString state = OUSTRING("E~xploit");
		if (com::haathi::exploit::pExploit != NULL)
			state += OUSTRING(": ") + com::haathi::exploit::pExploit->getName();
		featureStateEvent.State = Any(state);
	}
	xListener->statusChanged(featureStateEvent);
}

void SAL_CALL DocumentMenu::removeStatusListener(const Reference<XStatusListener>& xListener,
		const URL& aURL) throw (RuntimeException) {
}

void DocumentMenu::share() {
}

void DocumentMenu::history() {
}

void DocumentMenu::exploit() {
}

void DocumentMenu::watermark() {
}

void DocumentMenu::menu(const URL& url) {
}

// XDispatchProviderInterceptor
Reference<XDispatchProvider> SAL_CALL DocumentMenu::getSlaveDispatchProvider()
		throw (RuntimeException) {
	osl::MutexGuard guard(lock);
	return mxSlave;
}

void SAL_CALL DocumentMenu::setSlaveDispatchProvider(const Reference<XDispatchProvider>& xSlave)
	throw (RuntimeException) {
	osl::MutexGuard guard(lock);
	mxSlave = xSlave;
}

// TODO -- change this so that we are ALWAYS the master dispatch provider...
Reference<XDispatchProvider> SAL_CALL DocumentMenu::getMasterDispatchProvider()
		throw (RuntimeException) {
	osl::MutexGuard guard(lock);
	return mxMaster;
}

void SAL_CALL DocumentMenu::setMasterDispatchProvider(
		const Reference< XDispatchProvider >& xMaster) throw (RuntimeException) {
//	Reference< XDispatchProviderInterceptor > mxSlaveInterceptor(mxSlave, UNO_QUERY);
//
//	if(!mxSlaveInterceptor.is()){
//		std::cout << "Slave not proper instance..." << std::endl;
//		return;
//	}
//	mxSlaveInterceptor->setMasterDispatchProvider(xMaster);
//	
//	mxSlave = xMaster;

	// Do nothing...?

	osl::MutexGuard guard(lock);
	mxMaster = xMaster;
}

// XInterceptorInfo
Sequence<OUString> SAL_CALL DocumentMenu::getInterceptedURLs()
		throw (RuntimeException) {
	Sequence<OUString> result(2);

	result[0] = OUSTRING("slot:*");
	result[1] = OUSTRING(".uno:*");
	return result;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
