/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/BaseDialog.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__BASE_DIALOG_HPP
#	define COM__HAATHI__DIALOG__BASE_DIALOG_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/dialog/ListenerTypedefs.hpp"
#include "com/haathi/dialog/MessageBox.hpp"
#include "com/haathi/network/NetworkException.hpp"
#include "com/haathi/network/Packet.hpp"
#include "com/haathi/utils/Debug.hpp"
#include "com/haathi/utils/NamesValuesHelper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/ActionEvent.hpp>
#include <com/sun/star/awt/ItemEvent.hpp>
#include <com/sun/star/awt/PushButtonType.hpp>
#include <com/sun/star/awt/TextEvent.hpp>
#include <com/sun/star/awt/XButton.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XFixedText.hpp>
#include <com/sun/star/awt/XListBox.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/lang/EventObject.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/CloseVetoException.hpp>
#include "sdkEnd.hpp"

#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
DocumentData
******************************************************************************/
struct DocumentData {
	int id;
	std::string name;
	std::string uname;

	DocumentData(int id, std::string name);
	int compare(DocumentData& other);
};
/******************************************************************************
DialogMessageBoxProvider
******************************************************************************/
class DialogMessageBoxProvider: public MessageBoxProvider {
	protected:
		Reference<com::sun::star::awt::XToolkit>* xToolkit;
		Reference<com::sun::star::awt::XWindowPeer>* xWindowPeer;
		rtl::OUString* title;
		virtual rtl::OUString exceptionMessage(com::haathi::network::ResponsePacket& responsePacket);

	public:
		DialogMessageBoxProvider();
		void initializeDialogMessageBoxProvider(Reference<com::sun::star::awt::XToolkit>* xToolkit,
				Reference<com::sun::star::awt::XWindowPeer>* xWindowPeer, rtl::OUString* title);
		virtual ~DialogMessageBoxProvider();

		virtual sal_Int16 showErrorMessageBox(com::haathi::network::ResponsePacket& responsePacket);
		virtual sal_Int16 showErrorMessageBox(com::haathi::network::NetworkException& exception);
		virtual sal_Int16 showErrorMessageBox(std::exception& exception);
		virtual sal_Int16 showErrorMessageBox(com::sun::star::uno::Exception& exception);
		virtual sal_Int16 showErrorMessageBox(const char* message);
		
		sal_Int16 showErrorMessageBox(rtl::OUString message);
		sal_Int16 showInfoMessageBox(rtl::OUString message);
		sal_Int16 showQueryMessageBox(rtl::OUString message);
};
/******************************************************************************
Listeners
******************************************************************************/			
class ButtonClickListener {
	public:
		virtual void onButtonClicked(int id) = 0;
		virtual ~ButtonClickListener();
};			

class TextEditListener {
	public:
		virtual void onTextEdited(int id) = 0;
		virtual ~TextEditListener();
};

class ListSelectListener {
	public:
		virtual void onListSelected(int id, sal_Int32 selected) = 0;
		virtual void onListDoubleSelected(int id) = 0;
		virtual ~ListSelectListener();
};

class CloseListener {
	public:
		virtual void onClosed(int id) = 0;
		virtual ~CloseListener();
};

class WindowListener {
	public:
		virtual void onWindowClosed(int id) = 0;
		virtual ~WindowListener();
};
/******************************************************************************
Helpers
******************************************************************************/
class ButtonListener: public XActionListenerHelper {
	protected:
		ButtonClickListener* listener;
		int id;
	public:
		ButtonListener();
		ButtonListener(ButtonClickListener* listener, int id);
		ButtonListener& operator=(const ButtonListener& other);
		ButtonListener(const ButtonListener& other);

		virtual ~ButtonListener();
		virtual void SAL_CALL actionPerformed(const com::sun::star::awt::ActionEvent& event)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& object)
				throw (com::sun::star::uno::RuntimeException);
};

class TextListener: public XTextListenerHelper {
	protected:
		TextEditListener* listener;
		int id;
	public:
		TextListener();
		TextListener(TextEditListener* listener, int id);
		TextListener& operator=(const TextListener& other);
		TextListener(const TextListener& other);

		virtual ~TextListener();
		virtual void SAL_CALL textChanged(const com::sun::star::awt::TextEvent& event)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& object)
				throw (com::sun::star::uno::RuntimeException);
};

class ListListener: public XItemListenerHelper, public XActionListenerHelper {
	protected:
		ListSelectListener* listener;
		int id;
	public:
		ListListener();
		ListListener(ListSelectListener* listener, int id);
		ListListener& operator=(const ListListener& other);
		ListListener(const ListListener& other);
		virtual ~ListListener();
		virtual void SAL_CALL itemStateChanged(const com::sun::star::awt::ItemEvent& event)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL actionPerformed(const com::sun::star::awt::ActionEvent& event)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& object)
				throw (com::sun::star::uno::RuntimeException);
};

class ClosingListener: public XCloseListenerHelper {
	protected:
		CloseListener* listener;
		int id;
	public:
		ClosingListener();
		ClosingListener(CloseListener* listener, int id);
		ClosingListener& operator=(const ClosingListener& other);
		ClosingListener(const ClosingListener& other);
		virtual ~ClosingListener();
		virtual void SAL_CALL queryClosing(const com::sun::star::lang::EventObject& object, sal_Bool getsOwnership)
				throw (com::sun::star::util::CloseVetoException, com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL notifyClosing(const com::sun::star::lang::EventObject& object)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& object)
				throw (com::sun::star::uno::RuntimeException);
};

class TopWindowListener: public XTopWindowListenerHelper {
	protected:
		WindowListener* listener;
		int id;
	public:
		TopWindowListener();
		TopWindowListener(WindowListener* listener, int id);
		TopWindowListener& operator=(const TopWindowListener& other);
		TopWindowListener(const TopWindowListener& other);
		virtual ~TopWindowListener();
		virtual void SAL_CALL windowActivated(const com::sun::star::lang::EventObject& eventObject)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL windowClosed(const com::sun::star::lang::EventObject& eventObject)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL windowClosing(const com::sun::star::lang::EventObject& eventObject)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL windowDeactivated(const com::sun::star::lang::EventObject& eventObject)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL windowMinimized(const com::sun::star::lang::EventObject& eventObject)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL windowNormalized(const com::sun::star::lang::EventObject& eventObject)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL windowOpened(const com::sun::star::lang::EventObject& eventObject)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& object)
				throw (com::sun::star::uno::RuntimeException);
};
/******************************************************************************
BaseDialog
******************************************************************************/
class BaseDialog: public DialogMessageBoxProvider, protected ButtonClickListener,
		protected TextEditListener, protected ListSelectListener, protected CloseListener,
		protected WindowListener {
	protected:
		static const std::string emptyString;
		static const rtl::OUString emptyOUString;
		static const com::haathi::utils::NamesValuesHelper emptyNamesValuesHelper;

		static const sal_Int32 margin;

		// Margins are for unlike neighbors
		static const sal_Int32 btnMargin;
		static const sal_Int32 dlgMargin;
		static const sal_Int32 lstMargin;

		// Spaces are for like neighbors
		static const sal_Int32 lblSpace;
		static const sal_Int32 btnSpace;
		static const sal_Int32 txtSpace;

		static const sal_Int32 lblHeight;
		static const sal_Int32 txtHeight;
		static const sal_Int32 btnHeight;

		static const sal_Int32 lblAdjust;

		static const sal_Int32 lblWidth;
		static const sal_Int32 txtWidth;
		static const sal_Int32 btnWidth;

		Reference<com::sun::star::uno::XInterface> unoControlDialogModel;
		Reference<com::sun::star::uno::XInterface> unoControlDialog;

		Reference<com::sun::star::uno::XComponentContext> xComponentContext;
		Reference<com::sun::star::lang::XMultiServiceFactory> xMultiServiceFactory;
		Reference<com::sun::star::container::XNameContainer> xNameContainer;
		Reference<com::sun::star::awt::XControlContainer> xControlContainer;
		Reference<com::sun::star::awt::XControl> xControl;

		Reference<com::sun::star::awt::XWindowPeer> xWindowPeer;
		Reference<com::sun::star::awt::XToolkit> xToolkit;

		TopWindowListener topWindowListener;

		rtl::OUString title;
		// These measurements are in pixels
		sal_Int32 height;
		sal_Int32 width;
		sal_Int32 parentHeight;
		sal_Int32 parentWidth;
		bool isModal;

		sal_Int16 dialogResult;

		void enable(Reference<com::sun::star::awt::XControl> xControl, bool enabled);
		void enable(Reference<com::sun::star::awt::XButton> xButton, bool enabled);
		void setFocus(Reference<com::sun::star::awt::XButton> xButton);
		void setFocus(Reference<com::sun::star::awt::XTextComponent> xTextComponent);
		void setFocus(Reference<com::sun::star::awt::XListBox> xListBox);

		Reference<com::sun::star::uno::XInterface> newUnoControlDialogModel(
				Reference<com::sun::star::lang::XMultiServiceFactory> xMultiServiceFactory,
				sal_Int32 height, sal_Int32 width, rtl::OUString title, OUString helpUrl);

		void init(sal_Int32 height, sal_Int32 width, OUString helpUrl);
		void center();
		void setVisible(bool visible);

		rtl::OUString getURLToExtensionRoot();
		std::string getSystemFilePathToExtensionFile(const char* file);
		void sortDocumentList(std::vector<DocumentData>& documentList);

		virtual void onListSelected(int id, sal_Int32 selected);
		virtual void onListDoubleSelected(int id);
		virtual void onTextEdited(int id);
		virtual void onButtonClicked(int id);
		virtual void onClosed(int id);
		virtual void onWindowClosed(int id);

		void connectTextListener(Reference<com::sun::star::awt::XTextComponent>& xTextComponent, TextListener* textListener);
		void connectClosingListener(Reference<com::sun::star::lang::XComponent>& xComponent, ClosingListener* closingListener);
		void disconnectClosingListener(Reference<com::sun::star::lang::XComponent>& xComponent, ClosingListener* closingListener);
		void connectTopWindowListener(Reference<com::sun::star::awt::XControl>& xControl, TopWindowListener* topWindowListener);
		void disconnectTopWindowListener(Reference<com::sun::star::awt::XControl>& xControl, TopWindowListener* topWindowListener);

		ClosingListener closingListener;

	public:
		// height and width here are in dialog box units
		BaseDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				sal_Int32 height, sal_Int32 width, rtl::OUString title, OUString helpUrl, bool isModel = true);
		BaseDialog(Reference<com::sun::star::uno::XComponentContext> xComponentContext,
				sal_Int32 height, sal_Int32 width, rtl::OUString title, OUString helpUrl,
				sal_Int32 parentHeight, sal_Int32 parentWidth, bool isModel = true);
		virtual ~BaseDialog();

		// The dummy argument is to avoid a name clash with the LoggerDialog
		virtual sal_Int16 run(sal_Bool dummy = sal_False);

		void close();
		virtual void toFront();

		virtual sal_Int16 execute();
		void endExecute();

	protected:
		Reference<com::sun::star::awt::XControl> insertByName(rtl::OUString& name, Reference<com::sun::star::uno::XInterface>& model);

		void insertLine(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 length, rtl::OUString name, sal_Bool isHorizontal,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XFixedText> insertFixedText(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width, rtl::OUString name, rtl::OUString label,
				sal_Bool tabstop, sal_Int16 tabIndex,
				rtl::OUString helpText,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XTextComponent> insertTextComponent(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width, rtl::OUString name, rtl::OUString initialText,
				sal_Bool tabstop, sal_Int16 tabIndex,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XTextComponent> insertReadOnlyTextComponent(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width, rtl::OUString name, rtl::OUString initialText,
				sal_Bool tabstop, sal_Int16 tabIndex,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XTextComponent> insertPasswordComponent(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width, rtl::OUString name, rtl::OUString initialText,
				sal_Bool tabstop, sal_Int16 tabIndex,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XButton> insertButton(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width, rtl::OUString name, rtl::OUString label,
				sal_Bool tabstop, sal_Int16 tabIndex,
				sal_Bool defaultButton, com::sun::star::awt::PushButtonType pushButtonType,
				rtl::OUString helpText,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XButton> insertOkButton(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width,	
				sal_Bool tabstop, sal_Int16 tabIndex,
				rtl::OUString helpText,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XButton> insertCloseButton(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width,	
				sal_Bool tabstop, sal_Int16 tabIndex,
				rtl::OUString helpText,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XButton> insertCancelButton(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width,	
				sal_Bool tabstop, sal_Int16 tabIndex,
				rtl::OUString helpText,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XButton> insertStandardButton(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width, rtl::OUString name, rtl::OUString label,
				sal_Bool tabstop, sal_Int16 tabIndex,
				rtl::OUString helpText, ButtonListener& listener,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);

		Reference<com::sun::star::awt::XListBox> insertListBox(sal_Int32 posx, sal_Int32 posy,
				sal_Int32 height, sal_Int32 width, rtl::OUString name,
				sal_Bool tabstop, sal_Int16 tabIndex,
				ListListener& listener, sal_Bool multiSelection = sal_False,
				com::haathi::utils::NamesValuesHelper namesValuesHelper = emptyNamesValuesHelper);
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
