/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/document/ExportDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/ExportDocumentProvenance.hpp"
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
ExportDocumentProvenance
******************************************************************************/
ExportDocumentProvenance::ExportDocumentProvenance(): details("None") {
}

ExportDocumentProvenance::ExportDocumentProvenance(std::string details): details(details) {
}

std::string ExportDocumentProvenance::getEventType() const {
	return std::string("export document");
}

OUString ExportDocumentProvenance::printToTable() const {
	return OUSTRING("Exported document");
}

OUString ExportDocumentProvenance::getEventDescription() const {
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
BOOST_CLASS_EXPORT(com::haathi::provenance::ExportDocumentProvenance);
/*****************************************************************************/
