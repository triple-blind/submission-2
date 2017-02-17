/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/traitorTracer/com/haathi/utils/Timer.cpp#2 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/Timer.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <stdexcept>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::utils;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Timer
******************************************************************************/
Timer::Timer(): elapsedTime(0), stopped(true), count(0) {
}

double Timer::timeDiff(SYSTEMTIME& startTime, SYSTEMTIME& stopTime) {
	// Do not run this across midnight!
	int hDiff = stopTime.wHour - startTime.wHour;
	int mDiff = stopTime.wMinute - startTime.wMinute;
	int sDiff = stopTime.wSecond - startTime.wSecond;
	int msDiff = stopTime.wMilliseconds - startTime.wMilliseconds;

	int diff = ((hDiff * 60) + mDiff) * 60 + sDiff;
	return (double) diff + (double) msDiff / 1000;
}

void Timer::start() {
	if (!stopped)
		throw std::runtime_error("Timer already started!");
	stopped = false;
	GetLocalTime(&startTime);
}

void Timer::stop() {
	if (stopped)
		throw std::runtime_error("Timer already stopped!");
	stopped = true;

	SYSTEMTIME stopTime;

	GetLocalTime(&stopTime);
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
