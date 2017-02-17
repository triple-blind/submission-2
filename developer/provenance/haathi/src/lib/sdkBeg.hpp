/*****************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/sdkBeg.hpp#1 $
*****************************************************************************/
#ifndef SDK_BEG_HPP
#	define SDK_BEG_HPP
#	include "Config.hpp"
#endif
/*****************************************************************************
Includes
*****************************************************************************/
//#include "Config.hpp"

#if OSYS == OSYS_WIN
#	pragma warning(push)
//2>c:\Users\Keith Alcock\Documents\My Data\Projects\provenance\haathi\sdk\Windows\include\com/sun/star/uno/Reference.h(72) : warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
//2>c:\Users\Keith Alcock\Documents\My Data\Projects\provenance\haathi\sdk\Windows\include\rtl/ustring.hxx(1282) : warning C4800: 'sal_Bool' : forcing value to bool 'true' or 'false' (performance warning)
#	pragma warning(disable: 4290 4800)
#else
#	if OSYS != OSYS_MAC
#		pragma GCC diagnostic push
#	endif
#	pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
// Give in and let every sdk user know about this file
#include <com/sun/star/uno/Reference.hxx>
// and this one
#include <rtl/ustring.hxx>
/****************************************************************************/
//#endif
/****************************************************************************/
