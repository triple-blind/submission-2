/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/ShareDialog.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__SHARE_DIALOG_HPP
#	define COM__HAATHI__DIALOG__SHARE_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/dialog/BaseDialog.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/network/NetworkInfo.hpp"
#include "com/haathi/network/Packet.hpp"   // User

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
class ShareDialog: protected BaseDialog {
	private:
		// These are not implemented.
		ShareDialog& operator=(const ShareDialog& other);
		ShareDialog(const ShareDialog& other);

	protected:
		static const sal_Int32 lstWidth;
		static const sal_Int32 lstHeight;

		static const sal_Int32 dlgWidth;
		static const sal_Int32 dlgHeight;

		static const int lstNewSharersId;
		static const int lstOldSharersId;

		Reference<com::sun::star::awt::XListBox> lstNewSharers;
		Reference<com::sun::star::awt::XTextComponent> txtNewFirst;
		Reference<com::sun::star::awt::XTextComponent> txtNewLast;
		Reference<com::sun::star::awt::XTextComponent> txtNewEmail;
		Reference<com::sun::star::awt::XListBox> lstOldSharers;
		Reference<com::sun::star::awt::XTextComponent> txtOldFirst;
		Reference<com::sun::star::awt::XTextComponent> txtOldLast;
		Reference<com::sun::star::awt::XTextComponent> txtOldEmail;

		ListListener newSharersListener;
		ListListener oldSharersListener;

		virtual void onListSelected(int id, sal_Int32 selected);

		std::vector<com::haathi::network::User> allOtherUsers;
		std::vector<com::haathi::network::User> oldSharers;
		std::vector<com::haathi::network::User> newSharers;

		com::haathi::network::NetworkInfo& networkInfo;
		com::haathi::document::DocumentMetaInfo& documentMetaInfo;
		bool hasDocument;
		com::haathi::network::User me;

		bool isOk();
		void fillAllOtherUsers();
		void fillNewSharers();
		void fillOldSharers(bool hasDocument);
		void clearNewSharers();
		com::haathi::network::User getMe(com::haathi::network::UserInfo& userInfo);
		bool share();
		void init();

	public:
		virtual sal_Int16 run();
		std::vector<int> getNewSharerIds();
		std::vector<std::string> getNewSharerEmails();

		static sal_Int16 display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl,
			com::haathi::network::NetworkInfo& networkInfo,
			com::haathi::document::DocumentMetaInfo& documentMetaInfo, bool hasDocument);

		ShareDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo& documentMetaInfo, bool hasDocumeent);
		ShareDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl,
				com::haathi::network::NetworkInfo& networkInfo, com::haathi::document::DocumentMetaInfo& documentMetaInfo,
				sal_Int32 height, sal_Int32 width, bool hasDocumeent);
		virtual ~ShareDialog();
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
