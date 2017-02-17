/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/dialog/LoggerDialog.cpp#1 $

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
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include "com/haathi/utils/StringWrapper.hpp" // convertToOUString, OUSTRING
#include "com/haathi/utils/Logger.hpp" // getLog()
/******************************************************************************
Using
******************************************************************************/
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

using namespace com::haathi::utils;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
LoggerCloseListener
******************************************************************************/
class LoggerCloseListener: public ActionListenerHelper {
	private:
		LoggerDialog* dialog;
	public:
		LoggerCloseListener(LoggerDialog* _dialog): dialog(_dialog) {
		}
		virtual ~LoggerCloseListener() {
		}

		// XEventListener
		virtual void SAL_CALL disposing(const EventObject& eventOb) throw (RuntimeException) {
		}

		// XActionListener
		virtual void SAL_CALL actionPerformed(const ActionEvent& event) throw (RuntimeException) {
			dialog->close();
		}
};
/******************************************************************************
LoggerClearListener
******************************************************************************/
class LoggerClearListener: public ActionListenerHelper {
	private:
		LoggerDialog* dialog;
	public:
		LoggerClearListener(LoggerDialog* _dialog): dialog(_dialog) {
		}
		virtual ~LoggerClearListener() {
		}

		// XEventListener
		virtual void SAL_CALL disposing(const EventObject& eventOb) throw (RuntimeException) {
		}

		// XActionListener
		virtual void SAL_CALL actionPerformed(const ActionEvent& event) throw (RuntimeException) {
			dialog->clear();
		}
};
/******************************************************************************
Class
******************************************************************************/
void LoggerDialog::display(Reference<XComponentContext> xContext) {
	LoggerDialog dialog(xContext);

	dialog.create();
	dialog.BaseDialog::run(); // kwa: previously dialog.BaseDialog::run().  Why?  Need thread run?
	int i=10;
}

LoggerDialog::LoggerDialog(Reference<XComponentContext> xContext):
		BaseDialog(xContext, sal_False), log("<----------- Log Start ------------>\n") {
	try {
		setWindowProperties(50, 50, 212, 200,
				OUSTRING("LoggerWindow"));

		// Insert the text field and enable it to handle multiple lines of text.
		Sequence<OUString> names(2);
		Sequence<Any> values(2);
		names[0] = OUSTRING("MultiLine");
		values[0] <<= sal_True;
		names[1] = OUSTRING("ReadOnly");
		values[1] <<= sal_True;

		text_field = insertTextField(0, 0, 200, 200,
				OUSTRING("LoggerTextField"), convertToOUString(log),
				names, values);

		// Insert the buttons.
		LoggerCloseListener* close_listener = new LoggerCloseListener(this);
		Reference<XActionListener> xclose_listener =
				static_cast<XActionListener*>(close_listener);
		insertButton(xclose_listener, 150, 200, 50,
				OUSTRING("LoggerCloseButton"),
				OUSTRING("Close"));

		LoggerClearListener* clear_listener = new LoggerClearListener(this);
		Reference<XActionListener> xclear_listener =
				static_cast<XActionListener*>(clear_listener);
		insertButton(xclear_listener, 0, 200, 50,
				OUSTRING("LoggerClearButton"),
				OUSTRING("Clear"));
	}
	catch (com::sun::star::uno::Exception & e) {
		std::cerr << "uno Exception: " << e.Message << std::endl;
	}
	catch (std::exception & e1) {
		std::cerr << "std Exception: " << e1.what() << std::endl;
	}
}

LoggerDialog::~LoggerDialog() {
}

void LoggerDialog::clear() {
	log = "";
	update();
}

void LoggerDialog::run() {
	bool good = true;
	// kwa: Need to sleep somewhere or add update button?
	while (good) {
		log += getLog().getEntry();
		update();
	}
}

void LoggerDialog::update() {
	// Update the log's text.
	text_field->setText(convertToOUString(log));
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
