/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/dialog/LoginDialog.cpp#1 $

/*
 *  Created by Aaron Gibson on 4/1/12.
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/LoginDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/network/TrustedServer.hpp"
#include "com/haathi/utils/ExtensionHelper.hpp"
#include "com/haathi/utils/Serialization.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include <iostream>
#include <fstream>
/******************************************************************************
Define
******************************************************************************/
#define INITIAL_USERNAME 			"< User >"
#define CONFIGURATION_FILENAME 		"/config/ServerInformation.txt"
//#define CONFIGURATION_FILENAME 		"/config/TrustedServerList.txt"
#define ADD_SERVER_LISTBOX_OPTION	"Other..."
/******************************************************************************
Using
******************************************************************************/
using namespace com::sun::star::uno;
using namespace com::haathi::network;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
PingButtonListener
******************************************************************************/
class PingButtonListener: public ActionListenerHelper {
	protected:
		LoginDialog* dialog;
	public:
		PingButtonListener(LoginDialog* _dialog):
				dialog(_dialog) {
		}
		virtual ~PingButtonListener() {
		}

		virtual void SAL_CALL actionPerformed(const ActionEvent& event)
				throw (RuntimeException) {
			dialog->ping();
		}

		virtual void SAL_CALL disposing(const EventObject& object)
				throw (RuntimeException) {
		}
};
/******************************************************************************
AuthenticateListener
******************************************************************************/
class AuthenticateListener: public ActionListenerHelper {
	protected:
		LoginDialog* dialog;
		sal_Int16 page;
	public:
		AuthenticateListener(LoginDialog* _dialog, sal_Int16 _page):
				dialog(_dialog), page(_page) {
		}
		virtual ~AuthenticateListener() {
		}

		virtual void SAL_CALL actionPerformed(const ActionEvent& event)
				throw (RuntimeException) {
			if (dialog->ping()) {
				dialog->authenticateAndLoadPage(page);
			}
		}

		virtual void SAL_CALL disposing(const EventObject& object)
				throw (RuntimeException) {
		}
};
/******************************************************************************
AddServerListener
******************************************************************************/
class AddServerListener: public ActionListenerHelper {
	protected:
		LoginDialog* dialog;
	public:
		AddServerListener(LoginDialog* _dialog):
				dialog(_dialog) {
		}
		virtual ~AddServerListener() {
		}

		virtual void SAL_CALL actionPerformed(const ActionEvent& event)
				throw (RuntimeException) {
			dialog->addServerToList();
		}

		virtual void SAL_CALL disposing(const EventObject& object)
				throw (RuntimeException) {
		}
};
/******************************************************************************
RemoveServerListener
******************************************************************************/
class RemoveServerListener: public ActionListenerHelper {
	protected:
		LoginDialog* dialog;
	public:
		RemoveServerListener(LoginDialog* _dialog):
				dialog(_dialog) {
		}
		virtual ~RemoveServerListener() {
		}

		virtual void SAL_CALL actionPerformed(const ActionEvent& event)
				throw (RuntimeException) {
			dialog->removeServerFromList();
		}

		virtual void SAL_CALL disposing(const EventObject& object)
				throw (RuntimeException) {
		}
};
/******************************************************************************
ServerSelectListener
******************************************************************************/
class ServerSelectListener: public ItemListenerHelper {
	private:
		LoginDialog* dialog;
		Reference<XListBox> xListBox;
	public:
		ServerSelectListener(LoginDialog* _dialog, Reference<XListBox> _xListBox):
				dialog(_dialog), xListBox(_xListBox) {
		}
		virtual ~ServerSelectListener() {
		}

		// XEventListener
		virtual void SAL_CALL disposing(const EventObject& event)
				throw (RuntimeException) {
		}
		// XItemListener
		virtual void SAL_CALL itemStateChanged(const ItemEvent& event)
				throw (RuntimeException) {
			std::string selection = convertToString(xListBox->getSelectedItem());
			if (selection == ADD_SERVER_LISTBOX_OPTION) {
				dialog->loadPage(LOGIN_DIALOG_SETUP_SERVER_PAGE);
			}
		}
};
/******************************************************************************
Class
******************************************************************************/
void LoginDialog::display(Reference<XComponentContext> xContext) {
	LoginDialog(xContext).run();
}

LoginDialog::LoginDialog(Reference<XComponentContext> _xContext,
		sal_Int16 pageAfterAuthenticate):
		BaseDialog(_xContext, sal_False),
		xContext(_xContext) {
	// Insert the Option Page's "title" image.
	Reference<XGraphic> options_title_graphic = getGraphicFromFile("/images/option_page_title.png");
	insertImageOnPage(LOGIN_DIALOG_LOGIN_PAGE, 5, 5, 80, 175,
			OUSTRING("OptionsTitle"), options_title_graphic);

	// Properties for the buttons.
	Sequence<OUString> propNames(1);
	Sequence<Any> propValues(1);

	propNames[0] = OUSTRING("HelpText");
	propValues[0] <<= OUSTRING(
			"Selects the (trusted) server to connect to.");

	// Properties for the labels.
	Sequence<OUString> labelProps(2);
	Sequence<Any> labelVals(2);

	labelProps[0] = OUSTRING("BackgroundColor");
	labelVals[0] <<= ((com::sun::star::util::Color) 0xff000000);
	labelProps[1] = OUSTRING("TextColor");
	labelVals[1] <<= ((com::sun::star::util::Color) 0xffffffff);

	// We initialize the combo box of all available servers.
	// Eventually, we will want to load this from a configuration file somewhere...
	Sequence<OUString> list = getServerList(sal_True);
	Sequence<OUString> listboxProps(2);
	Sequence<Any> listboxVals(2);
	listboxProps[0] = OUSTRING("HelpText");
	listboxVals[0] <<= OUSTRING(
			"Select the (trusted) server to connect to.");
	listboxProps[1] = OUSTRING("Dropdown");
	listboxVals[1] <<= sal_True;

	available_server_field = insertListBoxOnPage(LOGIN_DIALOG_LOGIN_PAGE, 95,
			120, 12, 70, OUSTRING("StartupCombobox"), list, sal_False,
			listboxProps, listboxVals);
	// Initially, select the first item.
//	available_server_field->selectItemPos(0, sal_True);
	// Only allow 5 items to be visible. If there are more options, a scrollbar should appear.
	available_server_field->setDropDownLineCount(5);
	// Add the listener to detect when the user selects "Other".
	ServerSelectListener* item_listener = new ServerSelectListener(this,
			available_server_field);
	Reference<XItemListener> xitem_listener =
			static_cast<XItemListener*>(item_listener);
	available_server_field->addItemListener(xitem_listener);

	// If possible, select the EC2 Server...
	available_server_field->selectItem(OUSTRING(EC2_SERVER_NAME),
			sal_True );

	// Initialize the other fields.
	insertFixedTextOnPage(LOGIN_DIALOG_LOGIN_PAGE, 65, 120, 30,
			OUSTRING("ServerLabel"), OUSTRING("Server:"),
			HORIZONTAL_ALIGN_LEFT, labelProps, labelVals);

	// Initialize the "Username" text label and field.
	insertFixedTextOnPage(LOGIN_DIALOG_LOGIN_PAGE, 65, 135, 30,
			OUSTRING("UsernameLabel"), OUSTRING("Username:"),
			HORIZONTAL_ALIGN_LEFT, labelProps, labelVals);
	username_field = insertTextFieldOnPage(LOGIN_DIALOG_LOGIN_PAGE, 95, 135, 70,
			OUSTRING("UsernameField"),
			OUSTRING(INITIAL_USERNAME));

	// Initialize the "Password" text label and field.
	insertFixedTextOnPage(LOGIN_DIALOG_LOGIN_PAGE, 65, 150, 30,
			OUSTRING("PasswordLabel"), OUSTRING("Password:"),
			HORIZONTAL_ALIGN_LEFT, labelProps, labelVals);
	password_field = insertPasswordFieldOnPage(LOGIN_DIALOG_LOGIN_PAGE, 95, 150,
			70, OUSTRING("PasswordField"));

	// Initialize the "Setup Server" Button
	propValues[0] <<= OUSTRING(
			"Click this to continue after selecting a server.");
	AuthenticateListener* listener = new AuthenticateListener(this,
			pageAfterAuthenticate);
	Reference<XActionListener> next_listener =
			static_cast<XActionListener*>(listener);
	insertButtonOnPage(LOGIN_DIALOG_LOGIN_PAGE, next_listener, 300 - 70,
			200 - 12, 12, 70, OUSTRING("SetupServerButton"),
			OUSTRING("Setup Server"), propNames, propValues);

	// Initialize the "Ping Server" Button
	propValues[0] <<=
			OUSTRING(
					"Click this to \"ping\" the server and see if a connection can be established.");
	PingButtonListener* olistener2 = new PingButtonListener(this);
	Reference<XActionListener> ping_listener =
			static_cast<XActionListener*>(olistener2);
	insertButtonOnPage(LOGIN_DIALOG_LOGIN_PAGE, ping_listener, 95, 200 - 12, 12,
			70, OUSTRING("PingButton"),
			OUSTRING("Ping Server"), propNames, propValues);
//	
//	// Initialize the "Back Button
//	propValues[0] <<= OUSTRING("Click this to go back to the main screen.");
//	Reference< XActionListener > back_listener = getLoadPageListener(this, PROVENANCE_DIALOG_STARTUP_PAGE);
//	insertButtonOnPage(LOGIN_DIALOG_LOGIN_PAGE, back_listener, 0, 200-12, 12, 50, 
//					   OUSTRING("OptionPageBackButton"),
//					   OUSTRING("Back"), propNames, propValues);

	// Last but not least, we initialize the "dot" that is green if we can connect to the server,
	// and red if it cannot.
	red_dot = getGraphicFromFile("/images/red_dot.png");
	green_dot = getGraphicFromFile("/images/green_dot.png");
	connection_status_dot = insertImageOnPage(LOGIN_DIALOG_LOGIN_PAGE, 95 + 70,
			120, 12, 12, OUSTRING("ConnectionStatusDotImage"),
			red_dot);

	//--------------------------------------------------------
	// Now, we initialize the "Setup Server" page.
	//--------------------------------------------------------
	insertButtonOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, ping_listener, 95,
			200 - 12, 12, 70, OUSTRING("PingButton2"),
			OUSTRING("Ping Server"), propNames, propValues);

	// Add the back button.
	propValues[0] <<= OUSTRING("Go back to Login Screen");
	insertButtonOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE,
			getLoadPageListener(this, LOGIN_DIALOG_LOGIN_PAGE), 0, 200 - 12, 12,
			70, OUSTRING("LoginPageBackButton2"),
			OUSTRING("Back"), propNames, propValues);

	// Add the "Add server..." button.
	AddServerListener* add_listener = new AddServerListener(this);
	Reference<XActionListener> xadd_listener =
			static_cast<XActionListener*>(add_listener);
	propValues[0] <<= OUSTRING(
			"Add to list of currently available servers.");
	insertButtonOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, xadd_listener, 200,
			200 - 12, 12, 50, OUSTRING("AddServerButton"),
			OUSTRING("Add"), propNames, propValues);

	// Add the "Remove server..." button.
	RemoveServerListener* remove_listener = new RemoveServerListener(this);
	Reference<XActionListener> xremove_listener =
			static_cast<XActionListener*>(remove_listener);
	propValues[0] <<= OUSTRING(
			"Remove selected item from list of currently available servers.");
	insertButtonOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, xremove_listener, 250,
			200 - 12, 12, 50, OUSTRING("RemoveServerButton"),
			OUSTRING("Remove"), propNames, propValues);

	// Add the current servers listbox.
	Sequence<OUString> current_servers_list = getServerList(sal_False );
	current_servers_listbox = insertListBoxOnPage(
			LOGIN_DIALOG_SETUP_SERVER_PAGE, 200, 100, 100 - 12, 100,
			OUSTRING("CurrentServerListboxX"), current_servers_list);

	// Add a text label for the fields below:
	insertFixedTextOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, 50, 90, 100,
			OUSTRING("ServerSetupLabelsLoginBelow"),
			OUSTRING("Fill these out to add a new Server"),
			HORIZONTAL_ALIGN_CENTER, labelProps, labelVals);
	// Add the Label field.
	insertFixedTextOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, 50, 105, 50,
			OUSTRING("ServerNicknameLabel"),
			OUSTRING("Name:"), HORIZONTAL_ALIGN_LEFT, labelProps,
			labelVals);

	propValues[0] <<= OUSTRING(
			"Enter a name for this server for future reference.");
	label_field = insertTextFieldOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, 100,
			105, 50, OUSTRING("ServerNicknameField"),
			OUSTRING(""), propNames, propValues);

	// Add the server IP field.
	insertFixedTextOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, 50, 120, 50,
			OUSTRING("ServerIPLoginLabel"),
			OUSTRING("Server IP:"), HORIZONTAL_ALIGN_LEFT, labelProps,
			labelVals);

	propValues[0] <<= OUSTRING(
			"Enter the IP address or URL for the server.");
	host_field = insertTextFieldOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, 100, 120,
			50, OUSTRING("ServerIPFieldLoginField"),
			OUSTRING("< server IP >"), propNames, propValues);

	// Add the certificate path control.
	insertFixedTextOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, 50, 135, 50,
			OUSTRING("CertificatePathLoginLabel"),
			OUSTRING("Certificate Path"), HORIZONTAL_ALIGN_LEFT,
			labelProps, labelVals);

	propValues[0] <<= OUSTRING(
			"Enter the path to the certificate file.");
	certificate_path_field = insertFileControlOnPage(
			LOGIN_DIALOG_SETUP_SERVER_PAGE, 100, 135, 50,
			OUSTRING("CertificatePathLoginField"), propNames,
			propValues);

	// Add the port number control.
	insertFixedTextOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE, 50, 150, 50,
			OUSTRING("PortNumberFieldLabel"),
			OUSTRING("Port Number:"), HORIZONTAL_ALIGN_LEFT,
			labelProps, labelVals);

	propValues[0] <<= OUSTRING(
			"Port number that the server is listening on.");
	portnumber_field = insertNumericFieldOnPage(LOGIN_DIALOG_SETUP_SERVER_PAGE,
			100, 150, 50, OUSTRING("PortNumberFieldLogin"), sal_True,
			0.0, 65535.0, 9887, 1.0, 0, propNames, propValues);

}

LoginDialog::~LoginDialog() {
	// Nothing to do?
}

bool LoginDialog::ping() {
	// Here, we test the connection. In the case of an error, we display a messagebox,
	// and then return false.
	bool result = false;
	try {
		// First, determine the server to connect to.
		// TODO -- redo this so that it is dynamic as opposed to hard-coded.
		//
		// Right now, it is just hard-coded.

		getCurrentTrustedServer();

		Any val;
		TrustedServer server(trusted_server);

		result = server.ping();
		if (!result) {
			showErrorMessageBox(OUSTRING("Could not connect."), OUSTRING("Server Not found!"));
			val <<= red_dot;
		}
		else
			val <<= green_dot;
		// Update the image indicating the status.
		connection_status_dot->setPropertyValue(OUSTRING("Graphic"), val);
	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Error!"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Exception"), e2.Message);
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"),
				OUSTRING("This exception is unrecognized!"));
	}
	return result;
}

bool LoginDialog::authenticateAndLoadPage(sal_Int16 page) {
	// Should we ping first?
	try {
		// Make sure that we are using the current server information.
		getCurrentTrustedServer();

		UserInfo user_info;
		user_info.username = convertToString(username_field->getText());
		password = convertToString(password_field->getText());

		AuthenticationRequest request(user_info.username, password);
		TrustedServer server(trusted_server);

		AuthenticationResponse response = server.authenticate(request);

		if (response.getResult()) {
			meta_info.cloud_info = response.getNetworkInfo();
			// Use the same certificate (which was never passed by the trusted server).
			meta_info.cloud_info.caCertificatePath =
					trusted_server.caCertificatePath;

			meta_info.provenance_encryption = response.getCryptoInfo();

			meta_info.user_info = response.getUserInfo();
			meta_info.user_info.username = user_info.username;

			// At this point, everything has been setup properly. We load the 
			// next page.
			loadPage(page);
			return true;
		} else {
			showErrorMessageBox(OUSTRING("Authentication Error"), OUSTRING(
					"The Username or password is incorrect."));
		}
		return false;

	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Error!"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Error!"), e2.Message);
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"),
				OUSTRING(
						"This exception is \
unrecognized! From experience, it could be a segmentation fault, or something OS-related, \
but I could be wrong. Good luck figuring this one out..."));
	}
	return false;
}

void LoginDialog::getCurrentTrustedServer() {
	// Here, we check the combo box to see what is selected. If nothing is selected, we default to 
	// the usual local server thing.
	std::string selection = convertToString(
			available_server_field->getSelectedItem());

	std::map<std::string, NetworkInfo>::iterator itr = available_servers.find(
			selection);
	if (itr == available_servers.end()) {
		showErrorMessageBox(OUSTRING("No Server Selected"),
				OUSTRING("No server has been selected."));
		return;
	}
	// Shouldn't get called?
//	if(selection == ADD_SERVER_LISTBOX_OPTION){
//		return;
//	}

	trusted_server = itr->second;
	// kwa: Change to path relative to extension
	trusted_server.caCertificatePath = getSystemFilePathToExtensionFile(trusted_server.caCertificatePath.c_str());
}

Sequence<OUString> LoginDialog::getServerList(sal_Bool withOtherOption) {
	try {
		// First, load the server list from the config file. We use Boost Serialization here.
		// kwa: Must keep ext_path around, otherwise the c_str() will be invalid!
		std::string ext_path = getSystemFilePathToExtensionFile(CONFIGURATION_FILENAME);		
		available_servers = loadNetworkInfos(ext_path);
	}
	catch (...) {
		available_servers = generateDefaultNetworkInfos();
	}

	if (withOtherOption) {
		Sequence<OUString> result(available_servers.size() + 1);
		sal_Int32 i = 0;
		std::map<std::string, NetworkInfo>::iterator itr;
		for (itr = available_servers.begin(); itr != available_servers.end();
				++itr) {
			result[i++] = convertToOUString(itr->first);
		}
		result[i] = OUSTRING(ADD_SERVER_LISTBOX_OPTION);
		return result;
	} else {
		Sequence<OUString> result(available_servers.size());
		sal_Int32 i = 0;
		std::map<std::string, NetworkInfo>::iterator itr;
		for (itr = available_servers.begin(); itr != available_servers.end();
				++itr) {
			result[i++] = convertToOUString(itr->first);
		}
		return result;
	}
	// Shouldn't get here.
}

void LoginDialog::addServerToList() {
	// Adding a server to the list is straight-forward:
	// 1.) Get the information from each field.
	// 2.) Check for conflicts (display error messages accordingly)
	// 3.) If applicable, copy over the certificate file.
	// 4.) Rewrite the server list file with the new information.
	// 5.) Update the listboxes with the new server list.

	// So, step 1.
	std::string nickname = convertToString(label_field->getText());
	std::string host = convertToString(host_field->getText());
	int portNumber = (int) portnumber_field->getValue(); // configured for zero decimal digits

	// For now, set the default certificate. You can change this later to actually 
	// copy over the given certificate as opposed to the one bundled with the
	// extension initially.

	// Generate the certificate path in the extension.
	// kwa: Keep the one relative to extension home, but needs to be copied over to different name
	// std::string certPath = getSystemFilePathToExtensionFile("/config/cert.pem");
	std::string certPath = "/config/cert.pem";

	//std::string certPath = convertToString(certificate_path_field->getString());

	// Step 2
	// Iterate through the current server list and make sure that the "nickname" doesn't match 
	// anything else currently. If it does, warn the user and abort.
	//sal_Int32 i;
	for (sal_Int16 i = 0; i < current_servers_listbox->getItemCount(); ++i) {
		OUString item = current_servers_listbox->getItem(i);
		if (nickname == convertToString(item)) {
			showErrorMessageBox(OUSTRING("Name already taken!"), OUSTRING(
					"The name for this server already exists!"));
		}
	}
	// Step 3 -- TODO
	// Right now, I just assume the default certificate bundled with the extension.

	// Step 4
	NetworkInfo temp;
	temp.host = host;
	temp.portNumber = portNumber;
	temp.caCertificatePath = certPath;

	available_servers[nickname] = temp;

	// Step 5 is handled in this update function.
	updateAvailableServerList();
}

void LoginDialog::removeServerFromList() {
	// Get the currently selected entry.
	std::string item = convertToString(
			current_servers_listbox->getSelectedItem());

	// Remove the entry.
	std::map<std::string, NetworkInfo>::iterator itr = available_servers.find(
			item);

	available_servers.erase(itr);

	// Now, rewrite the configuration file.
	updateAvailableServerList();
}

void LoginDialog::updateAvailableServerList() {
	try {
		// kwa: Must keep filestring around, otherwise the c_str() will be invalid!
		std::string filestring = getSystemFilePathToExtensionFile(CONFIGURATION_FILENAME);
		
		saveNetworkInfos(filestring, available_servers);
	} catch (std::exception & e1) {
		// Do stuff.
		showErrorMessageBox(
				OUSTRING("Configuration File could not be written!"),
				convertToOUString(e1.what()));
	}
	// First, the current_servers listbox.
	current_servers_listbox->removeItems(0,
			current_servers_listbox->getItemCount());
	current_servers_listbox->addItems(getServerList(sal_False ), 0);

	// Then, the start screen listbox.
	available_server_field->removeItems(0,
			available_server_field->getItemCount());
	available_server_field->addItems(getServerList(sal_True ), 0);
	available_server_field->selectItem(OUSTRING(EC2_SERVER_NAME),
			sal_True );
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
