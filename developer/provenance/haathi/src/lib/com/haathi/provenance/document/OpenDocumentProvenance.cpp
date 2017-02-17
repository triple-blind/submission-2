/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/OpenDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/OpenDocumentProvenance.hpp"
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
OpenDocumentProvenance
******************************************************************************/
OpenDocumentProvenance::OpenDocumentProvenance() {
}

std::string OpenDocumentProvenance::getEventType() const {
	return std::string("open document");
}

OUString OpenDocumentProvenance::printToTable() const {
	return OUSTRING("Opened document");
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::OpenDocumentProvenance);
/*****************************************************************************/
