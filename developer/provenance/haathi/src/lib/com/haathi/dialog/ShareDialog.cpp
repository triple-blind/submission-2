/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/ShareDialog.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/ShareDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/network/TrustedServer.hpp"
#include "com/haathi/utils/AuthenticationCredentials.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/XDialog.hpp>
#include "sdkEnd.hpp"

#include <algorithm>
/******************************************************************************
Define
******************************************************************************/

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
const sal_Int32 ShareDialog::lstHeight = 100;
const sal_Int32 ShareDialog::lstWidth = lblWidth + txtWidth;

const sal_Int32 ShareDialog::dlgWidth = dlgMargin + lstWidth + lstMargin + lstWidth + dlgMargin;
const sal_Int32 ShareDialog::dlgHeight = dlgMargin + lblHeight + lblSpace + lstHeight +
	+lstMargin + txtHeight + txtSpace + txtHeight + txtSpace + txtHeight + txtSpace +
	btnMargin + btnHeight + dlgMargin;

const int ShareDialog::lstNewSharersId = 0;
const int ShareDialog::lstOldSharersId = 1;

sal_Int16 ShareDialog::display(Reference<XComponentContext> xComponentContext, OUString helpUrl,
		NetworkInfo& networkInfo, DocumentMetaInfo& documentMetaInfo, bool hasDocument) {
	return ShareDialog(xComponentContext, helpUrl, networkInfo, documentMetaInfo, hasDocument).run();
}

bool ShareDialog::share() {
	try {
		int id = documentMetaInfo.documentId;

		{
			std::vector<int> newSharerIds = getNewSharerIds();
			if (newSharerIds.size() == 0)
				return true;
			TrustedServer trustedServer(networkInfo);
			ShareDocumentRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(),
					&newSharerIds.front(), newSharerIds.size(), id);
			ShareDocumentResponse response;
			if (!trustedServer.shareDocument(request, response))
				throw response;
		}
		{
			GetDocumentAuditorsRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(), id);
			TrustedServer trustedServer(networkInfo);
			GetDocumentAuditorsResponse response;
			if (!trustedServer.getDocumentAuditorsWithKeys(request, response))
				throw response;
			documentMetaInfo.auditorList = response.getKeys();
			return true;
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
		showErrorMessageBox("Unexpected exception encountered while collecting documents!");
	}
	return false;
}

sal_Int16 ShareDialog::run() {
	bool done = false;
	sal_Int16 result;

	while (!done) {
		result = BaseDialog::run();
		if (result == 0) {
			clearNewSharers();
			done = true;
		}
		if (result == 1)
			done = this->hasDocument ? share() : true;
	}
	return dialogResult = result;
}

bool ShareDialog::isOk() {
	return true;
}

void ShareDialog::onListSelected(int id, sal_Int32 selected) {
	bool hide = selected < 0 || selected == 65535;

	if (id == lstNewSharersId) {
		txtNewFirst->setText(hide ? emptyOUString : convertToOUString(newSharers[selected].getFirst()));
		txtNewLast-> setText(hide ? emptyOUString : convertToOUString(newSharers[selected].getLast()));
		txtNewEmail->setText(hide ? emptyOUString : convertToOUString(newSharers[selected].getEmailAddress()));
		return;
	}
	if (id == lstOldSharersId) {
		txtOldFirst->setText(hide ? emptyOUString : convertToOUString(oldSharers[selected].getFirst()));
		txtOldLast-> setText(hide ? emptyOUString : convertToOUString(oldSharers[selected].getLast()));
		txtOldEmail->setText(hide ? emptyOUString : convertToOUString(oldSharers[selected].getEmailAddress()));
		return;
	}
}

bool sortSharers(User left, User right) {
	return left.compare(right) < 0;
}

void ShareDialog::fillOldSharers(bool hasDocument) {
	if (!hasDocument)
		return;
	try {
		TrustedServer trustedServer(networkInfo);
		GetDocumentEditorsRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(), documentMetaInfo.documentId);
		GetDocumentEditorsResponse response;
		if (!trustedServer.getDocumentEditors(request, response))
			throw response;
		oldSharers = response.getUsers();
		std::vector<User>::iterator meItr = find(oldSharers.begin(), oldSharers.end(), me);
		if (meItr != oldSharers.end())
			oldSharers.erase(meItr);
		std::sort(oldSharers.begin(), oldSharers.end(), sortSharers);
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
}

User ShareDialog::getMe(UserInfo& userInfo) {
	User me(
		userInfo.userId,
		userInfo.first,
		userInfo.last,
		userInfo.username
	);
	return me;
}

void ShareDialog::fillNewSharers() {
	newSharers = allOtherUsers;
	std::sort(newSharers.begin(), newSharers.end(), sortSharers);
	std::vector<User>::iterator endItr = std::set_difference(newSharers.begin(), newSharers.end(),
			oldSharers.begin(), oldSharers.end(), newSharers.begin(), sortSharers);
	newSharers.erase(endItr, newSharers.end());
}

void ShareDialog::fillAllOtherUsers() {
	try {
		TrustedServer trustedServer(networkInfo);
		GetUsersRequest request(documentMetaInfo.userInfo.username, authenticationCredentials.getPassword(), User::USER);
		GetUsersResponse response;
		if (!trustedServer.getProvenanceUsers(request, response))
			throw response;
		allOtherUsers = response.getUserList();
		std::vector<User>::iterator meItr = find(allOtherUsers.begin(), allOtherUsers.end(), me);
		if (meItr != allOtherUsers.end())
			allOtherUsers.erase(meItr);
		return;
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
}

void addSharers(Reference<XListBox>& xListBox, std::vector<User>& users) {
	std::vector<User>::iterator itr;
	sal_Int16 i = 0;
	for (itr = users.begin(); itr != users.end(); ++itr)
		xListBox->addItem(convertToOUString(itr->getFullName()), i++);
}

ShareDialog::ShareDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl, NetworkInfo& networkInfo,
		DocumentMetaInfo& documentMetaInfo, sal_Int32 height, sal_Int32 width, bool hasDocument):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth,
		OUSTRING("Provenance - Share"), helpUrl, height, width),
		networkInfo(networkInfo), documentMetaInfo(documentMetaInfo),
		hasDocument(hasDocument), me(getMe(documentMetaInfo.userInfo)) {
	init();
}

ShareDialog::ShareDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl, NetworkInfo& networkInfo,
		DocumentMetaInfo& documentMetaInfo, bool hasDocument):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth,
		OUSTRING("Provenance - Share"), helpUrl),
		networkInfo(networkInfo), documentMetaInfo(documentMetaInfo),
		hasDocument(hasDocument), me(getMe(documentMetaInfo.userInfo)) {
	init();
}

void ShareDialog::init() {
	newSharersListener = ListListener(this, lstNewSharersId);
	oldSharersListener = ListListener(this, lstOldSharersId);

	fillAllOtherUsers();
	fillOldSharers(hasDocument);
	fillNewSharers();

	int x = dlgMargin;
	int y = dlgMargin;
	int tabIndex = 0;

	insertFixedText(x, y, lblHeight, lstWidth,
			OUSTRING("lblNewSharers"), OUSTRING("~New sharers"),
			sal_False, tabIndex++,
			OUSTRING("Users with whom to share the document"));
	y += lblHeight + lblSpace;

	lstNewSharers = insertListBox(x, y, lstHeight, lstWidth,
			OUSTRING("lstNewSharers"),
			sal_True, tabIndex++, newSharersListener, sal_True);
	y += lstHeight + lstMargin;
	setFocus(lstNewSharers);

	addSharers(lstNewSharers, newSharers);

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblNewFirst"), OUSTRING("First"),
			sal_False, tabIndex++,
			OUSTRING("First name of new sharer"));
	x += lblWidth;

	txtNewFirst = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtNewFirst"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblNewLast"), OUSTRING("Last"),
			sal_False, tabIndex++,
			OUSTRING("Last name of new sharer"));
	x += lblWidth;

	txtNewLast = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtNewLast"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblNewEmail"), OUSTRING("Email"),
			sal_False, tabIndex++,
			OUSTRING("Email address of new sharer"));
	x += lblWidth;

	txtNewEmail = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtNewEmail"), emptyOUString,
			sal_False, tabIndex++);

	x = dlgMargin + lstWidth + lstMargin;
	y = dlgMargin;

	insertFixedText(x, y, lblHeight, lstWidth,
			OUSTRING("lblOldSharers"), OUSTRING("~Existing sharers"),
			sal_False, tabIndex++,
			OUSTRING("Users with whom the document is already shared"));
	y += lblHeight + lblSpace;

	lstOldSharers = insertListBox(x, y, lstHeight, lstWidth,
			OUSTRING("lstOldSharers"),
			sal_True, tabIndex++, oldSharersListener);
	y += lstHeight + lstMargin;

	addSharers(lstOldSharers, oldSharers);

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblOldFirst"), OUSTRING("First"),
			sal_False, tabIndex++,
			OUSTRING("First name of existing sharer"));
	x += lblWidth;

	txtOldFirst = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtOldFirst"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblOldLast"), OUSTRING("Last"),
			sal_False, tabIndex++,
			OUSTRING("Last name of existing sharer"));
	x += lblWidth;

	txtOldLast = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtOldLast"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + txtSpace;
	x -= lblWidth;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblOldEmail"), OUSTRING("Email"),
			sal_False, tabIndex++,
			OUSTRING("Email address of existing sharer"));
	x += lblWidth;

	txtOldEmail = insertReadOnlyTextComponent(x, y, txtHeight, txtWidth,
			OUSTRING("txtOldEmail"), emptyOUString,
			sal_False, tabIndex++);

	y = dlgHeight - btnMargin - btnHeight;
	x = dlgWidth - dlgMargin - btnWidth - btnSpace - btnWidth;

	insertOkButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Share the Provenance document with new sharers"));
	x += btnWidth + btnSpace;

	insertCancelButton(x, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Cancel the share operation"));
}

std::vector<int> ShareDialog::getNewSharerIds() {
	Sequence<sal_Int16> selectedItemsPos = lstNewSharers->getSelectedItemsPos();
	std::vector<int> newSharerIds;

	for (sal_Int16 i = 0; i < selectedItemsPos.getLength(); i++)
		newSharerIds.push_back(newSharers[selectedItemsPos[i]].getUserId());
	return newSharerIds;
}

std::vector<std::string> ShareDialog::getNewSharerEmails() {
	Sequence<sal_Int16> selectedItemsPos = lstNewSharers->getSelectedItemsPos();
	std::vector<std::string> newSharerEmails;

	for (sal_Int16 i = 0; i < selectedItemsPos.getLength(); i++)
		newSharerEmails.push_back(newSharers[selectedItemsPos[i]].getEmailAddress());
	return newSharerEmails;
}

void ShareDialog::clearNewSharers() {
	Sequence<sal_Int16> selectedItemsPos = lstNewSharers->getSelectedItemsPos();
	lstNewSharers->selectItemsPos(selectedItemsPos, sal_False);
}

ShareDialog::~ShareDialog() {
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
