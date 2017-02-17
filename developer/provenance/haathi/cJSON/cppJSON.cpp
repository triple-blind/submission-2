/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/cJSON/cppJSON.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "cppJSON.hpp"

#include <cstdlib>
/******************************************************************************
Content
******************************************************************************/
#ifdef __cplusplus
	extern "C" {
#endif

extern int mallocCount;
extern int freeCount;

#ifdef __cplusplus
	}
#endif

void cppJSON_FreeString(char* string) {
	if (string) {
		free(string);
		freeCount++;
	}
}

int cppJSON::getMallocCount() {
	return mallocCount;
}

int cppJSON::getFreeCount() {
	return freeCount;
}

cppJSON::cppJSON(cJSON* value) throw(std::bad_alloc): Destructor<cJSON, cJSON_Delete>(value) {
	if (!value)
		throw std::bad_alloc();
}

void cppJSON::freeString(char* string) {
	cppJSON_FreeString(string);
}

std::string cppJSON::print(cJSON* item) {
	char* charItem = cJSON_Print(item);
	std::string stringItem(charItem);

	freeString(charItem);
	return stringItem;
}

std::string cppJSON::print() {
	return print(value);
}
/*****************************************************************************/
