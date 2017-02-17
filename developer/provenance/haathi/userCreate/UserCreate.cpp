/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/userCreate/UserCreate.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout
#include <string>
#include "Config.hpp" // OSYS, OSYS_WIN
#include "com/haathi/network/Packet.hpp" // CreateUserRequest, CreateUserResponse, UserType
#include "com/haathi/network/TrustedServer.hpp" // TrustedServer
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::network;

using namespace std;
/******************************************************************************
Content
******************************************************************************/
int main() {
	// Note that the program will fail if this new user, developer@provenance.com already exists.
	// Add a different or delete the first one.
	/*
		The username and password need to match an entry, possibly the first and only and
		entered by hand, in the SQLite database provenance.db for the TrustedServer.
		Authentication is based on username and password with username being stored in
		the emailAddress field.  This will create a second user with username in the
		the emailAddress field and default password "provenance" from an IDataAccess.java
		implementation.
	*/
	//                         (existing) username,       password,    firstName,  lastName, emailAddress,                userTypeID            pwd
	CreateUserRequest request("developer@provenance.com","provenance","Super",    "Supper", "supersupper@provenance.com", User::ADMINISTRATOR, "newpwd");
	CreateUserResponse response;

#if OSYS == OSYS_WIN
	string caCertificatePath = "..\\..\\servers\\keystore\\ca-cert.pem";
#else
	string caCertificatePath = "../../servers/keystore/ca-cert.pem";
#endif

	NetworkInfo networkInfo("127.0.0.1", 9987, caCertificatePath);
	TrustedServer trustedServer(networkInfo);
	
	if (trustedServer.createUser(request, response))
		cout << "User created :)" << endl;
	else
		cout << "Failed :(" << endl;
	return 0;
}
/*****************************************************************************/
