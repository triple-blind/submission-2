/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/LoggerDialog.cpp#1 $

/*
 *  Created by Aaron Gibson on 3/16/12.
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/LoggerDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Logger.hpp"
#include "com/haathi/utils/NamesValuesHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include "com/sun/star/awt/Selection.hpp"
#include "sdkEnd.hpp"
/******************************************************************************
Define
******************************************************************************/
#define STRATEGY_HIDDEN 0
#define STRATEGY_LISTEN 1

#define STRATEGY STRATEGY_HIDDEN
/******************************************************************************
Using
******************************************************************************/
using namespace com::sun::star::awt;
using namespace com::sun::star::uno;
using namespace com::haathi::utils;

using namespace com::sun::star::lang;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
Class
******************************************************************************/
const sal_Int32 LoggerDialog::logWidth = 188;
const sal_Int32 LoggerDialog::logHeight = 184;
const sal_Int32 LoggerDialog::dlgWidth = dlgMargin + logWidth + dlgMargin;
const sal_Int32 LoggerDialog::dlgHeight = dlgMargin + logHeight + btnMargin +
		btnHeight + dlgMargin;

LoggerDialog* LoggerDialog::instance = NULL;

const int LoggerDialog::btnClearId = 0;

void LoggerDialog::display(Reference<XComponentContext> xComponentContext, OUString helpUrl) {
	if (instance != NULL) {
		instance->toFront();
		return;
	}
	LoggerDialog *dialog = new LoggerDialog(xComponentContext, helpUrl);
	instance = dialog;
	dialog->create();
#if STRATEGY == STRATEGY_HIDDEN
	dialog->BaseDialog::run();
	dialog->quit("Transcript closing...\n");
#endif
}

void SAL_CALL LoggerDialog::onTerminated() {
	instance = NULL;
#if STRATEGY == STRATEGY_LISTEN
	setVisible(false);
	Reference<XComponent> xComponent(unoControlDialog, UNO_QUERY);
	xComponent->dispose();
#endif
	delete this; // Approved for non-modal dialog
}

void LoggerDialog::quit(const char* message) {
	quitting = true;
	getLog() << std::string(message);
}

void LoggerDialog::onButtonClicked(int id) {
	clear();
}

LoggerDialog::LoggerDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth,
		OUSTRING("Provenance - Transcript"), helpUrl, sal_False) {
	length = 0;
	quitting = false;

	clearListener = ButtonListener(this, btnClearId);

	int x = dlgMargin;
	int y = dlgMargin;
	int tabIndex = 0;

	NamesValuesHelper namesAndValues(4);
	Any anyTrue(sal_True);
	namesAndValues
		.add("MultiLine", anyTrue)
		.add("ReadOnly", anyTrue)
		.add("HScroll", anyTrue)
		.add("VScroll", anyTrue);

	txtLog = insertTextComponent(x, y, logHeight, logWidth,
				OUSTRING("txtLog"), emptyOUString,
				sal_True, tabIndex++, namesAndValues);

	y += logHeight + btnMargin;

	insertStandardButton(x, y, btnHeight, btnWidth,
			OUSTRING("btnClear"), OUSTRING("C~lear"),
			sal_True, tabIndex++, OUSTRING("Clear the transcript text"),
			clearListener);

	insertCloseButton(dlgWidth - dlgMargin - btnWidth, y,
			btnHeight, btnWidth, sal_True, tabIndex++,
			OUSTRING("Close the transcript"));

	setFocus(txtLog);
}

LoggerDialog::~LoggerDialog() {
}

void LoggerDialog::clear() {
	txtLog->setText(OUSTRING(""));
	length = 0;
}

void SAL_CALL LoggerDialog::run() {
	bool done = false;

	while (!done) {
		try {
			// The code can be waiting when the dialog is closed
			// and when it returns, the program will crash
			std::string entry = getLog().getEntry(); 
			done = update(entry);
		}
		catch (...) {
		}
	}
}

bool LoggerDialog::update(std::string text) {
	if (quitting)
		return quitting;
	Selection selection;
	selection.Min = length;
	selection.Max = length;
	txtLog->insertText(selection, convertToOUString(text));
	length += text.length();
	return quitting;
}

void LoggerDialog::onWindowClosed(int id) {
	if (!quitting)
		quit("Transcript closing...\n");
} 
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
