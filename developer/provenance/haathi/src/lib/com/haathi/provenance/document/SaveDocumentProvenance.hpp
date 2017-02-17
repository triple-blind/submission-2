/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/SaveDocumentProvenance.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__DOCUMENT__SAVE_DOCUMENT_PROVENANCE_H
#	define COM__HAATHI__PROVENANCE__DOCUMENT__SAVE_DOCUMENT_PROVENANCE_H
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/provenance/ProvenanceEvent.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
SaveDocumentProvenance
******************************************************************************/
class SaveDocumentProvenance: public ProvenanceDocumentEvent {
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
	}

	public:
		SaveDocumentProvenance();
		//virtual ~SaveDocumentProvenance();
		//SaveDocumentProvenance& operator=(const SaveDocumentProvenance& other);
		//SaveDocumentProvenance(const SaveDocumentProvenance& other);

		virtual std::string getEventType() const;
		virtual rtl::OUString printToTable() const;
		//virtual rtl::OUString getEventDescription() const;

		//virtual bool redo(Reference<com::sun::star::lang::XComponent> text_doc, com::sun::star::util::Color color) const;
		//virtual bool undo(Reference<com::sun::star::lang::XComponent> text_doc, com::sun::star::util::Color color) const;
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
