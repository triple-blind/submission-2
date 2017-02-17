/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/menu/DispatchHelper.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/menu/DispatchHelper.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Timer.hpp"

#include "cppJSON.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::Logger;
using com::haathi::utils::Timer;

using com::haathi::utils::differencerTimer;
using com::haathi::utils::encryptorTimer;
using com::haathi::utils::textHelperTimer;

using com::sun::star::util::URL;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace menu {
/******************************************************************************
Class or Content
******************************************************************************/
int eventCount;

void showTimer(Logger& logger, const char* name, Timer& timer) {
	logger
		<< name
		<< ": "
		<< timer.getElapsedTime()
		<< " sec, "
		<< timer.getCount()
		<< " calls"
		<< "\n";
}

void showPlainCounts(Logger& logger) {
	logger 
		<< "mallocCount: "
		<< cppJSON::getMallocCount() 
		<< " freeCount: "
		<< cppJSON::getFreeCount()
		<< " eventCount: "
		<< eventCount
		<< "\n";
	showTimer(logger, "TextHelper", textHelperTimer);
	showTimer(logger, "Differencer", differencerTimer);
	showTimer(logger, "Encryptor", encryptorTimer);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
