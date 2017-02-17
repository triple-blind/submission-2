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
int cyasslClientMain(int argc, char *argv[]);
extern char reply[]; // cyasslClientMain
/******************************************************************************
Content
******************************************************************************/
bool cyasslClientTest(int argc, char* argv[]) {
	const char* expected = "I hear you fa shizzle!"; // Taken from cyasslServerMain

	if (cyasslClientMain(argc, argv) != 0)
		return true;

	char* actual = reply;
	std::cout << "Does '" << actual << "'=='" << expected << "'?" << std::endl;

	return strcmp(expected, actual) != 0;
}
/*****************************************************************************/
