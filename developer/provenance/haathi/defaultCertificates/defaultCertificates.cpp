/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/defaultCertificates/defaultCertificates.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout, cerr
#include <string> // string

#include "com/haathi/network/NetworkInfo.hpp" // generateDefaultNetworkInfos()
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::network;
/******************************************************************************
Main
******************************************************************************/
int main(int argc, char *argv[]) {
	std::string filename = argc > 1 ? std::string(argv[0]) : std::string("ServerInformation.txt");

	try {
		std::map<std::string, NetworkInfo> networkInfos = generateDefaultNetworkInfos();
		saveNetworkInfos(filename, networkInfos);
	}
	catch (...) {
		std::cerr << "There was a problem!" << std::endl;
		return -1;
	}
	std::cout << "Finished!" << std::endl;
	return 0;
}
/*****************************************************************************/
