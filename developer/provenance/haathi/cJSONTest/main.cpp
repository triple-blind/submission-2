/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/cJSONTest/main.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout
/******************************************************************************
External
******************************************************************************/
bool objectTest(); // Don't worry about includes!
bool stringTest();
bool escapeTest();
bool arrayTest();
/******************************************************************************
Content
******************************************************************************/
int passfail(bool testResult) {
	std::cout << (testResult ? "Failed" : "Passed") << std::endl;
	return testResult ? 1 : 0;
}

int main(int argc, char* argv[]) {
	int fail = 0;

	std::cout << "objectTest: " << std::endl;
	fail |= passfail(objectTest());

	std::cout << "stringTest: " << std::endl;
	fail |= passfail(stringTest());

	std::cout << "escapeTest: " << std::endl;
	fail |= passfail(escapeTest());

	std::cout << "arrayTest: " << std::endl;
	fail |= passfail(arrayTest());

	return passfail(fail != 0);
}
/*****************************************************************************/
