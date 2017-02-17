/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/Timer.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/Timer.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
Timer textHelperTimer;
Timer differencerTimer;
Timer encryptorTimer;

Timer::Timer(): elapsedTime(0), stopped(true), count(0) {
}

double Timer::timeDiff(TimeValue& startTime, TimeValue& stopTime) {
	sal_Int32 sDiff = stopTime.Seconds - startTime.Seconds;
	sal_Int32 nsDiff = stopTime.Nanosec - startTime.Nanosec;

	return (double) sDiff + (double) nsDiff / 1000000000;
}

void Timer::start() {
	if (!stopped)
		throw std::exception("Timer already started!");
	stopped = false;
	osl_getSystemTime(&startTime);
}

void Timer::stop() {
	if (stopped)
		throw std::exception("Timer already stopped!");
	stopped = true;

	TimeValue stopTime;

	osl_getSystemTime(&stopTime);
	elapsedTime += timeDiff(startTime, stopTime);
	count++;
}

double Timer::getElapsedTime() {
	return elapsedTime;
}

int Timer::getCount() {
	return count;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
