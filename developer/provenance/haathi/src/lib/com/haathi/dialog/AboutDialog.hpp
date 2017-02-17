/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/AboutDialog.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__ABOUT_DIALOG_HPP
#	define COM__HAATHI__DIALOG__ABOUT_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/dialog/BaseDialog.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
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
class AboutDialog: public BaseDialog {
	private:
		Reference<com::sun::star::awt::XTextComponent> txtAbout;

	protected:
		static const sal_Int32 aboutWidth;
		static const sal_Int32 aboutHeight;
		static const sal_Int32 dlgWidth;
		static const sal_Int32 dlgHeight;

		virtual sal_Int16 run();
	public:
		static void display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl);

		AboutDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl);
		virtual ~AboutDialog();
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
