/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/LoggerDialog.hpp#1 $

/*
 *  Created by Aaron Gibson on 3/16/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__LOGGER_DIALOG_HPP
#	define COM__HAATHI__DIALOG__LOGGER_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/dialog/BaseDialog.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
#include <osl/thread.hxx>
#include "sdkEnd.hpp"

#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
Class
******************************************************************************/
class LoggerDialog: public BaseDialog, public osl::Thread {
	private:
		static LoggerDialog* instance;
		Reference<com::sun::star::awt::XTextComponent> txtLog;

	protected:
		static const sal_Int32 logWidth;
		static const sal_Int32 logHeight;
		static const sal_Int32 dlgWidth;
		static const sal_Int32 dlgHeight;

		static const int btnClearId;

		ButtonListener clearListener;
		virtual void onButtonClicked(int id);

		int length;
		bool quitting;

		virtual void SAL_CALL run();
		virtual void SAL_CALL onTerminated();

		void quit(const char* message);
		bool update(std::string text);
		void onWindowClosed(int id);
	public:
		static void display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl);

		LoggerDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl);
		virtual ~LoggerDialog();

		void clear();
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
