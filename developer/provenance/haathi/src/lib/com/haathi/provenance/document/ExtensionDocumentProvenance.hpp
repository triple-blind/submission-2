/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/ExtensionDocumentProvenance.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__DOCUMENT__EXTENSION_DOCUMENT_PROVENANCE_H
#	define COM__HAATHI__PROVENANCE__DOCUMENT__EXTENSION_DOCUMENT_PROVENANCE_H
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
ExtensionDocumentProvenance
******************************************************************************/
class ExtensionDocumentProvenance: public ProvenanceDocumentEvent {
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
		ar & HAATHI_SERIALIZATION_NVP("n", name);
		ar & HAATHI_SERIALIZATION_NVP("dn", displayName);
		ar & HAATHI_SERIALIZATION_NVP("v", this->version);
		ar & HAATHI_SERIALIZATION_NVP("r", repository);
		ar & HAATHI_SERIALIZATION_NVP("ok", registered);
	}

	private:
		ExtensionDocumentProvenance();
	protected:	
		std::string name;
		std::string displayName;
		std::string version;
		std::string repository;
		std::string registered;

	public:
		ExtensionDocumentProvenance(std::string name, std::string displayName, std::string version,
				std::string repository, std::string registered);
		//virtual ~ExtensionDocumentProvenance();
		ExtensionDocumentProvenance& operator=(const ExtensionDocumentProvenance& other);
		ExtensionDocumentProvenance(const ExtensionDocumentProvenance& other);

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
