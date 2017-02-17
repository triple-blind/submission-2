/*****************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/CloudConfig.hpp#4 $
*****************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__CLOUD_CONFIG_HPP
#	define COM__HAATHI__DOCUMENT__CLOUD_CONFIG_HPP
/*****************************************************************************
Definitions
*****************************************************************************/
#define WATERMARK_ENGINE_EVERYWHERE 0
#define WATERMARK_ENGINE_NOWHERE    1
#define WATERMARK_ENGINE_SOMEWHERE  2
#define WATERMARK_ENGINE_OFFSCREEN  4

#define WATERMARK_ENGINE_BOTHWHERE  10 // Combines EveryWhere with SomeWhere, OffScreen, or Synonym

#define WATERMARK_EMBEDDER_STYLE     0
#define WATERMARK_EMBEDDER_ATTRIBUTE 1

#define ENCRYPTOR_ASYNC 0
#define ENCRYPTOR_SYNC  1

#define AUTO_TYPE_OFF 0
#define AUTO_TYPE_ON  1

#define PROVENANCE_OFF 0
#define PROVENANCE_ON  1

#define WATERMARK_OFF 0
#define WATERMARK_ON  1

#define DIFFERENCES_OFF 0
#define DIFFERENCES_ON  1

#define SYNONYM_OFF 0
#define SYNONYM_ON  1

#if !defined(ENCRYPTOR)
#	define ENCRYPTOR ENCRYPTOR_SYNC
#endif

#if !defined(WATERMARK_ENGINE)
//#	define WATERMARK_ENGINE WATERMARK_ENGINE_EVERYWHERE
#	define WATERMARK_ENGINE WATERMARK_ENGINE_BOTHWHERE
#endif

#if !defined(BOTHWHERE_ENGINE_WHERE)
//#	define BOTHWHERE_ENGINE_WHERE WATERMARK_ENGINE_SOMEWHERE
#	define BOTHWHERE_ENGINE_WHERE WATERMARK_ENGINE_OFFSCREEN
#endif

#if !defined(AUTO_TYPE)
#	define AUTO_TYPE AUTO_TYPE_OFF
#endif

#if !defined(PROVENANCE)
#	define PROVENANCE PROVENANCE_ON
#endif

#if !defined(WATERMARK)
#	if EXPLOIT == EXPLOIT_OFF
#		define WATERMARK WATERMARK_ON
#	else
#		define WATERMARK WATERMARK_ON
#	endif
#endif

#if !defined(WATERMARK_EMBEDDER)
#	define WATERMARK_EMBEDDER WATERMARK_EMBEDDER_STYLE
#endif

#define DIFFERENCES DIFFERENCES_ON

#if !defined(DIFFERENCES)
#	if (PROVENANCE == PROVENANCE_ON) || (WATERMARK == WATERMARK_ON)
#		define DIFFERENCES DIFFERENCES_ON
#	else
#		define DIFFERENCES DIFFERENCES_OFF
#	endif
#endif
/****************************************************************************/
#endif
/****************************************************************************/
