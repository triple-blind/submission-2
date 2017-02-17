/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/OpenDialog.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/OpenDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/crypto/ProvenanceHelper.hpp"
#include "com/haathi/dialog/HistoryDialog.hpp"
#include "com/haathi/document/CloudProvenanceDocument.hpp"
#include "com/haathi/network/ProvenanceServer.hpp"
#include "com/haathi/network/TrustedServer.hpp"
#include "com/haathi/utils/AuthenticationCredentials.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/XDialog.hpp>
#include "sdkEnd.hpp"
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
const sal_Int32 OpenDialog::lstHeight = 150; // Must be tall enough for right side
const sal_Int32 OpenDialog::lstWidth = 70;

const sal_Int32 OpenDialog::dlgWidth = dlgMargin + lstWidth + lstMargin + lblWidth + txtWidth + dlgMargin;
const sal_Int32 OpenDialog::dlgHeight = dlgMargin + lblHeight + lblSpace + lstHeight + dlgMargin;

const int OpenDialog::lstExistingDocumentsId = 0;
const int OpenDialog::btnHistoryId = 0;

void OpenDialog::display(Reference<XComponentContext> xComponentContext, OUString helpUrl, OUString historyHelpUrl,
		NetworkInfo& networkInfo, DocumentMetaInfo documentMetaInfo) {
	OpenDialog(xComponentContext, helpUrl, historyHelpUrl, networkInfo, documentMetaInfo).run();
}

int OpenDialog::getDocumentId() {
	return documentList[lstExistingDocuments->getSelectedItemPos()].id;
}

static std::string lastProvenanceEvent(std::string provenance, DocumentMetaInfo& documentMetaInfo) {
#if ENCRYPT_PROVENANCE == ENCRYPT_PROVENANCE_ON
	provenance = ProvenanceHelper::decryptData(documentMetaInfo.documentEncryptionKey,
			ProvenanceHelper::base64Decode(provenance));
#endif
	cJSON* provenances = cJSON_Parse(provenance.c_str());

	if (!provenances)
		return std::string();

	cppJSON cppProvenances(provenances);
	int count = cJSON_GetArraySize(provenances);

	if (count > 0)
		return cJSON_GetArrayItem(provenances, count - 1)->valuestring;
	return std::string();
}

bool OpenDialog::open() {
	try {
		std::string decryptedDocument;
		int id = getDocumentId();
		std::string versionId = documentInfoMap[id].versionId;
		int versionNo = documentInfoMap[id].versionNo;

		{
			TrustedServer trustedServer(networkInfo);
			GetDocumentAuditorsRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(), id);
			GetDocumentAuditorsResponse response;
			if (!trustedServer.getDocumentAuditorsWithKeys(request, response))
				throw response;
			documentMetaInfo.auditorList = response.getKeys();
		}
		{
			TrustedServer trustedServer(networkInfo);
			AccessDocumentRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(),
					0, id, versionId, User::OPEN);
			AccessDocumentResponse response;
			if (!trustedServer.accessDocument(request, response))
				throw response;
		}
		{
			ProvenanceServer provenanceServer(documentMetaInfo.cloudInfo);
			LoadProvenanceDocumentRequest request(versionId);
			LoadProvenanceDocumentResponse response;
			if (!provenanceServer.loadDocument(request, response))
				throw response;

			Key uniqueKey(documentLoadKeysMap[id]);
			documentMetaInfo.documentId = id;
			documentMetaInfo.versionId = versionId;
			documentMetaInfo.versionNo = versionNo;
			documentMetaInfo.documentEncryptionKey = uniqueKey;
			documentMetaInfo.documentTitle = documentInfoMap[id].name;
			documentMetaInfo.previousSavedEvent = lastProvenanceEvent(response.getProvenance(), documentMetaInfo);

			// Here, we are decrypting the document, NOT the provenance. Thus, we use
			// the "documentEncryption" field of DocumentMetaInfo.
			decryptedDocument = ProvenanceHelper::decryptData(uniqueKey,
					ProvenanceHelper::base64Decode(response.getDocument()));
		}
		{
			TrustedServer trustedServer(networkInfo);
			AccessDocumentRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(),
					1, id, versionId, User::OPEN);
			AccessDocumentResponse response;
			if (!trustedServer.accessDocument(request, response))
				throw response;
		}
		CloudProvenanceDocument::newCloudProvenanceDocument(xComponentContext,
				documentMetaInfo, networkInfo, decryptedDocument);
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
		showErrorMessageBox("Unexpected exception encountered while opening document!");
	}
	return false;
}

sal_Int16 OpenDialog::run() {
	bool done = false;
	sal_Int16 result;

	while (!done) {
		doubleClicked = false;
		result = BaseDialog::run();
		if (result == 0 && !doubleClicked)
			done = true;
		if (((result == 0 && doubleClicked) || result == 1) && isOk())
			done = open();
	}
	return dialogResult = result;
}

void OpenDialog::onButtonClicked(int id) {
	if (!isHistoryOk())
		return;
	HistoryDialog::display(xComponentContext, historyHelpUrl, networkInfo, documentMetaInfo, 
			height, width, getDocumentId());
}

void OpenDialog::onListSelected(int id, sal_Int32 selected) {
	DocumentInfo documentInfo = documentInfoMap[documentList[selected].id];

	enable(btnOk, isOk());
	enable(btnHistory, isHistoryOk());

	txtFirst->setText(   convertToOUString(documentInfo.first));
	txtLast->setText(    convertToOUString(documentInfo.last));
	txtEmail->setText(   convertToOUString(documentInfo.email));
	lblCreated->setText( convertToOUString(documentInfo.createdOn));
	lblModified->setText(convertToOUString(documentInfo.updatedOn));
	lblVersion->setText( convertToOUString(documentInfo.versionNo));
}

void OpenDialog::onListDoubleSelected(int id) {
	if (isOk()) {
		doubleClicked = true;
		endExecute();
	}
}

bool OpenDialog::isOk() {
	return lstExistingDocuments->getSelectedItemPos() >= 0;
}

bool OpenDialog::isHistoryOk() {
	return isOk();
}

void OpenDialog::addDocuments() {
	try {
		TrustedServer trustedServer(networkInfo);
		ListDocumentsRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(),
				documentMetaInfo.userInfo.userId, User::USER);
		ListDocumentsResponse response;
		if (!trustedServer.listAvailableDocuments(request, response))
			throw response;
		documentInfoMap = response.getDocumentInfos();
		documentLoadKeysMap = response.getDocumentKeys();

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

OpenDialog::OpenDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl, OUString historyHelpUrl,
		NetworkInfo& networkInfo, DocumentMetaInfo documentMetaInfo):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth,
		OUSTRING("Provenance - Open"), helpUrl),
		historyHelpUrl(historyHelpUrl),
		networkInfo(networkInfo), documentMetaInfo(documentMetaInfo) {
	existingDocumentsListener = ListListener(this, lstExistingDocumentsId);
	historyListener = ButtonListener(this, btnHistoryId);

	int x = dlgMargin;
	int y = dlgMargin;
	int tabIndex = 0;

	insertFixedText(x, y, lblHeight, lstWidth,
			OUSTRING("lblExistingDocuments"), OUSTRING("~Existing documents"),
			sal_False, tabIndex++,
			OUSTRING("Documents already existing which you can open"));
	y += lblHeight + lblSpace;

	lstExistingDocuments = insertListBox(x, y, lstHeight, lstWidth,
			OUSTRING("lstExistingDocuments"),
			sal_True, tabIndex++, existingDocumentsListener);
	x += lstWidth + lstMargin;

	setFocus(lstExistingDocuments);
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
	x -= lblWidth;

	y = dlgHeight - dlgMargin - btnHeight;

	btnHistory = insertStandardButton(x, y, btnHeight, btnWidth, 
			OUSTRING("btnHistory"), OUSTRING("~History..."),
			sal_True, tabIndex + 2, // should be after the next two buttons
			OUSTRING("Show the history of the existing document"), historyListener);

	x = dlgWidth - dlgMargin - btnWidth - btnSpace - btnWidth;

	btnOk = insertOkButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Open the existing Provenance document"));
	x += btnWidth + btnSpace;

	insertCancelButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Cancel the open operation"));

	if (lstExistingDocuments->getItemCount() > 0)
		lstExistingDocuments->selectItemPos(0, sal_True);
	else {
		enable(btnOk, isOk());
		enable(btnHistory, isHistoryOk());
	}
}

OpenDialog::~OpenDialog() {
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
