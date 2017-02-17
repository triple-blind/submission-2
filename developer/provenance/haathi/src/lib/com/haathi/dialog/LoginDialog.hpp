/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/LoginDialog.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__LOGIN_DIALOG_HPP
#	define COM__HAATHI__DIALOG__LOGIN_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/dialog/BaseDialog.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

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
class LoginDialog: protected BaseDialog {
	private:
		// These are not implemented.
		LoginDialog& operator=(const LoginDialog& other);
		LoginDialog(const LoginDialog& other);

	protected:
		static const sal_Int32 dlgWidth;
		static const sal_Int32 dlgHeight;

		static const int btnTestId;
		static const int txtEmailId;
		static const int txtPasswordId;

		static bool loggedIn;
		static com::haathi::document::DocumentMetaInfo documentMetaInfo;
		static com::haathi::network::NetworkInfo networkInfo;
		static rtl::OUString mostRecentFirstAndLast;

		ButtonListener testListener;
		TextListener emailListener;
		TextListener passwordListener;

		Reference<com::sun::star::awt::XTextComponent> txtEmail;
		Reference<com::sun::star::awt::XTextComponent> txtPassword;
		Reference<com::sun::star::awt::XButton> btnOk;

		void onButtonClicked(int id);
		void onTextEdited(int id);
		
		bool isOk();
		
		bool login();
		void test();
		com::haathi::network::NetworkInfo loadNetworkInfo();
		
		virtual sal_Int16 run();
	public:
		static bool isLoggedIn();
		static void logout();
		static rtl::OUString& getFirstAndLast();
		static com::haathi::document::DocumentMetaInfo getDocumentMetaInfo(); // might get changed when edit document, so need to copy
		static com::haathi::network::NetworkInfo& getNetworkInfo();

		static void display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl);

		LoginDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl);
		virtual ~LoginDialog();
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
