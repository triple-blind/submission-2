/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/dependencyTest/main.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout
/******************************************************************************
External
******************************************************************************/
bool cJSONTest(int argc, char *argv[]);
bool boostTest(int argc, char *argv[]);
bool cryptoppTest(int argc, char *argv[]);
bool cyasslTest(int argc, char *argv[]);
bool leptonicaTest(int argc, char *argv[]);
bool tesseractTest(int argc, char *argv[]);
/******************************************************************************
Content
******************************************************************************/
int passfail(bool testResult) {
	std::cout << (testResult ? "Failed" : "Passed") << std::endl;
	return testResult ? 1 : 0;
}

int main(int argc, char* argv[]) {
	int fail = 0;
	
	fail |=  passfail(cJSONTest(argc, argv));
	fail |=  passfail(boostTest(argc, argv));
	fail |=  passfail(cryptoppTest(argc, argv));
	fail |=  passfail(cyasslTest(argc, argv));
	fail |=  passfail(leptonicaTest(argc, argv));
	fail |=  passfail(tesseractTest(argc, argv));
	return passfail(fail != 0);
}
/*****************************************************************************/
