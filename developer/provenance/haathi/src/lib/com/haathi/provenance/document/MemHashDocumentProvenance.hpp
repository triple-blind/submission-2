/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/src/lib/com/haathi/provenance/document/MemHashDocumentProvenance.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__DOCUMENT__MEM_HASH_DOCUMENT_PROVENANCE_H
#	define COM__HAATHI__PROVENANCE__DOCUMENT__MEM_HASH_DOCUMENT_PROVENANCE_H
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
struct ImageSectionInfo {
	char *sectionAddress;
	int sectionSize;

	ImageSectionInfo(char* sectionAddress, int sectionSize);
};

ImageSectionInfo getImageSectionInfo();

class MemHashDocumentProvenance: public ProvenanceDocumentEvent {
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
		ar & HAATHI_SERIALIZATION_NVP("mh", memHash);
	}

	private:
		MemHashDocumentProvenance();
	protected:	
		std::string memHash;
	public:
		MemHashDocumentProvenance(std::string& memHash);
		//virtual ~ExtensionDocumentProvenance();
		MemHashDocumentProvenance& operator=(const MemHashDocumentProvenance& other);
		MemHashDocumentProvenance(const MemHashDocumentProvenance& other);

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
