/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/ExtensionDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/Document/ExtensionDocumentProvenance.hpp"
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
ExtensionDocumentProvenance
******************************************************************************/
ExtensionDocumentProvenance::ExtensionDocumentProvenance(): name(), displayName(), version(), repository(), registered() {
}

ExtensionDocumentProvenance::ExtensionDocumentProvenance(std::string name, std::string displayName,
		std::string version, std::string repository, std::string registered):
		name(name), displayName(displayName), version(version), repository(repository), registered(registered) {
}

ExtensionDocumentProvenance& ExtensionDocumentProvenance::operator=(const ExtensionDocumentProvenance& other) {
	ProvenanceDocumentEvent::operator=(other);
	this->name = other.name;
	this->displayName = other.displayName;
	this->version = other.version;
	this->repository = other.repository;
	this->registered = other.registered;
	return *this;
}

ExtensionDocumentProvenance::ExtensionDocumentProvenance(const ExtensionDocumentProvenance& other):
		ProvenanceDocumentEvent(other), name(other.name), displayName(other.displayName), version(other.version),
		repository(other.repository), registered(other.registered) {
}

std::string ExtensionDocumentProvenance::getEventType() const {
	return std::string("extension document");
}

OUString ExtensionDocumentProvenance::printToTable() const {
	return OUSTRING("Extended document with \"") + convertToOUString(displayName) +OUSTRING("\"");
}

OUString ExtensionDocumentProvenance::getEventDescription() const {
	return printToTable() + convertToOUString(std::string(" (") + name + std::string("), v. ") + version +
			", " + repository + ", " + registered);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::ExtensionDocumentProvenance);
/*****************************************************************************/
