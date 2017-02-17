/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/StringWrapper.hpp#1 $

/*
 *  Created by Aaron Gibson on 9/5/11.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI_UTIL__STRING_WRAPPER_H
#	define COM__HAATHI_UTIL__STRING_WRAPPER_H
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/Archive.hpp"

#include "sdkBeg.hpp"
#include <rtl/ustrbuf.hxx>
#include <rtl/ustring.hxx>
#include "sdkEnd.hpp"

#include <iostream>
#include <sstream>
#include <string>

#ifdef LIBREOFFICE_3_5S
#	include <rtl/oustringostreaminserter.hxx>
#endif
/******************************************************************************
Using
******************************************************************************/
using rtl::OUString;
/******************************************************************************
Namespace
******************************************************************************/
// Clients expect this to be defined
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
OUString withoutCrLf(OUString text);
OUString escape(OUString text);
OUString unescape(OUString text);

/*!
 This function will convert the passed data into a string. 
 Note that the data must be able to be pushed onto a stringstream.
 
 @param data -- the data to convert to a string.
 @result -- the string representation of "data".
 */
template<class T>
inline std::string convertToString(const T& data) {
	std::stringstream stream;

	stream << data;
	return stream.str();
}

/*!
 This function is a special case of convertToString to handle the unique case
 of converting OUString into std::string.
 
 @param data -- the OUString to convert.
 @result -- an instance of std::string after conversion.
 */
template<>
inline std::string convertToString(const rtl::OUString& input) {
	rtl::OString oString = rtl::OUStringToOString(input, RTL_TEXTENCODING_UTF8);
	std::string output(oString.getStr(), oString.getLength());
	
	return output;
}

/*!
 This function will convert a string back into the specified type.
 Like convertToString(), the parameter "data" must be able to be
 read from a stringstream.
 
 @param data -- the string to read from.
 @result -- the instance of T represented in "data".
 */
/*
template<class T>
inline T convertFromString(const std::string& data) {
	T result;

	std::stringstream stream(data);
	stream >> result;
	return result;
}
*/
/*!
 This function will convert the passed parameter "data" to OUString.
 
 @param data -- the object to convert to an OUString.
 @result -- the resulting OUString.
 */
template<class T>
inline rtl::OUString convertToOUString(const T& data) {
	rtl::OUStringBuffer buffer;

	buffer.appendAscii(convertToString(data).c_str());
	return buffer.makeStringAndClear();
}

template<>
inline rtl::OUString convertToOUString<std::string>(const std::string& input) {
	OUString output = rtl::OUString(input.c_str(), input.size(), RTL_TEXTENCODING_UTF8);

	return output;
}

// Plain, static text should use this simpler version
inline rtl::OUString convertToOUString(const char* text) {
	return rtl::OUString::createFromAscii(text);
}

#define OUSTRING(x) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(x))
//#define OUSTRING(x) rtl::OUString(reinterpret_cast<const sal_Unicode*>(L ## x))
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
