/*****************************************************************************
$Header: //MyDataDepot/Projects/provenance-autotype2/haathi/src/lib/com/haathi/watermark/scheme/SchemeConfig.hpp#2 $
*****************************************************************************/
#ifndef COM__HAATHI__WATERMARK__SCHEME__SCHEME_CONFIG_HPP
#	define COM__HAATHI__WATERMARK__SCHEME__SCHEME_CONFIG_HPP
/*****************************************************************************
Definitions
*****************************************************************************/
#define SCHEME_VIS_HI 3
#define SCHEME_VIS_MD 2
#define SCHEME_VIS_LO 1

#define SCHEME_VIS_NO 0
#define SCHEME_VIS_DB -1 // debug

#if !defined(SCHEME_VIS)
#	define SCHEME_VIS SCHEME_VIS_HI
#endif
/****************************************************************************/
#endif
/****************************************************************************/
