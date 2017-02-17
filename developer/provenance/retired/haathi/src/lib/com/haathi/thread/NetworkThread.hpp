/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance/haathi/src/lib/com/haathi/dialog/Base2Dialog.hpp#28 $

/*
 *  Created by Aaron Gibson on 3/17/12.
 *  Do we even use this? I don't think so...
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__THREAD__NETWORK_THREAD_HPP
#	define COM__HAATHI__THREAD__NETWORK_THREAD_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/thread/SynchronizedQueue.hpp"

#include "sdkBeg.hpp"
#include <osl/thread.hxx>
#include <osl/mutex.hxx>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace thread {
/******************************************************************************
Class
******************************************************************************/
class NetworkThread: public osl::Thread {
	private:
		com::haathi::network::NetworkInfo _cloud_info;
		SynchronizedQueue<std::string>* _input_queue;

		// Not implemented!
		NetworkThread(const NetworkThread& other);
		NetworkThread& operator=(const NetworkThread& other);

	public:
		NetworkThread(com::haathi::network::NetworkInfo cloud_info,
				SynchronizedQueue<std::string>* input_queue);
		virtual ~NetworkThread();

		virtual void SAL_CALL run();
};
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

