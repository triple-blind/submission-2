/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/src/lib/com/haathi/provenance/document/FileHashDocumentProvenance.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/FileHashDocumentProvenance.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/provenance/BasicProvenanceEvents.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include <tchar.h>
#include <stdio.h>
#include <psapi.h>

#include <process.h>

#include <string.h>
#include <vector>


#include <Windows.h>
#include <winnt.h>



#include "Config.hpp"

//#pragma comment(lib, “psapi.lib”)
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

FileHashDocumentProvenance::FileHashDocumentProvenance(): fileHash() {
}

FileHashDocumentProvenance::FileHashDocumentProvenance(std::string fileHash): fileHash(fileHash) {
}

std::string FileHashDocumentProvenance::getEventType() const {
	return std::string("File hash document");
}

OUString FileHashDocumentProvenance::printToTable() const {
	return OUSTRING("Calculated File hash of module");
}

OUString FileHashDocumentProvenance::getEventDescription() const {
	return printToTable() + convertToOUString(std::string(" (") + fileHash + std::string(")"));
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::FileHashDocumentProvenance);
/*****************************************************************************/

std::vector<std::string> GetFileHashTable (DWORD processID)  {
 
	std::vector<std::string> fileModules;
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

    // Get a handle to the process.	
    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                            PROCESS_VM_READ,
                            FALSE, processID );
    if (NULL == hProcess)
        return fileModules;

   // Get a list of all the modules in this process.
    if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.

            if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
                                      sizeof(szModName) / sizeof(TCHAR)))
            {
				fileModules.push_back(szModName);
            }
        }
    }
    
    // Release the handle to the process.
    CloseHandle( hProcess );

    return fileModules;
}