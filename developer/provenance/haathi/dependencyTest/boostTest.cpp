/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/dependencyTest/boostTest.cpp#1 $
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

bool serializationTest() {
	std::string outText = "Test string to serialize.";
	std::stringstream stream;

	{
		boost::archive::text_oarchive oa(stream);
		oa << outText;
	}

	std::string inText;
	{
		boost::archive::text_iarchive ia(stream);
		ia >> inText;
	}
	std::cout << "Does '" << outText << "'=='" << inText << "'?" << std::endl;
	return outText != inText;
}

bool boostTest(int argc, char *argv[]) {
	if (sharedPointerTest())
		return true;
	if (serializationTest())
		return true;
	return false;
}
/*****************************************************************************/
