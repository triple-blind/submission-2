/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/tesseractAll.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef TESSERACT_ALL_HPP
#	define TESSERACT_ALL_HPP
/******************************************************************************
Include
******************************************************************************/
#include "Config.hpp"

// Tesseract will include Leptonica, but without the proper fixes,
// so include our version first.
#include "leptonicaAll.hpp"

#if OSYS == OSYS_WIN
#	pragma warning(push)
// 2>c:\users\keith alcock\documents\my data\projects\provenance\libcpp\tesseract\include\tesseract\strngs.h(61) : warning C4996: 'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
// 2>c:\users\keith alcock\documents\my data\projects\provenance\libcpp\tesseract\include\tesseract\unicharset.h(47) : warning C4996: 'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
// 2>c:\users\keith alcock\documents\my data\projects\provenance\libcpp\tesseract\include\tesseract\unicharset.h(274) : warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
// 2>c:\users\keith alcock\documents\my data\projects\provenance\libcpp\tesseract\include\tesseract\unicharset.h(299) : warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
// 2>c:\users\keith alcock\documents\my data\projects\provenance\libcpp\tesseract\include\tesseract\platform.h(33) : warning C4005: 'snprintf' : macro redefinition
#	pragma warning(disable: 4005 4996)
#endif

#include <tesseract/baseapi.h>

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
