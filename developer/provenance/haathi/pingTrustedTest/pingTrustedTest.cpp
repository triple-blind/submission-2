/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/pingTrustedTest/pingTrustedTest.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout
#include "Config.hpp" // OSYS, OSYS_WIN
#include "com/haathi/network/TrustedServer.hpp" // TrustedServer
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

int pingTrusted(char* addr, int extra) {
#if OSYS == OSYS_WIN
	// This is generated from ca.jks.
//	string caCertificatePath = "..\\..\\servers\\keystore\\ts-cert.pem";
	string caCertificatePath = "..\\..\\servers\\keystore\\ca-cert.pem";
//	string caCertificatePath = "..\\..\\servers\\keystore\\ca-cert.pem";
//	string caCertificatePath = "..\\..\\servers\\keystore\\trusted.pem";
#else
	//	string caCertificatePath = "../../servers/keystore/ts-cert.pem";
		string caCertificatePath = "../../servers/keystore/ca-cert.pem";
	//	string caCertificatePath = "../../servers/keystore/ca-cert.pem";
	//	string caCertificatePath = "../../servers/keystore/trusted.pem";
#endif
	NetworkInfo networkInfo(addr ? addr : "127.0.0.1", 9987, caCertificatePath); // localhost works
//	NetworkInfo networkInfo(addr ? addr : "192.168.0.10", 9987, caCertificatePath); // local machine internal works
//	NetworkInfo networkInfo(addr ? addr : "67.1.37.160", 9987, caCertificatePath); //  Keith's server
//	NetworkInfo networkInfo(addr ? addr : "128.196.203.26", 9987, caCertificatePath); // escrow.cs.arizona.edu
	TrustedServer server(networkInfo);

	return passfail(!server.ping(extra));
}

int main(int argc, char* argv[]) {
	char *addr = argc > 1 ? argv[1] : NULL;
	
	if (argc <= 2)
		pingTrusted(addr, 0);
	else
		for (int i = 0; i < 8000; i+=100) {
			// A problem has been caught with length 2700 on jdk1.7.0_07
			// Sometimes it's at 7700 or 900.  It's seldom the same.
			// javax.crypto.BadPaddingException: Padding length invalid: 199
			// javax.crypto.BadPaddingException: Padding length invalid: 200
			// According to http://www.wowza.com/forums/content.php?528-How-to-fix-intermittent-HTTP-SSL-failure-(padding-exception)
			// one can work arround this by using 1.7.0_67 or 1.8.0_20 or greater.
			// The server configuration needs to be checked!
			std::cout << i << ": ";
			pingTrusted(addr, i);
		}
}
/*****************************************************************************/
