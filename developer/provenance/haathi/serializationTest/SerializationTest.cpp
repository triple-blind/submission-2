/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/serializationTest/SerializationTest.cpp#1 $

//
// File: SerializationTest.cxx
//
// Author: Aaron Gibson
//
// This file implements a basic serialization test. This can be used to 
// check whether certain types can serialize, notably OUString, since these
// serialization methods are implemented separately.
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Archive.hpp"
#include "com/haathi/utils/Serialization.hpp" // load and save
#include "com/haathi/utils/StringWrapper.hpp" // OUString

// These are redundant but they keep eclipse from complaining
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <iostream> // cout
#include <sstream> // stringstream
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::utils;

using namespace rtl;
/******************************************************************************
Content
******************************************************************************/
static bool OUStringTest(OUString data) {
	std::stringstream stream;

	std::cout << "Initial Text: " << data << std::endl;
	// The change of scope here forces the destructor of the boost::serialization
	// archive instance, which in turn will flush the buffer.
	{
		HAATHI_OARCHIVE oa(stream);
		oa << BOOST_SERIALIZATION_NVP(data);
	}

	std::cout << "Serialization: " << stream.str();

	// Now, deserialize the data.
	OUString result;
	{
		HAATHI_IARCHIVE ia(stream);
		ia >> BOOST_SERIALIZATION_NVP(result);
	}
	std::cout << "Final Text: " << result << std::endl;
	return data != result;
}

static bool EscapeTest() {
	return OUStringTest(OUString::createFromAscii("<!-- \"'& -->"));
}

// This works internally to boost, but the XML output includes \r and \n
// which can be interpreted by, e.g., the List program in the wrong way.
static bool CrLfTest() {
	return OUStringTest(OUString::createFromAscii(" \r \n \\ \t \r\n "));
}
/*
static OUString escape(OUString text) {
	const sal_Unicode* str = text.getStr();
	OUStringBuffer buffer;

	for (int i = 0; i < text.getLength(); i++)
		switch (str[i]) {
			case '\t':
				buffer.appendAscii("\\t");
				break;
			case '\n':
				buffer.appendAscii("\\n");
				break;
			case '\r':
				buffer.appendAscii("\\r");
				break;
			case '\\':
				buffer.appendAscii("\\\\");
				break;
			default:
				buffer.append(str[i]);
		}
	return buffer.makeStringAndClear();
}

static OUString unescape(OUString text) {
	const sal_Unicode* str = text.getStr();
	OUStringBuffer buffer;

	for (int i = 0; i < text.getLength(); i++)
		if (str[i] == '\\' && i + 1 < text.getLength())
			switch (str[++i]) {
				case 't':
					buffer.appendAscii("\t");
					break;
				case 'n':
					buffer.appendAscii("\n");
					break;
				case 'r':
					buffer.appendAscii("\r");
					break;
				case '\\':
					buffer.appendAscii("\\");
					break;
				default:
					buffer.append(str[i - 1]);
					buffer.append(str[i]);
			}
		else
			buffer.append(str[i]);
	return buffer.makeStringAndClear();
}
*/
static bool CrLfTest2() {
	OUString data = OUString::createFromAscii(" \r \n \\ \t \r\n ");

	std::stringstream stream;

	std::cout << "Initial Text: " << data << std::endl;
	// The change of scope here forces the destructor of the boost::serialization
	// archive instance, which in turn will flush the buffer.
	{
		OUString escapedData = ::escape(data);

		HAATHI_OARCHIVE oa(stream);
		oa << boost::serialization::make_nvp("data", escapedData);
	}

	std::cout << "Serialization: " << stream.str();

	// Now, deserialize the data.
	OUString result;
	{
		HAATHI_IARCHIVE ia(stream);
		ia >> BOOST_SERIALIZATION_NVP(result);
	}
	result = ::unescape(result);
	std::cout << "Final Text: " << result << std::endl;
	return data != result;
}

static bool OUStringTest() {
	return OUStringTest(OUString::createFromAscii("Test string to serialize."));
}

static bool OUStringBufferTest()
{
	OUStringBuffer data;
	std::stringstream stream;

	data.appendAscii("Test string to serialize.");
	std::cout << "Initial Text: " << OUString(data) << std::endl;
	std::cout << "Initial Data: " << data << std::endl;
	// The change of scope here forces the destructor of the boost::serialization
	// archive instance, which in turn will flush the buffer.
	{
		boost::archive::text_oarchive oa(stream);
		oa << BOOST_SERIALIZATION_NVP(data);
	}

	std::cout << "Serialization: " << stream.str();

	// Now, deserialize the data.
	OUStringBuffer result;
	{
		boost::archive::text_iarchive ia(stream);
		ia >> BOOST_SERIALIZATION_NVP(result);
	}
	std::cout << "Final Data: " << result << std::endl;
	std::cout << "Final Text: " << OUString(result) << std::endl;
	return OUString(data) != OUString(result);
}

int passfail(bool testResult) {
	std::cout << (testResult ? "Failed" : "Passed") << std::endl;
	return testResult ? 1 : 0;
}

int main(int argc, char* argv[]) {
	int fail = 0;

	std::cout << "EscapeTest: " << std::endl;
	fail |= passfail(EscapeTest());

	std::cout << "CrLfTest: " << std::endl;
	fail |= passfail(CrLfTest());

	std::cout << "CrLfTest2: " << std::endl;
	fail |= passfail(CrLfTest2());

	std::cout << "OUStringTest: " << std::endl;
	fail |= passfail(OUStringTest());

	std::cout << "OUStringBufferTest: " << std::endl;
	fail |= passfail(OUStringBufferTest());

	return passfail(fail != 0);
}
/*****************************************************************************/
