/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/document/FileProvenanceDocument.hpp#1 $

/*!
 
 @header FileProvenanceDocument.hpp
 
 @author Aaron Gibson
 @date March 22, 2012
 
 This header implements classes that will send all of the provenance events to the designated file,
 without encrypting them. This is mainly for debugging and interface fine-tuning.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__FILE_PROVENANCE_DOCUMENT_HPP
#	define COM__HAATHI__DOCUMENT__FILE_PROVENANCE_DOCUMENT_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/ProvenanceDocument.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include "sdkEnd.hpp"

#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class
******************************************************************************/
class FileProvenanceDocument: public ProvenanceDocument {
	private:
		std::vector<com::haathi::provenance::ProvenanceEvent*> events;
		std::string path;
	protected:
	public:
		/*!
		 This constructor will create the ProvenanceDocument that stores the provenance to a file
		 specified by the string path. There will not be any encryption in this.
		 
		 @param _xContext -- the XComponentContext for this document.
		 @param _xMCF -- the XMultiComponentFactory for this document.
		 @param _xComponent -- the XComponent for this document.
		 @param _path -- the path to the file to store the provenance to.
		 */
		FileProvenanceDocument(Reference<com::sun::star::uno::XComponentContext> _xContext,
				Reference<com::sun::star::lang::XMultiComponentFactory> _xMCF,
				Reference<com::sun::star::lang::XComponent> _xComponent, std::string _path);
		virtual ~FileProvenanceDocument();

		virtual bool addProvenanceEvent(
				com::haathi::provenance::ProvenanceEvent* event,
				std::string document_state);
		virtual void save();
		virtual void copy();
		virtual void print();
		
		virtual void watermark(com::haathi::provenance::ProvenanceEvent* current_event);
		/*!
		 This function will return a new FileProvenanceDocument that will store the provenance
		 information to the given path on saves.
		 */
		static FileProvenanceDocument* createFileProvenanceDocument(
				Reference<com::sun::star::uno::XComponentContext> xContext, std::string path);
		/*!
		 This function will return a new FileProvenanceDocument after first showing a dialog
		 that prompts for the path to store the provenance.
		 */
		static FileProvenanceDocument* createFileProvenanceDocumentWithDialog(
				Reference<com::sun::star::uno::XComponentContext> xContext);
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
