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




#ifndef _OSL_CONDITION_H_
#define _OSL_CONDITION_H_

#include <osl/time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* oslCondition;

typedef enum {
    osl_cond_result_ok,		/* successful completion */
    osl_cond_result_error,  /* error occured, check osl_getLastSocketError() for details */
	osl_cond_result_timeout, /* blocking operation timed out */
	osl_cond_result_FORCE_EQUAL_SIZE = SAL_MAX_ENUM
} oslConditionResult;

/** Creates a condition.
	The condition is in the reset-state.
	@returns 0 if condition could not be created.
*/
oslCondition SAL_CALL osl_createCondition(void);

/** Free the memory used by the condition.
	@param Condition the condition handle.
*/
void SAL_CALL osl_destroyCondition(oslCondition Condition);

/** Sets condition to True => wait() will not block, check() returns True.
	NOTE: ALL threads waiting on this condition are unblocked!
	@param Condition handle to a created condition.
	@return False if system-call failed.
*/
sal_Bool SAL_CALL osl_setCondition(oslCondition Condition);

/** Sets condition to False => wait() will block, check() returns False
	@param Condition handle to a created condition.
	@return False if system-call failed.
*/
sal_Bool SAL_CALL osl_resetCondition(oslCondition Condition);

/** Blocks if condition is not set<BR>
	If condition has been destroyed prematurely, wait() will
	return with False.
	@param Condition handle to a created condition.
	@param pTimeout Tiemout value or NULL for infinite waiting
	@return False if system-call failed.
*/
oslConditionResult SAL_CALL osl_waitCondition(oslCondition Condition, const TimeValue* pTimeout);

/** Queries the state of the condition without blocking.
	@param Condition handle to a created condition.
	@return True: condition is set. <BR>
	False: condition is not set. <BR>
*/
sal_Bool SAL_CALL osl_checkCondition(oslCondition Condition);

#ifdef __cplusplus
}
#endif

#endif /* _OSL_CONDITION_H_ */

