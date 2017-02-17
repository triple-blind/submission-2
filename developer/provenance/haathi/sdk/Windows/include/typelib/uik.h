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


#ifndef _TYPELIB_UIK_H_
#define _TYPELIB_UIK_H_

#include <sal/types.h>

#if defined( SAL_W32)
#pragma pack(push, 8)
#elif defined(SAL_OS2)
#pragma pack(push, 8)
#endif

/** Binary typelib uik struct.  Internally not used anymore.
*/
typedef struct _typelib_Uik
{
    sal_uInt32	m_Data1;
    sal_uInt16	m_Data2;
    sal_uInt16	m_Data3;
    sal_uInt32	m_Data4;
    sal_uInt32	m_Data5;
} typelib_Uik;

#if defined( SAL_W32) ||  defined(SAL_OS2)
#	pragma pack(pop)
#endif

#endif
