/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/Timer.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__TIMER_HPP
#	define COM__HAATHI__UTILS__TIMER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <osl/time.h>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
class Timer {
	protected:
		TimeValue startTime;
		double elapsedTime;
		bool stopped;
		int count;

		double timeDiff(TimeValue& startTime, TimeValue& stopTime);
	public:
		Timer();
		void start();
		void stop();
		double getElapsedTime();
		int getCount();
};

class TimeKeeper {
	protected:
		Timer &timer;

	public:
		TimeKeeper(Timer& timer): timer(timer) {
			timer.start();
		}

		~TimeKeeper() {
			timer.stop();
		}
};

extern Timer textHelperTimer;
extern Timer differencerTimer;
extern Timer encryptorTimer;
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
