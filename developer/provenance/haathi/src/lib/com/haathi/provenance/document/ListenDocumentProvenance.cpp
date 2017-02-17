/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/ListenDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/ListenDocumentProvenance.hpp"
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
ListenDocumentProvenance
******************************************************************************/
ListenDocumentProvenance::ListenDocumentProvenance(): channel(), message() {
}

ListenDocumentProvenance::ListenDocumentProvenance(std::string channel, std::string message):
		channel(channel), message(message) {
}

ListenDocumentProvenance& ListenDocumentProvenance::operator=(const ListenDocumentProvenance& other) {
	ProvenanceDocumentEvent::operator=(other);
	this->channel = other.channel;
	this->message = other.message;
	return *this;
}

ListenDocumentProvenance::ListenDocumentProvenance(const ListenDocumentProvenance& other):
		ProvenanceDocumentEvent(other), channel(other.channel), message(other.message) {
}

std::string ListenDocumentProvenance::getEventType() const {
	return std::string("listen document");
}

OUString ListenDocumentProvenance::printToTable() const {
	return OUSTRING("Listened to \"") + convertToOUString(message) + OUSTRING("\"");
}

OUString ListenDocumentProvenance::getEventDescription() const {
	rtl::OUStringBuffer result;

	result.append(printToTable());
	result.appendAscii(" on \"");
	result.append(convertToOUString(channel));
	result.appendAscii("\"");
	return result.makeStringAndClear();
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::ListenDocumentProvenance);
/*****************************************************************************/
