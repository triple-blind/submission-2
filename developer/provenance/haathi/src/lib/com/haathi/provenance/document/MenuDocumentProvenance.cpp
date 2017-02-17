/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/MenuDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/MenuDocumentProvenance.hpp"
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
MenuDocumentProvenance
******************************************************************************/
MenuDocumentProvenance::MenuDocumentProvenance(): cmd() {
}

MenuDocumentProvenance::MenuDocumentProvenance(std::string cmd): cmd(cmd) {
}

MenuDocumentProvenance& MenuDocumentProvenance::operator=(const MenuDocumentProvenance& other) {
	ProvenanceDocumentEvent::operator=(other);
	this->cmd = other.cmd;
	return *this;
}

MenuDocumentProvenance::MenuDocumentProvenance(const MenuDocumentProvenance& other):
		ProvenanceDocumentEvent(other), cmd(other.cmd) {
}

std::string MenuDocumentProvenance::getEventType() const {
	return std::string("menu document");
}

OUString MenuDocumentProvenance::printToTable() const {
	return OUSTRING("Selected menu item \"") + convertToOUString(cmd) + OUSTRING("\"");
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::MenuDocumentProvenance);
/*****************************************************************************/
