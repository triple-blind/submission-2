/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/leptonicaTest/main.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout
/******************************************************************************
External
******************************************************************************/
bool leptonicaTest(int argc, char *argv[]); // Don't worry about includes!
/******************************************************************************
Content
******************************************************************************/
int passfail(bool testResult) {
	std::cout << (testResult ? "Failed" : "Passed") << std::endl;
	return testResult ? 1 : 0;
}

int main(int argc, char* argv[]) {
	return passfail(leptonicaTest(argc, argv));
}
/*****************************************************************************/
