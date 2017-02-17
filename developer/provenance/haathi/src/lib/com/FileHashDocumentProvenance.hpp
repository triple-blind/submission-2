/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/FileHashDocumentProvenance.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__DOCUMENT__FILE_HASH_DOCUMENT_PROVENANCE_H
#	define COM__HAATHI__PROVENANCE__DOCUMENT__FILE_HASH_DOCUMENT_PROVENANCE_H
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/provenance/ProvenanceEvent.hpp"
#include <vector>
#include <string>
//std::vector<std::string> GetFileHashTable ();
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
FileHashDocumentProvenance
******************************************************************************/
class FileHashDocumentProvenance: public ProvenanceDocumentEvent {
    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
        ar & BOOST_SERIALIZATION_NVP(fileHash);
    }

    protected:
        std::string fileHash;
        FileHashDocumentProvenance();
    
    public:
        FileHashDocumentProvenance(std::string fileHash);

        virtual std::string getEventType() const;
        virtual rtl::OUString printToTable() const;
        virtual rtl::OUString getEventDescription() const;
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
