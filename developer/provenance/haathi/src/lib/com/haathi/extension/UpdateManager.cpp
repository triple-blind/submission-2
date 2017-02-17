/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/extension/UpdateManager.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/extension/CommandEnvironment.hpp"
#include "com/haathi/extension/UpdateManager.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/deployment/PackageInformationProvider.hpp>
#include <com/sun/star/deployment/ExtensionManager.hpp>
#include <com/sun/star/deployment/XExtensionManager.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::sun::star::beans::NamedValue;
using com::sun::star::deployment::ExtensionManager;
using com::sun::star::deployment::PackageInformationProvider;
using com::sun::star::deployment::XExtensionManager;
using com::sun::star::deployment::XPackage;
using com::sun::star::deployment::XPackageInformationProvider;
using com::sun::star::lang::XMultiComponentFactory;
using com::sun::star::ucb::XCommandEnvironment;
using com::sun::star::uno::Any;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::XComponentContext;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace extension {
/******************************************************************************
DialogMessageBoxProvider
******************************************************************************/
OUString UpdateManager::extensionIdentifier = OUSTRING("com.provenance.extensions.haathi");
OUString UpdateManager::extensionName = OUSTRING("haathi.oxt");
OUString UpdateManager::updateURL = OUSTRING("http://haathi.cs.arizona.edu/update/haathi.oxt");

UpdateManager::UpdateManager(Reference<XComponentContext> xComponentContext):
		xComponentContext(xComponentContext) {
	Reference<XMultiComponentFactory> xMultiComponentFactory = xComponentContext->getServiceManager();
			
	Sequence<Any> aArgs(2);
	aArgs[0] <<= OUSTRING("Local");
	aArgs[1] <<= OUSTRING("Office");

	// The variable is never used, but this has to be created anyway according to UCB documentation
	universalContentBroker = xMultiComponentFactory->createInstanceWithArgumentsAndContext(
			OUSTRING("com.sun.star.ucb.UniversalContentBroker"), aArgs, xComponentContext);
}

bool UpdateManager::isExtensionUpdateAvailable() {
	Reference<XPackageInformationProvider> xPackageInformationProvider(
			PackageInformationProvider::get(xComponentContext));
	Sequence<Sequence<OUString> > availableUpdates =
			xPackageInformationProvider->isUpdateAvailable(extensionIdentifier);

	return availableUpdates.getLength() > 0;
}

void UpdateManager::updateExtension() {
	Reference<XExtensionManager> xExtensionManager(ExtensionManager::get(xComponentContext));

	UpdateCommandEnvironment* updateCommandEnvironment = new UpdateCommandEnvironment();
	Reference<XCommandEnvironment> xCommandEnvironment =
			static_cast<XCommandEnvironment*>(updateCommandEnvironment);

	Reference<XPackage> xPackage = xExtensionManager->addExtension(
			updateURL, Sequence<NamedValue>(), OUSTRING("user"),
			xExtensionManager->createAbortChannel(), xCommandEnvironment);

	if (xPackage != NULL) {
		std::cout << "Updated Version : " << xPackage->getVersion()
				<< std::endl;
	}
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
