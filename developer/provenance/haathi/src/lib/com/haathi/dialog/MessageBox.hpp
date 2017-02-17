/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/dialog/MessageBox.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DIALOG__MESSAGE_BOX_HPP
#	define COM__HAATHI__DIALOG__MESSAGE_BOX_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/network/NetworkException.hpp"
#include "com/haathi/network/Packet.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/awt/MessageBoxType.hpp>
#include <com/sun/star/awt/XMessageBoxFactory.hpp>
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "sdkEnd.hpp"

#include <exception>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace dialog {
/******************************************************************************
Class
******************************************************************************/
class MessageBoxProvider {
	protected:
		virtual rtl::OUString exceptionMessage(com::haathi::network::NetworkException& exception);
		virtual rtl::OUString exceptionMessage(std::exception& exception);
		virtual rtl::OUString exceptionMessage(com::sun::star::uno::Exception& exception);
		virtual rtl::OUString exceptionMessage(const char* message);

	public:
		virtual sal_Int16 showErrorMessageBox(rtl::OUString message) = 0;
		virtual sal_Int16 showInfoMessageBox(rtl::OUString message) = 0;
		virtual sal_Int16 showQueryMessageBox(rtl::OUString message) = 0;

		virtual sal_Int16 showErrorMessageBox(com::haathi::network::NetworkException& exception);
		virtual sal_Int16 showErrorMessageBox(std::exception& exception);
		virtual sal_Int16 showErrorMessageBox(com::sun::star::uno::Exception& exception);
		virtual sal_Int16 showErrorMessageBox(const char* message);
};

class MessageBox {
	protected:
		com::sun::star::uno::Reference<com::sun::star::awt::XToolkit> xToolkit;
		com::sun::star::uno::Reference<com::sun::star::awt::XWindowPeer> xWindowPeer;

		static sal_Int16 executeMessageBox(com::sun::star::uno::Reference<com::sun::star::awt::XMessageBoxFactory> xMessageBoxFactory,
				com::sun::star::uno::Reference<com::sun::star::awt::XWindowPeer> xWindowPeer, com::sun::star::awt::MessageBoxType messageBoxType,
				sal_Int32 buttons, const rtl::OUString& title, const rtl::OUString& message);
	public:
		MessageBox(com::sun::star::uno::Reference<com::sun::star::lang::XMultiServiceFactory> xMultiServiceFactory);
		MessageBox(com::sun::star::uno::Reference<com::sun::star::uno::XComponentContext> xComponentContext);

		virtual sal_Int16 showErrorMessageBox(rtl::OUString title, rtl::OUString message);
		virtual sal_Int16 showInfoMessageBox(rtl::OUString title, rtl::OUString message);
		virtual sal_Int16 showQueryMessageBox(rtl::OUString title, rtl::OUString message);

		static sal_Int16 showErrorMessageBox(com::sun::star::uno::Reference<com::sun::star::awt::XToolkit> xToolkit,
				com::sun::star::uno::Reference<com::sun::star::awt::XWindowPeer> xWindowPeer, const rtl::OUString& title, const rtl::OUString& message);
		static sal_Int16 showInfoMessageBox(com::sun::star::uno::Reference<com::sun::star::awt::XToolkit> xToolkit,
				com::sun::star::uno::Reference<com::sun::star::awt::XWindowPeer> xWindowPeer, const rtl::OUString& title, const rtl::OUString& message);
		static sal_Int16 showQueryMessageBox(com::sun::star::uno::Reference<com::sun::star::awt::XToolkit> xToolkit,
				com::sun::star::uno::Reference<com::sun::star::awt::XWindowPeer> xWindowPeer, const rtl::OUString& title, const rtl::OUString& message);
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
