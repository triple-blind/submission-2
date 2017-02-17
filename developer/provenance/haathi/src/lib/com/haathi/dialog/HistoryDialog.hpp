/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/HistoryDialog.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__HISTORY_DIALOG_HPP
#	define COM__HAATHI__DIALOG__HISTORY_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
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
class HistoryDialog: protected BaseDialog {
	private:
		// These are not implemented.
		HistoryDialog& operator=(const HistoryDialog& other);
		HistoryDialog(const HistoryDialog& other);

	protected:
		static const sal_Int32 lstWidth;
		static const sal_Int32 lstHeight;

		static const sal_Int32 dlgWidth;
		static const sal_Int32 dlgHeight;

		static const int lstVersionsId;

		Reference<com::sun::star::awt::XListBox> lstVersions;
		Reference<com::sun::star::awt::XTextComponent> txtFirst;
		Reference<com::sun::star::awt::XTextComponent> txtLast;
		Reference<com::sun::star::awt::XTextComponent> txtEmail;
		Reference<com::sun::star::awt::XFixedText> txtDated;

		ListListener versionsListener;

		virtual void onListSelected(int id, sal_Int32 selected);

		com::haathi::network::NetworkInfo& networkInfo;
		com::haathi::document::DocumentMetaInfo& documentMetaInfo;
		int documentId;
		std::vector<com::haathi::network::Version> versions;
		std::map<int, com::haathi::network::User> userMap;

		void addVersions();
		com::haathi::network::User getUser(int userId);
		void init();

	public:
		static void display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo& documentMetaInfo,
				int documentId);
		static void display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo& documentMetaInfo,
				sal_Int32 height, sal_Int32 width, int documentId);

		HistoryDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo& documentMetaInfo,
				int documentId);
		HistoryDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo& documentMetaInfo,
				sal_Int32 height, sal_Int32 width, int documentId);
		virtual ~HistoryDialog();
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
