/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/ExportDocumentProvenance.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__DOCUMENT__EXPORT_DOCUMENT_PROVENANCE_H
#	define COM__HAATHI__PROVENANCE__DOCUMENT__EXPORT_DOCUMENT_PROVENANCE_H
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
ExportDocumentProvenance
******************************************************************************/
class ExportDocumentProvenance: public ProvenanceDocumentEvent {
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
		ar & BOOST_SERIALIZATION_NVP(details);
	}

	protected:
		std::string details;
		ExportDocumentProvenance();
	
	public:
		ExportDocumentProvenance(std::string details);
		//virtual ~ExportDocumentProvenance();
		//ExportDocumentProvenance& operator=(const ExportDocumentProvenance& other);
		//ExportDocumentProvenance(const ExportDocumentProvenance& other);

		virtual std::string getEventType() const;
		virtual rtl::OUString printToTable() const;
		virtual rtl::OUString getEventDescription() const;

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
