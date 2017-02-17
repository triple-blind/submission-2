/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/DocumentStore.hpp#1 $

/*!
 @header StreamLoader.hpp
 
 @author Aaron Gibson
 @date February 23, 2012
 
 This header file implements some static functions that will save and load a document to various 
 streams. More functionality will be added as the need arises.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__DOCUMENT_STORE_HPP
#	define COM__HAATHI__DOCUMENT__DOCUMENT_STORE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "sdkEnd.hpp"

#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
DocumentStore
******************************************************************************/
class DocumentStore {
	protected:
		Reference<com::sun::star::frame::XComponentLoader> xComponentLoader;

		Reference<com::sun::star::frame::XComponentLoader> getXComponentLoader(
				Reference<com::sun::star::uno::XComponentContext> xComponentContext);

	public:
		DocumentStore(Reference<com::sun::star::uno::XComponentContext> xComponentContext);
		DocumentStore();

		Reference<com::sun::star::lang::XComponent> loadDocumentFromString(
				const std::string& str,
				com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue> propertyValues = com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>())
				throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);

		std::string saveDocumentToString(
				Reference<com::sun::star::frame::XStorable> xStorable,
				com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue> propertyValues = com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>())
				throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);

		Reference<com::sun::star::lang::XComponent> createDocument(
				com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue> propertyValues = com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>());
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
