/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/Serialization.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/Serialization.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"

#include <string>
/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::convertToOUString;
using com::haathi::utils::convertToString;
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace boost {
	namespace serialization {
/******************************************************************************
Text versions for ARCHIVE_CPP
******************************************************************************/
void save(boost::archive::text_oarchive& ar, const rtl::OUString& str, unsigned int version) {
	rtl::OUStringBuffer buffer(str);

	ar & BOOST_SERIALIZATION_NVP(buffer);
}

void save(boost::archive::text_oarchive& ar, const rtl::OUStringBuffer& buffer, unsigned int version) {
	sal_Int32 length = buffer.getLength();
	const sal_Unicode* str = buffer.getStr();

	ar & BOOST_SERIALIZATION_NVP(length);
	for (sal_Int32 i = 0; i < length; ++i)
		ar & BOOST_SERIALIZATION_NVP(str[i]);
}

void load(boost::archive::text_iarchive& ar, rtl::OUString& str, unsigned int version) {
	::rtl::OUStringBuffer buffer;

	ar & BOOST_SERIALIZATION_NVP(buffer);
	// This creates the string from the buffer.
	str = buffer.makeStringAndClear();
}

void load(boost::archive::text_iarchive& ar, rtl::OUStringBuffer& buffer, unsigned int version) {
	sal_Int32 length;
	sal_Unicode ch;

	// Now for the deserialization
	ar & BOOST_SERIALIZATION_NVP(length);
	for (sal_Int32 i = 0; i < length; ++i) {
		ar & BOOST_SERIALIZATION_NVP(ch);
		buffer.append(ch);
	}
}
/******************************************************************************
XML versions for ARCHIVE_XML
******************************************************************************/
// Boost wants to put a std::wstring only into an xml_woarchive and that's overkill
// for other data types.  The xml_oarchive is UTF-8, so need to convert to this format.
void save(boost::archive::xml_oarchive& ar, const rtl::OUString& str, unsigned int version) {
	std::string text = convertToString(str);

	ar & BOOST_SERIALIZATION_NVP(text);
}

void load(boost::archive::xml_iarchive& ar, rtl::OUString& str, unsigned int version) {
	std::string text;

	ar & BOOST_SERIALIZATION_NVP(text);
	str = convertToOUString(text);
}
/******************************************************************************
Namespace
******************************************************************************/
	};
};
/*****************************************************************************/
