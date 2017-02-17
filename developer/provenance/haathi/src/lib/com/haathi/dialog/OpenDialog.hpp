/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/OpenDialog.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__OPEN_DIALOG_HPP
#	define COM__HAATHI__DIALOG__OPEN_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/Key.hpp"
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/dialog/BaseDialog.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/NetworkInfo.hpp"

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
class OpenDialog: protected BaseDialog {
	private:
		// These are not implemented.
		OpenDialog& operator=(const OpenDialog& other);
		OpenDialog(const OpenDialog& other);

	protected:
		static const sal_Int32 lstWidth;
		static const sal_Int32 lstHeight;

		static const sal_Int32 dlgWidth;
		static const sal_Int32 dlgHeight;

		static const int lstExistingDocumentsId;
		static const int btnHistoryId;
	
		OUString historyHelpUrl;

		Reference<com::sun::star::awt::XListBox> lstExistingDocuments;
		Reference<com::sun::star::awt::XTextComponent> txtFirst;
		Reference<com::sun::star::awt::XTextComponent> txtLast;
		Reference<com::sun::star::awt::XTextComponent> txtEmail;
		Reference<com::sun::star::awt::XFixedText> lblCreated;
		Reference<com::sun::star::awt::XFixedText> lblModified;
		Reference<com::sun::star::awt::XFixedText> lblVersion;
		Reference<com::sun::star::awt::XButton> btnHistory;
		Reference<com::sun::star::awt::XButton> btnOk;

		ListListener existingDocumentsListener;
		ButtonListener historyListener;

		void onListSelected(int id, sal_Int32 selected);
		void onListDoubleSelected(int id);
		void onButtonClicked(int id);

		std::map<int, com::haathi::crypto::Key> documentLoadKeysMap;
		std::map<int, com::haathi::document::DocumentInfo> documentInfoMap;
		std::vector<DocumentData> documentList;

		com::haathi::network::NetworkInfo& networkInfo;
		com::haathi::document::DocumentMetaInfo documentMetaInfo;

		bool doubleClicked;
		virtual sal_Int16 run();

		bool isOk();
		bool isHistoryOk();
		bool open();

		void addDocuments();
		int getDocumentId();

	public:
		static void display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl, OUString historyHelpUrl,
			com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo documentMetaInfo);

		OpenDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl, OUString historyHelpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo documentMetaInfo);
		virtual ~OpenDialog();
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
