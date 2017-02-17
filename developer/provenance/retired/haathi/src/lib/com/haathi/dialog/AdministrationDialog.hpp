/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/dialog/AdministrationDialog.hpp#1 $

/*
 *  Created by Aaron Gibson on 3/31/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef _AdministrationDialog_HPP
#define _AdministrationDialog_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/dialog/LoginDialog.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
Class
******************************************************************************/
class AdministrationDialog: public LoginDialog {
	private:
		// Instance variables.
		Reference<XButton> plus_button;
		Reference<XButton> minus_button;
		Reference<XListBox> user_listbox;
		Reference<XListBox> user_type_listbox;

		void initializeEditPage();

		// We don't implement these functions...
		AdministrationDialog(const AdministrationDialog& other);
		AdministrationDialog& operator=(const AdministrationDialog& other);
	public:
		static void display(Reference<XComponentContext> xContext);

		AdministrationDialog(Reference<XComponentContext> xContext);
		virtual ~AdministrationDialog();
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

