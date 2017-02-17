/*****************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/Config.hpp#1 $
*****************************************************************************/
#if !defined(CONFIG_HPP)
#	define CONFIG_HPP
/*****************************************************************************
Define internal values
*****************************************************************************/
#define _OFFICE_OPEN  1
#define _OFFICE_LIBRE 2

#define OSYS_WIN   1
#define OSYS_MAC   2
#define OSYS_LINUX 3
/*****************************************************************************
Convert from command line defines
*****************************************************************************/
#if defined(WIN32)
#	define OSYS OSYS_WIN
#else
#	if defined(LINUX)
#		define OSYS OSYS_LINUX
#	else
#		if defined(MACOSX)
#			define OSYS OSYS_MAC
#		endif
#	endif
#endif

#if defined(OFFICE_OPEN) && defined(OFFICE_LIBRE)
#	error "Define only one of OFFICE_OPEN and OFFICE_LIBRE."
#endif

#if defined(OFFICE_OPEN)
#	undef OFFICE_OPEN
#	define OFFICE _OFFICE_OPEN
#endif

#if defined(OFFICE_LIBRE)
#	undef OFFICE_LIBRE
#	define OFFICE _OFFICE_LIBRE
#endif

#define OFFICE_OPEN  _OFFICE_OPEN
#define OFFICE_LIBRE _OFFICE_LIBRE
/*****************************************************************************
Definitions
*****************************************************************************/
#define GUARD_OFF 0
#define GUARD_ON  1

#define UPDATE_OFF 0
#define UPDATE_ON  1

#define ARCHIVE_CPP 0
#define ARCHIVE_XML 1

#define SHARED_EVENT_PTR_OFF 0
#define SHARED_EVENT_PTR_ON  1

#define ENCRYPT_PROVENANCE_OFF 0
#define ENCRYPT_PROVENANCE_ON  1
/*****************************************************************************
Check existence
*****************************************************************************/
#if !defined(OSYS)
#	error "OSYS must be defined for compilation."
#endif

#if !defined(OFFICE)
#	error "OFFICE must be defined for compilation."
#endif

#if !defined(GUARD)
#	define GUARD GUARD_OFF
#endif

#if !defined(UPDATE)
#	define UPDATE UPDATE_OFF
#endif

#if !defined(ARCHIVE)
#	define ARCHIVE ARCHIVE_XML
#endif

#if !defined(SHARED_EVENT_PTR)
#	if OSYS != OSYS_WIN
#		define SHARED_EVENT_PTR SHARED_EVENT_PTR_ON
#	else
#		define SHARED_EVENT_PTR SHARED_EVENT_PTR_ON
#	endif
#endif

#if !defined(ENCRYPT_PROVENANCE)
#	define ENCRYPT_PROVENANCE ENCRYPT_PROVENANCE_ON
#endif
/*****************************************************************************
Undefine internal values
*****************************************************************************/
// This doesn't work on Windows at least.  Internal names will now leak.
//#undef _OFFICE_OPEN
//#undef _OFFICE_LIBRE
/****************************************************************************/
#endif
/****************************************************************************/
