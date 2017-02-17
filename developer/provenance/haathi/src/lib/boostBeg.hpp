/*****************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/boostBeg.hpp#1 $
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
#	pragma warning(push)
//1>c:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\include\functional(143) : warning C4800: 'sal_Bool' : forcing value to bool 'true' or 'false' (performance warning)
#	pragma warning(disable: 4800)
#else
///home/kwalcock/Documents/provenance/haathi/../libcpp/boost/NonWindows/include/boost/serialization/static_warning.hpp:104:18: warning: typedef ‘STATIC_WARNING_LINE102’ locally defined but not used [-Wunused-local-typedefs]
#	if OSYS != OSYS_MAC
#		pragma GCC diagnostic push
#		pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#	endif
#	pragma GCC diagnostic ignored "-Wunused-variable"
#	pragma GCC diagnostic ignored "-Wunused-function"
#endif
/****************************************************************************/
//#endif
/****************************************************************************/
