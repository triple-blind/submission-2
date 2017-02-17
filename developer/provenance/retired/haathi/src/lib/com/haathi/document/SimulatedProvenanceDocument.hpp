/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/document/SimulatedProvenanceDocument.hpp#1 $

/*!
 
 @header SimulatedProvenanceDocument.hpp
 
 @author Nitin Shinde
 @date July 23, 2012
 
 This header implements the instance of CloudProvenanceDocument that will actually output 
 the generated provenance to the cloud after first encrypting it.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__SIMULATED_PROVENANCE_DOCUMENT_HPP
#	define COM__HAATHI__DOCUMENT__SIMULATED_PROVENANCE_DOCUMENT_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/CloudProvenanceDocument.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/document/ProvenanceDocument.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class
******************************************************************************/
class SimulatedProvenanceDocument: public CloudProvenanceDocument {

	protected:

	public:
		SimulatedProvenanceDocument(Reference<com::sun::star::uno::XComponentContext> _xContext,
			Reference<com::sun::star::lang::XMultiComponentFactory> _xMCF, Reference<com::sun::star::lang::XComponent> _xComponent,
			DocumentMetaInfo info);
			
		virtual ~SimulatedProvenanceDocument();
		
		virtual void save();
		
		static SimulatedProvenanceDocument* createCloudProvenanceDocument( 
				Reference<com::sun::star::uno::XComponentContext> xContext, DocumentMetaInfo _meta_info);
		
		static CloudProvenanceDocument* loadProvenanceDocumentFromStream(
				Reference<com::sun::star::uno::XComponentContext> xContext, std::string str,
				com::haathi::document::DocumentMetaInfo info,
				com::haathi::network::NetworkInfo& networkInfo, std::string& password);
			
		void autoType(std::string pathToInputFile, std::string pathToLogFile);
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
