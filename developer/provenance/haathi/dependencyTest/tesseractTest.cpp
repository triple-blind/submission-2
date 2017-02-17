/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/dependencyTest/tesseractTest.cpp#1 $

// See https://code.google.com/p/tesseract-ocr/wiki/APIExample
/******************************************************************************
Include others
******************************************************************************/
#include <stdio.h> // printf
#include <string.h> // strcmp
#ifdef _MSC_VER
#	pragma warning(disable: 4996)
#endif
#include <tesseract/baseapi.h> // everything tesseract
/******************************************************************************
Content
******************************************************************************/
bool tesseractTest(int argc, char* argv[]) {
	const char *expected = "3.02";

	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	const char *version = api->Version();

	printf("Tesseract version: %s\n", version);
	if (strcmp(expected, version)) {
		printf("Unexpected version number.\n");
		delete api;
		return true;
	}

    if (api->Init(NULL, "eng")) {
        printf("Could not initialize tesseract.\n");
		delete api;
		return true;
    }

	delete api;
	return false;
}
/*****************************************************************************/
