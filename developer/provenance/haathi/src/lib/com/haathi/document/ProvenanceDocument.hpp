/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/ProvenanceDocument.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__PROVENANCE_DOCUMENT_HPP
#	define COM__HAATHI__DOCUMENT__PROVENANCE_DOCUMENT_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/menu/DocumentMenu.hpp"

#include <com/sun/star/view/PrintJobEvent.hpp>
#include <com/sun/star/view/XPrintJobListener.hpp>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class or Content
******************************************************************************/
class ProvenanceDocument: public com::haathi::menu::DocumentMenu {
	public:
		ProvenanceDocument(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				Reference<com::sun::star::lang::XComponent> xComponent);
		virtual ~ProvenanceDocument();

		virtual void SAL_CALL printJobEvent(const com::sun::star::view::PrintJobEvent& event)
				throw (com::sun::star::uno::RuntimeException) { }
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
