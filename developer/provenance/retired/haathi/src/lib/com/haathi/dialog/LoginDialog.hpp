/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/dialog/LoginDialog.hpp#1 $

/*
 *  Created by Aaron Gibson on 4/1/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef _LOGIN_DIALOG_HPP
#define _LOGIN_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <string> // std::string
#include <map> // std::map

#include "com/haathi/dialog/BaseDialog.hpp" // X...Component
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/NetworkInfo.hpp"
/******************************************************************************
Define
******************************************************************************/
#define LOGIN_DIALOG_LOGIN_PAGE				1 // This is ALWAYS page 1!
#define LOGIN_DIALOG_SETUP_SERVER_PAGE		99
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
Class
******************************************************************************/
class LoginDialog: public BaseDialog {
	private:
		/*!
		 This function will update the instance variable "trusted_server" with the
		 currently selected server in available_server_listbox.
		 */
		void getCurrentTrustedServer();

		/*!
		 This function will update the list of available servers in both the
		 available_server_listbox, and current_servers_listbox with the most current
		 list as defined in the configuration file for the servers.
		 
		 This should be called after a server is added or deleted.
		 */
		void updateAvailableServerList();

		/*!
		 This function will load the available server list from the configuration file
		 for the servers. By passing "sal_True" to the withOtherOption field, the label
		 "Other..." will be appended to the end of the list.
		 This function is used to populate the available_server_listbox and the
		 current_servers_listbox GUI elements.
		 
		 @param withOtherOption -- (optional) sal_True appends "Other..." to the end of
		 the list.
		 @result -- a Sequence of server labels/identifiers to display in a GUI listbox.
		 */
		Sequence<OUString> getServerList(sal_Bool withOtherOption = sal_False );

		// We don't implement these.
		LoginDialog(const LoginDialog& other);
		LoginDialog& operator=(const LoginDialog& other);
	protected:
		Reference<XComponentContext> xContext;
		/*!
		 This stores the list of available servers in the dialog.
		 */
		std::map<std::string, com::haathi::network::NetworkInfo> available_servers;

		/*!
		 This stores the currently acquired DocumentMetaInfo for the creation
		 of provenance-enabled documents.
		 */
		com::haathi::document::DocumentMetaInfo meta_info;

		/*!
		 This stores the currently selected server.
		 */
		com::haathi::network::NetworkInfo trusted_server;

		/*!
		 This stores the password for the user.
		 (The username is a property of DocumentMetaInfo...
		 I suppose it is a bit confusing. Oh well, I thought it easier.)
		 */
		std::string password;

		// For the login page.
		/*!
		 This stores the Dropdown GUI listbox for the server.
		 */
		Reference<XListBox> available_server_field;
		/*!
		 This stores the "Ping" button.
		 */
		Reference<XButton> test_connection;

		/*!
		 This stores an image indicating the connection status with the
		 server. This is recognizable as the green, yellow, or red dot that
		 appears at login.
		 */
		Reference<XPropertySet> connection_status_dot;
		/*!
		 This stores the text field for the username in the dialog.
		 */
		Reference<XTextComponent> username_field;

		/*!
		 This stores the password field for the password in the dialog.
		 */
		Reference<XTextComponent> password_field;

		/*!
		 This stores the image for the red dot.
		 */
		Reference<XGraphic> red_dot;
		/*!
		 This stores the image for the yellow dot.
		 */
		Reference<XGraphic> yellow_dot;
		/*!
		 This stores the image for the green dot.
		 */
		Reference<XGraphic> green_dot;

		// For the setup server page.
		/*!
		 This stores the text field for the "label" or "identifier" for
		 another server to be added.
		 */
		Reference<XTextComponent> label_field;

		/*!
		 This stores the IP address (or URL?) for the new trusted server.
		 */
		Reference<XTextComponent> host_field;

		/*!
		 This stores the path to the certificate to use.
		 Note: this currently isn't used... we don't copy the certificate 
		 anywhere. We can change this later, but currently, the same certificate
		 is used for every trusted server.
		 */
		Reference<XTextComponent> certificate_path_field;

		/*!
		 This stores the field to specify the port number for the next server.
		 */
		Reference<XNumericField> portnumber_field;

		/*!
		 This stores the GUI listbox of the current servers for adding or removing.
		 */
		Reference<XListBox> current_servers_listbox;

	public:
		static void display(Reference<XComponentContext> xContext);

		/*!
		 This constructor will create the LoginDialog from the given service factory.
		 The constructor also allows for a user to pass in the next page to load after
		 a successful login. This should be used by the subclasses (ProvenanceDialog is
		 a notable subclass) to load the main page after successful authentication.
		 
		 @param xMSF -- the service factory to create the dialog with.
		 @param pageAfterAuthenticate -- the page of the dialog to load after \
		 			(successful) authentication.
		 */

		LoginDialog(Reference<XComponentContext> xContext,
				sal_Int16 pageAfterAuthenticate = 2);

		/*!
		 Standard destructor. Should we save anything before deletion (such as login 
		 information so that the user doesn't have to type this in again for each
		 operation)?
		 */
		virtual ~LoginDialog();

		/*!
		 This ping function is the generic ping that the user selects when in the start window.
		 */
		bool ping();

		/*!
		 This function will attempt to connect to the trusted server with the username and
		 password in username_field and password_field respectively. On success, the specified
		 page is loaded and the meta_info field is populated with the relevant response after
		 authentication.
		 
		 @param page -- the page to load after (successful) authentication.
		 @result -- true on successful authentication; false otherwise.
		 */
		bool authenticateAndLoadPage(sal_Int16 page);

		/*!
		 This function will remove the selected servers in "current_servers_listbox" from
		 the list of available servers.
		 */
		void removeServerFromList();

		/*!
		 This function will add the server with the information specified in the fields:
		 label_field, host_field, certificate_path_field, and portnumber_field to the
		 list of available servers.
		 */
		void addServerToList();
};
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/

