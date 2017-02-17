/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/component/ProvenanceManager.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__COMPONENT__PROVENANCE_MANAGER_HPP
#	define COM__HAATHI__COMPONENT__PROVENANCE_MANAGER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/lang/XMain.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/XWeak.hpp>
#include <cppuhelper/compbase3.hxx>
#include <haathi/ProvenanceNetworkException.hpp>
#include <haathi/XProvenanceManager.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace haathi {
/******************************************************************************
Class
******************************************************************************/
/**
 @class Provenance Manager

 This class implements the ProvenanceManager.

 One advantage to implement this as a standalone service is that this service can only
 be constructed through Openoffice mechanisms. This means that software protection can
 be implemented much more easily. And if you don't trust this approach, proper software
 protection on this extension can secure that ONLY this code can be executed and
 connected with the server.
 */
class ProvenanceManager: public cppu::WeakImplHelper3<
		haathi::XProvenanceManager,
		com::sun::star::lang::XMain,
		com::sun::star::lang::XServiceInfo> {
	private:
		ProvenanceManager(const ProvenanceManager& other);
		ProvenanceManager& operator=(const ProvenanceManager& other);

	public:
		//===================================================================
		// Constructors and Destructors
		//===================================================================
		ProvenanceManager();
		virtual ~ProvenanceManager();

		//===================================================================
		// XMain
		//===================================================================
		sal_Int32 SAL_CALL run(const com::sun::star::uno::Sequence< rtl::OUString > & args)
		throw (com::sun::star::uno::RuntimeException);

		//===================================================================
		// XServiceInfo
		//===================================================================
		sal_Bool SAL_CALL supportsService(const rtl::OUString& rServiceName)
		throw (com::sun::star::uno::RuntimeException);

		com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames()
		throw (com::sun::star::uno::RuntimeException);

		rtl::OUString SAL_CALL getImplementationName()
		throw (com::sun::star::uno::RuntimeException);

		//===================================================================
		// Utility
		//===================================================================
		rtl::OUString getURLToExtensionRoot();

		//===================================================================
		// XProvenanceManager
		//===================================================================
		/**
		 * This function will login to the Trusted Server with the given username and password.
		 *
		 * @param serverURL -- the location of the server.
		 * @param username -- the username to login with.
		 * @param password -- the user's password.
		 * @return -- true if the login was successful, false otherwise.
		 * @throw ProvenanceNetworkException -- Thrown in the event of a network error.
		 * @throw RuntimeException -- Thrown when an internal problem occurs.
		 */
		sal_Bool SAL_CALL login(
				const ::rtl::OUString& serverURL,
				const ::rtl::OUString& username,
				const ::rtl::OUString& password )
		throw (ProvenanceNetworkException, com::sun::star::uno::RuntimeException);

		/**
		 * This function will logout from the Trusted Server.
		 *
		 * @return -- true if the logout was successful; false otherwise.
		 * @throw ProvenanceNetworkException -- Thrown in the event of a network error.
		 * @throw RuntimeException -- Thrown when an internal problem occurs.
		 */
		sal_Bool SAL_CALL logout(  )
		throw (ProvenanceNetworkException, com::sun::star::uno::RuntimeException);

		/**
		 * This function will list all of the users currently available on the server
		 * (if they are visible to see). This queries the Trusted Server for the relevant
		 * information.
		 *
		 * @return -- A list of users available on the given server that are visible to the
		 * 			currently logged in user.
		 * @throw ProvenanceNetworkException -- Thrown in the event of a network error.
		 * @throw RuntimeException -- Thrown when an internal problem occurs.
		 */
		com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL listUsers(  )
		throw (ProvenanceNetworkException, com::sun::star::uno::RuntimeException);

		/**
		 * This function will list all of the auditors currently available on the server
		 * (that are visible to the current user). This queries the Trusted server for the
		 * relevant information.
		 *
		 * @return -- A list of available auditors on the server that are visible to the
		 * 			currently logged in user.
		 * @throw ProvenanceNetworkException -- Thrown in the event of a network error.
		 * @throw RuntimeException -- Thrown when an internal problem occurs.
		 */
		com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL listAvailableAuditors(  )
		throw (ProvenanceNetworkException, com::sun::star::uno::RuntimeException);

		/**
		 * This function will list all of the documents that are available for edit by the
		 * current user.
		 *
		 * @return -- A list of available documents for edit.
		 * @throw ProvenanceNetworkException -- Thrown in the event of a network error.
		 * @throw RuntimeException -- Thrown when an internal problem occurs.
		 */
		com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL listEditableDocuments(  )
		throw (ProvenanceNetworkException, com::sun::star::uno::RuntimeException);

		/**
		 * This function will list all of the documents that are available for audit by the
		 * current user.
		 *
		 * @return -- A list of available documents for audit.
		 * @throw ProvenanceNetworkException -- Thrown in the event of a network error.
		 * @throw RuntimeException -- Thrown when an internal problem occurs.
		 */
		com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL listAuditableDocuments(  )
		throw (ProvenanceNetworkException, com::sun::star::uno::RuntimeException);

		/**
		 * This function will create a provenance document of the given type and information.
		 *
		 * TODO -- Document the available properties.
		 *
		 * @param documentType -- the type of document to create
		 * @param documentName -- the name of the document to create
		 * @param options -- the sequence of Properties for creating the Provenance Document.
		 * @return -- The document component that is created.
		 * @throw ProvenanceNetworkException -- Thrown in the event of a network error.
		 * @throw RuntimeException -- Thrown when an internal error occurs.
		 */
		com::sun::star::uno::Reference< com::sun::star::lang::XComponent > SAL_CALL createProvenanceDocument(
				::haathi::ProvenanceDocumentType documentType,
				const ::rtl::OUString& documentName,
				const com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >& options )
		throw (ProvenanceNetworkException, com::sun::star::uno::RuntimeException);

		/**
		 * This function will load an existing provenance document from the cloud server.
		 *
		 * TODO -- Document the available properties.
		 *
		 * @param document_name -- the name of the document to load.
		 * @return -- The component of the loaded provenance document.
		 * @throw ProvenanceNetworkException -- Thrown in the event of a network error.
		 * @throw RuntimeException -- Thrown when an internal error occurs.
		 */
		com::sun::star::uno::Reference< com::sun::star::lang::XComponent > SAL_CALL loadProvenanceDocument(
				const ::rtl::OUString& document_name )
		throw (ProvenanceNetworkException, com::sun::star::uno::RuntimeException);
};

//========================================================
// C Wrapper Functions
//========================================================

/*!
 This function will return the implementation name of this component. This is simply a "C"
 wrapper function for the member function "getImplementationName()".

 @result -- the name of the implementation
 @throw RuntimeException -- this exception is thrown for any errors.
 */
rtl::OUString ProvenanceManager_getImplementationName()
		throw (com::sun::star::uno::RuntimeException);

/*!
 This function will return whether the given service is supported by this component.
 Again, this is simply a "C" wrapper function for the member function "supportsService".

 @result -- true if the given service is supported, false otherwise

 @throw RuntimeException -- thrown for any errors
 */
sal_Bool SAL_CALL ProvenanceManager_supportsService(const rtl::OUString& ServiceName)
		throw (com::sun::star::uno::RuntimeException);

/*!
 This function will return all supported services of this component. This is a "C" wrapper function
 around the member function "getSupportedServiceNames()".

 @result -- a Sequence of strings indicating all supported services.

 @throw RuntimeException -- thrown for any errors.
 */
com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL ProvenanceManager_getSupportedServiceNames()
		throw (com::sun::star::uno::RuntimeException);

/*!
 This function will create an instance of this component "ProvenanceMenu". It will return
 an appropriate reference that can be "casted" into any service that this component actually
 supports.

 @param rSMgr -- the XMultiServiceFactory to construct the ProvenanceMenu component from.
 @result -- a Reference to the constructed component.

 @throw Exception -- this exception is thrown if there is a serious problem during the creation
 of this component.
 */
com::sun::star::uno::Reference< com::sun::star::uno::XInterface >
		SAL_CALL ProvenanceManager_createInstance(
		const com::sun::star::uno::Reference<com::sun::star::lang::XMultiServiceFactory> & rSMgr)
		throw (com::sun::star::uno::Exception);
/******************************************************************************
Namespace
******************************************************************************/
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
