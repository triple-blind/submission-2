/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/leptonicaTest/leptonicaTest.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#ifdef _MSC_VER
#	pragma warning(disable: 4305)
#endif
#include <leptonica/allheaders.h> // just about everything
#include <string.h> // strncmp, strlen
/******************************************************************************
Content
******************************************************************************/
bool leptonicaTest(int argc, char* argv[]) {
	char *leptonicaVersion = getLeptonicaVersion();
	char *libraryVersions = getImagelibVersions();
	const char *expected1 = "leptonica-1.68";
	const char *expected2 = "leptonica-1.69";

    printf("liblept version:\n%s\n", leptonicaVersion);
    printf("image library versions:\n%s", libraryVersions);
	return strncmp(expected1, leptonicaVersion, strlen(expected1)) &&
			strncmp(expected2, leptonicaVersion, strlen(expected2));
}
/*****************************************************************************/
