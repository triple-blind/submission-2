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




#ifndef _RTL_MEMORY_H_
#define _RTL_MEMORY_H_

#	include <sal/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void SAL_CALL rtl_zeroMemory(void *Ptr, sal_Size Bytes);
void SAL_CALL rtl_fillMemory(void *Ptr, sal_Size Bytes, sal_uInt8 Fill);
void SAL_CALL rtl_copyMemory(void *Dst, const void *Src, sal_Size Bytes);
void SAL_CALL rtl_moveMemory(void *Dst, const void *Src, sal_Size Bytes);
sal_Int32 SAL_CALL rtl_compareMemory(const void *MemA, const void *MemB, sal_Size Bytes);
void* SAL_CALL rtl_findInMemory(const void *MemA, sal_uInt8 ch, sal_Size Bytes);

#ifdef __cplusplus
}
#endif


#endif /*_RTL_MEMORY_H_ */

