/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/NewDialog.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__NEW_DIALOG_HPP
#	define COM__HAATHI__DIALOG__NEW_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/dialog/BaseDialog.hpp"
#include "com/haathi/dialog/ShareDialog.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/NetworkInfo.hpp"

#include "boostBeg.hpp"
#include <boost/shared_ptr.hpp>
#include "boostEnd.hpp"

#include <map>
#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
Class
******************************************************************************/
class NewDialog: protected BaseDialog {
	private:
		// These are not implemented.
		NewDialog& operator=(const NewDialog& other);
		NewDialog(const NewDialog& other);

	protected:
		static const sal_Int32 lstWidth;
		static const sal_Int32 lstHeight;

		static const sal_Int32 dlgWidth;
		static const sal_Int32 dlgHeight;

		static const int btnShareId;
		static const int txtNameId;
		static const int lstExistingDocumentsId;

		OUString shareHelpUrl;

		Reference<com::sun::star::awt::XListBox> lstExistingDocuments;
		Reference<com::sun::star::awt::XTextComponent> txtFirst;
		Reference<com::sun::star::awt::XTextComponent> txtLast;
		Reference<com::sun::star::awt::XTextComponent> txtEmail;
		Reference<com::sun::star::awt::XFixedText> lblCreated;
		Reference<com::sun::star::awt::XFixedText> lblModified;
		Reference<com::sun::star::awt::XFixedText> lblVersion;
		Reference<com::sun::star::awt::XTextComponent> txtName;
		Reference<com::sun::star::awt::XButton> btnOk;

		ListListener existingDocumentsListener;
		TextListener nameListener;
		ButtonListener shareListener;

		void onListSelected(int id, sal_Int32 selected);
		void onListDoubleSelected(int id);
		void onButtonClicked(int id);
		void onTextEdited(int id);

		std::map<int, com::haathi::document::DocumentInfo> documentInfoMap;
		std::vector<DocumentData> documentList;

		com::haathi::network::NetworkInfo& networkInfo;
		com::haathi::document::DocumentMetaInfo documentMetaInfo;

		boost::shared_ptr<ShareDialog> pShareDialog;

		bool isOk();
		bool isExistingName();
		void addDocuments();
		bool anew();
		virtual sal_Int16 run();

	public:
		static void display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl, OUString shareHelpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo documentMetaInfo);

		NewDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl, OUString shareHelpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo documentMetaInfo);
		virtual ~NewDialog();
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
