/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance/haathi/cJSONTest/cJSONTest.cpp#4 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout
#include <memory.h> // free
#include <stdlib.h> // free
#include <string.h> // strcmp
/******************************************************************************
External
******************************************************************************/
int cyasslServerMain(int argc, char *argv[]);
extern char input[]; // cyasslServerMain
/******************************************************************************
Content
******************************************************************************/
bool cyasslServerTest(int argc, char* argv[]) {
	const char* expected = "hello cyassl!"; // Taken from cyasslClientMain

	if (cyasslServerMain(argc, argv) != 0)
		return true;

	char* actual = input;
	std::cout << "Does '" << actual << "'=='" << expected << "'?" << std::endl;

	return strcmp(expected, actual) != 0;
}
/*****************************************************************************/
