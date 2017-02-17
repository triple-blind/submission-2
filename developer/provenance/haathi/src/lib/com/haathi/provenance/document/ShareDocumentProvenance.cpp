/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/ShareDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/ShareDocumentProvenance.hpp"
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
ShareDocumentProvenance
******************************************************************************/
ShareDocumentProvenance::ShareDocumentProvenance():
		userEmails(std::vector<std::string>()) {
}

ShareDocumentProvenance::ShareDocumentProvenance(std::vector<std::string> userEmails):
		userEmails(userEmails) {
}

ShareDocumentProvenance& ShareDocumentProvenance::operator=(
		const ShareDocumentProvenance& other) {
	ProvenanceDocumentEvent::operator=(other);
	userEmails = other.userEmails;
	return *this;
}

ShareDocumentProvenance::ShareDocumentProvenance(const ShareDocumentProvenance& other):
		ProvenanceDocumentEvent(other), userEmails(other.userEmails) {
}

std::string ShareDocumentProvenance::getEventType() const {
	return std::string("share document");
}

OUString ShareDocumentProvenance::printToTable() const {
	rtl::OUStringBuffer result;

	result.appendAscii("Shared document with ");
	appendOthers(result, userEmails);
	return result.makeStringAndClear();
}

OUString ShareDocumentProvenance::getEventDescription() const {
	rtl::OUStringBuffer result;

	result.append(printToTable());
	appendUserEmails(result, userEmails);
	return result.makeStringAndClear();
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::ShareDocumentProvenance);
/*****************************************************************************/
