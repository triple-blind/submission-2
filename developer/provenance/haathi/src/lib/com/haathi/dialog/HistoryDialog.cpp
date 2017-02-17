/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/HistoryDialog.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/HistoryDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/network/ProvenanceServer.hpp"
#include "com/haathi/network/TrustedServer.hpp"
#include "com/haathi/utils/AuthenticationCredentials.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::network;
using namespace com::haathi::utils;

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
const sal_Int32 HistoryDialog::lstHeight = 150; // Must be tall enough for right side
const sal_Int32 HistoryDialog::lstWidth = 70;

const sal_Int32 HistoryDialog::dlgWidth = dlgMargin + lstWidth + lstMargin + lblWidth + txtWidth + dlgMargin;
const sal_Int32 HistoryDialog::dlgHeight = dlgMargin + lblHeight + lblSpace + lstHeight + dlgMargin;

const int HistoryDialog::lstVersionsId = 0;

void HistoryDialog::display(Reference<XComponentContext> xComponentContext, OUString helpUrl,
		NetworkInfo& networkInfo, DocumentMetaInfo& documentMetaInfo,
		sal_Int32 height, sal_Int32 width, int documentId) {
	HistoryDialog(xComponentContext, helpUrl, networkInfo, documentMetaInfo, height, width, documentId).run();
}

void HistoryDialog::display(Reference<XComponentContext> xComponentContext, OUString helpUrl,
		NetworkInfo& networkInfo, DocumentMetaInfo& documentMetaInfo,
		int documentId) {
	HistoryDialog(xComponentContext, helpUrl, networkInfo, documentMetaInfo, documentId).run();
}

User HistoryDialog::getUser(int userId) {
	static User unknownUser;

	try {
		return userMap[userId];
	}
	catch (...) {
	}
	return unknownUser;
}

void HistoryDialog::onListSelected(int id, sal_Int32 selected) {
	Version version = versions[selected];
	User user = getUser(version.getCreatedBy());
	txtFirst->setText(convertToOUString(user.getFirst()));
	txtLast->setText(convertToOUString(user.getLast()));
	txtEmail->setText(convertToOUString(user.getEmailAddress()));
	txtDated->setText(convertToOUString(version.getCreatedOn()));
}

void HistoryDialog::addVersions() {
	try {
		{
			TrustedServer trustedServer(networkInfo);
			GetHistoryRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(), documentId);
			GetHistoryResponse response;
			if (!trustedServer.getHistory(request, response))
				throw response;
			versions = response.getVersions();
			std::vector<Version>::iterator vectorItr;
			sal_Int16 i = 0;

			for (vectorItr = versions.begin(); vectorItr != versions.end(); ++vectorItr)
				lstVersions->addItem(convertToOUString((*vectorItr).getId()), i++);
		}
		{
			TrustedServer trustedServer(networkInfo);
			GetDocumentEditorsRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(), documentId);
			GetDocumentEditorsResponse response;
			if (!trustedServer.getDocumentEditors(request, response))
				throw response;
			std::vector<User> userList = response.getUsers();
			std::vector<User>::iterator vectorItr;
			for (vectorItr = userList.begin(); vectorItr != userList.end(); ++vectorItr) {
				User user(*vectorItr);
				userMap[user.getUserId()] = user;
			}
		}
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
		showErrorMessageBox("Unexpected exception encountered while collecting document versions!");
	}
}

HistoryDialog::HistoryDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl,
		NetworkInfo& networkInfo, DocumentMetaInfo& documentMetaInfo,
		sal_Int32 height, sal_Int32 width, int documentId):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth,
		OUSTRING("Provenance - History"), helpUrl, height, width),
		networkInfo(networkInfo), documentMetaInfo(documentMetaInfo),
		documentId(documentId) {
	init();
}

HistoryDialog::HistoryDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl,
		NetworkInfo& networkInfo, DocumentMetaInfo& documentMetaInfo,
		int documentId):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth,
		OUSTRING("Provenance - History"), helpUrl),
		networkInfo(networkInfo), documentMetaInfo(documentMetaInfo),
		documentId(documentId) {
	init();
}

void HistoryDialog::init() {
	versionsListener = ListListener(this, lstVersionsId);

	int x = dlgMargin;
	int y = dlgMargin;
	int tabIndex = 0;

	insertFixedText(x, y, lblHeight, lstWidth,
			OUSTRING("lblVersions"), OUSTRING("~Versions"),
			sal_False, tabIndex++,
			OUSTRING("Versions of the document"));
	y += lblHeight + lblSpace;

	lstVersions = insertListBox(x, y, lstHeight, lstWidth,
			OUSTRING("lstVersions"),
			sal_True, tabIndex++, versionsListener);
	x += lstWidth + lstMargin;
	setFocus(lstVersions);
	addVersions();

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblFirst"), OUSTRING("First"),
			sal_False, tabIndex++,
			OUSTRING("First name of version creator"));
	x += lblWidth;

	txtFirst = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtFirst"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblLast"), OUSTRING("Last"),
			sal_False, tabIndex++,
			OUSTRING("Last name of version creator"));
	x += lblWidth;

	txtLast = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtLast"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblEmail"), OUSTRING("Email"),
			sal_False, tabIndex++,
			OUSTRING("Email address of version creator"));
	x += lblWidth;

	txtEmail = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtEmail"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y, lblHeight, lblWidth,
			OUSTRING("lblDated"), OUSTRING("Dated"),
			sal_False, tabIndex++,
			OUSTRING("Date and time of version"));
	x += lblWidth;

	txtDated = insertFixedText(x, y, txtHeight, txtWidth,
			OUSTRING("lblDatedData"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);

	y = dlgHeight - dlgMargin - btnHeight;

	x = dlgWidth - dlgMargin - btnWidth - btnSpace - btnWidth;

	insertOkButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Dismiss the dialog"));
	x += btnWidth + btnSpace;

	insertCancelButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Dismiss the dialog"));

	if (lstVersions->getItemCount() > 0)
		lstVersions->selectItemPos(0, sal_True);
}

HistoryDialog::~HistoryDialog() {
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
