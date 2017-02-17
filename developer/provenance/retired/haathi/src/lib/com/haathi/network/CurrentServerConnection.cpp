//
// CurrentServerConnection.cpp
//
// Author: Aaron Gibson
// Date: February 22, 2012
//
// This file implements all of the details for the CurrentServerConnection.
//

// The obvious include
#include "com/haathi/menu/CurrentServerConnection.hpp"

// Used to load a dialog to set the appropriate information.
#include "com/haathi/dialog/ConnectToServerDialog.hpp"

// namespaces
using namespace com::provenance::dialog;
using namespace com::provenance::network;

//=================================================
// Constructors and Such
//=================================================
CurrentServerConnection::CurrentServerConnection() {

}

CurrentServerConnection::~CurrentServerConnection() {

}

//=================================================
// Member Functions
//=================================================
NetworkInfo getCurrentServerInfo() const {
	return server_info;
}

void CurrentServerConnection::changeCurrentServer(std::string _serverIP,
		std::string _username, int _portNumber) {

}

sal_Bool changeCurrentServerWithDialog(Reference<XMultiServiceFactory> xMSF) {
	try {
		ConnectToServerDialog dialog(xMSF);
		DialogResult result = dialog.run();

		if (result == DialogResult.OK) {
			return true;
		}
		return sal_True ;
	} catch (...) {
		// Error loading dialog...
		return sal_False ;
	}
}

