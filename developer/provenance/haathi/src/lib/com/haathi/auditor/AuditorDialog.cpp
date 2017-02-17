/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/auditor/AuditorDialog.cpp#1 $

/*
 *  Created by Aaron Gibson on 3/17/12.
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/auditor/AuditorDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/auditor/AuditorRunner.hpp"
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/network/Packet.hpp" // User
#include "com/haathi/utils/PropertySetHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/XTopWindow.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::dialog;
using namespace com::haathi::document;
using namespace com::haathi::network;
using namespace com::haathi::provenance;
using namespace com::haathi::utils;

using namespace com::sun::star::awt;
using namespace com::sun::star::beans;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace auditor {
/******************************************************************************
Class
******************************************************************************/
const sal_Int32 AuditorDialog::lstWidth = 150;
const sal_Int32 AuditorDialog::lstHeight = 100;

const sal_Int32 AuditorDialog::dlgWidth = dlgMargin + lstWidth + dlgMargin;
const sal_Int32 AuditorDialog::dlgHeight = dlgMargin +
		lblHeight + lblSpace +  // Events
		lstHeight + btnSpace + 
		btnHeight + btnMargin + // Prev & Next
		lblHeight + lblSpace +  // Color
		lblHeight + lblSpace +  // Type
		lblHeight + lblSpace +  // Dated
		lblHeight + lblSpace +  // Status1
		lblHeight + lblSpace +  // Status2
		lblHeight + lblSpace +  // Status3
		txtHeight + txtSpace +  // Description need tall
		txtHeight + txtSpace +  // First
		txtHeight + txtSpace +  // Last
		txtHeight + btnMargin + // Email
		btnHeight + dlgMargin;  // Close

const int AuditorDialog::btnFirstId = 0;
const int AuditorDialog::btnPrevId = 1;
const int AuditorDialog::btnNextId = 2;
const int AuditorDialog::btnLastId = 3;
const int AuditorDialog::lstEventsId = 0;
const int AuditorDialog::winOtherId = 0;

void AuditorDialog::display(Reference<XComponentContext> xComponentContext, OUString helpUrl,
		std::string document, std::vector<std::string>& provenances, DocumentMetaInfo documentMetaInfo,
		Reference<XComponent> xDocumentComponent, std::map<std::string, User> userMap) {
	if (!AuditorRunner::toFront()) {
		AuditorRunner* auditorRunner = new AuditorRunner(xComponentContext, helpUrl, document, provenances,
				documentMetaInfo, xDocumentComponent, userMap);
		auditorRunner->create();
	}
}

void AuditorDialog::onButtonClicked(int id) {
	sal_Int16 pos = lstEvents->getSelectedItemPos();
	bool good = false;

	if (id == btnFirstId)
		pos = 0, good = isFirstOk();
	if (id == btnPrevId)
		pos--,  good = isPrevOk();
	if (id == btnNextId)
		pos++, good = isNextOk();
	if (id == btnLastId)
		pos = lstEvents->getItemCount() - 1, good = isLastOk();
	if (good)
		lstEvents->selectItemPos(pos, sal_True);
}

void AuditorDialog::onClosed(int id) {
	otherClosed = true;
}

bool AuditorDialog::isFirstOk() {
	return isPrevOk();
}

bool AuditorDialog::isPrevOk() {
	return 0 < lstEvents->getItemCount() && 0 <= lstEvents->getSelectedItemPos() - 1;
}

bool AuditorDialog::isNextOk() {
	return 0 < lstEvents->getItemCount() && lstEvents->getSelectedItemPos() + 1 < lstEvents->getItemCount();
}

bool AuditorDialog::isLastOk() {
	return isNextOk();
}

void AuditorDialog::updateButtons() {
	enable(btnFirst, isFirstOk());
	enable(btnPrev, isPrevOk());
	enable(btnNext, isNextOk());
	enable(btnLast, isLastOk());
}

void AuditorDialog::setColor(OUString name, Color color) {
	Reference<XPropertySet> xPropertySet(xNameContainer->getByName(name), UNO_QUERY);
	PropertySetHelper propertySetHelper(xPropertySet);
	propertySetHelper.set("BackgroundColor", Any(color & 0x00ffffff)); // for opacity
}

std::string AuditorDialog::getFirst(ProvenanceEvent* provenanceEvent) {
	if (!provenanceEvent)
		return emptyString;
	std::map<std::string, User>::iterator itr = userMap.find(provenanceEvent->getUser());
	if (itr != userMap.end())
		return itr->second.getFirst();
	return emptyString;
}

std::string AuditorDialog::getLast(ProvenanceEvent* provenanceEvent) {
	if (!provenanceEvent)
		return emptyString;
	std::map<std::string, User>::iterator itr = userMap.find(provenanceEvent->getUser());
	if (itr != userMap.end())
		return itr->second.getLast();
	return emptyString;
}

void AuditorDialog::onListSelected(int id, sal_Int32 selected) {
	std::vector<EventSummary>* eventSummaries = auditor->getEventSummaries();
	EventSummary eventSummary = (*eventSummaries)[selected];

	updateButtons();

	ProvenanceEvent *provenanceEvent = eventSummary.provenanceEvent;
	lblType->setText(provenanceEvent ? convertToOUString(provenanceEvent->getEventType()) : emptyOUString);
	lblDated->setText(provenanceEvent ? convertToOUString(provenanceEvent->getTime().printDateAndMillisecondsTime()) : emptyOUString);

	lblStatus21->setText(eventSummary.getAuditorStatus());
	lblStatus31->setText(eventSummary.getIntegrityStatus());

	lblStatus22->setText(eventSummary.getForwardEventStatus());
	lblStatus32->setText(eventSummary.getForwardDocumentStatus());

	lblStatus23->setText(eventSummary.getBackwardEventStatus());
	lblStatus33->setText(eventSummary.getBackwardDocumentStatus());

	txtDescription->setText(provenanceEvent ? provenanceEvent->getEventDescription() : emptyOUString);
	txtFirst->setText(convertToOUString(getFirst(provenanceEvent)));
	txtLast->setText(convertToOUString(getLast(provenanceEvent)));
	txtEmail->setText(provenanceEvent ? convertToOUString(provenanceEvent->getUser()) : emptyOUString);
	setColor(OUSTRING("lblColor"), eventSummary.color);
	if (!otherClosed)
		auditor->loadEventAt(selected);
}

void AuditorDialog::addEvents() {
	if (otherClosed)
		return;
	std::vector<EventSummary>* eventSummaries = auditor->getEventSummaries();
	Sequence<OUString> contents(eventSummaries->size());

	std::vector<EventSummary>::iterator vectorItr;
	sal_Int16 i = 0;
	for (vectorItr = eventSummaries->begin(); vectorItr != eventSummaries->end(); ++vectorItr)
		contents[i++] = (*vectorItr).toOUString();
	lstEvents->addItems(contents, 0);
}

AuditorDialog::AuditorDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl,
		std::string document, std::vector<std::string>& provenances, DocumentMetaInfo documentMetaInfo,
		Reference<XComponent> xDocumentComponent, std::map<std::string, User> userMap):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth, OUSTRING("Provenance - Auditor"), helpUrl, sal_False),
		xDocumentComponent(xDocumentComponent), userMap(userMap), otherClosed(false) {
	eventsListener = ListListener(this, lstEventsId);
	firstListener = ButtonListener(this, btnFirstId);
	prevListener = ButtonListener(this, btnPrevId);
	nextListener = ButtonListener(this, btnNextId);
	lastListener = ButtonListener(this, btnLastId);
	closingListener = ClosingListener(this, winOtherId);

	connectClosingListener(xDocumentComponent, &closingListener);

	int x = dlgMargin;
	int y = dlgMargin;
	int tabIndex = 0;

	insertFixedText(x, y, lblHeight, lstWidth,
			OUSTRING("lblEvents"), OUSTRING("~Events"),
			sal_False, tabIndex++,
			OUSTRING("Events captured by the Provenance system"));
	y += lblHeight + lblSpace;

	lstEvents = insertListBox(x, y, lstHeight, lstWidth,
			OUSTRING("lstEvents"), sal_True, tabIndex++,
			eventsListener);
	y += lstHeight + btnSpace;

	int tmpBtnWidth = (dlgWidth - 2 * dlgMargin - 3 * btnSpace) / 4;

	btnFirst = insertStandardButton(x, y, btnHeight, tmpBtnWidth,
			OUSTRING("btnFirst"), OUSTRING("~First"),
			sal_True, tabIndex++, 
			OUSTRING("Show the first event"), firstListener);
	x += tmpBtnWidth + btnSpace;

	btnPrev = insertStandardButton(x, y, btnHeight, tmpBtnWidth,
			OUSTRING("btnPrev"), OUSTRING("~Prev"),
			sal_True, tabIndex++, 
			OUSTRING("Show the previous event"), prevListener);
	x = dlgWidth - dlgMargin - tmpBtnWidth - btnSpace - tmpBtnWidth;

	btnNext = insertStandardButton(x, y, btnHeight, tmpBtnWidth,
			OUSTRING("btnNext"), OUSTRING("~Next"),
			sal_True, tabIndex++, 
			OUSTRING("Show the next event"), nextListener);
	x += tmpBtnWidth + btnSpace;

	btnLast = insertStandardButton(x, y, btnHeight, tmpBtnWidth,
			OUSTRING("btnLast"), OUSTRING("~Last"),
			sal_True, tabIndex++, 
			OUSTRING("Show the last event"), lastListener);
	x = dlgMargin;
	y += btnHeight + btnMargin;

	lblColor = insertFixedText(x, y, lblHeight, dlgWidth - dlgMargin - dlgMargin,
			OUSTRING("lblColor"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	y += lblHeight + lblSpace;

	insertFixedText(x, y, lblHeight, lblWidth,
			OUSTRING("lblType"), OUSTRING("Type"),
			sal_False, tabIndex++,
			OUSTRING("What kind of event this is"));
	x += lblWidth;

	int tmpTxtWidth = dlgWidth - dlgMargin - dlgMargin - lblWidth;
	lblType = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblTypeData"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	x = dlgMargin;
	y += lblHeight + lblSpace;

	insertFixedText(x, y, lblHeight, lblWidth,
		OUSTRING("lblDated"), OUSTRING("Dated"),
		sal_False, tabIndex++,
		OUSTRING("When this event was recorded"));
	x += lblWidth;

	lblDated = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblDatedData"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	x = dlgMargin;
	y += lblHeight + lblSpace;

	insertFixedText(x, y, lblHeight, lblWidth,
		OUSTRING("lblStatus"), OUSTRING("Status"),
		sal_False, tabIndex++,
		OUSTRING("The extent to which the provenance data matches the document"));
	x += lblWidth;

	sal_Int32 lblStatusWidth = 36;
	sal_Int32 lblStatusIndent = 4;

	lblStatus11 = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblStatusData11"), OUSTRING("Entry"),
			sal_False, tabIndex++,
			OUSTRING("Details about the validity of the entry independent of event"));
	x += lblStatusWidth;

	lblStatus12 = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblStatusData12"), OUSTRING("Forward"),
			sal_False, tabIndex++,
			OUSTRING("The validity of the event when played forward"));
	x += lblStatusWidth;

	lblStatus13 = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblStatusData13"), OUSTRING("Backward"),
			sal_False, tabIndex++,
			OUSTRING("The validity of the event when played backward"));
	x = dlgMargin + lblWidth + lblStatusIndent;
	y += lblHeight + lblSpace;

	lblStatus21 = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblStatusData21"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	x += lblStatusWidth;

	lblStatus22 = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblStatusData22"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	x += lblStatusWidth;

	lblStatus23 = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblStatusData23"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	x = dlgMargin + lblWidth + lblStatusIndent + lblStatusIndent;
	y += lblHeight + lblSpace;

	lblStatus31 = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblStatusData31"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	x += lblStatusWidth;

	lblStatus32 = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblStatusData32"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	x += lblStatusWidth;

	lblStatus33 = insertFixedText(x, y, lblHeight, tmpTxtWidth,
			OUSTRING("lblStatusData33"), emptyOUString,
			sal_False, tabIndex++,
			emptyOUString);
	x = dlgMargin;
	y += lblHeight + lblSpace;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
		OUSTRING("lblDescription"), OUSTRING("Description"),
		sal_False, tabIndex++,
		OUSTRING("More details about this event"));
	x += lblWidth;

	txtDescription = insertReadOnlyTextComponent(x, y, txtHeight, tmpTxtWidth,
			OUSTRING("txtDescription"), emptyOUString,
			sal_False, tabIndex++);
	x = dlgMargin;
	y += txtHeight + txtSpace;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblFirst"), OUSTRING("First"),
			sal_False, tabIndex++,
			OUSTRING("First name of event producer"));
	x += lblWidth;

	txtFirst = insertReadOnlyTextComponent(x, y, txtHeight, tmpTxtWidth,
			OUSTRING("txtFirst"), emptyOUString,
			sal_False, tabIndex++);
	x -= lblWidth;
	y += txtHeight + txtSpace;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblLast"), OUSTRING("Last"),
			sal_False, tabIndex++,
			OUSTRING("Last name of document creator"));
	x += lblWidth;

	txtLast = insertReadOnlyTextComponent(x, y, txtHeight, tmpTxtWidth,
			OUSTRING("txtLast"), emptyOUString,
			sal_False, tabIndex++);
	x -= lblWidth;
	y += txtHeight + txtSpace;

	insertFixedText(x, y + lblAdjust, lblHeight, lblWidth,
			OUSTRING("lblEmail"), OUSTRING("Email"),
			sal_False, tabIndex++,
			OUSTRING("Email address of document creator"));
	x += lblWidth;

	txtEmail = insertReadOnlyTextComponent(x, y, txtHeight, tmpTxtWidth,
			OUSTRING("txtEmail"), emptyOUString,
			sal_False, tabIndex++);
	y += txtHeight + btnMargin;

	insertCloseButton(dlgWidth - dlgMargin - btnWidth, y, btnHeight, btnWidth, 
			sal_True, tabIndex++,
			OUSTRING("Close the auditor dialog"));

	auditor = boost::shared_ptr<EncryptedProvenanceAuditor>(new EncryptedProvenanceAuditor(xDocumentComponent,
			provenances, documentMetaInfo, this));
	addEvents();
	setFocus(lstEvents);
	if (lstEvents->getItemCount() > 0)
		lstEvents->selectItemPos(lstEvents->getItemCount() - 1, sal_True);
	else
		updateButtons();
}

AuditorDialog::~AuditorDialog() {
	if (!otherClosed)
		disconnectClosingListener(xDocumentComponent, &closingListener);
}

void AuditorDialog::toFront() {
	if (!otherClosed) {
		Reference<XModel> xModel(xDocumentComponent, UNO_QUERY);
		if (xModel.is()) {
			Reference<XTopWindow> xTopWindow(xModel->getCurrentController()->getFrame()->getContainerWindow(), UNO_QUERY);
			if (xTopWindow.is())
				xTopWindow->toFront();
		}
	}
	BaseDialog::toFront();
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
