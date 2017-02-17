/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/auditor/AuditorDialog.hpp#1 $

/*
 *  Created by Aaron Gibson on 3/17/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__AUDITOR__AUDITOR_DIALOG_HPP
#	define COM__HAATHI__AUDITOR__AUDITOR_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/auditor/EncryptedProvenanceAuditor.hpp"
#include "com/haathi/dialog/BaseDialog.hpp"
#include "com/haathi/dialog/ListenerTypedefs.hpp"
#include "com/haathi/document/DocumentMetaInfo.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/util/Color.hpp>
#include "sdkEnd.hpp"

#include "boostBeg.hpp"
#include <boost/shared_ptr.hpp>
#include "boostEnd.hpp"

#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace auditor {
/******************************************************************************
Class
******************************************************************************/
class EncryptedProvenanceAuditor;

class AuditorDialog: public com::haathi::dialog::BaseDialog {
	private:
		// These are not implemented.
		AuditorDialog& operator=(const AuditorDialog& other);
		AuditorDialog(const AuditorDialog& other);

	protected:
		static const sal_Int32 lstWidth;
		static const sal_Int32 lstHeight;

		static const sal_Int32 dlgWidth;
		static const sal_Int32 dlgHeight;

		static const int btnFirstId;
		static const int btnPrevId;
		static const int btnNextId;
		static const int btnLastId;
		static const int lstEventsId;
		static const int winOtherId;

		com::haathi::dialog::ListListener eventsListener;
		com::haathi::dialog::ButtonListener firstListener;
		com::haathi::dialog::ButtonListener prevListener;
		com::haathi::dialog::ButtonListener nextListener;
		com::haathi::dialog::ButtonListener lastListener;
		com::haathi::dialog::ClosingListener closingListener;

		Reference<com::sun::star::awt::XListBox> lstEvents;
		Reference<com::sun::star::awt::XButton> btnFirst;
		Reference<com::sun::star::awt::XButton> btnPrev;
		Reference<com::sun::star::awt::XButton> btnNext;
		Reference<com::sun::star::awt::XButton> btnLast;
		Reference<com::sun::star::awt::XFixedText> lblColor;
		Reference<com::sun::star::awt::XFixedText> lblType;
		Reference<com::sun::star::awt::XFixedText> lblDated;

		Reference<com::sun::star::awt::XFixedText> lblStatus11;
		Reference<com::sun::star::awt::XFixedText> lblStatus12;
		Reference<com::sun::star::awt::XFixedText> lblStatus13;

		Reference<com::sun::star::awt::XFixedText> lblStatus21;
		Reference<com::sun::star::awt::XFixedText> lblStatus22;
		Reference<com::sun::star::awt::XFixedText> lblStatus23;

		Reference<com::sun::star::awt::XFixedText> lblStatus31;
		Reference<com::sun::star::awt::XFixedText> lblStatus32;
		Reference<com::sun::star::awt::XFixedText> lblStatus33;

		Reference<com::sun::star::awt::XTextComponent> txtDescription;
		Reference<com::sun::star::awt::XTextComponent> txtFirst;
		Reference<com::sun::star::awt::XTextComponent> txtLast;
		Reference<com::sun::star::awt::XTextComponent> txtEmail;

		Reference<com::sun::star::lang::XComponent> xDocumentComponent;
		boost::shared_ptr<EncryptedProvenanceAuditor> auditor;
		std::map<std::string, com::haathi::network::User> userMap;

		bool otherClosed;

		std::string getFirst(com::haathi::provenance::ProvenanceEvent *provenanceEvent);
		std::string getLast(com::haathi::provenance::ProvenanceEvent *provenanceEvent);

		void onButtonClicked(int id);
		void onListSelected(int id, sal_Int32 selected);
		void onClosed(int id);

		bool isFirstOk();
		bool isPrevOk();
		bool isNextOk();
		bool isLastOk();
		void updateButtons();

		void addEvents();
		void setColor(OUString name, com::sun::star::util::Color color);

	public:
		static void display(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl,
				std::string document, std::vector<std::string>& provenances, com::haathi::document::DocumentMetaInfo documentMetaInfo,
				Reference<com::sun::star::lang::XComponent> xDocumentComponent, std::map<std::string, com::haathi::network::User> userMap);

		AuditorDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext, OUString helpUrl, std::string document,
				std::vector<std::string>& provenances, com::haathi::document::DocumentMetaInfo documentMetaInfo,
				Reference<com::sun::star::lang::XComponent> xDocumentComponent, std::map<std::string, com::haathi::network::User> userMap);
		virtual ~AuditorDialog();
		virtual void toFront();
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
