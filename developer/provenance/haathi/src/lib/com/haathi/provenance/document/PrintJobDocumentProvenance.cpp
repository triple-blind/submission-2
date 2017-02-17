/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/PrintJobDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/PrintJobDocumentProvenance.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/provenance/BasicProvenanceEvents.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::convertToOUString;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
PrintJobDocumentProvenance
******************************************************************************/
PrintJobDocumentProvenance::PrintJobDocumentProvenance(): state() {
}

PrintJobDocumentProvenance::PrintJobDocumentProvenance(std::string state): state(state) {
}

PrintJobDocumentProvenance& PrintJobDocumentProvenance::operator=(const PrintJobDocumentProvenance& other) {
	ProvenanceDocumentEvent::operator=(other);
	this->state = other.state;
	return *this;
}

PrintJobDocumentProvenance::PrintJobDocumentProvenance(const PrintJobDocumentProvenance& other):
		ProvenanceDocumentEvent(other), state(other.state) {
}

std::string PrintJobDocumentProvenance::getEventType() const {
	return std::string("print document job");
}

OUString PrintJobDocumentProvenance::printToTable() const {
	return OUSTRING("Printed document job: ") + convertToOUString(state);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::PrintJobDocumentProvenance);
/*****************************************************************************/
