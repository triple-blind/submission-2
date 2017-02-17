/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/thread/SynchronizedQueue.hpp#1 $

/*
 *  Created by Aaron Gibson on 1/11/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__THREAD__SYNCHRONIZED_QUEUE_HPP
#	define COM__HAATHI__THREAD__SYNCHRONIZED_QUEUE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <osl/mutex.hxx>
#include <osl/conditn.hxx>
#include "sdkEnd.hpp"

#include <queue>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace thread {
/******************************************************************************
SynchronizedQueue
******************************************************************************/
/*!
 @class SynchronizedQueue
 
 @author Aaron Gibson
 
 This templated class implements a synchronized "queue" data structure that should be used
 in a "Producer-Consumer" Design pattern. This queue has condition variables to handle when
 the queue is empty.
 */
template<class T> class SynchronizedQueue {
	// Need to do something with the mutex in order to copy this

	protected:
		// Synchronization variables
		/*!
		 This mutex must be acquired to handle data in this queue.
		 It is declared mutable because the mutex must always be acquired, even in a "const" context.
		 */
		mutable osl::Mutex mutex;

		std::queue<T> queue;

		/*!
		 This is the condition variable to handle when the queue is empty.
		 Any thread trying to extract data from the queue when it is empty will block until something
		 is added back into the queue.
		 */
		osl::Condition emptyCondition;

	public:
		SynchronizedQueue()  {
			emptyCondition.reset();
		}

		/*!
		 Destructor
		 
		 The destructor for this function must destruct the mutex and condition variable. It must also handle
		 any threads that are currently blocking on this object.
		 */
		virtual ~SynchronizedQueue() {
			// Handle any blocking threads?
		}

		/*!
		 This function will add data to the queue (as well as notify any blocking threads that the queue
		 is no longer empty). Naturally, this function will automatically acquire the mutex for the queue.
		 
		 @param data -- the element to push to the back of the queue.
		 @result -- true if the element was added; false if there was an error.
		 */
		bool enqueue(T data);

		/*!
		 This function will remove the first element fo the queue. This function will automatically acquire
		 the mutex for this queue and set the condition as "empty", if applicable.
		 
		 @result -- the first element of the queue.
		 */
		T dequeue(bool pop = true);

		void pop();

		/*!
		 This function returns whether the queue is empty or not. This function is NOT synchronized, 
		 meaning that the mutex will not be obtained for this function. It should be obtained prior.
		 
		 @result -- true if the queue is empty; false otherwise.
		 */
		bool empty() const {
			osl::MutexGuard guard(mutex);

			return queue.empty();
		}

		/*!
		 This function will cause any threads currently blocking on this queue (waiting for the queue to fill)
		 to wake-up. 
		 */
		void wakeBlockingThreads() {
			// Just set the emptyCondition variable. This will wake up any blocking threads.
			emptyCondition.set();
		}

		size_t size() {
			return queue.size();
		}
};

template<class T>
bool SynchronizedQueue<T>::enqueue(T data) {
	osl::MutexGuard guard(mutex);

	queue.push(data);
	emptyCondition.set();
	return true;
}

template<class T>
T SynchronizedQueue<T>::dequeue(bool pop) {
	osl::ResettableMutexGuard guard(mutex);

	while (queue.empty()) {
		guard.clear();
		emptyCondition.wait();
		guard.reset();
	}

	// We are still in the critical region. Add the object to the queue.
	// This might be a problem in some cases. Worse-case scenario, use pointers instead.
	T result(queue.front());

	if (pop)
		queue.pop();
	if (queue.empty())
		emptyCondition.reset();
	return result;
}

template<class T>
void SynchronizedQueue<T>::pop() {
	osl::ResettableMutexGuard guard(mutex);

	while (queue.empty()) {
		guard.clear();
		emptyCondition.wait();
		guard.reset();
	}
	queue.pop();
	if (queue.empty())
		emptyCondition.reset();
}
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
