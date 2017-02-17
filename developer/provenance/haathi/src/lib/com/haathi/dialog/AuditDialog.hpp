/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/AuditDialog.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__AUDIT_DIALOG_HPP
#	define COM__HAATHI__DIALOG__AUDIT_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/Key.hpp"
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
class AuditDialog: protected BaseDialog {
	private:
		// These are not implemented.
		AuditDialog& operator=(const AuditDialog& other);
		AuditDialog(const AuditDialog& other);

	protected:
		static const sal_Int32 lstWidth;
		static const sal_Int32 lstHeight;

		static const sal_Int32 dlgWidth;
		static const sal_Int32 dlgHeight;

		static const int lstExistingDocumentsId;
		static const int btnHistoryId;
	
		OUString historyHelpUrl;
		OUString auditorHelpUrl;

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

		com::haathi::document::DocumentMetaInfo documentMetaInfo;
		com::haathi::network::NetworkInfo& networkInfo;

		bool doubleClicked;
		virtual sal_Int16 run();

		bool isOk();
		bool isHistoryOk();
		bool audit();

		void addDocuments();
		int getDocumentId();

	public:
		static void display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl, OUString historyHelpUrl, OUString auditorHelpUrl,
			com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo documentMetaInfo);
		AuditDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl, OUString historyHelpUrl, OUString auditorHelpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo documentMetaInfo);
		virtual ~AuditDialog();
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
