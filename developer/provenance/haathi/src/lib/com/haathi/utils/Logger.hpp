/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/Logger.hpp#1 $

/*
 *  Created by Aaron Gibson on 3/27/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__LOGGER_HPP
#	define COM__HAATHI__UTILS__LOGGER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/thread/SynchronizedQueue.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "boostBeg.hpp"
#include <boost/shared_ptr.hpp>
#include "boostEnd.hpp"

#include <string>
#include <sstream>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
class Logger {
	private:
		static Logger* globalLog;

		com::haathi::thread::SynchronizedQueue<std::string> queue;
		Logger();
	public:
		static Logger& getStaticLog();
	
		virtual ~Logger();
		template<typename T> friend Logger& operator<<(Logger& log, const T& data);
		std::string getEntry();
};

// We have to implement the templated functions in the header.
template<typename T> Logger& operator<<(Logger& currentLog, const T& data) {
	std::stringstream stream;
	stream << com::haathi::utils::convertToString(data);

	std::string str = stream.str();
	currentLog.queue.enqueue(str);
	// Immediately after, we reset the has_changed condition. The net effect of this
	// toggling of the condition variable signals all of the other threads that are
	// waiting for a change, but any threads that come after the change effectively
	// wait again.
	return currentLog;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

com::haathi::utils::Logger& getLog(); // Convenience
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
