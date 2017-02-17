/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/menu/ExtensionMenu.hpp#1 $

/*
 *  Created by Aaron Gibson on 7/28/11.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__MENU__EXTENSION_MENU_H
#	define COM__HAATHI__MENU__EXTENSION_MENU_H
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/menu/DispatchHelper.hpp"
#include "com/haathi/utils/Debug.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/XInterface.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/URL.hpp>
#include <cppuhelper/implbase4.hxx> // WeakImplHelper4
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace menu {
/******************************************************************************
Class
******************************************************************************/
/*!
 @class ExtensionMenu
 
 @author: Aaron Gibson
 
 This class implements the details of the "Provenance" menu to be displayed as OpenOffice is running.
 */
class ExtensionMenu: public cppu::WeakImplHelper4<com::sun::star::frame::XDispatch,
		com::sun::star::frame::XDispatchProvider,
		com::sun::star::lang::XInitialization, com::sun::star::lang::XServiceInfo> {
	protected:
		static com::haathi::menu::DispatchData<ExtensionMenu> dispatchData[];
		static com::haathi::menu::DispatchHelper<ExtensionMenu> dispatchHelper;

		Reference<com::sun::star::uno::XComponentContext> xComponentContext; // Needed to find paths to extension
		Reference<com::sun::star::frame::XFrame> xFrame;		                // Presumably the one this object is attached to

		bool alwaysEnabled();
		bool alwaysDisabled();
		bool isLoginEnabled();
		bool isLogoutEnabled();

		void startProvenance();
		void showProvenanceLogger();
		void showAdministratorDialog();

		void about();
		void login();
		void anew();
		void open();
		void audit();
		void logout();
		
		bool isModified(Reference<com::sun::star::frame::XFrame>& xFrame);
		int countFrames(Reference<com::sun::star::frame::XDesktop> xDesktop);
		bool closeAnyway(int count);
		bool closeAll();
		void displayFatalError(const rtl::OUString name);

	public:
		/*!
		 Default Constructor
		 
		 This constructor will create the provenance menu from the passed XMultiServiceFactory. 
		 TODO -- actually create a new XMultiServiceFactory when constructing this...
		 
		 @param rxMSF -- the remote XMultiServiceFactory to construct the menu with.
		 */
		ExtensionMenu(const Reference<com::sun::star::uno::XComponentContext>& xComponentContext);

		/*!
		 Destructor
		 */
		virtual ~ExtensionMenu();

		//----------------------------------------------------------------
		// XDispatchProvider
		//----------------------------------------------------------------	
		/*!
		 This function is called to determine whether an event that has been dispatched should be resolved
		 in this class. 
		 
		 @param aURL -- the URL of the dispatch (indicates type of dispatch).
		 @param sTargetFrameName -- don't know
		 @param nSearchFlags -- any appropriate flags needed for resolving the dispatch.
		 @result -- an instance of the created XDispatch.
		 
		 @throw RuntimeException -- this exception is thrown if there was any type of error
		 */
		virtual Reference<com::sun::star::frame::XDispatch> SAL_CALL queryDispatch(const com::sun::star::util::URL& aURL,
				const rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags)
				throw (com::sun::star::uno::RuntimeException);

		/*!
		 This function is called to determine whether the sequence of events that have been dispatched should
		 be resolved in this class. This function works by simply calling "queryDispatch" on each element of
		 the Sequence in "seqDescriptor".
		 
		 @param seqDescriptor -- the sequence of dispatches to determine the resolution of.
		 @result -- a Sequence of the appropriate XDispatches
		 
		 @throw RuntimeException -- this exception is thrown for all errors.
		 */
		virtual com::sun::star::uno::Sequence<Reference<com::sun::star::frame::XDispatch> > SAL_CALL queryDispatches(
				const com::sun::star::uno::Sequence<com::sun::star::frame::DispatchDescriptor>& seqDescriptor)
				throw (com::sun::star::uno::RuntimeException);

		//----------------------------------------------------------------
		// XDispatch
		//----------------------------------------------------------------	

		/*!
		 This function will actually resolve the dispatch event based on the passed in parameters.
		 The URL object indicates the type of dispatch. In this class, a dispatch occurs when a
		 menu item is selected. This function will determine which menu item was selected and respond
		 accordingly.
		 
		 @param aURL -- the URL indicating the type of dispatch (i.e what menu item was selected)
		 @param lArgs -- any additional information about the dispatch event.
		 
		 @throw RuntimeException -- this exception is thrown for all errors.
		 */
		virtual void SAL_CALL dispatch(const com::sun::star::util::URL& aURL,
				const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& lArgs) throw (com::sun::star::uno::RuntimeException);

		/*!
		 This function is called to add a status listener... I don't use this, but it has to be defined 
		 for the XDispatch interface. Maybe I'll use it later.
		 */
		virtual void SAL_CALL addStatusListener(
				const Reference<com::sun::star::frame::XStatusListener>& xStatusListener, const com::sun::star::util::URL& aURL)
				throw (com::sun::star::uno::RuntimeException);

		/*!
		 This function is called to remove a status listener... I don't use this, but it has to be defined
		 for the XDispatch interface. I might use this later.
		 */
		virtual void SAL_CALL removeStatusListener(
				const Reference<com::sun::star::frame::XStatusListener>& xStatusListener, const com::sun::star::util::URL& aURL)
				throw (com::sun::star::uno::RuntimeException);

		//----------------------------------------------------------------
		// XInitialization
		//----------------------------------------------------------------	
		/*!
		 This function is called whenever the menu is created. This will initialize all relevent parameters
		 that are passed in through the Sequence "aArguments".
		 
		 @param aArguments -- any relevant parameters for the proper initialization of this menu.

		 @throw Exception -- this exception is thrown for any improper initialization or type mismatches.
		 @throw RuntimeException -- this exception is thrown for everything else.
		 */
		virtual void SAL_CALL initialize(const com::sun::star::uno::Sequence<com::sun::star::uno::Any>& aArguments)
				throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);

		//----------------------------------------------------------------
		// XServiceInfo
		//----------------------------------------------------------------	

		/*!
		 This function is required for the UNO registry to actually load the menu and its actions
		 to the screen. This function will return the implementation name for this menu component.
		 
		 More accurately, this returns the name of the component AS IT IS DEFINED IN THE XCU (XML)
		 file. This part of OpenOffice gets excessively confusing, but every component in the GUI
		 must be "registered" and the XCU file appropriately sets this up.
		 
		 @result -- a String indicating the name of the implementation for this component.
		 
		 @throw RuntimeException -- this exception is thrown for any problems.
		 */
		virtual rtl::OUString SAL_CALL getImplementationName()
				throw (com::sun::star::uno::RuntimeException);

		/*!
		 This function will return whether the given service is supported by this component.
		 
		 @param ServiceName -- the name of the service in the query.
		 @result -- true if the given service is supported by this component; false otherwise
		 
		 @throw RuntimeException -- this exception is thrown for any errors.
		 */
		virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName)
				throw (com::sun::star::uno::RuntimeException);

		/*!
		 This function willr eturn a list of all of the supported services in this component.
		 
		 @result -- a Sequence of strings indicating all supported services.
		 
		 @throw RuntimeException -- this exception is thrown for any errors.
		 */
		virtual com::sun::star::uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames()
				throw (com::sun::star::uno::RuntimeException);
};

/*!
 This function will return the implementation name of this component. This is simply a "C" 
 wrapper function for the member function "getImplementationName()".
 
 @result -- the name of the implementation
 @throw RuntimeException -- this exception is thrown for any errors.
 */
rtl::OUString ExtensionMenu_getImplementationName() throw (com::sun::star::uno::RuntimeException);

/*!
 This function will return whether the given service is supported by this component.
 Again, this is simply a "C" wrapper function for the member function "supportsService".
 
 @result -- true if the given service is supported, false otherwise
 
 @throw RuntimeException -- thrown for any errors
 */
sal_Bool SAL_CALL ExtensionMenu_supportsService(const rtl::OUString& ServiceName)
		throw (com::sun::star::uno::RuntimeException);

/*!
 This function will return all supported services of this component. This is a "C" wrapper function
 around the member function "getSupportedServiceNames()".
 
 @result -- a Sequence of strings indicating all supported services.
 
 @throw RuntimeException -- thrown for any errors.
 */
com::sun::star::uno::Sequence<rtl::OUString> SAL_CALL ExtensionMenu_getSupportedServiceNames()
		throw (com::sun::star::uno::RuntimeException);

/*!
 This function will create an instance of this component "ExtensionMenu". It will return
 an appropriate reference that can be "casted" into any service that this component actually
 supports.
 
 @param rSMgr -- the XMultiServiceFactory to construct the ExtensionMenu component from.
 @result -- a Reference to the constructed component.
 
 @throw Exception -- this exception is thrown if there is a serious problem during the creation
 of this component.
 */
/*Reference<XInterface>
SAL_CALL ExtensionMenu_createInstance(
		const Reference<XMultiServiceFactory> & rSMgr)
throw (com::sun::star::uno::Exception);
*/
Reference<com::sun::star::uno::XInterface> SAL_CALL ExtensionMenu_createInstance(
		const Reference<com::sun::star::uno::XComponentContext>& xComponentContext)
		throw (com::sun::star::uno::Exception);
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
