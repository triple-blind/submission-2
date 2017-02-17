/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _OSL_THREAD_H_
#define _OSL_THREAD_H_

#include <osl/time.h>

#ifndef _RTL_TEXTENC_H_
#	include <rtl/textenc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
	Opaque data type for threads. As with all other osl-handles
	you can initialize and/or test it to/for 0.
*/
typedef void* oslThread;

/** the function-ptr. representing the threads worker-function.
*/
typedef void (SAL_CALL *oslWorkerFunction)(void*);

/** levels of thread-priority
	Note that oslThreadPriorityUnknown might be returned
	by getPriorityOfThread() (e.g. when it is terminated),
	but mustn't be used with setPriority()!
*/
typedef enum
{
	osl_Thread_PriorityHighest,
	osl_Thread_PriorityAboveNormal,
	osl_Thread_PriorityNormal,
	osl_Thread_PriorityBelowNormal,
	osl_Thread_PriorityLowest,
	osl_Thread_PriorityUnknown,			/* don't use to set */
	osl_Thread_Priority_FORCE_EQUAL_SIZE = SAL_MAX_ENUM
} oslThreadPriority;


typedef sal_uInt32 oslThreadIdentifier;

typedef sal_uInt32 oslThreadKey;

/** Create the thread, using the function-ptr pWorker as
	its main (worker) function. This functions receives in
	its void* parameter the value supplied by pThreadData.
	Once the OS-structures are initialized,the thread starts
	running.
	@return 0 if creation failed, otherwise a handle to the thread
*/
oslThread SAL_CALL osl_createThread(oslWorkerFunction pWorker, void* pThreadData);

/** Create the thread, using the function-ptr pWorker as
	its main (worker) function. This functions receives in
	its void* parameter the value supplied by pThreadData.
	The thread will be created, but it won't start running.
	To wake-up the thread, use resume().
	@return 0 if creation failed, otherwise a handle to the thread
*/
oslThread SAL_CALL osl_createSuspendedThread(oslWorkerFunction pWorker, void* pThreadData);

/** Get the identifier for the specified thread or if parameter
    Thread is NULL of the current active thread.
	@return identifier of the thread
*/
oslThreadIdentifier SAL_CALL osl_getThreadIdentifier(oslThread Thread);

/** Release the thread handle.
	If Thread is NULL, the function won't do anything.
	Note that we do not interfere with the actual running of
	the thread, we just free up the memory needed by the handle.
*/
void SAL_CALL osl_destroyThread(oslThread Thread);

/** Wake-up a thread that was suspended with suspend() or
	createSuspended(). The oslThread must be valid!
*/
void SAL_CALL osl_resumeThread(oslThread Thread);

/** Suspend the execution of the thread. If you want the thread
	to continue, call resume(). The oslThread must be valid!
*/
void SAL_CALL osl_suspendThread(oslThread Thread);

/** Changes the threads priority.
	The oslThread must be valid!
*/
void SAL_CALL osl_setThreadPriority(oslThread Thread, oslThreadPriority Priority);

/** Retrieves the threads priority.
	Returns oslThreadPriorityUnknown for invalid Thread-argument or
	terminated thread. (I.e.: The oslThread might be invalid.)
*/
oslThreadPriority SAL_CALL osl_getThreadPriority(const oslThread Thread);

/** Returns True if the thread was created and has not terminated yet.
	Note that according to this definition a "running" thread might be
	suspended! Also returns False is Thread is NULL.
*/
sal_Bool SAL_CALL osl_isThreadRunning(const oslThread Thread);

/** Blocks the calling thread until Thread has terminated.
	Returns immediately if Thread is NULL.
*/
void SAL_CALL osl_joinWithThread(oslThread Thread);

/** Blocks the calling thread at least for the given number
    of time.
*/
void SAL_CALL osl_waitThread(const TimeValue* pDelay);

/** The requested thread will get terminate the next time
	scheduleThread() is called.
*/
void SAL_CALL osl_terminateThread(oslThread Thread);

/** Offers the rest of the threads time-slice to the OS.
	scheduleThread() should be called in the working loop
	of the thread, so any other thread could also get the
	processor. Returns False if the thread should terminate, so
	the thread could free any allocated resources.
*/
sal_Bool SAL_CALL osl_scheduleThread(oslThread Thread);

/** Offers the rest of the threads time-slice to the OS.
	Under POSIX you _need_ to yield(), otherwise, since the
	threads are not preempted during execution, NO other thread
	(even with higher priority) gets the processor. Control is
	only given to another thread if the current thread blocks
	or uses yield().
*/
void SAL_CALL osl_yieldThread(void);

/** Attempts to set the name of the current thread.

    The name of a thread is usually evaluated for debugging purposes.  Not all
    platforms support this.  On Linux, a set thread name can be observed with
    "ps -L".  On Windows with the Microsoft compiler, a thread name set while a
    debugger is attached can be observed within the debugger.

    @param name  the name of the thread; must not be null; on Linux, only the
    first 16 characters are used
*/
void SAL_CALL osl_setThreadName(char const * name);

/* Callback when data stored in a thread key is no longer needed */

typedef void (SAL_CALL *oslThreadKeyCallbackFunction)(void *);

/** Create a key to an associated thread local storage pointer. */
oslThreadKey SAL_CALL osl_createThreadKey(oslThreadKeyCallbackFunction pCallback);

/** Destroy a key to an associated thread local storage pointer. */
void SAL_CALL osl_destroyThreadKey(oslThreadKey Key);

/** Get to key associated thread specific data. */
void* SAL_CALL osl_getThreadKeyData(oslThreadKey Key);

/** Set to key associated thread specific data. */
sal_Bool SAL_CALL osl_setThreadKeyData(oslThreadKey Key, void *pData);

/** Get the current thread local text encoding. */
rtl_TextEncoding SAL_CALL osl_getThreadTextEncoding(void);

/** Set the thread local text encoding. 
	@return the old text encoding.
*/
rtl_TextEncoding SAL_CALL osl_setThreadTextEncoding(rtl_TextEncoding Encoding);

#ifdef __cplusplus
}
#endif

#endif	/* _OSL_THREAD_H_ */

