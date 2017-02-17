/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/network/NetworkInfo.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/network/NetworkInfo.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <fstream>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace network {
/******************************************************************************
Class
******************************************************************************/
std::map<std::string, NetworkInfo> generateDefaultNetworkInfos() {
		std::map<std::string, NetworkInfo> networkInfos;
		NetworkInfo temp;

		// kwa: Note that there is room for a caCertificationPath but not a
		// public or private key path.  Therefore, the Provenance Server cannot
		// use client validation and it has been disabled there.  As
		// pingProvenanceTest shows, the server is capable of validating and
		// KeystoreGenerator will generate the appropriate keys.

		temp.host = "128.196.203.26";
		temp.portNumber = 9987;
		temp.caCertificatePath = "/config/ca-cert-LocalServer.pem";
		networkInfos[std::string("escrow.cs.arizona.edu")] = temp;

		temp.host = "127.0.0.1";
		temp.portNumber = 9987;
		temp.caCertificatePath = "/config/ca-cert-LocalServer.pem";
		networkInfos[std::string("localhost")] = temp;
/*
		temp.host = "50.19.223.119";
		temp.portNumber = 32132;
		// The certificate doesn't yet exist and should be assigned.
		temp.caCertificatePath = "/config/ca-cert-EC2Server.pem";
		networkInfos[std::string(EC2_SERVER_NAME)] = temp;

		temp.host = "http://cgi.cs.arizona.edu/projects/haathi";
		temp.portNumber = 32132;
		// The certificate doesn't yet exist and should be assigned.
		temp.caCertificatePath = "/config/ca-cert-CGIServer.pem";
		networkInfos[std::string("CGI Server")] = temp;
*/
		return networkInfos;
}

std::map<std::string, NetworkInfo> loadTextNetworkInfos(std::string filename) {
	std::map<std::string, NetworkInfo> networkInfos;

	std::ifstream input(filename.c_str());
	{
		boost::archive::text_iarchive archive(input);
		archive >> BOOST_SERIALIZATION_NVP(networkInfos);
	}
	return networkInfos;
}

std::map<std::string, NetworkInfo> loadXmlNetworkInfos(std::string filename) {
	std::map<std::string, NetworkInfo> networkInfos;

	std::ifstream input(filename.c_str());
	{
		boost::archive::xml_iarchive archive(input);
		archive >> BOOST_SERIALIZATION_NVP(networkInfos);
	}
	return networkInfos;
}

std::map<std::string, NetworkInfo> loadNetworkInfos(std::string filename) {
//	return loadTextNetworkInfos(filename);
	return loadXmlNetworkInfos(filename);
}

void saveTextNetworkInfos(std::string filename,
		std::map<std::string, NetworkInfo>& networkInfos) {
	// A convoluted call to actually open the configuration file for output.
	std::ofstream output(filename.c_str());
	{
		// This scope change forces the boost::archive::iarchive's constructor 
		// and destructor to be called before continuing. The destructor will
		// flush the buffer, making sure that the file is written.
		boost::archive::text_oarchive archive(output);
		archive << BOOST_SERIALIZATION_NVP(networkInfos);
	}
}

void saveXmlNetworkInfos(std::string filename,
		std::map<std::string, NetworkInfo>& networkInfos) {
	// A convoluted call to actually open the configuration file for output.
	std::ofstream output(filename.c_str());
	{
		// This scope change forces the boost::archive::iarchive's constructor 
		// and destructor to be called before continuing. The destructor will
		// flush the buffer, making sure that the file is written.
		boost::archive::xml_oarchive archive(output);
		archive << BOOST_SERIALIZATION_NVP(networkInfos);
	}
}

void saveNetworkInfos(std::string filename,
		std::map<std::string, NetworkInfo>& networkInfos) {
//	saveTextNetworkInfos(filename, networkInfos);
	saveXmlNetworkInfos(filename, networkInfos);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
