/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/src/lib/com/haathi/provenance/document/ProcessTableDocumentEvent.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/document/ProcessTableDocumentEvent.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "Config.hpp"

#include "com/haathi/utils/StringWrapper.hpp"

#if OSYS == OSYS_WIN
//#	include <stdio.h>
#	include <Windows.h> 
#	include <Winnt.h> 
#	include <tlhelp32.h>
#endif
/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
ProcessTableDocumentEvent
******************************************************************************/
std::vector<std::string> getProcessTable() {
	std::vector<std::string> processTable;
#if OSYS == OSYS_WIN
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return processTable;
	pe32.dwSize = sizeof(PROCESSENTRY32);
 
	//int i= 0;
	Process32First(hProcessSnap, &pe32);
	processTable.push_back(pe32.szExeFile);
	//printf ("%d  %s\n",++i,  pe32.szExeFile);
	//fprintf(f,"%s \n",pe32.szExeFile);
	while (Process32Next(hProcessSnap, &pe32)) {
		 processTable.push_back(pe32.szExeFile);
		//printf ("%d  %s\n",++i,  pe32.szExeFile);
		//fprintf(f,"%s \n",pe32.szExeFile);
	} 
	CloseHandle(hProcessSnap);
#endif
	return processTable;
}
			
ProcessTableDocumentEvent::ProcessTableDocumentEvent(): processName("None") {
}

ProcessTableDocumentEvent::ProcessTableDocumentEvent(std::string processName): processName(processName) {
}

std::string ProcessTableDocumentEvent::getEventType() const {
    return std::string("processTable");
}

OUString ProcessTableDocumentEvent::printToTable() const {
    return OUSTRING("processTable");
}

OUString ProcessTableDocumentEvent::getEventDescription() const {
    rtl::OUStringBuffer result;

    result.append(printToTable());
    result.appendAscii(": ");
    result.append(com::haathi::utils::convertToOUString(processName));
    return result.makeStringAndClear();
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// Serialization Macros
BOOST_CLASS_EXPORT(com::haathi::provenance::ProcessTableDocumentEvent);
/*****************************************************************************/ 
