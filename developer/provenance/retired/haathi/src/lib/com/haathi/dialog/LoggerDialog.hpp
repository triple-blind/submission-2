/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/dialog/LoggerDialog.hpp#1 $

/*
 *  Created by Aaron Gibson on 3/16/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef _PROVENANCE_DIALOG_LOGGER_HPP
#define _PROVENANCE_DIALOG_LOGGER_HPP
/******************************************************************************
Include
******************************************************************************/
#include <com/sun/star/uno/XComponentContext.hpp>

#include <osl/thread.hxx>

#include "com/haathi/dialog/BaseDialog.hpp" // X...Component
/******************************************************************************
Using
******************************************************************************/
using namespace com::sun::star::awt;
//using namespace com::sun::star::lang;
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
class LoggerDialog: public BaseDialog, public osl::Thread {
	private:
		std::string log;
		Reference<XTextComponent> text_field;

	public:
		static void display(Reference<XComponentContext> xContext);

		LoggerDialog(Reference<XComponentContext> xContext);
		virtual ~LoggerDialog();

		void clear();
		void update();
		virtual void SAL_CALL run();
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
