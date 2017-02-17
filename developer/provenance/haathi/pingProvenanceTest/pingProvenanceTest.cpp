/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/pingProvenanceTest/pingProvenanceTest.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout
#include "Config.hpp" // OSYS, OSYS_WIN
#include "com/haathi/network/Server.hpp" // Server
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::network;
using namespace std;
/******************************************************************************
Content
******************************************************************************/
int passfail(bool testResult) {
	std::cout << (testResult ? "Failed" : "Passed") << std::endl;
	return testResult ? 1 : 0;
}

int main(int argc, char* argv[]) {
	char *addr = argc > 1 ? argv[1] : NULL;

#if OSYS == OSYS_WIN
//	string caCertificatePath = "..\\..\\servers\\keystore\\ps-cert.pem";
	string caCertificatePath = "..\\..\\servers\\keystore\\ca-cert.pem";
	string publicKeyPath = "..\\..\\servers\\keystore\\haathi-cert.pem";
	string privateKeyPath = "..\\..\\servers\\keystore\\haathi-key.pem";
#else
//	string caCertificatePath = "../../servers/keystore/ps-cert.pem";
	string caCertificatePath = "../../servers/keystore/ca-cert.pem";
	string publicKeyPath = "../../servers/keystore/haathi-cert.pem";
	string privateKeyPath = "../../servers/keystore/haathi-key.pem";
#endif
//	NetworkInfo networkInfo(addr ? addr : "127.0.0.1", 9988, caCertificatePath, publicKeyPath, privateKeyPath);
	NetworkInfo networkInfo(addr ? addr : "128.196.203.26", 9988, caCertificatePath, publicKeyPath, privateKeyPath); // escrow.cs.arizona.edu
	Server server(networkInfo);

	return passfail(!server.ping());
}
/*****************************************************************************/
