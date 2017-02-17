/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/PrintJobDocumentProvenance.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__DOCUMENT__PRINT_JOB_DOCUMENT_PROVENANCE_H
#	define COM__HAATHI__PROVENANCE__DOCUMENT__PRINT_JOB_DOCUMENT_PROVENANCE_H
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
PrintJobDocumentProvenance
******************************************************************************/
class PrintJobDocumentProvenance: public ProvenanceDocumentEvent {
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
		ar & BOOST_SERIALIZATION_NVP(state);
	}

	private:
		PrintJobDocumentProvenance();
	protected:	
		std::string state;

	public:
		PrintJobDocumentProvenance(std::string state);
		//virtual ~PrintJobDocumentProvenance();
		PrintJobDocumentProvenance& operator=(const PrintJobDocumentProvenance& other);
		PrintJobDocumentProvenance(const PrintJobDocumentProvenance& other);

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
