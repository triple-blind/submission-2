/*
 *  Created by Aaron Gibson on 3/31/12.
 */

#include "com/haathi/dialog/AdministrationDialog.hpp"

#include "com/haathi/utils/ExtensionHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

namespace com {
namespace haathi {
namespace dialog {

#define ADMIN_DIALOG_LOGIN_PAGE 1
#define ADMIN_DIALOG_EDIT_PAGE	2

#define ADMIN_DIALOG_INITIAL_LOGINNAME "auditor@gmail.com"

void AdministrationDialog::display(Reference<XComponentContext> xContext) {
	AdministrationDialog(xContext).run();
}

AdministrationDialog::AdministrationDialog(Reference<XComponentContext> xContext):
		LoginDialog(xContext) {
	Any val;
	setWindowProperties(50, 50, 200, 300, OUSTRING("Administrate..."));

	val <<= getGraphicFromFile("/images/background.jpg");
	windowProps->setPropertyValue(OUSTRING("Graphic"), val);

	// Initialize each of the components.
	initializeEditPage();
}
/*
AdministrationDialog::AdministrationDialog(Reference<XComponentContext> rContext) :
		LoginDialog(rContext) {
	Any val;
	setWindowProperties(50, 50, 200, 300, OUSTRING("Administrate..."));

	val <<= com::haathi::utils::getGraphicFromFile(
			OUSTRING("/images/background.jpg"));
	windowProps->setPropertyValue(OUSTRING("Graphic"), val);

	// Initialize each of the components.
	initializeEditPage();
}
*/
AdministrationDialog::~AdministrationDialog() {

}

void AdministrationDialog::initializeEditPage() {
	// First, we have the UserType listbox.
	Sequence<OUString> listboxProps(2);
	Sequence<Any> listboxVals(2);
	listboxProps[0] = OUSTRING("Dropdown");
	listboxVals[0] <<= sal_True;
	listboxProps[1] = OUSTRING("HelpText");
	listboxVals[1] <<= OUSTRING("Select the user type here.");
//	user_type_listbox = insertListBox(ADMIN_DIALOG_LOGIN_PAGE);

}

}
;
}
;
}
;
// namespace com::haathi::dialog
