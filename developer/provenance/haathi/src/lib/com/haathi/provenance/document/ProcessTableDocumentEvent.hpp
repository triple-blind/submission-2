/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/src/lib/com/haathi/provenance/document/ProcessTableDocumentEvent.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__DOCUMENT_PROCESS_TABLE_DOCUMENT_H
#	define COM__HAATHI__PROVENANCE__DOCUMENT_PROCESS_TABLE_DOCUMENT_H
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/provenance/ProvenanceEvent.hpp"

#include <vector>
#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
ProcessTableDocumentEvent
******************************************************************************/
std::vector<std::string> getProcessTable();

class ProcessTableDocumentEvent: public ProvenanceDocumentEvent {
    friend class boost::serialization::access;
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & HAATHI_SERIALIZATION_BASE_OBJECT_NVP("pc", ProvenanceDocumentEvent);
        ar & BOOST_SERIALIZATION_NVP(processName);
    }

    protected:
        std::string processName;
        ProcessTableDocumentEvent();
    
    public:
        ProcessTableDocumentEvent(std::string processName);
        //virtual ~ProcessTableDocumentEvent();
        //ProcessTableDocumentEvent& operator=(const ProcessTableDocumentEvent& other);
        //ProcessTableDocumentEvent(const ProcessTableDocumentEvent& other);

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
