/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/PrintDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/PrintDocumentProvenance.hpp"
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
PrintDocumentProvenance
******************************************************************************/
PrintDocumentProvenance::PrintDocumentProvenance(): details("None") {
}

PrintDocumentProvenance::PrintDocumentProvenance(std::string details): details(details) {
}

std::string PrintDocumentProvenance::getEventType() const {
	return std::string("print document");
}

OUString PrintDocumentProvenance::printToTable() const {
	return OUSTRING("Printed document");
}

OUString PrintDocumentProvenance::getEventDescription() const {
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
BOOST_CLASS_EXPORT(com::haathi::provenance::PrintDocumentProvenance);
/*****************************************************************************/
