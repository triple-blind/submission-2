/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/Serialization.hpp#1 $

/*!
 @header serialization.hpp
 
 @author Aaron Gibson
 @date March 22, 2012
 
 This header file will contain all of the proper includes for "serializing" an object. This allows
 serializing from streams and strings, an absolute must for reading provenance from a file.
 
 To implement this functionality, the "Boost" libraries are used. This is because the Boost library
 allows for serializing into otherwise abstract classes. This makes implementing new provenance event
 types much, MUCH easier. The drawback is that another library must be linked against; while most 
 Boost libraries are header-only, this library is system-dependent, probably for the very fact that 
 this can serialize to an abstract base class.
 
 First, we discuss how to implement serialization in some class. The following code snippet shows an
 example class that can serialize:
 <code>
 class Example : public BaseClass {
 int members;
 
 friend class boost::serialization::access;
 template< class Archive >
 void serialize( Archive ar, const unsigned int version )
 {
 // If your class derives from another base class, do this first.
 ar & boost::serialization::base_object< BaseClass >(*this);

 // Use this notation for all members. Obviously, if you have a member class of your
 // own design, you should also implement this function "serialize" in them as well.
 ar & members; 
 }
 };
 <code/>
 That's it! Extremely easy, one of the reasons this library is pretty cool. There are some cases
 where you may have to do a few additional things; for instance, if you have an abstract base class 
 that you want to serialize, you should add the following macro at the end of the header for that
 abstract class:
 <code>
 BOOST_SERIALIZATION_ASSUME_ABSTRACT( <abstract-class-name > )
 <code/>
 This might not be needed, but the Boost library suggests you do this anyway for portability reasons.
 (Some compilers need to explicitly know; this is related to whether your system supports the standard
 template function std::typeid()... Your system probably will, but still.) 
 Furthermore, for each class that derives from the abstract class, you must add the macro:
 "BOOST_CLASS_EXPORT( < derived-class-name >) at the end of its definition/implementation.
 
 Also, things might get a bit more complicated in the very rare case of "diamond-inheritance". C++
 offers a mechanism to declare a base class as "virtual" which instanciates only one instance of the 
 original class at the top of the "diamond". Supposedly, you don't have to do anything, but read the 
 Boost library documentation to be absolutely sure. The URL is: 
 <url>http://www.boost.org/doc/libs/1_49_0/libs/serialization/doc/index.html<url/>
 
 In addition to saving, we need a way to load the data back into the program. The code below
 demonstrates how to do this:
 <code>
 int main(int argc, char** argv)
 {
 // Assume this class has properly implemented "void serialize()".
 SerializableClass before;
 
 // Create a stream to store the object. Note that this can be any Standard Template stream.
 std::stringstream ss;
 
 std::cout << "Before Serialization: " << before << std::endl;
 {
 // The extra brackets here give the proper "scope" to boost's archive class. The use
 // of this scope will automatically close the archive once we exit this scope. 
 boost::archive::text_oarchive output(ss);
 output << before;
 }
 
 // ss now stores the object.
 
 SerializableClass after;
 {
 boost::archive::text_iarchive input(ss);
 input >> after;
 }
 
 std::cout << "After Serialization: " << after << std::endl;
 return 0;
 }
 <code/>
 This really is about as easy as it gets for serialization. Anyway, to use this, you must include
 this header, which includes all of the relevent boost files, as well as link against the 
 boost_serialization library (typically called libboost_serialization.a).
 
 Remember, for OpenOffice, the library to link against should be static, and compiled with -fPIC.
 So, when you compile the Boost libraries, make sure you specify these flags!
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__SERIALIZATION_HPP
#	define COM__HAATHI__UTILS__SERIALIZATION_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"

#include "boostBeg.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include "boostEnd.hpp"
/******************************************************************************
Define
******************************************************************************/
#define HAATHI_SERIALIZATION_NVP(name, value) \
	boost::serialization::make_nvp(name, value)

#define HAATHI_SERIALIZATION_BASE_OBJECT_NVP(name, value) \
	boost::serialization::make_nvp(name, \
		boost::serialization::base_object<value >(*this) \
	)
/******************************************************************************
Namespace
******************************************************************************/
// Clients expect this to be defined
namespace com {
	namespace haathi {
		namespace utils {
		};
	};
};

// These macros must be declared outside of any namespace.
BOOST_SERIALIZATION_SPLIT_FREE(rtl::OUString);
BOOST_SERIALIZATION_SPLIT_FREE(rtl::OUStringBuffer);

namespace boost {
	namespace serialization {
/******************************************************************************
Class
******************************************************************************/
/*!
 This function is used to serialize an OUString. This is implemented via an
 OUStringBuffer for mutability reasons.
 
 @param ar -- the archive stream
 @param str -- the OUString to save.
 @param version -- the version, not used as of yet.
 */
void save(boost::archive::text_oarchive& ar, const rtl::OUString& str, unsigned int version);
void save(boost::archive::text_oarchive& ar, const rtl::OUStringBuffer& buffer, unsigned int version);
void save(boost::archive::xml_oarchive& ar, const rtl::OUString& str, unsigned int version);

/*!
 This function is used to deserialize an OUString. This is implemented via an
 OUStringBuffer since the OUStringBuffer is mutable.
 
 @param ar -- the archive stream
 @param str -- the OUString to save.
 @param version -- the version, not used as of yet.
 */
void load(boost::archive::text_iarchive& ar, rtl::OUString& str, unsigned int version);
void load(boost::archive::text_iarchive& ar, rtl::OUStringBuffer& buffer, unsigned int version);
void load(boost::archive::xml_iarchive& ar, rtl::OUString& str, unsigned int version);
/******************************************************************************
Namespace
******************************************************************************/
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
