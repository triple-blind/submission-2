/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/extension/ExtensionHelper.hpp#1 $

/*!
 
 @header ExtensionHelper.hpp
 
 @author Aaron Gibson
 @date March 4, 2012
 
 This header file implements helper functions for managing extensions.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__EXTENSION__EXTENSION_HELPER_HPP
#	define COM__HAATHI__EXTENSION__EXTENSION_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/deployment/XExtensionManager.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XDispatchHelper.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/ucb/XFileIdentifierConverter.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "sdkEnd.hpp"

#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace extension {
/******************************************************************************
ExtensionHelper
******************************************************************************/
class ExtensionHelper {
	protected:
		Reference<com::sun::star::uno::XComponentContext> xComponentContext;

		Reference<com::sun::star::ucb::XFileIdentifierConverter> getXFileIdentifierConverter();

	public:
		static Reference<com::sun::star::uno::XComponentContext> getXComponentContext();
		static Reference<com::sun::star::awt::XWindowPeer> getHiddenXWindowPeer(
				Reference<com::sun::star::lang::XMultiServiceFactory> xMultiServiceFactory,
				Reference<com::sun::star::awt::XToolkit> xToolkit, bool visible = false);
		static void setTitle(Reference<com::sun::star::lang::XComponent> xComponent, std::string documentName, bool isReadOnly);

		ExtensionHelper(Reference<com::sun::star::uno::XComponentContext> xComponentContext);

		OUString getURLToExtensionRoot(rtl::OUString& name);
		OUString getURLToExtensionRoot();

		OUString getSystemFilePathFromURL(rtl::OUString url);
		OUString getURLFromSystemFilePath(rtl::OUString filepath);

		std::string getSystemFilePathToExtensionFile(const char* file);

		Reference<com::sun::star::graphic::XGraphic> getGraphicFromFile(
				rtl::OUString file, sal_Bool inExtension = sal_True);

		Reference<com::sun::star::lang::XMultiComponentFactory> getXMultiComponentFactory();
		Reference<com::sun::star::lang::XMultiServiceFactory> getXMultiServiceFactory();
		Reference<com::sun::star::awt::XToolkit> getXToolkit();
		Reference<com::sun::star::frame::XDesktop> getXDesktop();
		Reference<com::sun::star::frame::XDispatchHelper> getXDispatchHelper();
		Reference<com::sun::star::deployment::XExtensionManager> getXExtensionManager();
};
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/

