/*
 *  Created by Aaron Gibson on 2/26/12.
 */

#ifndef PROVENANCE_DIALOG_HPP
#define PROVENANCE_DIALOG_HPP

// The obvious include
#include "com/haathi/dialog/BaseDialog.hpp"
#include "com/haathi/dialog/LoginDialog.hpp"

// Other includes
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/TrustedServer.hpp"
#include "com/haathi/network/Packet.hpp"

// String functions.
#include "com/haathi/utils/StringWrapper.hpp"

// For debugging.
#include "com/haathi/utils/Debug.hpp"

// System includes
#include <map>

namespace com {
namespace haathi {
namespace dialog {

#define PROVENANCE_DIALOG_ALL_PAGES		0
#define PROVENANCE_DIALOG_STARTUP_PAGE	2
#define PROVENANCE_DIALOG_CREATE_PAGE	3
#define PROVENANCE_DIALOG_EDIT_PAGE		4
#define PROVENANCE_DIALOG_AUDIT_PAGE	5
#define PROVENANCE_DIALOG_SERVER_PAGE	6
#define PROVENANCE_DIALOG_CUSTOM_PAGE	7

class ProvenanceDialog: public LoginDialog {

private:

	//----------------------------------------------------------
	// Universal Components
	//----------------------------------------------------------

	// These are static, because we want them to be "persistent". This way, the
	// user doesn't have to type this stuff in every time. Remember, we have to 
	// declare these in the source file as well to initialize them.
//		static com::haathi::network::NetworkInfo trusted_server;
//		static std::string password;
//		static bool first_time;

	//----------------------------------------------------------
	// Startup Page Components
	//----------------------------------------------------------
	Reference<XPropertySet> current_server_label;
	Reference<XButton> edit_button;
	Reference<XButton> audit_button;
	Reference<XButton> create_button;
	Reference<XButton> server_button;

	//----------------------------------------------------------
	// Custom Server Page Components
	//----------------------------------------------------------		
	/*!
	 This is the text field that prompts for the trusted server IP information.
	 */
	Reference<XTextComponent> serverIP_field;

	/*!
	 This is the text field for the username. Note that this is the username to
	 connect to the trusted server.
	 */
//		Reference< XTextComponent > username_field;
	/*!
	 This is the text field for the certificate path.
	 */
	Reference<XTextComponent> certificate_path_field;

	/*!
	 This is the text field for the password.
	 */
//		Reference< XTextComponent > password_field;
	/*!
	 * This is the number field to type in the relevent port.
	 */
	Reference<XNumericField> port_field;

	/*!
	 This stores a reference to the image on the dialog indicating the connection status.
	 */
	Reference<XPropertySet> connection_status_image;

	/*!
	 This stores a reference to the text on the dialog below the image that displays 
	 the connection status.
	 */
	Reference<XPropertySet> connection_label;

	//----------------------------------------------------------
	// Create Document Page Components
	//----------------------------------------------------------
	/*!
	 This is the text field for the name of the to-be-created document
	 */
	Reference<XTextComponent> document_name_field;

	/*!
	 This listbox stores the type of document to create. Currently, only
	 "Writer" (Text) Documents are supported, but this will change later.
	 */
	Reference<XListBox> document_type_field;

	/*!
	 This listbox stores the list of available auditors to audit the\
		 to-be-created document.
	 */
	Reference<XListBox> auditor_listbox;

	/*!
	 This stores the list of users to allow to also edit the document.
	 */
	Reference<XListBox> user_listbox;

	// This is only used when the CreateListener is fired after clicking the
	// "Create" button. I know that it is inefficient to declare these instance
	// variables when they are only used in one place, but it is easier to do
	// this than to try and pass the arguments in the listener call, since you
	// still have to get the arguments to actually pass in. Whatever.
	/*!
	 This stores the list of users that are selected to create the document.
	 */
	std::vector<com::haathi::network::User> user_list;

	/*!
	 This stores the list of auditors that are selected for the to-be-created
	 document
	 */
	std::vector<com::haathi::network::User> auditor_list;
//		std::map< std::string, int > auditor_map;
//		std::map< std::string, int > user_map;
//		std::vector< Reference< XPropertySet > > user_labels;
//		std::vector< Reference< XCheckBox > > edit_checkboxes;
//		std::vector< Reference< XCheckBox > > audit_checkboxes;		

	//----------------------------------------------------------
	// Load Document Page Components
	//----------------------------------------------------------
	/*!
	 This stores the GUI listbox of available users for the document.
	 */
	Reference<XListBox> available_document_listbox;
	/*!
	 This stores the document ID/name pairs that are used to load the document
	 for editing.
	 */
	std::map<int, std::string> document_load_map;
	/*!
	 This stores the auditor/key pairs with which to encrypt the document and
	 its provenance.
	 */
	std::map<int, com::haathi::crypto::CryptoInfo> document_load_keys_map;

	//----------------------------------------------------------
	// Audit Document Page Components
	//----------------------------------------------------------
	/*!
	 This stores the GUI listbox of the documents that are available
	 to audit.
	 */
	Reference<XListBox> audit_document_listbox;
	/*!
	 This stores the auditor ID/name pairs. The auditor ID is used to interface
	 with the servers (it is a unique ID), whereas the name is something that
	 a user can understand.
	 */
	std::map<int, std::string> audit_load_map;

	/*!
	 This stores the auditor ID/key pairs for the decryption of the
	 provenance chains.
	 */
	std::map<int, com::haathi::crypto::CryptoInfo> audit_load_keys_map;

	//----------------------------------------------------------
	// Initialization Functions
	//----------------------------------------------------------

	// These functions are called in the constructor of ProvenanceDialog.
	// They are added here to split up the actual initialization of the
	// dialog into the relevant pages, since each page has quite a few
	// components and it can be hard to find things.
	void initializeStartupPage();
	void initializeCreateDocumentPage();
	void initializeLoadDocumentPage();
	void initializeAuditDocumentPage();
	void initializeServerPage();
	void initializeCustomServerPage();

	//----------------------------------------------------------
	// Helper Functions
	//----------------------------------------------------------

	/*!
	 This function is a helper function that requests the list of available users
	 of the given type from the trusted server. It returns a Sequence of OUString, which
	 is what is required to render the GUI listboxes.
	 
	 @param auditor -- the type of user to query for.
	 @result -- A sequence of the names of the available users.
	 */
	Sequence<OUString> getAvailableUsersOfType(
			com::haathi::network::UserType auditor);

	// Don't implement these quite yet..>?
	// I see no reason to implement these functions.
	ProvenanceDialog& operator=(const ProvenanceDialog& other);
	ProvenanceDialog(const ProvenanceDialog& other);

public:
	static void display(Reference<XComponentContext> xContext);

	ProvenanceDialog(Reference<XComponentContext> xContext);

	virtual ~ProvenanceDialog();

	// Some getters and setters.

	// Some other member functions
	/*!
	 This function is a helper function that will get the available documents
	 for audit (to display in a GUI listbox).
	 
	 @result -- A sequence of the document names available for audit.
	 */
	Sequence<OUString> getAuditableDocuments();

	/*!
	 This function is a helper function that will get the available documents
	 for editing (to display in a GUI listbox).
	 
	 @result -- A sequence of the document names available for edit.
	 */
	Sequence<OUString> getAvailableDocuments();

	/*!
	 This function will run through the process of creating a document and return
	 true when the document is created. If there was a problem, it will return false.
	 
	 Note that this function will cause the dialog to display error messageboxes when
	 the document couldn't be created to indicate the problem.
	 
	 There are some pre-requisites for this function:
	 1.) User must be logged in via the stuff in the superclass "LoginDialog"
	 2.) User must select the type of document to create.
	 3.) User must pass in a name for the created document.
	 4.) User must select auditor(s) to audit the document.
	 5.) (optional) User can select additional users that are allowed to edit.
	 
	 These pre-requisites are all handled in the "Create" page of the dialog.
	 
	 @result -- true on successful creation; false otherwise.
	 */
	bool createProvenanceDocument();

	/*!
	 This function will run through the process of loading a document from the
	 server and will return true when it is created. If there was a problem, it will
	 return false, although not before displaying an error messagebox indicating the
	 problem. 
	 
	 As in createProvenanceDocument(), there are some pre-requisites:
	 1.) User must be logged in via the stuff in the superclass "LoginDialog".
	 2.) User must select a document to load. 
	 
	 The "Load" page of the dialog handles the pre-requisites, but the listbox displaying
	 the available documents must be refreshed with the available documents. This is 
	 handled by the refreshLoadDocumentPage() function.
	 
	 @result -- true on successful loading; false otherwise.
	 */
	bool loadProvenanceDocument();

	/*!
	 This function will run through the process of loadinfg a document AND its 
	 provenance chain from the cloud. It will then initialize the tools to decrypt
	 the document and the provenance chain and display them in the auditor. In this
	 case, the auditor is the class: EncryptedProvenanceAuditor.
	 
	 As in loadProvenanceDocument() and createProvenanceDocument(), there are some
	 pre-requisites:
	 1.) User must be logged in via the stuff in the superclass "LoginDialog".
	 2.) User must select the document to audit.
	 
	 The "Audit" page of the dialog handles these pre-requisites, but the listbox
	 displaying the auditable documents must be refreshed accordingly. This is
	 handled by the refreshAuditDocumentPage() function.
	 
	 @result true if the auditor loaded; false otherwise.
	 */
	bool auditProvenanceDocument();

	/*!
	 This function will refresh the GUI listbox in the "Create" page of the dialog
	 to display the currently available users and auditors.
	 */
	void refreshCreateDocumentPage();

	/*!
	 This function will refresh the GUI listbox in the "Load" page of the dialog
	 to display the currently available documents to edit.
	 */
	void refreshLoadDocumentPage();

	/*!
	 This function will refresh the GUI listbox in the "Audit" page of the dialog
	 to display the currently available documents to audit.
	 */
	void refreshAuditDocumentPage();
};

}
;
}
;
}
;
// namespace com::haathi::dialog

#endif
