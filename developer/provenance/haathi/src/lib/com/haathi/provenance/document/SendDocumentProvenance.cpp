/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/SendDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/SendDocumentProvenance.hpp"
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
SendDocumentProvenance
******************************************************************************/
SendDocumentProvenance::SendDocumentProvenance(): details("None") {
}

SendDocumentProvenance::SendDocumentProvenance(std::string details): details(details) {
}

std::string SendDocumentProvenance::getEventType() const {
	return std::string("send document");
}

OUString SendDocumentProvenance::printToTable() const {
	return OUSTRING("Sent document");
}

OUString SendDocumentProvenance::getEventDescription() const {
	rtl::OUStringBuffer result;

	result.append(printToTable());
	result.appendAscii(": ");
	result.append(convertToOUString(details));
	return result.makeStringAndClear();
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::SendDocumentProvenance);
/*****************************************************************************/
