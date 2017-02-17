/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/WatermarkDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/WatermarkDocumentProvenance.hpp"
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
WatermarkDocumentProvenance
******************************************************************************/
WatermarkDocumentProvenance::WatermarkDocumentProvenance(): abbrev() {
}

WatermarkDocumentProvenance::WatermarkDocumentProvenance(std::string abbrev): abbrev(abbrev) {
}

WatermarkDocumentProvenance& WatermarkDocumentProvenance::operator=(const WatermarkDocumentProvenance& other) {
	ProvenanceDocumentEvent::operator=(other);
	this->abbrev = other.abbrev;
	return *this;
}

WatermarkDocumentProvenance::WatermarkDocumentProvenance(const WatermarkDocumentProvenance& other):
		ProvenanceDocumentEvent(other), abbrev(other.abbrev) {
}

std::string WatermarkDocumentProvenance::getEventType() const {
	return std::string("watermark document");
}

OUString WatermarkDocumentProvenance::printToTable() const {
	return OUSTRING("Watermarked document with \"") + convertToOUString(abbrev) +OUSTRING("\" scheme");
}

OUString WatermarkDocumentProvenance::getEventDescription() const {
	return printToTable();
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::WatermarkDocumentProvenance);
/*****************************************************************************/
