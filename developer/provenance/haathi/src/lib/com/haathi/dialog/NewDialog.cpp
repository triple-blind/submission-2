/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/NewDialog.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/NewDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/crypto/Key.hpp"
#include "com/haathi/dialog/MessageBoxResults.hpp"
#include "com/haathi/document/CloudProvenanceDocument.hpp"
#include "com/haathi/network/TrustedServer.hpp"
#include "com/haathi/utils/AuthenticationCredentials.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/awt/XDialog.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <osl/time.h>
#include "sdkEnd.hpp"

#include <fstream>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::crypto;
using namespace com::haathi::document;
using namespace com::haathi::network;
using namespace com::haathi::utils;

using namespace com::sun::star::awt; // MessageBoxResults
using namespace com::sun::star::beans;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
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
const sal_Int32 NewDialog::lstHeight = 150; // Must be tall enough for right side
const sal_Int32 NewDialog::lstWidth = 70;

const sal_Int32 NewDialog::dlgWidth = dlgMargin + lstWidth + lstMargin + lblWidth + txtWidth + dlgMargin;
const sal_Int32 NewDialog::dlgHeight = dlgMargin + lblHeight + lblSpace + lstHeight + dlgMargin;

const int NewDialog::lstExistingDocumentsId = 0;
const int NewDialog::btnShareId = 0;
const int NewDialog::txtNameId = 0;

void NewDialog::display(Reference<XComponentContext> xComponentContext, OUString helpUrl, OUString shareHelpUrl,
		NetworkInfo& networkInfo, DocumentMetaInfo documentMetaInfo) {
	NewDialog(xComponentContext, helpUrl, shareHelpUrl, networkInfo, documentMetaInfo).run();
}

bool NewDialog::anew() {
	try {
		{
			TrustedServer trustedServer(networkInfo);	
			CreateDocumentRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(),
					convertToString(txtName->getText()));
			CreateDocumentResponse response;
			if (!trustedServer.createDocument(request, response))
				throw response;
			documentMetaInfo.documentId = response.getDocumentId();
			documentMetaInfo.versionId = response.getVersionId();
			documentMetaInfo.versionNo = 0;
			documentMetaInfo.documentEncryptionKey = response.getUniqueKey();
			documentMetaInfo.documentTitle = convertToString(txtName->getText());
		}

		std::vector<int> userIds = pShareDialog ? pShareDialog->getNewSharerIds() : std::vector<int>();
		std::vector<std::string> userEmails = pShareDialog ? pShareDialog->getNewSharerEmails() : std::vector<std::string>();

		if (userIds.size() > 0) {
			TrustedServer trustedServer(networkInfo);	
			ShareDocumentRequest request(documentMetaInfo.userInfo.username,
					authenticationCredentials.getPassword(), &userIds.front(), userIds.size(),
					documentMetaInfo.documentId);
			ShareDocumentResponse response;
			if (!trustedServer.shareDocument(request, response))
				throw response;
		}

		// Since the user is now also an auditor as well as any of the sharers, this is always true
		userIds.push_back(documentMetaInfo.userInfo.userId);
		if (true) {
			TrustedServer trustedServer(networkInfo);	
			RetrievePublicKeyRequest request(documentMetaInfo.userInfo.username,
					authenticationCredentials.getPassword(), &userIds.front(), userIds.size());
			RetrievePublicKeyResponse response;
			if (!trustedServer.getPublicKey(request, response))
				throw response;
			documentMetaInfo.auditorList = response.getKeys();
		}

#if 0
		Reference<XMultiComponentFactory> xMCF = xComponentContext->getServiceManager();
		Reference<XInterface> desktop = xMCF->createInstanceWithContext(
				OUSTRING("com.sun.star.frame.Desktop"), xComponentContext);
		Reference<XComponentLoader> xComponentLoader(desktop, UNO_QUERY);
		Reference<XComponent> xComponent = xComponentLoader->loadComponentFromURL(
				OUSTRING("private:factory/swriter"),
				OUSTRING("_blank"), 0,
				Sequence<PropertyValue>());
#endif

#if 1
		/*CloudProvenanceDocument* provenanceDocument =*/ CloudProvenanceDocument::newCloudProvenanceDocument(xComponentContext, documentMetaInfo,
				networkInfo, userEmails);
		// TODO: Can the component simply be returned?
		// Make some kind of supplier that is more easily configured
		//SynonymProvenanceDocument* provenanceDocument = SynonymProvenanceDocument::create(xComponentContext, documentMetaInfo, networkInfo, userEmails);
#endif
		return true;
	}
	catch (ResponsePacket& responsePacket) {
		showErrorMessageBox(responsePacket);
	}
	catch (NetworkException& networkException) {
		showErrorMessageBox(networkException);
	}
	catch (Exception& exception) {
		showErrorMessageBox(exception);
	}
	catch (...) {
		showErrorMessageBox("Unexpected exception encountered while creating a new document!");
	}
	return false;
}

sal_Int16 NewDialog::run() {
	bool done = false;
	sal_Int16 result;

	while (!done) {
		result = BaseDialog::run();
		if (result == 0)
			done = true;
		if (result == 1 && isOk()) {
			if (!isExistingName())
				done = anew();
			else
				if (showQueryMessageBox(OUSTRING("The name \"") + txtName->getText() +
						OUSTRING("\" is already in use.\nAre you sure want to reuse it?"))
						== MessageBoxResults::YES)
					done = anew();
		}
	}
	return dialogResult = result;
}

void NewDialog::onButtonClicked(int id) {
	if (!pShareDialog)
		pShareDialog = boost::shared_ptr<ShareDialog>(new ShareDialog(xComponentContext, shareHelpUrl, networkInfo, documentMetaInfo,
				height, width, false));
	pShareDialog->run();
}

void NewDialog::onListSelected(int id, sal_Int32 selected) {
	OUString empty;

	enable(btnOk, isOk());

	if (selected != 65535) {
		DocumentInfo documentInfo = documentInfoMap[documentList[selected].id];

	txtFirst->setText(   convertToOUString(documentInfo.first));
	txtLast->setText(    convertToOUString(documentInfo.last));
	txtEmail->setText(   convertToOUString(documentInfo.email));
	lblCreated->setText( convertToOUString(documentInfo.createdOn));
	lblModified->setText(convertToOUString(documentInfo.updatedOn));
	lblVersion->setText( convertToOUString(documentInfo.versionNo));
}
	else {
		   txtFirst->setText(empty);
		    txtLast->setText(empty);
		   txtEmail->setText(empty);
		 lblCreated->setText(empty);
		lblModified->setText(empty);
		 lblVersion->setText(empty);
	}
}

void NewDialog::onListDoubleSelected(int id) {
	OUString text = lstExistingDocuments->getSelectedItem();
	sal_Int32 len = text.getLength();
	com::sun::star::awt::Selection selection(len, len);

	txtName->setText(text);
	txtName->setSelection(selection);
	setFocus(txtName);
}

void NewDialog::onTextEdited(int id) {
	std::string name = convertToString(txtName->getText());
	bool found = false;

	if (name.length() > 0)
		for (size_t i = 0; i < documentList.size(); i++)
			if (documentList[i].name.compare(0, name.length(), name) == 0) {
				lstExistingDocuments->selectItemPos((sal_Int16) i, sal_True);
				lstExistingDocuments->makeVisible((sal_Int16) i);
				found = true;
				break;
			}
	if (!found) {
		sal_Int16 pos = lstExistingDocuments->getSelectedItemPos();
		if (pos >=0 && pos != 65535)
			lstExistingDocuments->selectItemPos(pos, sal_False);
	}
	enable(btnOk, isOk());
}

bool NewDialog::isOk() {
	return txtName->getText().getLength() > 0;
}

bool NewDialog::isExistingName() {
	std::string name = convertToString(txtName->getText());

	std::vector<DocumentData>::iterator vectorItr;
	for (vectorItr = documentList.begin(); vectorItr != documentList.end(); ++vectorItr)
		if ((*vectorItr).name == name)
			return true;
	return false;
}

void NewDialog::addDocuments() {
	try {
		TrustedServer trustedServer(networkInfo);
		ListDocumentsRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(),
				documentMetaInfo.userInfo.userId, User::USER);
		ListDocumentsResponse response;
		if (!trustedServer.listAvailableDocuments(request, response))
			throw response;
		documentInfoMap = response.getDocumentInfos();

		std::map<int, DocumentInfo>::iterator mapItr;
		Sequence<OUString> result(documentInfoMap.size());

		for (mapItr = documentInfoMap.begin(); mapItr != documentInfoMap.end(); ++mapItr)
			documentList.push_back(DocumentData((*mapItr).first, (*mapItr).second.name));

		sortDocumentList(documentList);

		std::vector<DocumentData>::iterator vectorItr;
		sal_Int16 i = 0;
		for (vectorItr = documentList.begin(); vectorItr != documentList.end(); ++vectorItr)
			lstExistingDocuments->addItem(convertToOUString((*vectorItr).name), i++);
	}
	catch (ResponsePacket& responsePacket) {
		showErrorMessageBox(responsePacket);
	}
	catch (NetworkException& networkException) {
		showErrorMessageBox(networkException);
	}
	catch (Exception& exception) {
		showErrorMessageBox(exception);
	}
	catch (...) {
		showErrorMessageBox("Unexpected exception encountered while collecting documents!");
	}
}

NewDialog::NewDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl, OUString shareHelpUrl,
		NetworkInfo& networkInfo, DocumentMetaInfo documentMetaInfo):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth,
		OUSTRING("Provenance - New"), helpUrl),
		shareHelpUrl(shareHelpUrl),
		networkInfo(networkInfo), documentMetaInfo(documentMetaInfo) {
	existingDocumentsListener = ListListener(this, lstExistingDocumentsId);
	shareListener = ButtonListener(this, btnShareId);
	nameListener = TextListener(this, txtNameId);

	int x = dlgMargin;
	int y = dlgMargin;
	int tabIndex = 0;

	insertFixedText(x, y, lblHeight, lstWidth,
			OUSTRING("lblExistingDocuments"), OUSTRING("~Existing documents"),
			sal_False, tabIndex++,
			OUSTRING("Existing documents whose names shouldn't be reused"));
	y += lblHeight + lblSpace;

	lstExistingDocuments = insertListBox(x, y, lstHeight, lstWidth,
			OUSTRING("lstExistingDocuments"),
			sal_True, tabIndex++, existingDocumentsListener);
	x += lstWidth + lstMargin;

	addDocuments();

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblFirst"), OUSTRING("First"),
			sal_False, tabIndex++,
			OUSTRING("First name of document creator"));
	x += lblWidth;

	txtFirst = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtFirst"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblLast"), OUSTRING("Last"),
			sal_False, tabIndex++,
			OUSTRING("Last name of document creator"));
	x += lblWidth;

	txtLast = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtLast"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblEmail"), OUSTRING("Email"),
			sal_False, tabIndex++,
			OUSTRING("Email address of document creator"));
	x += lblWidth;

	txtEmail = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtEmail"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y, lblHeight, lblWidth,
			OUSTRING("lblCreated"), OUSTRING("Created"),
			sal_False, tabIndex++,
			OUSTRING("Date and time of document creation"));
	x += lblWidth;

	lblCreated = insertFixedText(x, y, txtHeight, txtWidth,
			OUSTRING("lblCreatedData"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	y += lblHeight + lblSpace;
	x -= lblWidth;

	insertFixedText(x, y, lblHeight, lblWidth,
			OUSTRING("lblModified"), OUSTRING("Modified"),
			sal_False, tabIndex++,
			OUSTRING("Date and time of document modification"));
	x += lblWidth;

	lblModified = insertFixedText(x, y, txtHeight, txtWidth,
			OUSTRING("lblModifiedData"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	y += lblHeight + lblSpace;
	x -= lblWidth;

	insertFixedText(x, y, lblHeight, lblWidth,
			OUSTRING("lblVersion"), OUSTRING("Version"),
			sal_False, tabIndex++,
			OUSTRING("Latest version number"));
	x += lblWidth;

	lblVersion = insertFixedText(x, y, txtHeight, txtWidth,
			OUSTRING("lblVersionData"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	y += lblHeight + dlgMargin;
	x -= lblWidth;

	insertLine(x, y, lblWidth + txtWidth,
			OUSTRING("sepName"), sal_True);
	y += dlgMargin;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblName"), OUSTRING("~Name"),
			sal_False, tabIndex++,
			OUSTRING("Name of the new document"));
	x += lblWidth;

	txtName = insertTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtName"), emptyOUString,
			sal_True, tabIndex++);
	setFocus(txtName);
	connectTextListener(txtName, &nameListener);
	x -= lblWidth;

	y = dlgHeight - dlgMargin - btnHeight;

	insertStandardButton(x, y, btnHeight, btnWidth, 
			OUSTRING("btnShare"), OUSTRING("~Share..."),
			sal_True, tabIndex + 2, // should be after the next two buttons
			OUSTRING("Share the new document with others"), shareListener);

	x = dlgWidth - dlgMargin - btnWidth - btnSpace - btnWidth;

	btnOk = insertOkButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Create a new Provenance document"));
	x += btnWidth + btnSpace;

	insertCancelButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Cancel the new operation"));

//	if (lstExistingDocuments->getItemCount() > 0)
//		lstExistingDocuments->selectItemPos(0, sal_True);
//	else
		enable(btnOk, isOk());
}

NewDialog::~NewDialog() {
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
