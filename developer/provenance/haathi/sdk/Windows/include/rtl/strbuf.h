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



#ifndef _RTL_STRBUF_H_
#define _RTL_STRBUF_H_

#include <rtl/string.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @HTML
	Allocates a new <code>String</code> that contains characters from 
	the character array argument.

    The <code>count</code> argument specifies 
	the length of the array. The initial capacity of the string buffer is 
	<code>16</code> plus the length of the string argument. 

	@param  newStr   out parameter, contains the new string. The reference count is 1.
	@param  value    the initial value of the string.
	@param  count    the length of value.
 */
void SAL_CALL rtl_stringbuffer_newFromStr_WithLength( rtl_String ** newStr,
													  const sal_Char * value, 
													  sal_Int32 count);

/**
	Allocates a new <code>String</code> that contains the same sequence of
	characters as the string argument.

    The initial capacity is the larger of:
	<ul>
	<li> The <code>bufferLen</code> argument.
	<li> The <code>length</code> of the string argument.
	</ul> 

	@param  newStr   	out parameter, contains the new string. The reference count is 1.
	@param  capacity   	the initial len of the string buffer.
	@param  oldStr	    the initial value of the string.
	@return the new capacity of the string buffer
 */
sal_Int32 SAL_CALL rtl_stringbuffer_newFromStringBuffer( rtl_String ** newStr,
													     sal_Int32 capacity,
													     rtl_String * olsStr );

/**
	Ensures that the capacity of the buffer is at least equal to the
	specified minimum.

	If the current capacity of this string buffer is less than the 
	argument, then a new internal buffer is allocated with greater 
	capacity. The new capacity is the larger of: 
	<ul>
	<li>The <code>minimumCapacity</code> argument. 
	<li>Twice the old capacity, plus <code>2</code>. 
	</ul>
	If the <code>minimumCapacity</code> argument is nonpositive, this
	method takes no action and simply returns.

	@param   capacity		  in: old capicity, out: new capacity.
	@param   minimumCapacity   the minimum desired capacity.
 */
void SAL_CALL rtl_stringbuffer_ensureCapacity(	/*inout*/rtl_String ** This, 
												/*inout*/sal_Int32* capacity, 
												sal_Int32 minimumCapacity);


/**
	Inserts the string representation of the <code>char</code> array 
	argument into this string buffer. 

	The characters of the array argument are inserted into the 
	contents of this string buffer at the position indicated by 
	<code>offset</code>. The length of this string buffer increases by 
	the length of the argument. 

	@param	capacity 	the capacity of the string buffer
	@param 	offset  	the offset.
	@param 	ch      	a character array.
	@param	len    		the number of characters to append.
	@return	this string buffer.
 */
void SAL_CALL rtl_stringbuffer_insert( /*inout*/rtl_String ** This, 
									   /*inout*/sal_Int32 * capacity,
									   sal_Int32 offset, 
									   const sal_Char * str, 
									   sal_Int32 len);

#ifdef __cplusplus
}
#endif

#endif	/* _RTL_STRBUF_H_ */
