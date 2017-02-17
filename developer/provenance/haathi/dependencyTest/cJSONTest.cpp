/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/dependencyTest/cJSONTest.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "cppJSON.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout
#include <memory.h> // free
#include <stdlib.h> // free
#include <string.h> // strcmp
/******************************************************************************
Content
******************************************************************************/
bool cJSONTest(int argc,char *argv[]) {
	const char *expected =
		"{\n"
			"\t\"Null\":\tnull,\n"
			"\t\"True\":\ttrue,\n"
			"\t\"False\":\tfalse,\n"
			"\t\"Number\":\t123,\n"
			"\t\"String\":\t\"abc\"\n"
		"}";
	bool different;

	cJSON *object = cJSON_CreateObject();

	cJSON_AddNullToObject(object, "Null");
	cJSON_AddTrueToObject(object, "True");
	cJSON_AddFalseToObject(object, "False");
	cJSON_AddNumberToObject(object, "Number", 123);
	cJSON_AddStringToObject(object, "String", "abc");

	char *actual = cJSON_Print(object);

	std::cout << "Does '" << actual << "'=='" << expected << "'?" << std::endl;
	different = strcmp(expected, actual) != 0;

	free(actual);
	cJSON_Delete(object);
	return different;
}
/*****************************************************************************/
