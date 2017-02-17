/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/dependencyTest/leptonicaTest.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#ifdef _MSC_VER
#	pragma warning(disable: 4305 4800)
#endif
#include <leptonica/allheaders.h> // just about everything
#include <string.h> // strncmp, strlen
/******************************************************************************
Content
******************************************************************************/
bool leptonicaTest(int argc, char* argv[]) {
	char *leptonicaVersion = getLeptonicaVersion();
	char *libraryVersions = getImagelibVersions();
	const char *expected = "leptonica-1.68";

    printf("liblept version:\n%s\n", leptonicaVersion);
    printf("image library versions:\n%s", libraryVersions);
	return strncmp(expected, leptonicaVersion, strlen(expected));
}
/*****************************************************************************/
