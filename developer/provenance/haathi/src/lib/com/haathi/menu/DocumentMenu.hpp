/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/menu/DocumentMenu.hpp#1 $

/*!
 @header DocumentMenu.hpp
 @author Aaron Gibson
 @date October 16, 2011

 This header defines the class "DocumentMenu" which is used to instantiate 
 a new document that is loaded with the proper listeners for provenance events.

 This class will also create a separate thread that will handle the actual encryption
 and sending of the provenance data, so that the user experience is quicker and the
 relevant functionality is isolated. 

 More documentation will come as this class is completed. 
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__DOCUMENT_MENU_HPP
#	define COM__HAATHI__DOCUMENT__DOCUMENT_MENU_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/menu/DispatchHelper.hpp"
#include "com/haathi/menu/DispatchTunneler.hpp"
#include "com/haathi/menu/TunnelerHelper.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"
#include "com/haathi/listener/PrintJob.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "cppJSON.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XDispatchProviderInterceptor.hpp>
#include <com/sun/star/frame/XInterceptorInfo.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/util/URL.hpp>
#include <cppuhelper/implbase3.hxx>
#include "sdkEnd.hpp"

#include <map>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace menu {
/******************************************************************************
DocumentMenu
******************************************************************************/
/*!
 
 @class DocumentMenu
 
 This class is designed to outline the basic features that a DocumentMenu
 MUST implement. This class contains purely virtual functions, depending on how
 the implementor chooses to actually handle the provenance. 
 
 For instance, there will be a subclass of this that will queue the provenance
 events and then encrypted them, before sending them off to the cloud server. 
 Other instances of this class might instead not encrypt the provenance. This gives
 this class and all of its dependencies great flexibility.
 
 Note that this class implements the following interfaces:
 XDispatch, XDispatchProviderInterceptor, XInterceptorInfo
 (XDispatchProviderInterceptor extends XDispatchProvider).
 */
class DocumentMenu: public cppu::WeakImplHelper3<
		com::sun::star::frame::XDispatch,
		com::sun::star::frame::XDispatchProviderInterceptor,
		com::sun::star::frame::XInterceptorInfo> {
	protected:
		static com::haathi::menu::DispatchData<DocumentMenu> dispatchData[];
		static com::haathi::menu::DispatchHelper<DocumentMenu> dispatchHelper;

		static TunnelerData<DocumentMenu> tunnelerData[];
		static TunnelerHelper<DocumentMenu> tunnelerHelper;

		bool isProvenanceURL(const com::sun::star::util::URL& aURL);
		bool alwaysEnabled();
		bool alwaysDisabled();
		bool exploitEnabled();

		virtual void history();
		virtual void share();
		virtual void exploit();
		virtual void watermark();
		virtual void menu(const com::sun::star::util::URL& url);

		Reference<com::sun::star::lang::XComponent> xComponent;
		Reference<com::sun::star::uno::XComponentContext> xComponentContext;

		/*!
		 This makes sure that the component is thread-safe.
		 */
		osl::Mutex lock;

		/*!
		 This stores the top of the chain of the XDispatchProvider's.
		 But, due to the nature of provenance collection, we cannot allow
		 any other interface to happen first. Hence, the mxMaster is really
		 still a slave to this.
		 */
		Reference<com::sun::star::frame::XDispatchProvider> mxMaster;

		/*!
		 This stores the next part of the chain from this XDispatchProvider
		 instance. 
		 */
		Reference<com::sun::star::frame::XDispatchProvider> mxSlave;

	public:
		DocumentMenu(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent);
		virtual ~DocumentMenu() { };

		// Just add an event not knowing document state
		virtual bool addProvenanceEvent(com::haathi::provenance::ProvenanceEvent* event, bool modify = true) { return false; }
		// Add the event with the given document state as OUString
		virtual bool addProvenanceEvent(com::haathi::provenance::ProvenanceEvent* event,
				rtl::OUString documentState) { return false; }
		// Add the multiple events all with the given document state as OUString
		virtual bool addProvenanceEvents(std::vector<com::haathi::provenance::ProvenanceEvent*>& events,
				rtl::OUString documentState) { return false; }

		Reference<com::sun::star::lang::XComponent> getXComponent() {
			return xComponent;
		}

		virtual void save() { }
		virtual void saveAs() { }
		virtual void saveAsTemplate() { }
		virtual void close() { }

		virtual void copy(int subCommand) { }
		virtual void cut(int subCommand) { }
		virtual void paste(int subCommand) { }

		virtual void print(int subCommand) { }
		virtual void exportIt(int subCommand) { }
		virtual void send(int subCommand) { }

		// XDispatchProvider
		virtual Reference<com::sun::star::frame::XDispatch> SAL_CALL queryDispatch(const com::sun::star::util::URL& aURL,
				const rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags)
				throw (com::sun::star::uno::RuntimeException);

		virtual com::sun::star::uno::Sequence<Reference<com::sun::star::frame::XDispatch> > SAL_CALL queryDispatches(
				const com::sun::star::uno::Sequence<com::sun::star::frame::DispatchDescriptor>& seqDescriptor)
				throw (com::sun::star::uno::RuntimeException);

		// XDispatch
		virtual void SAL_CALL dispatch(const com::sun::star::util::URL& aURL,
				const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& lArgs)
				throw (com::sun::star::uno::RuntimeException);

		virtual void SAL_CALL addStatusListener(
				const Reference<com::sun::star::frame::XStatusListener>& xListener, const com::sun::star::util::URL& aURL)
				throw (com::sun::star::uno::RuntimeException);

		virtual void SAL_CALL removeStatusListener(
				const Reference<com::sun::star::frame::XStatusListener>& xListener, const com::sun::star::util::URL& aURL)
				throw (com::sun::star::uno::RuntimeException);

		// XDispatchProviderInterceptor
		virtual Reference<com::sun::star::frame::XDispatchProvider> SAL_CALL getSlaveDispatchProvider()
				throw (com::sun::star::uno::RuntimeException);

		virtual void SAL_CALL setSlaveDispatchProvider(
				const Reference<com::sun::star::frame::XDispatchProvider>& xSlave)
				throw (com::sun::star::uno::RuntimeException);

		virtual Reference<com::sun::star::frame::XDispatchProvider> SAL_CALL getMasterDispatchProvider()
				throw (com::sun::star::uno::RuntimeException);

		virtual void SAL_CALL setMasterDispatchProvider(
				const Reference<com::sun::star::frame::XDispatchProvider>& xMaster)
				throw (com::sun::star::uno::RuntimeException);

		// XInterceptorInfo
		virtual com::sun::star::uno::Sequence<OUString> SAL_CALL getInterceptedURLs()
				throw (com::sun::star::uno::RuntimeException);

		virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& object)
				throw (com::sun::star::uno::RuntimeException) {
		}
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
