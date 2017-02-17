/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/AboutDialog.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/dialog/AboutDialog.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/NamesValuesHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
/******************************************************************************
Define
******************************************************************************/

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
const sal_Int32 AboutDialog::aboutWidth = 188;
const sal_Int32 AboutDialog::aboutHeight = 184;
const sal_Int32 AboutDialog::dlgWidth = dlgMargin + aboutWidth + dlgMargin;
const sal_Int32 AboutDialog::dlgHeight = dlgMargin + aboutHeight + btnMargin +
		btnHeight + dlgMargin;

void AboutDialog::display(Reference<XComponentContext> xComponentContext, OUString helpUrl) {
	AboutDialog(xComponentContext, helpUrl).run();
}

AboutDialog::AboutDialog(Reference<XComponentContext> xComponentContext, OUString helpUrl):
		BaseDialog(xComponentContext, dlgHeight, dlgWidth,
		OUSTRING("Provenance - About"), helpUrl) {
	int x = dlgMargin;
	int y = dlgMargin;
	int tabIndex = 0;
	OUString text = OUSTRING(
			"This extension allows for creating provenance-enabled documents with multiple collaborators, while storing the provenance securely in the cloud.\n\n"
			"Authors: Aaron Gibson, Nitin Shinde, Sam Martin, Keith Alcock\n\n"
			"Configuration: The SmartTextDocumentListener takes care of document differencing. Watermarking is supplied by the BothwhereEngine using the FgLuminanceScheme with the StyleEmbedder.");

	NamesValuesHelper namesAndValues(4);
	Any anyTrue(sal_True);
	Any anyFalse(sal_False);
	namesAndValues
		.add("MultiLine", anyTrue)
		.add("ReadOnly", anyTrue)
		.add("HScroll", anyFalse)
		.add("VScroll", anyTrue);

	txtAbout = insertTextComponent(x, y, aboutHeight, aboutWidth,
				OUSTRING("txtAbout"), text,
				sal_True, tabIndex++, namesAndValues);

	y += aboutHeight + btnMargin;

	Reference<XButton> btnClose = insertCloseButton(dlgWidth - dlgMargin - btnWidth, y,
			btnHeight, btnWidth, sal_True, tabIndex++,
			OUSTRING("Close the dialog"));

	setFocus(btnClose);
}

AboutDialog::~AboutDialog() {
}

sal_Int16 AboutDialog::run() {
	bool done = false;
	sal_Int16 result;

	while (!done) {
		result = BaseDialog::run();
		done = true;
	}
	return dialogResult = result;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
