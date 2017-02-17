/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/LoginDialog.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/LoginDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/menu/MenuConfig.hpp"
#include "com/haathi/network/TrustedServer.hpp"
#include "com/haathi/network/UserInfo.hpp"
#include "com/haathi/utils/AuthenticationCredentials.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/XDialog.hpp>
#include "sdkEnd.hpp"

#include <sstream>
/******************************************************************************
Define
******************************************************************************/
static const char* CONFIGURATION_FILENAME = "/config/ServerInformation.txt";
static const char* CONFIGURATION_SERVER = "escrow.cs.arizona.edu";
//static const char* CONFIGURATION_SERVER = "localhost";
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::network;
using namespace com::haathi::utils;

using namespace com::sun::star::awt;
using namespace com::sun::star::uno;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
Class
******************************************************************************/
const sal_Int32 LoginDialog::dlgWidth = dlgMargin + lblWidth + txtWidth + dlgMargin;
const sal_Int32 LoginDialog::dlgHeight = dlgMargin + txtHeight + txtSpace +
		txtHeight + btnMargin + btnHeight + dlgMargin;

const int LoginDialog::btnTestId = 0;
const int LoginDialog::txtEmailId = 0;
const int LoginDialog::txtPasswordId = 1;

bool LoginDialog::loggedIn = false;
OUString LoginDialog::mostRecentFirstAndLast;
DocumentMetaInfo LoginDialog::documentMetaInfo;
NetworkInfo LoginDialog::networkInfo;

void LoginDialog::display(Reference<XComponentContext> xComponentContext, OUString helpUrl) {
	LoginDialog(xComponentContext, helpUrl).run();
}

void LoginDialog::logout() {
	loggedIn = false;
	authenticationCredentials.setPassword(std::string());
	documentMetaInfo = DocumentMetaInfo();
	networkInfo = NetworkInfo();
	mostRecentFirstAndLast = emptyOUString;
}

bool LoginDialog::login() {
	try {
		documentMetaInfo = DocumentMetaInfo();
		NetworkInfo tmpNetworkInfo(loadNetworkInfo());
		TrustedServer trustedServer(tmpNetworkInfo);
		OUString email(txtEmail->getText());
		std::string password = convertToString(txtPassword->getText());
		UserInfo userInfo;
		userInfo.username = convertToString(email);

		AuthenticationRequest request(userInfo.username, password);
		AuthenticationResponse response;
		if (!trustedServer.authenticate(request, response))
			throw response;
		documentMetaInfo.cloudInfo = response.getNetworkInfo();
		// Use the same certificate (which was never passed by the trusted server).
		documentMetaInfo.cloudInfo.caCertificatePath = tmpNetworkInfo.caCertificatePath;
		documentMetaInfo.provenanceEncryption = response.getCryptoInfo();
		documentMetaInfo.userInfo = response.getUserInfo();
		documentMetaInfo.userInfo.username = userInfo.username;

		loggedIn = true;
		authenticationCredentials.setEmailAddress(email);
		authenticationCredentials.setPassword(password);
		networkInfo = tmpNetworkInfo;
		std::string sep = (documentMetaInfo.userInfo.first.length() > 0 && documentMetaInfo.userInfo.last.length() > 0) ?
				std::string(" ") : std::string();
		std::string name = documentMetaInfo.userInfo.first + sep + documentMetaInfo.userInfo.last;
		mostRecentFirstAndLast = convertToOUString(name);
		return true;
	}
	catch (ResponsePacket& responsePacket) {
		showErrorMessageBox(responsePacket);
	}
	catch (NetworkException& e1) {
		showErrorMessageBox(e1);
	}
	catch (Exception& e2) {
		showErrorMessageBox(e2);
	}
	catch (...) {
		showErrorMessageBox("Unexpected exception encountered while authenticating user!");
	}
	return false;
}

sal_Int16 LoginDialog::run() {
	bool done = false;
	sal_Int16 result;

	while (!done) {
		result = BaseDialog::run();

		if (result == 0)
			done = true;
		if (result == 1 && isOk())
			done = login();
	}
	return dialogResult = result;
}

void LoginDialog::test() {
	NetworkInfo networkInfo;

	try {
		networkInfo = loadNetworkInfo();
		TrustedServer server(networkInfo);
		bool result = server.ping();

		if (result)
			showInfoMessageBox(OUSTRING("Connection to server succeeded!"));
		else
			showErrorMessageBox(OUSTRING("Connection to server failed!"));
	}
	catch (NetworkException& networkException) {
		std::stringstream messageStream;
		messageStream << networkException.getMessage()
				<< "\nName: \"" << CONFIGURATION_SERVER
				<< "\" Address: " << networkInfo.host
				<< " Port: " << networkInfo.portNumber
				<< ".";
		NetworkException editedNetworkException(messageStream.str());
		showErrorMessageBox(editedNetworkException);
	}
	catch (Exception& exception) {
		showErrorMessageBox(exception);
	}
	catch (...) {
		showErrorMessageBox("Unexpected exception encountered while testing server!");
	}
}

void LoginDialog::onButtonClicked(int id) {
	test();
}

void LoginDialog::onTextEdited(int id) {
	enable(btnOk, isOk());
}

bool LoginDialog::isOk() {
	return true &&
			txtEmail->getText().getLength() > 0 &&
			txtPassword->getText().getLength() > 0;
}

LoginDialog::LoginDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth,
		OUSTRING("Provenance - Login"), helpUrl) {
	testListener = ButtonListener(this, btnTestId);
	emailListener = TextListener(this, txtEmailId);
	passwordListener = TextListener(this, txtPasswordId);

	int x = dlgMargin;
	int y = dlgMargin;
	int tabIndex = 0;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblEmail"), OUSTRING("E~mail"),
			sal_False, tabIndex++,
			OUSTRING("The email address for your Provenance account"));
	x += lblWidth;

	txtEmail = insertTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtEmail"), authenticationCredentials.getEmailAddress(),
			sal_True, tabIndex++);
	connectTextListener(txtEmail, &emailListener);

	x = dlgMargin;
	y += txtHeight + txtSpace;
	
	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblPassword"), OUSTRING("~Password"),
			sal_False, tabIndex++,
			OUSTRING("The password for your Provenance account"));
	x += lblWidth;

	txtPassword = insertPasswordComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtPassword"), emptyOUString,
			sal_True, tabIndex++);
	connectTextListener(txtPassword, &passwordListener);
	y += txtHeight;

	setFocus(authenticationCredentials.getEmailAddress().getLength() == 0 ? txtEmail : txtPassword);

	x = dlgMargin;
	y += btnMargin;

	insertStandardButton(x, y, btnHeight, 44, 
			OUSTRING("btnTest"), OUSTRING("~Test server"),
			sal_True, tabIndex + 2, // should be after the next two buttons
			OUSTRING("Test the network connection to the server"), testListener);

	x = dlgWidth - dlgMargin - btnWidth - btnSpace - btnWidth;

	btnOk = insertOkButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Login"));
	enable(btnOk, isOk());
	x += btnWidth + btnSpace;

	insertCancelButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Cancel the login"));
}

LoginDialog::~LoginDialog() {
}

NetworkInfo LoginDialog::loadNetworkInfo() {
	NetworkInfo networkInfo;

	try {
		std::string path = getSystemFilePathToExtensionFile(CONFIGURATION_FILENAME);		
		std::map<std::string, NetworkInfo> servers = loadNetworkInfos(path);
		networkInfo = servers[std::string(CONFIGURATION_SERVER)];
	}
	catch (...) {
		std::map<std::string, NetworkInfo> servers = generateDefaultNetworkInfos();
		networkInfo = servers[std::string(CONFIGURATION_SERVER)];
	}
	networkInfo.caCertificatePath = getSystemFilePathToExtensionFile(networkInfo.caCertificatePath.c_str());
	return networkInfo;
}

bool LoginDialog::isLoggedIn() {
	return loggedIn;
}

OUString& LoginDialog::getFirstAndLast() {
	static OUString noFirstAndLast;

	return loggedIn ? mostRecentFirstAndLast : noFirstAndLast;
}

DocumentMetaInfo LoginDialog::getDocumentMetaInfo() {
	return loggedIn ? documentMetaInfo : DocumentMetaInfo();
}

NetworkInfo& LoginDialog::getNetworkInfo() {
	static NetworkInfo noNetworkInfo;

	return loggedIn ? networkInfo : noNetworkInfo;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
