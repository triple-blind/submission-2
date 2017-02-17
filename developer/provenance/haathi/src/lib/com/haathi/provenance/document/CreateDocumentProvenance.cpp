/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/CreateDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/CreateDocumentProvenance.hpp"
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
CreateDocumentProvenance
******************************************************************************/
CreateDocumentProvenance::CreateDocumentProvenance():
		userEmails(std::vector<std::string>()) {
}

CreateDocumentProvenance::CreateDocumentProvenance(std::vector<std::string> userEmails):
		userEmails(userEmails) {
}

CreateDocumentProvenance& CreateDocumentProvenance::operator=(
		const CreateDocumentProvenance& other) {
	ProvenanceDocumentEvent::operator=(other);
	userEmails = other.userEmails;
	return *this;
}

CreateDocumentProvenance::CreateDocumentProvenance(const CreateDocumentProvenance& other):
		ProvenanceDocumentEvent(other), userEmails(other.userEmails) {
}

std::string CreateDocumentProvenance::getEventType() const {
	return std::string("create document");
}

OUString CreateDocumentProvenance::printToTable() const {
	rtl::OUStringBuffer result;

	result.appendAscii("Created document ");
	if (userEmails.size() == 0)
		result.appendAscii("but did not share");
	else {
		result.appendAscii("and shared with ");
		appendOthers(result, userEmails);
	}
	return result.makeStringAndClear();
}

OUString CreateDocumentProvenance::getEventDescription() const {
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
BOOST_CLASS_EXPORT(com::haathi::provenance::CreateDocumentProvenance);
/*****************************************************************************/
