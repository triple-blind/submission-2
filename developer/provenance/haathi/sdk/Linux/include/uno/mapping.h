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


#ifndef _UNO_MAPPING_H_
#define _UNO_MAPPING_H_

#include <sal/types.h>
#include <rtl/ustring.h>


#ifdef __cplusplus
extern "C"
{
#endif

struct _typelib_InterfaceTypeDescription;
struct _uno_Mapping;
struct _uno_Environment;

/**
   Function pointer declaration to acquire a UNO mapping.
*/
typedef void (SAL_CALL * uno_AcquireMappingFunc)(struct _uno_Mapping *);

/**
   Function pointer declaration to release a UNO mapping.
*/
typedef void (SAL_CALL * uno_ReleaseMappingFunc)(struct _uno_Mapping *);

/** Function pointer declaration to map an interface from one environment to another.
    
	@param pMapping			mapping
	@param ppOut			[inout] destination interface; existing interfaces are released
	@param pInterface		source interface
	@param pInterfaceTypeDescr type description of the interface
*/
typedef void (SAL_CALL * uno_MapInterfaceFunc)(
	struct _uno_Mapping * pMapping,
	void ** ppOut, void * pInterface,
	struct _typelib_InterfaceTypeDescription * pInterfaceTypeDescr );


#if defined( SAL_W32)
#pragma pack(push, 8)
#elif defined(SAL_OS2)
#pragma pack(push, 8)
#endif

/** This is the binary specification of a mapping.
*/
typedef struct _uno_Mapping
{
	/** Acquires mapping
	*/
	uno_AcquireMappingFunc acquire;

	/** Releases mapping. The last release may unload bridges.
	*/
	uno_ReleaseMappingFunc release;

	/** mapping function
	*/
	uno_MapInterfaceFunc mapInterface;
} uno_Mapping;

#if defined( SAL_W32) ||  defined(SAL_OS2)
#pragma pack(pop)
#endif

/** Gets an interface mapping from one environment to another.
    
	@param ppMapping	[inout] mapping; existing mapping will be released
	@param pFrom		source environment
	@param pTo			destination environment
						(interfaces resulting in mapInterface() call can be used
						in this language environment)
	@param pAddPurpose	additional purpose of mapping (e.g., protocolling); defaults to 0 (none)
*/
void SAL_CALL uno_getMapping(
	struct _uno_Mapping ** ppMapping,
	struct _uno_Environment * pFrom,
	struct _uno_Environment * pTo,
	rtl_uString * pAddPurpose )
	SAL_THROW_EXTERN_C();

/** Callback function pointer declaration to get a mapping.
    
	@param ppMapping	inout mapping
	@param pFrom		source environment
	@param pTo			destination environment
	@param pAddPurpose	additional purpose
*/
typedef void (SAL_CALL * uno_getMappingFunc)(
	struct _uno_Mapping ** ppMapping,
	struct _uno_Environment * pFrom,
	struct _uno_Environment * pTo,
	rtl_uString * pAddPurpose );

/** Registers a callback being called each time a mapping is demanded.
    
	@param pCallback	callback function
*/
void SAL_CALL uno_registerMappingCallback(
	uno_getMappingFunc pCallback )
	SAL_THROW_EXTERN_C();

/** Revokes a mapping callback registration.
    
	@param pCallback	callback function
*/
void SAL_CALL uno_revokeMappingCallback(
	uno_getMappingFunc pCallback )
	SAL_THROW_EXTERN_C();

/** Function pointer declaration to free a mapping.
    
	@param pMapping		mapping to be freed
*/
typedef void (SAL_CALL * uno_freeMappingFunc)( struct _uno_Mapping * pMapping );

/** Registers a mapping. A mapping registers itself on first acquire and revokes itself on last
    release. The given freeMapping function is called by the runtime to cleanup any resources.
    
	@param ppMapping	inout mapping to be registered
	@param freeMapping	called by runtime to delete mapping
	@param pFrom		source environment
	@param pTo			destination environment
	@param pAddPurpose	additional purpose string; defaults to 0
*/
void SAL_CALL uno_registerMapping(
	struct _uno_Mapping ** ppMapping, uno_freeMappingFunc freeMapping,
	struct _uno_Environment * pFrom, struct _uno_Environment * pTo, rtl_uString * pAddPurpose )
	SAL_THROW_EXTERN_C();

/** Revokes a mapping. A mapping registers itself on first acquire and revokes itself on last
    release.
    
	@param pMapping		mapping to be revoked
*/
void SAL_CALL uno_revokeMapping(
	struct _uno_Mapping * pMapping )
	SAL_THROW_EXTERN_C();

/** Gets an interface mapping from one language environment to another by corresponding environment
    type names.
    
	@param ppMapping	[inout] mapping; existing mapping will be released
	@param pFrom		source environment type name
	@param pTo			destination environment type name
						(interfaces resulting in mapInterface() call can be used
						in this language environment)
	@param pAddPurpose	additional purpose of mapping (e.g., protocolling); defaults to 0 (none)
*/
void SAL_CALL uno_getMappingByName(
	struct _uno_Mapping ** ppMapping,
	rtl_uString * pFrom,
	rtl_uString * pTo,
	rtl_uString * pAddPurpose )
	SAL_THROW_EXTERN_C();

/* symbol exported by each language binding library */
#define UNO_EXT_GETMAPPING "uno_ext_getMapping"

/** Function pointer declaration to get a mapping from a loaded bridge. Bridges export a function
    called uno_ext_getMapping() of this signature.
    
	@param ppMapping	[inout] mapping; existing mapping will be released
	@pFrom				source environment
	@pTo				destination environment
*/
typedef void (SAL_CALL * uno_ext_getMappingFunc)(
	struct _uno_Mapping ** ppMapping,
	struct _uno_Environment * pFrom,
	struct _uno_Environment * pTo );

#ifdef __cplusplus
}
#endif

#endif
