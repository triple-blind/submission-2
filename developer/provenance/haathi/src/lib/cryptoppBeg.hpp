/*****************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/cryptoppBeg.hpp#1 $
*****************************************************************************/
#ifndef BOOST_BEG_HPP
#	define BOOST_BEG_HPP
#	include "Config.hpp"
#endif
/*****************************************************************************
Includes
*****************************************************************************/
//#include "Config.hpp"

#if OSYS == OSYS_WIN
#else
#	if OSYS != OSYS_MAC
#		pragma GCC diagnostic push
#	endif
#	pragma GCC diagnostic ignored "-Wunused-variable"
#	ifdef W64LIT
#		define TMP_W64LIT W64LIT
#		undef W64LIT
#	endif
#endif
/****************************************************************************/
//#endif
/****************************************************************************/
