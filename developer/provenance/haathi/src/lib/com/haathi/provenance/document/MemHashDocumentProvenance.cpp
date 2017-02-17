/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/src/lib/com/haathi/provenance/document/MemHashDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/Document/MemHashDocumentProvenance.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/provenance/BasicProvenanceEvents.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "Config.hpp"

#if OSYS == OSYS_WIN
#	include <Windows.h>
#	include <Dbghelp.h>
#endif
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
ImageSectionInfo::ImageSectionInfo(char* sectionAddress, int sectionSize): 
		sectionAddress(sectionAddress), sectionSize(sectionSize) {
}

#if OSYS == OSYS_WIN

HMODULE GetCurrentModule() {
	HMODULE hModule = NULL;

	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCSTR) GetCurrentModule, &hModule);
	return hModule;
}

ImageSectionInfo getImageSectionInfo() {
	HMODULE hModule = GetCurrentModule();
	char* dllImageBase = (char*) hModule;
	IMAGE_NT_HEADERS *pNtHdr = ImageNtHeader(hModule);
	IMAGE_SECTION_HEADER *pSectionHdr = (IMAGE_SECTION_HEADER*) (pNtHdr + 1);

	for (int i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++) {
		char *name = (char*) pSectionHdr->Name;
		if (memcmp(name, ".text", 5) == 0) {
			char* sectionAddress = dllImageBase + pSectionHdr->VirtualAddress;
			int sectionSize = pSectionHdr->Misc.VirtualSize;

			return ImageSectionInfo(sectionAddress, sectionSize);
		}
		pSectionHdr++;
	}
	return ImageSectionInfo(NULL, 0);
}

#endif

MemHashDocumentProvenance::MemHashDocumentProvenance(): memHash() {
}

MemHashDocumentProvenance::MemHashDocumentProvenance(std::string& memHash): memHash(memHash) {
}

MemHashDocumentProvenance& MemHashDocumentProvenance::operator=(const MemHashDocumentProvenance& other) {
	ProvenanceDocumentEvent::operator=(other);
	this->memHash = other.memHash;
	return *this;
}

MemHashDocumentProvenance::MemHashDocumentProvenance(const MemHashDocumentProvenance& other):
		ProvenanceDocumentEvent(other), memHash(other.memHash) {
}

std::string MemHashDocumentProvenance::getEventType() const {
	return std::string("memory hash document");
}

OUString MemHashDocumentProvenance::printToTable() const {
	return OUSTRING("Calculated memory hash of module");
}

OUString MemHashDocumentProvenance::getEventDescription() const {
	return printToTable() + convertToOUString(std::string(" (") + memHash + std::string(")"));
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::MemHashDocumentProvenance);
/*****************************************************************************/
