/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/catch/catch.cpp#1 $
/******************************************************************************
Define
******************************************************************************/
#define CATCH_CONFIG_RUNNER

// Unfortunately none of these help Visual Studio get color output when
// interactively debugging.  Use the command line for color :(
// For running within eclipse on Linux, the colors don't work.
#define CATCH_CONFIG_COLOUR_NONE    // completely disables all text colouring
//#define CATCH_CONFIG_COLOUR_WINDOWS // forces the Win32 console API to be used
//#define CATCH_CONFIG_COLOUR_ANSI    // forces ANSI colour codes to be used
/******************************************************************************
Include self
******************************************************************************/
#include "../../libcpp/catch/catch.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <iostream>
#include <string>
/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
Content
******************************************************************************/
int main(int argc, char* const argv[]) {
	std::string text;
	int result;

	{
		// Output buffers are cleared when scope exits and we
		// want to see it before waiting for user input below
		result = Catch::Session().run(argc, argv);
	}
#ifndef __GNUC__
	// So that output doesn't disappear from a window
	std::cout << "Press enter to continue..." << std::endl;
	// Prevent output from disappearing from Visual Studio
	std::getline(std::cin, text);
#endif
	return result;
}
/******************************************************************************
Namespace
******************************************************************************/

/*****************************************************************************/
