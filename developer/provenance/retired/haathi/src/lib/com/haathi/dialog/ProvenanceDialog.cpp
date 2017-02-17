// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/dialog/ProvenanceDialog.cpp#1 $

/*
 *  Created by Aaron Gibson on 2/26/12.
 */

// Include this first.
#include "com/haathi/crypto/ProvenanceHelper.hpp"

// The obvious include
#include "com/haathi/dialog/ProvenanceDialog.hpp"

// UNO includes
// In com::sun::star::awt
#include <com/sun/star/awt/ImageAlign.hpp>
// In com::sun::star::util
#include <com/sun/star/util/Color.hpp>

// Other includes for proper implementation
// Document loading stuff
#include "com/haathi/auditor/EncryptedProvenanceAuditor.hpp"
#include "com/haathi/document/FileProvenanceDocument.hpp"
#include "com/haathi/document/CloudProvenanceDocument.hpp"
#include "com/haathi/document/SimulatedProvenanceDocument.hpp"
// Network stuff.
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/network/NetworkException.hpp"
#include "com/haathi/network/Packet.hpp"
#include "com/haathi/network/TrustedServer.hpp"
#include "com/haathi/network/CloudServer.hpp"
// Other helper stuff.
#include "com/haathi/utils/ExtensionHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

using namespace com::haathi::auditor;
using namespace com::haathi::crypto;
using namespace com::haathi::document;
using namespace com::haathi::network;
using namespace com::haathi::utils;

namespace com {
namespace haathi {
namespace dialog {

//----------------------------------------------------------
// Helper Classes
//----------------------------------------------------------
namespace {
class CreateButtonListener: public LoadPageListener {
protected:
	ProvenanceDialog* dialog;
public:
	CreateButtonListener(ProvenanceDialog* _dialog) :
			LoadPageListener(_dialog, LOGIN_DIALOG_LOGIN_PAGE), dialog(_dialog) {
	}
	virtual ~CreateButtonListener() {
	}

	virtual void SAL_CALL actionPerformed(const ActionEvent& event)
			throw (RuntimeException) {
		// Create the new document and close the dialog (if successful).
		if (dialog->createProvenanceDocument()) {
			dialog->setReturnValue(0);
			dialog->close();
			return;
		}
		dialog->setReturnValue(1);
	}
};
// class CreateButtonListener

class LoadButtonListener: public LoadPageListener {
protected:
	ProvenanceDialog* dialog;
public:
	LoadButtonListener(ProvenanceDialog* _dialog) :
			LoadPageListener(_dialog, LOGIN_DIALOG_LOGIN_PAGE), dialog(_dialog) {
	}
	virtual ~LoadButtonListener() {
	}

	virtual void SAL_CALL actionPerformed(const ActionEvent& event)
			throw (RuntimeException) {
		if (dialog->loadProvenanceDocument()) {
			dialog->setReturnValue(0);
			dialog->close();
			return;
		}
		dialog->setReturnValue(1);
	}
};
// class Load ButtonListener

class AuditButtonListener: public ActionListenerHelper {
protected:
	ProvenanceDialog* dialog;
public:
	AuditButtonListener(ProvenanceDialog* _dialog) :
			dialog(_dialog) {
	}
	virtual ~AuditButtonListener() {
	}

	virtual void SAL_CALL actionPerformed(const ActionEvent& event)
			throw (RuntimeException) {
		if (dialog->auditProvenanceDocument()) {
			dialog->setReturnValue(0);
			dialog->close();
			return;
		}
		dialog->setReturnValue(1);
	}

	virtual void SAL_CALL disposing(const EventObject& object) throw (RuntimeException) {
	}

};
// class AuditButtonListener

class RefreshAuditPageListener: public ActionListenerHelper {
protected:
	ProvenanceDialog* dialog;
public:
	RefreshAuditPageListener(ProvenanceDialog* _dialog) :
			dialog(_dialog) {
	}
	virtual ~RefreshAuditPageListener() {
	}

	virtual void SAL_CALL actionPerformed(const ActionEvent& event)
			throw (RuntimeException) {
		dialog->refreshAuditDocumentPage();
		dialog->loadPage(PROVENANCE_DIALOG_AUDIT_PAGE);
	}
	virtual void SAL_CALL disposing(const EventObject& object) throw (RuntimeException) {
	}

};
// class RefreshAuditPageListener	

class RefreshLoadDocumentPageListener: public ActionListenerHelper {
private:
	ProvenanceDialog* dialog;
public:
	RefreshLoadDocumentPageListener(ProvenanceDialog* _dialog) :
			dialog(_dialog) {
	}
	virtual ~RefreshLoadDocumentPageListener() {
	}

	virtual void SAL_CALL actionPerformed(const ActionEvent& event)
			throw (RuntimeException) {
		dialog->refreshLoadDocumentPage();
		dialog->loadPage(PROVENANCE_DIALOG_EDIT_PAGE);
	}

	virtual void SAL_CALL disposing(const EventObject& object) throw (RuntimeException) {
	}
};

class RefreshCreateDocumentPageListener: public ActionListenerHelper {
private:
	ProvenanceDialog* dialog;
public:
	RefreshCreateDocumentPageListener(ProvenanceDialog* _dialog) :
			dialog(_dialog) {
	}
	virtual ~RefreshCreateDocumentPageListener() {
	}

	virtual void SAL_CALL actionPerformed(const ActionEvent& event)
			throw (RuntimeException) {
		dialog->refreshCreateDocumentPage();
		dialog->loadPage(PROVENANCE_DIALOG_CREATE_PAGE);
	}

	virtual void SAL_CALL disposing(const EventObject& object) throw (RuntimeException) {
	}
};

}
;
// namespace listener
//----------------------------------------------------------
// Constructors and Destructor
//----------------------------------------------------------
void ProvenanceDialog::display(Reference<XComponentContext> xContext) {
	ProvenanceDialog(xContext).run();
}

// Constructor
ProvenanceDialog::ProvenanceDialog(Reference<XComponentContext> xContext) :
		LoginDialog(xContext, PROVENANCE_DIALOG_STARTUP_PAGE) {
	setWindowProperties(50, 50, 200, 300, OUSTRING("Provenance"));
	// Set the background.
	Any val;
	val <<= getGraphicFromFile("/images/background.jpg");
	windowProps->setPropertyValue(OUSTRING("Graphic"), val);

	// Initialize each page. Note as a possible optimization, we could load these pages
	// on demand... I don't really care about efficiency right now, but just a 
	// thought.
	initializeStartupPage();
	initializeCreateDocumentPage();
	initializeLoadDocumentPage();
	initializeAuditDocumentPage();
	initializeServerPage();
	initializeCustomServerPage();
	// Initialized all of the pages

	// Load the Server Page first.
	loadPage(LOGIN_DIALOG_LOGIN_PAGE);

	// Repaint the dialog to make sure that everything actually renders.
	repaint();

	// Added for testing
	/*
	 showErrorMessageBox(OUSTRING("Welcome Exception"),
	 OUSTRING("This exception is \
unrecognized! From experience, this is for version 2."));
	 */
}
/*
ProvenanceDialog::ProvenanceDialog(Reference<XComponentContext> rContext) :
		LoginDialog(rContext, PROVENANCE_DIALOG_STARTUP_PAGE) {
	setWindowProperties(50, 50, 200, 300, OUSTRING("Provenance"));
	// Set the background.
	Any val;
	val <<= getGraphicFromFile(OUSTRING("/images/background.jpg"));
	windowProps->setPropertyValue(OUSTRING("Graphic"), val);

	// Initialize each page. Note as a possible optimization, we could load these pages
	// on demand... I don't really care about efficiency right now, but just a 
	// thought.
	initializeStartupPage();
	initializeCreateDocumentPage();
	initializeLoadDocumentPage();
	initializeAuditDocumentPage();
	initializeServerPage();
	initializeCustomServerPage();
	// Initialized all of the pages

	// Load the Server Page first.
	loadPage(LOGIN_DIALOG_LOGIN_PAGE);

	// Repaint the dialog to make sure that everything actually renders.
	repaint();

	// Added for testing
	//
	 //showErrorMessageBox(OUSTRING("Welcome Exception"),
	 //OUSTRING("This exception is \
//unrecognized! From experience, this is for version 2."));
	 //
}
*/
// Destructor
ProvenanceDialog::~ProvenanceDialog() {
	// Nothing to do?
}

//----------------------------------------------------------
// initializeStartupPage()
//----------------------------------------------------------
void ProvenanceDialog::initializeStartupPage() {
	Sequence<OUString> labelProps(2);
	Sequence<Any> labelVals(2);
	labelProps[0] = OUSTRING("BackgroundColor");
	labelVals[0] <<= (sal_Int32) 0xff000000;
	labelProps[1] = OUSTRING("TextColor");
	labelVals[1] <<= ((com::sun::star::util::Color) 0x00ffffff);

	// Set the TextLabel indicating the currently selected server.
	current_server_label = insertFixedTextOnPage(PROVENANCE_DIALOG_STARTUP_PAGE,
			0, 88, 300, OUSTRING("StartupServerLabel"),
			OUSTRING("Current Trusted Server: Local Server"),
			HORIZONTAL_ALIGN_LEFT, labelProps, labelVals);

	// Initialize the "Title" for this page.
	Reference<XGraphic> startup_title_graphic = getGraphicFromFile(
			"/images/startup_page_title.png");
	insertImageOnPage(PROVENANCE_DIALOG_STARTUP_PAGE, 0, 5, 80, 175,
			OUSTRING("StartupTitle"), startup_title_graphic);

	// Initialize each of the buttons.
	// Create the listener.
	// Note that "getLoadPageListener()" is a static function. I omit the BaseDialog:: scope 
	// for readability (granted that striving for this goal in OpenOffice is in vain anyway).

	Sequence<OUString> helpProps(3);
	Sequence<Any> helpVals(3);

	helpProps[0] = OUSTRING("Graphic");
	helpVals[0] <<= getGraphicFromFile(
			"/images/create_image.png");
	helpProps[1] = OUSTRING("HelpText");
	helpVals[1] <<= OUSTRING(
			"Click this to create a provenance document.");
	helpProps[2] = OUSTRING("ImageAlign");
	helpVals[2] <<= ImageAlign::LEFT;

	// Create Button
	RefreshCreateDocumentPageListener* create_listener =
			new RefreshCreateDocumentPageListener(this);
	Reference<XActionListener> xcreate_listener =
			static_cast<XActionListener*>(create_listener);
	create_button = insertButtonOnPage(PROVENANCE_DIALOG_STARTUP_PAGE,
			xcreate_listener, 0, 100, 50, 150,
			OUSTRING("CreateProvenanceButton"),
			OUSTRING("Create Provenance Document"), helpProps,
			helpVals);

	// Audit Button
	RefreshAuditPageListener* audit_page_listener =
			new RefreshAuditPageListener(this);
	Reference<XActionListener> xaudit_listener =
			static_cast<XActionListener*>(audit_page_listener);
	helpVals[0] <<= getGraphicFromFile(
			"/images/audit_image.png");
	helpVals[1] <<= OUSTRING(
			"Click this to audit a provenance document");
	audit_button = insertButtonOnPage(PROVENANCE_DIALOG_STARTUP_PAGE,
			xaudit_listener, 0, 150, 50, 150, OUSTRING("AuditButton"),
			OUSTRING("Audit Provenance Document"), helpProps,
			helpVals);
	// Edit Button
	RefreshLoadDocumentPageListener* edit_listener =
			new RefreshLoadDocumentPageListener(this);
	Reference<XActionListener> xedit_listener =
			static_cast<XActionListener*>(edit_listener);
	helpVals[0] <<= getGraphicFromFile(
			"/images/edit_image.png");
	helpVals[1] <<= OUSTRING(
			"Click this to edit an already existing provenance document");
	edit_button = insertButtonOnPage(PROVENANCE_DIALOG_STARTUP_PAGE,
			xedit_listener, 150, 100, 50, 150, OUSTRING("EditButton"),
			OUSTRING("Edit Provenance Document"), helpProps, helpVals);

	// Server Settings Button
	Reference<XActionListener> xoptions_listener = getLoadPageListener(this,
			LOGIN_DIALOG_LOGIN_PAGE);
	helpVals[0] <<= getGraphicFromFile(
			"/images/server_image.png");
	helpVals[1] <<= OUSTRING(
			"Click this to change the server settings.");
	server_button = insertButtonOnPage(PROVENANCE_DIALOG_STARTUP_PAGE,
			xoptions_listener, 150, 150, 50, 150,
			OUSTRING("ServerButton"),
			OUSTRING("Connect to Different Server"), helpProps,
			helpVals);
}

//----------------------------------------------------------
// initializeCreateDocumentPage()
//----------------------------------------------------------
void ProvenanceDialog::initializeCreateDocumentPage() {
	// Insert this page's "title" image.
	Reference<XGraphic> create_title_graphic = getGraphicFromFile(
			"/images/create_page_title.png");
	insertImageOnPage(PROVENANCE_DIALOG_CREATE_PAGE, 5, 5, 80, 175,
			OUSTRING("CreatePageTitle"), create_title_graphic);

	// Setup some custom properties for the soon-to-be-created buttons.
	Sequence<OUString> helpProps(1);
	Sequence<Any> helpVals(1);
	helpProps[0] = OUSTRING("HelpText");

	// Set some properties for the Text Labels.
	Sequence<OUString> labelProps(2);
	Sequence<Any> labelVals(2);

	labelProps[0] = OUSTRING("BackgroundColor");
	labelVals[0] <<= ((com::sun::star::util::Color) 0xff000000);
	labelProps[1] = OUSTRING("TextColor");
	labelVals[1] <<= ((com::sun::star::util::Color) 0x00ffffff);

	// Insert the "Document Name" label
	insertFixedTextOnPage(PROVENANCE_DIALOG_CREATE_PAGE, 0, 130, 50,
			OUSTRING("CreateDocLabel"),
			OUSTRING("Document Name"), HORIZONTAL_ALIGN_CENTER,
			labelProps, labelVals);

	document_name_field = insertTextFieldOnPage(PROVENANCE_DIALOG_CREATE_PAGE,
			50, 130, 50, OUSTRING("CreateDocNameField"),
			OUSTRING("ProvDoc"));

	// Insert the "Document Type" label
//	insertFixedTextOnPage(PROVENANCE_DIALOG_CREATE_PAGE, 0, 145, 50, 
//		OUSTRING("CreateDocTypeField"), OUSTRING("Document Type"), 
//		HORIZONTAL_ALIGN_CENTER, labelProps, labelVals);

	// Insert the document-type ComboBox.
	Sequence<OUString> allowed_types(1);
	Sequence<OUString> document_typeProps(2);
	Sequence<Any> document_typeVals(2);
	document_typeProps[0] = OUSTRING("HelpText");
	document_typeVals[0] <<= OUSTRING(
			"Select the Document Type here.");
	document_typeProps[1] = OUSTRING("Dropdown");
	document_typeVals[1] <<= sal_True;
	allowed_types[0] = OUSTRING("Text Document (Writer)");
	document_type_field = insertListBoxOnPage(PROVENANCE_DIALOG_CREATE_PAGE, 0,
			145, 12, 100, OUSTRING("CreateDocTypeComboBox"),
			allowed_types, sal_False, document_typeProps, document_typeVals);
	document_type_field->selectItemPos(0, sal_True );

	// Insert the auditor-listbox label
	insertFixedTextOnPage(PROVENANCE_DIALOG_CREATE_PAGE, 200, 90, 100,
			OUSTRING("CreateDocAuditorLabel"),
			OUSTRING("Select Auditors"), HORIZONTAL_ALIGN_CENTER,
			labelProps, labelVals);

	// Get the list of available auditors. This is handled by a helper function.
//	Sequence< OUString > available_auditors = getAvailableUsersOfType(true);
	Sequence<OUString> available_auditors;

	// Finally, add the list-box itself with the required items.
	helpVals[0] <<= OUSTRING(
			"Select the auditors for the document here.");
	auditor_listbox = insertListBoxOnPage(PROVENANCE_DIALOG_CREATE_PAGE, 200,
			102, 80, 100, OUSTRING("CreateDocAuditorListbox"),
			available_auditors, sal_True, helpProps, helpVals);

	Sequence<OUString> available_users;
	insertFixedTextOnPage(PROVENANCE_DIALOG_CREATE_PAGE, 100, 90, 100,
			OUSTRING("CreateDocUsersLabel"),
			OUSTRING("Select Additional Editors"), HORIZONTAL_ALIGN_CENTER,
			labelProps, labelVals);

	// Finally, add the list-box itself with the required items.
	helpVals[0] <<= OUSTRING(
			"Select the users for the document here.");
	user_listbox = insertListBoxOnPage(PROVENANCE_DIALOG_CREATE_PAGE, 100, 102,
			80, 100, OUSTRING("CreateDocUserListbox"), available_users,
			sal_True, helpProps, helpVals);

	// Insert the "back" button.
	Reference<XActionListener> back_listener = getLoadPageListener(this,
			PROVENANCE_DIALOG_STARTUP_PAGE);
	helpVals[0] <<= OUSTRING(
			"Click this to go back to the starting screen.");
	insertButtonOnPage(PROVENANCE_DIALOG_CREATE_PAGE, back_listener, 0,
			200 - 12, 12, 50, OUSTRING("CreateDocBackButton"),
			OUSTRING("Back"), helpProps, helpVals);

	// Insert the "Change Server..." button.
	Reference<XActionListener> server_listener = getLoadPageListener(this,
			LOGIN_DIALOG_LOGIN_PAGE);
	helpVals[0] <<= OUSTRING(
			"Click this to change the server settings.");
	insertButtonOnPage(PROVENANCE_DIALOG_CREATE_PAGE, server_listener, 60,
			200 - 12, 50, OUSTRING("CreateDocServerButton"),
			OUSTRING("Change Server..."), helpProps, helpVals);

	// Insert the "Create" button.
	CreateButtonListener* clistener = new CreateButtonListener(this);
	Reference<XActionListener> create_listener =
			static_cast<XActionListener*>(clistener);
	helpVals[0] <<= OUSTRING(
			"Click this to create the Provenance-enabled Document.");
	insertButtonOnPage(PROVENANCE_DIALOG_CREATE_PAGE, create_listener, 250,
			200 - 12, 50, OUSTRING("CreateDocCreateButton"),
			OUSTRING("Create"), helpProps, helpVals);

	// Insert the "Help" button. (TODO)
}

//----------------------------------------------------------
// initializeLoadDocumentPage()
//----------------------------------------------------------
void ProvenanceDialog::initializeLoadDocumentPage() {
	// Place the LoadDocument Page's title image.
	Reference<XGraphic> load_document_graphic = getGraphicFromFile(
			"/images/load_page_title.png");
	insertImageOnPage(PROVENANCE_DIALOG_EDIT_PAGE, 5, 5, 80, 175,
			OUSTRING("LoadPageTitle"), load_document_graphic);

	// Setup some custom properties for the soon-to-be-created buttons.
	Sequence<OUString> helpProps(1);
	Sequence<Any> helpVals(1);
	helpProps[0] = OUSTRING("HelpText");

	// Set some properties for the Text Labels.
	Sequence<OUString> labelProps(2);
	Sequence<Any> labelVals(2);

	labelProps[0] = OUSTRING("BackgroundColor");
	labelVals[0] <<= ((com::sun::star::util::Color) 0xff000000);
	labelProps[1] = OUSTRING("TextColor");
	labelVals[1] <<= ((com::sun::star::util::Color) 0x00ffffff);

	// Insert the "back" button.
	Reference<XActionListener> back_listener = getLoadPageListener(this,
			PROVENANCE_DIALOG_STARTUP_PAGE);
	helpVals[0] <<= OUSTRING(
			"Click this to go back to the starting screen.");
	insertButtonOnPage(PROVENANCE_DIALOG_EDIT_PAGE, back_listener, 0, 200 - 12,
			12, 50, OUSTRING("LoadDocBackButton"),
			OUSTRING("Back"), helpProps, helpVals);

	// Insert the "Change Server..." button.
	Reference<XActionListener> server_listener = getLoadPageListener(this,
			LOGIN_DIALOG_LOGIN_PAGE);
	helpVals[0] <<= OUSTRING(
			"Click this to change the server settings.");
	insertButtonOnPage(PROVENANCE_DIALOG_EDIT_PAGE, server_listener, 60,
			200 - 12, 50, OUSTRING("LoadDocServerButton"),
			OUSTRING("Change Server..."), helpProps, helpVals);

	// Insert the "Load" button.
	LoadButtonListener* loadlistener = new LoadButtonListener(this);
	Reference<XActionListener> load_listener =
			static_cast<XActionListener*>(loadlistener);
	helpVals[0] <<= OUSTRING(
			"Click this to create the Provenance-enabled Document.");
	insertButtonOnPage(PROVENANCE_DIALOG_EDIT_PAGE, load_listener, 250,
			200 - 12, 50, OUSTRING("LoadDocCreateButton"),
			OUSTRING("Load"), helpProps, helpVals);

	// Insert the label for the list of documents.
	insertFixedTextOnPage(PROVENANCE_DIALOG_EDIT_PAGE, 0, 90, 100,
			OUSTRING("LoadDocListLabel"),
			OUSTRING("Available Documents"), HORIZONTAL_ALIGN_CENTER,
			labelProps, labelVals);

	// Get the list of available documents. We call a helper function to actually get the list.
	Sequence<OUString> list_of_documents;	// = getAvailableDocuments();

	// Insert the list of documents.
	helpVals[0] <<= OUSTRING("Select the document to edit.");
	available_document_listbox = insertListBoxOnPage(
			PROVENANCE_DIALOG_EDIT_PAGE, 0, 105, 80, 110,
			OUSTRING("LoadDocListbox"), list_of_documents, sal_False,
			helpProps, helpVals);
}

//----------------------------------------------------------
// initializeAuditDocumentPage()
//----------------------------------------------------------
void ProvenanceDialog::initializeAuditDocumentPage() {
	// Setup some custom properties for the soon-to-be-created buttons.
	Sequence<OUString> helpProps(1);
	Sequence<Any> helpVals(1);
	helpProps[0] = OUSTRING("HelpText");

	// Set some properties for the Text Labels.
	Sequence<OUString> labelProps(2);
	Sequence<Any> labelVals(2);

	// Place the LoadDocument Page's title image.
	Reference<XGraphic> audit_document_graphic = getGraphicFromFile(
			"/images/audit_page_title.png");
	insertImageOnPage(PROVENANCE_DIALOG_AUDIT_PAGE, 5, 5, 80, 175,
			OUSTRING("AuditPageTitle"), audit_document_graphic);

	labelProps[0] = OUSTRING("BackgroundColor");
	labelVals[0] <<= ((com::sun::star::util::Color) 0xff000000);
	labelProps[1] = OUSTRING("TextColor");
	labelVals[1] <<= ((com::sun::star::util::Color) 0x00ffffff);

	// Insert the "back" button.
	Reference<XActionListener> back_listener = getLoadPageListener(this,
			PROVENANCE_DIALOG_STARTUP_PAGE);
	helpVals[0] <<= OUSTRING(
			"Click this to go back to the starting screen.");
	insertButtonOnPage(PROVENANCE_DIALOG_AUDIT_PAGE, back_listener, 0, 200 - 12,
			12, 50, OUSTRING("AuditBackButton"),
			OUSTRING("Back"), helpProps, helpVals);

	// Insert the "Change Server..." button.
	Reference<XActionListener> server_listener = getLoadPageListener(this,
			LOGIN_DIALOG_LOGIN_PAGE);
	helpVals[0] <<= OUSTRING(
			"Click this to change the server settings.");
	insertButtonOnPage(PROVENANCE_DIALOG_AUDIT_PAGE, server_listener, 60,
			200 - 12, 50, OUSTRING("AuditServerButton"),
			OUSTRING("Change Server..."), helpProps, helpVals);

	// Insert the "Audit" button.
	AuditButtonListener* audit_listener = new AuditButtonListener(this);
	Reference<XActionListener> xaudit_listener =
			static_cast<XActionListener*>(audit_listener);
	helpVals[0] <<= OUSTRING(
			"Click this to create the Provenance-enabled Document.");
	insertButtonOnPage(PROVENANCE_DIALOG_AUDIT_PAGE, xaudit_listener, 250,
			200 - 12, 50, OUSTRING("AuditStartButton"),
			OUSTRING("Audit"), helpProps, helpVals);

	// Insert the label for the list of documents.
	insertFixedTextOnPage(PROVENANCE_DIALOG_AUDIT_PAGE, 0, 90, 100,
			OUSTRING("AuditListLabel"),
			OUSTRING("Available Documents"), HORIZONTAL_ALIGN_CENTER,
			labelProps, labelVals);

	// Get the list of available documents. We call a helper function to actually get the list.
	Sequence<OUString> list_of_documents;	// = getAvailableDocuments();

	// Insert the list of documents.
	helpVals[0] <<= OUSTRING("Select the document to audit.");
	audit_document_listbox = insertListBoxOnPage(PROVENANCE_DIALOG_AUDIT_PAGE,
			0, 105, 80, 100, OUSTRING("AuditListbox"),
			list_of_documents, sal_False, helpProps, helpVals);
}

//----------------------------------------------------------
// initializeServerPage()
//----------------------------------------------------------	
void ProvenanceDialog::initializeServerPage() {

}

//----------------------------------------------------------
// initializeCustomServerPage()
//----------------------------------------------------------
void ProvenanceDialog::initializeCustomServerPage() {

}

//----------------------------------------------------------
// Member Functions
//----------------------------------------------------------
void ProvenanceDialog::refreshLoadDocumentPage() {
	available_document_listbox->removeItems(0,
			available_document_listbox->getItemCount());
	available_document_listbox->addItems(getAvailableDocuments(), 0);
}

void ProvenanceDialog::refreshAuditDocumentPage() {
	audit_document_listbox->removeItems(0,
			audit_document_listbox->getItemCount());
	audit_document_listbox->addItems(getAuditableDocuments(), 0);
}

void ProvenanceDialog::refreshCreateDocumentPage() {
	try {
//		
//		TrustedServer ts(trusted_server);
//		
//		UserType auditor_type = AUDITOR;
//		GetUsersRequest request(meta_info.user_info.username, password, auditor_type);
//		
//		GetUsersResponse response = ts.getProvenanceUsers(request);
//		if(response.getResult()){
//			std::vector< User > users = response.getUserList();
//			
//			Sequence< OUString > seq(users.size());
//			size_t i;
//			for(i = 0; i < users.size(); ++i){
//				seq[i] = convertToOUString(users[i].getFullName());
//				auditor_map[users[i].getFullName()] = users[i].getUserID();
//			}
//			
		Sequence<OUString> seq = getAvailableUsersOfType(AUDITOR);
		auditor_listbox->removeItems(0, auditor_listbox->getItemCount());
		auditor_listbox->addItems(seq, 0);

		seq = getAvailableUsersOfType(USER);
		user_listbox->removeItems(0, user_listbox->getItemCount());
		user_listbox->addItems(seq, 0);

//		} else {
//			showErrorMessageBox(OUSTRING("Server Error"), 
//				OUSTRING("The server could not create the document. This is most \
//				likely a handshaking problem with the trusted server."));
//			return;
//		}

	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Exception"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Exception"), e2.Message);
	} catch (std::exception & e3) {
		showErrorMessageBox(OUSTRING("Standard Library Exception"),
				convertToOUString(e3.what()));
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"),
				OUSTRING("This exception is unrecognized!"));
	}

}

bool ProvenanceDialog::loadProvenanceDocument() {
	try {

		// First, acquire the selected documents
		std::string selected_document = convertToString(
				available_document_listbox->getSelectedItem());

		std::map<int, std::string>::iterator itr;
		int ID = -1;
		for (itr = document_load_map.begin(); itr != document_load_map.end();
				++itr) {
			if (itr->second == selected_document) {
				ID = itr->first;
				break;
			}
		}

		CloudServer cs(meta_info.cloud_info);

		LoadProvenanceDocumentRequest request(ID);

		LoadProvenanceDocumentResponse response = cs.LoadDocument(request);

		GetDocumentAuditorsRequest documentAuditorsRequest(
				meta_info.user_info.username, password, ID);
		TrustedServer ts(trusted_server);
		GetDocumentAuditorsResponse documentAuditorsResponse =
				ts.getDocumentAuditorsWithKeys(documentAuditorsRequest);

		if (response.getResult() && documentAuditorsResponse.getResult()) {
			meta_info.documentID = ID;
			meta_info.document_encryption.sessionKeySize =
					document_load_keys_map[ID].sessionKeySize;
			meta_info.document_encryption.sessionKey =
					document_load_keys_map[ID].sessionKey;
			meta_info.document_title = selected_document;

			meta_info.auditor_list = documentAuditorsResponse.getKeys();
			meta_info.previous_saved_event = response.getProvenance();

			// Here, we are decrypting the document, NOT the provenance. Thus, we use
			// the "document_encryption" field of DocumentMetaInfo.
			ProvenanceHelper helper(meta_info.document_encryption);

			std::string temp = helper.DecryptData(
					helper.Base64Decode(response.getDocument()));

//			std::cout << "Decrypted Document: " << temp << std::endl;

			NetworkInfo emptyNetworkInfo;
			CloudProvenanceDocument::loadProvenanceDocumentFromStream(xContext, temp,
					meta_info, emptyNetworkInfo, password);

			return true;
		} else {
			showErrorMessageBox(OUSTRING("Server Error"), OUSTRING(
					"The server could not create the document. "
					"This is most likely a handshaking problem between the trusted server and the cloud."));
			return false;
		}

	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Exception"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Exception"), e2.Message);
	} catch (std::exception & e3) {
		showErrorMessageBox(OUSTRING("Standard Library Exception"),
				convertToOUString(e3.what()));
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"), OUSTRING(
				"This exception is unrecognized! From experience, this is probably a "
				"segmentation fault or some sort of memory mismanagement."));
	}
	return false;
}

bool ProvenanceDialog::createProvenanceDocument() {
	bool result = false;
	try {
		// First, check if an auditor has been selected. If not, then there is no
		// point for this provenance widget! 
		if (auditor_listbox->getSelectedItemsPos().getLength() < 1) {
			showErrorMessageBox(OUSTRING("No Auditor"), OUSTRING(
					"You must select at least one auditor!"));
			return false;
		}

		TrustedServer ts(trusted_server);

		CreateDocumentRequest request(meta_info.user_info.username, password,
				meta_info.user_info.userID, convertToString(document_name_field->getText()));

		CreateDocumentResponse response = ts.createDocument(request);
		if (response.getResult()) {
			meta_info.documentID = response.getDocumentID();
			meta_info.document_encryption.sessionKeySize =
					response.getUniqueKeySize();
			meta_info.document_encryption.sessionKey = response.getUniqueKey();
			meta_info.document_title = convertToString(document_name_field->getText());
			Sequence<sal_Int16> selections =
					auditor_listbox->getSelectedItemsPos();
			std::cout << "Document ID:" << response.getDocumentID() << std::endl;
			std::cout << "DocumentKey:" << response.getUniqueKey() << std::endl;

			std::string selected_user;

			//----------------------------------------------------------
			// Add AUDITORS
			//----------------------------------------------------------

			std::vector<int> auditorIDs;
			for (int i = 0; i < selections.getLength(); ++i) {
				selected_user = convertToString(auditor_listbox->getItem(selections[i]));
				for (unsigned int j = 0; j < auditor_list.size(); ++j) {
					if (selected_user == auditor_list[j].getFullName()) {
						int ID = auditor_list[j].getUserID();
						auditorIDs.push_back(ID);
						break;
					}
				}
			}

			// Add the auditors.
			TrustedServer ts2(trusted_server);
			AddDocumentAuditorRequest request2(meta_info.user_info.username,
					password, &auditorIDs.front(), auditorIDs.size(), // kwa was auditorIDs.data()
					meta_info.documentID);
			AddDocumentAuditorResponse response = ts2.addDocumentAuditors(
					request2);
			if (response.getResult() == false) {
				showErrorMessageBox(OUSTRING("Server Error"), OUSTRING(
						"The server could not add the requested auditors."));
			}
			TrustedServer ts3(trusted_server);
			RetrievePublicKeyRequest request3(meta_info.user_info.username,
					password, &auditorIDs.front(), auditorIDs.size()); // kwa was auditorIDs.data()

			RetrievePublicKeyResponse response3 = ts3.getPublicKey(request3);
			meta_info.auditor_list = response3.getKeys();

			//----------------------------------------------------------
			// Add USERS
			//----------------------------------------------------------

			std::vector<int> userIDs;
			// kwa: Need to add the current user?  No, that implies sharing.
			// userIDs.push_back(meta_info.user_info.userID);
			selections = user_listbox->getSelectedItemsPos();
			for (int i = 0; i < selections.getLength(); ++i) {
				selected_user = convertToString(user_listbox->getItem(selections[i]));
				for (unsigned int j = 0; j < user_list.size(); ++j) {
					if (selected_user == user_list[j].getFullName()) {
						int ID = user_list[j].getUserID();
						userIDs.push_back(ID);
						break;
					}
				}
			}

			// Add the users.
			// kwa: This will crash when userIDs is empty!
			// Besides, the server will return false.  Skip it.
			if (userIDs.size() > 0) {
				TrustedServer ts4(trusted_server);
				ShareDocumentRequest request4(meta_info.user_info.username,
						password, &userIDs.front(), userIDs.size(), // kwa was userIDs.data()
						meta_info.documentID);
				ShareDocumentResponse response4 = ts4.shareDocument(request4);

				if (response4.getResult() == false) {
					showErrorMessageBox(OUSTRING("Server Error"), OUSTRING(
							"The server could not add the requested users."));
				}
			}

			// At this point, meta_info (of type DocumentMetaInfo) should contain all of
			// the necessary information to handle provenance events over the cloud. 
			// Create the document in OpenOffice.

			meta_info.document_title = convertToString(
					document_name_field->getText());

			#ifdef ENABLE_AUTO_TYPE
			SimulatedProvenanceDocument::createCloudProvenanceDocument(xContext, meta_info);
			#else
			NetworkInfo emptyNetworkInfo;
			std::string emptyPassword;
			CloudProvenanceDocument::createCloudProvenanceDocument(xContext, meta_info, emptyNetworkInfo, emptyPassword, std::vector<std::string>());
			#endif
			return true;
		} else {
			showErrorMessageBox(OUSTRING("Server Error"), OUSTRING(
					"The server could not create the document. This "
					"is most likely a handshaking problem between the trusted server and the cloud."));
			return false;
		}

	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Exception"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Exception"), e2.Message);
	} catch (std::exception & e3) {
		showErrorMessageBox(OUSTRING("Standard Library Exception"),
				convertToOUString(e3.what()));
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"), OUSTRING(
				"This exception is unrecognized! From experience, it could be a segmentation "
				"fault, or something OS-related, but I could be wrong. Good luck figuring this one out..."));
	}
	return false;
}

bool ProvenanceDialog::auditProvenanceDocument() {
	try {

		// First, acquire the selected documents
		std::string selected_document = convertToString(
				audit_document_listbox->getSelectedItem());

		std::map<int, std::string>::iterator itr;
		int ID = -1;
		for (itr = audit_load_map.begin(); itr != audit_load_map.end(); ++itr) {
			if (itr->second == selected_document) {
				ID = itr->first;
				break;
			}
		}

		CloudServer cs(meta_info.cloud_info);

		AuditDocumentRequest request(ID);

		AuditDocumentResponse response = cs.AuditDocument(request);

		if (response.getResult()) {
			meta_info.documentID = ID;
			meta_info.document_encryption.sessionKeySize =
					audit_load_keys_map[ID].sessionKeySize;
			meta_info.document_encryption.sessionKey =
					audit_load_keys_map[ID].sessionKey;

			meta_info.document_title = selected_document;

			// Here, we are decrypting the document, NOT the provenance. Thus, we use
			// the "document_encryption" field of DocumentMetaInfo.
			ProvenanceHelper helper(meta_info.document_encryption);

			std::string decrypted_document = helper.DecryptData(
					helper.Base64Decode(response.getDocument()));

//			std::cout << "Document: " << decrypted_document << std::endl;
			std::string provenance = response.getProvenance();

			EncryptedProvenanceAuditor::auditProvenanceFromStream(
					xContext, decrypted_document, provenance, meta_info);

			return true;
		} else {
			showErrorMessageBox(OUSTRING("Server Error"), OUSTRING(
					"The server could not create the document. "
					"This is most likely a handshaking problem between the trusted server and the cloud."));
			return false;
		}

	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Exception"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Exception"), e2.Message);
	} catch (std::exception & e3) {
		showErrorMessageBox(OUSTRING("Standard Library Exception"),
				convertToOUString(e3.what()));
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"), OUSTRING(
				"This exception is unrecognized! From experience, this is probably a segmentation "
				"fault or some sort of memory mismanagement."));
	}
	return false;
}

//----------------------------------------------------------
// Server Connection Member Functions
//----------------------------------------------------------
Sequence<OUString> ProvenanceDialog::getAuditableDocuments() {
	try {
		UserType user_type = AUDITOR;
		TrustedServer ts(trusted_server);

		ListDocumentsRequest request(meta_info.user_info.username, password,
				meta_info.user_info.userID, user_type);
		ListDocumentsResponse response = ts.listAvailableDocuments(request);

		if (response.getResult()) {
			audit_load_map = response.getDocuments();
			audit_load_keys_map = response.getDocumentKeys();

			// Now, load the contents of the response into a Sequence to be displayed
			// in an OpenOffice listbox.
			std::map<int, std::string>::iterator itr;
			Sequence<OUString> result(audit_load_map.size());
			int i = 0;
			for (itr = audit_load_map.begin(); itr != audit_load_map.end();
					++itr) {
				result[i++] = convertToOUString(itr->second);
			}
			return result;

		} else {
			// Display a message box?
			showErrorMessageBox(OUSTRING("Error"),
					OUSTRING("No documents currently available"));

			// Return an empty string.
			return Sequence<OUString>();
		}
	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Exception"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Exception"), e2.Message);
	} catch (std::exception & e3) {
		showErrorMessageBox(OUSTRING("Standard Library Exception"),
				convertToOUString(e3.what()));
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"),
				OUSTRING("This exception is unrecognized!"));
	}
	return Sequence<OUString>();
}

Sequence<OUString> ProvenanceDialog::getAvailableDocuments() {
	try {
		// First, check whether we have initialized the trusted server and user_info objects.
		// If not, inform the user to do so (as well as how to do so, if applicable).
		UserType user_type = USER;
		TrustedServer ts(trusted_server);
		ListDocumentsRequest request(meta_info.user_info.username, password,
				meta_info.user_info.userID, user_type);
		ListDocumentsResponse response = ts.listAvailableDocuments(request);
		if (response.getResult()) {
			document_load_map = response.getDocuments();
			document_load_keys_map = response.getDocumentKeys();
		} else {
			// Display a message box?
			showErrorMessageBox(OUSTRING("Error"),
					OUSTRING("No documents currently available"));

			// Return an empty string.
			return Sequence<OUString>();
		}

		// Now, we return a Sequence of OUStrings that OpenOffice can use to display the list.	
		std::map<int, std::string>::iterator itr;
		Sequence<OUString> result(document_load_map.size());
		int i = 0;
		for (itr = document_load_map.begin(); itr != document_load_map.end();
				++itr) {
			result[i++] = convertToOUString(itr->second);
		}
		return result;
	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Exception"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Exception"), e2.Message);
	} catch (std::exception & e3) {
		showErrorMessageBox(OUSTRING("Standard Library Exception"),
				convertToOUString(e3.what()));
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"), OUSTRING(
				"This exception is unrecognized! From experience, it could be a segmentation "
				"fault, or something OS-related, but I could be wrong. Good luck figuring this one out..."));
	}
	return Sequence<OUString>();
}

Sequence<OUString> ProvenanceDialog::getAvailableUsersOfType(UserType type) {
	try {
		// First, check whether we have initialized the trusted server and user_info objects.
		// If not, inform the user to do so (as well as how to do so, if applicable).
		TrustedServer ts(trusted_server);

		GetUsersRequest request(meta_info.user_info.username, password, type);
		GetUsersResponse response = ts.getProvenanceUsers(request);
		if (response.getResult()) {
			if (type == AUDITOR) {
				auditor_list = response.getUserList();
				std::vector<User>::iterator itr1;
//					for(itr1 = auditor_list.begin(); itr1 != auditor_list.end(); ++itr1){
//						std::cout << "User: " << (*itr1).getFullName() << " ID: " << (*itr1).getUserID() << std::endl;
//					}
			} else if (type == USER) {
				user_list = response.getUserList();
			}
		} else {
			// Display a message box?
//				showErrorMessageBox(OUSTRING("Error"), 
//									OUSTRING("No documents currently available"));

			// Return an empty string.
			return Sequence<OUString>();
		}

		// Now, we return a Sequence of OUStrings that OpenOffice can use to display the list.
		// First, remove ourselves from the user list; we already have access to the document
		// and displaying our name in the listbox is redundant.

		if (type == USER) {
			unsigned int i;
			std::vector<User>::iterator itr;
			for (itr = user_list.begin(); itr != user_list.end(); ++itr) {
				if (itr->getUserID() == meta_info.user_info.userID) {
					user_list.erase(itr);
					break;
				}
			}

			Sequence<OUString> result(user_list.size());
			for (i = 0; i < user_list.size(); ++i) {
				//result[i] = user_list[i].getEmailAddress();
				result[(sal_Int32) i] = convertToOUString(user_list[i].getFullName());
			}

			return result;
		} else if (type == AUDITOR) {
			unsigned int j;
			std::vector<User>::iterator itr2;

			// After talking with Collberg, we decided that users should be able to 
			// audit themselves, if they have auditor privledges. Thus, I commented
			// this snippet below out.
			// -- Aaron
//				for( itr2 = auditor_list.begin(); itr2 != auditor_list.end(); ++itr2){
//					if((*itr2).getUserID() == meta_info.user_info.userID){
//						auditor_list.erase(itr2);
//						break;
//					}
//				}

			Sequence<OUString> result(auditor_list.size());
			for (j = 0; j < auditor_list.size(); ++j) {
				//result[i] = user_list[i].getEmailAddress();
				result[(sal_Int32) j] = convertToOUString(auditor_list[j].getFullName());
			}

			return result;
		}
	} catch (com::haathi::network::NetworkException & e1) {
		showErrorMessageBox(OUSTRING("Connection Exception"),
				convertToOUString(e1.getMessage()));
	} catch (com::sun::star::uno::Exception & e2) {
		showErrorMessageBox(OUSTRING("UNO Exception"), e2.Message);
	} catch (std::exception & e3) {
		showErrorMessageBox(OUSTRING("Standard Library Exception"),
				convertToOUString(e3.what()));
	} catch (...) {
		showErrorMessageBox(OUSTRING("Unknown Exception"), OUSTRING(
				"This exception is unrecognized! From experience, it could be a segmentation "
				"fault, or something OS-related, but I could be wrong. Good luck figuring this one out..."));
	}
	return Sequence<OUString>();
}

}
;
}
;
}
;
// namespace com::haathi::dialog

