/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/extension/UpdateManager.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__EXTENSION__UPDATE_MANAGER_HPP
#	define COM__HAATHI__EXTENSION__UPDATE_MANAGER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/uno/XInterface.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace extension {
/******************************************************************************
Class
******************************************************************************/
class UpdateManager {
	protected:
		static OUString extensionIdentifier;
		static OUString extensionName;
		static OUString updateURL;

		Reference<com::sun::star::uno::XComponentContext> xComponentContext;
		Reference<com::sun::star::uno::XInterface> universalContentBroker;

	public:
		UpdateManager(Reference<com::sun::star::uno::XComponentContext> xComponentContext);
		bool isExtensionUpdateAvailable();
		void updateExtension();
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
