/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/leptonicaAll.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef LEPTONICA_ALL_HPP
#	define LEPTONICA_ALL_HPP
/******************************************************************************
Include
******************************************************************************/
#include "Config.hpp"

#if OSYS == OSYS_WIN
#	pragma warning(push)
// 2>c:\users\keith alcock\documents\my data\projects\provenance\libcpp\leptonica\include\leptonica\pix.h(169) : warning C4305: 'initializing' : truncation from 'double' to 'const l_float32'
// 2>c:\users\keith alcock\documents\my data\projects\provenance\libcpp\leptonica\include\leptonica\pix.h(171) : warning C4305: 'initializing' : truncation from 'double' to 'const l_float32'
#	pragma warning(disable: 4305)
#endif

#include <leptonica/allheaders.h>

#if OSYS == OSYS_WIN
#	pragma warning(pop)
#endif
/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
Class
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
