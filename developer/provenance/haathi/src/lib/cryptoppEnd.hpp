/*****************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/cryptoppEnd.hpp#1 $
*****************************************************************************/
#ifndef BOOST_END_HPP
#	define BOOST_END_HPP
#	include "Config.hpp"
#	if OSYS != OSYS_WIN
		template<typename T> void cryptoppEnd(T unused) { std::string (*dummy)(const wchar_t*, bool) = &CryptoPP::StringNarrow; }
#	endif
#endif
/*****************************************************************************
Define internal values
*****************************************************************************/
#if OSYS == OSYS_WIN
#else
#	if OSYS != OSYS_MAC
#		pragma GCC diagnostic pop
#	endif
#	ifdef TMP_W64LIT
#		undef TMP_W64LIT
#	endif
#	if OSYS == OSYS_MAC
#	else
// Unfortunately, it's not good enough to suppress this for the header files.
// GCC does not complain until the end of a compilation unit, which is after
// the implementation file, so the code would need to be wrapped in the
// cryptoppBeg and cryptoppEnd.  For the sake of cleaner code, abandon this warning.
// See hack above that was initially for Mac.  Abandonment is not necessary.
//#		pragma GCC diagnostic ignored "-Wunused-functione"
#	endif
#endif
/****************************************************************************/
//#endif
/****************************************************************************/
