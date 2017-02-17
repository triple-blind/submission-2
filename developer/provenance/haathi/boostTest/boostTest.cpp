/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/boostTest/boostTest.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout, string
#include <sstream> // stringstream


#ifdef __GNUC__
#	ifndef __llvm__
#		pragma GCC diagnostic push
#		pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#	endif
#endif

// This one requires only header files, so is not a good test of the library.
#include <boost/shared_ptr.hpp> // boost::shared_ptr<>
// These require actual library functions.
#include <boost/archive/text_iarchive.hpp> // boost::archive::text_iarchive
#include <boost/archive/text_oarchive.hpp> // boost::archive::text_oarchive

#include <boost/archive/xml_iarchive.hpp> // boost::archive::xml_iarchive
#include <boost/archive/xml_oarchive.hpp> // boost::archive::xml_oarchive

#ifdef __GNUC__
#	ifndef __llvm__
#		pragma GCC diagnostic pop
#	endif
#endif
/******************************************************************************
Content
******************************************************************************/
static int helloCount = 0;

struct Hello {
	Hello() {
		std::cout << "Hello constructor" << std::endl;
		helloCount++;
	}

	~Hello() {
		std::cout << "Hello destructor" << std::endl;
		helloCount--;
	}
};

class Base {
	public:
		virtual int answer() {
			return 4;
		}
};

class Derived: Base {
	public:
		virtual int answer() {
			return 5;
		}
};

bool sharePointer() {
	int hcBefore = helloCount;
	boost::shared_ptr<Hello> sharedHello(new Hello);
	int hcAfter = helloCount;
	if (hcAfter != hcBefore + 1)
		return true;
	return false;
}

bool sharedPointerTest() {
	if (helloCount != 0)
		return true;
	if (sharePointer())
		return true;
	if (helloCount != 0)
		return true;
	return false;
}

bool textSerializationTest() {
	std::string outText = "Test string to serialize.";
	std::stringstream stream;

	{
		boost::archive::text_oarchive oa(stream);
		oa << BOOST_SERIALIZATION_NVP(outText);
	}

	std::string inText;
	{
		boost::archive::text_iarchive ia(stream);
		ia >> BOOST_SERIALIZATION_NVP(inText);
	}
	std::cout << "Does '" << outText << "'=='" << inText << "'?" << std::endl;
	return outText != inText;
}

bool xmlSerializationTest() {
	std::string outText = "Test string to serialize.";
	std::stringstream stream;

	{
		boost::archive::xml_oarchive oa(stream);
		oa << BOOST_SERIALIZATION_NVP(outText);
	}

	std::string inText;
	{
		boost::archive::xml_iarchive ia(stream);
		ia >> BOOST_SERIALIZATION_NVP(inText);
	}
	std::cout << "Does '" << outText << "'=='" << inText << "'?" << std::endl;
	return outText != inText;
}

bool xmlInheritanceTest() {
	return false;
/*
	Derived outDerived;
	std::stringstream stream;

	if (outDerived.answer() != 5)
		return false;

	{
		boost::archive::xml_oarchive oa(stream);
		oa << BOOST_SERIALIZATION_NVP(outDerived);
	}

	Base inBase;
	std::string inText;
	{
		boost::archive::xml_iarchive ia(stream);
		ia >> BOOST_SERIALIZATION_NVP(inBase);
	}
	if (inBase.answer() != 5)
		return false;
	return true; */
}

bool boostTest(int argc, char *argv[]) {
	if (sharedPointerTest())
		return true;
	if (textSerializationTest())
		return true;
	if (xmlSerializationTest())
		return true;
	if (xmlInheritanceTest())
		return true;
	return false;
}
/*****************************************************************************/
