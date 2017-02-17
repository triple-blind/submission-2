/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/cJSONTest/cJSONTest.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/

/******************************************************************************
Include others
******************************************************************************/
#include "cppJSON.hpp"

#include <iostream> // cout
#include <memory.h> // free
#include <stdlib.h> // free
#include <string.h> // strcmp
/******************************************************************************
Content
******************************************************************************/
bool arrayTest() {
	const char* array1 = "[ 1, 2, 3]";
	const char* array2 = "[ 4, 5, 6, 7]";

	cJSON *cArray1 = cJSON_Parse(array1);
	if (cJSON_GetArraySize(cArray1) != 3)
		return true;

	cJSON *cArray2 = cJSON_Parse(array2);
	if (cJSON_GetArraySize(cArray2) != 4)
		return true;

	while (cJSON_GetArraySize(cArray2) > 0) {
		cJSON *item = cJSON_DetachItemFromArray(cArray2, 0);
		cJSON_AddItemToArray(cArray1, item);
	}

	if (cJSON_GetArraySize(cArray1) != 7)
		return true;
	if (cJSON_GetArraySize(cArray2) != 0)
		return true;
	cJSON_Delete(cArray1);
	cJSON_Delete(cArray2);
	return false;
}

bool stringTest(const char* expectedString) {
	bool different;

	cJSON* expectedObject = cJSON_CreateObject();
	cJSON_AddStringToObject(expectedObject, "String", expectedString);
	char* tmpString = cJSON_Print(expectedObject);
	cJSON* actualObject = cJSON_Parse(tmpString);
	cJSON* tmpObject = cJSON_GetObjectItem(actualObject, "String");
	char* actualString = tmpObject->valuestring;

	std::cout << "Does '" << actualString << "'=='" << expectedString << "'?" << std::endl;
	different = strcmp(expectedString, actualString) != 0;

//	cJSON_Delete(tmpObject); // Subobject that doesn't need to be deleted!
	cJSON_Delete(actualObject);
	cppJSON_FreeString(tmpString);
	cJSON_Delete(expectedObject);

	bool badCounts = cppJSON::getMallocCount() != cppJSON::getFreeCount();

	return different || badCounts;
}

bool escapeTest() {
	return stringTest("\r \n \r\n");
}

bool stringTest() {
	return stringTest("testing");
}

bool objectTest() {
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

	cppJSON_FreeString(actual);
	cJSON_Delete(object);

	bool badCounts = cppJSON::getMallocCount() != cppJSON::getFreeCount();

	return different || badCounts;
}
/*****************************************************************************/
