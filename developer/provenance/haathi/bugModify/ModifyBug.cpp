/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/bugModify/ModifyBug.cpp#4 $
/******************************************************************************
Include others
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/EventObject.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/text/XParagraphCursor.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/view/XMultiSelectionSupplier.hpp>

#include <cppuhelper/bootstrap.hxx>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>

#include <sal/main.h> // Required for this to be a free-standing executable.

#include <rtl/ustrbuf.hxx>
#include "sdkEnd.hpp"

#include <iostream>
/******************************************************************************
Using
******************************************************************************/
using namespace com::sun::star::awt;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
using namespace com::sun::star::view;
/******************************************************************************
ModifyBug
******************************************************************************/
class MyModifyListener: public cppu::WeakImplHelper1<com::sun::star::util::XModifyListener> {
	public:
		MyModifyListener(Reference<XComponent> xComponent): xComponent(xComponent) {
			int k = 10;
		}

		virtual OUString getString(Reference<XText> xText) {
			//return OUString();

			// Use previous cursor and maybe just set range to current one
			Reference<XTextCursor> xTextCursor = xText->createTextCursor();
			xTextCursor->collapseToStart();
			Reference<XParagraphCursor> xParagraphCursor(xTextCursor, UNO_QUERY);
			rtl::OUStringBuffer buffer;
			bool isMore;

			do { // The document must have have at least one paragraph by design.
				xParagraphCursor->gotoEndOfParagraph(sal_True);
				buffer.append(xParagraphCursor->getString());
				isMore = xParagraphCursor->gotoNextParagraph(sal_False) == sal_True;
				if (isMore)
					buffer.append((sal_Unicode) '\r');  // Close the paragraph with \r.
			} while (isMore);
			OUString string = buffer.makeStringAndClear();

			return string;
		}

		virtual void SAL_CALL test() {
		}

		virtual void SAL_CALL modified(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
			Reference<XText> xText = xTextDocument->getText();

			//Reference<XTextCursor> xTextCursor1 = xText->createTextCursor();
			//Reference<XTextCursor> xTextCursor2 = xText->createTextCursor();
			//Reference<XTextCursor> xTextCursor3 = xText->createTextCursor();
			//OUString text = getString(xText);

			//xText->getString(); // This works, unless just return from getString above
//			Reference<XTextRange> xTextRange = xText->getStart(); // This usually doesn't
//			Reference<XTextRange> xTextRange = xText->getEnd(); // This usually doesn't
			
			//test(); // 		virtual void SAL_CALL test() { works
			
			//test(); // 		void SAL_CALL test() { works doesn't
			//test(); // 		void test() { doesn't
			//test(xText); // 
			//test(xText); //		OUString test(Reference<XText> xText) { doesn't!
			getString(xText); 	//	void test(Reference<XText> xText) { doesn't
			// But in this test the getStart comes after the test, not before.
			//Reference<XTextRange> xTextRange = xText->getStart(); // This usually doesn't
			//getString(xText);
			//getString(xText);


			//xText->getString();
//			text = xText->getString(); // This works
			// Will hang on return
		}

		virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException) {
			int j = 20;
		}

		virtual ~MyModifyListener() {
			int j = 20;
		}
	protected:
		Reference<XComponent> xComponent;
};

class ModifyBug: public cppu::WeakImplHelper1<com::sun::star::util::XModifyListener> /*,
		com::sun::star::awt::XWindowListener> */{
	protected:
		virtual void SAL_CALL modified(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException);

		virtual void SAL_CALL windowResized(const com::sun::star::awt::WindowEvent& event)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL windowMoved(const com::sun::star::awt::WindowEvent& event)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL windowShown(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL windowHidden(const com::sun::star::lang::EventObject& event)
				throw (com::sun::star::uno::RuntimeException);

		// Remove this so no data at beginning
		Reference<XComponent> savedXComponent; // needed for testing listener

		Reference<XTextDocument> newXTextDocument(Reference<XInterface> desktop);
		
		//Reference<XModifyListener> xModifyListener;
		//const Reference<MyModifyListener> rMyModifyListener;

	public:
		ModifyBug() {
		}

		void run();
		void SAL_CALL getSelection(Reference<XComponent> xComponent);
		void SAL_CALL getSelections(Reference<XComponent> xComponent);
		void SAL_CALL fakeGetSelection(Reference<XComponent> xComponent);

		virtual ~ModifyBug() {
			int k = 10;
		}
};

void SAL_CALL ModifyBug::disposing(const com::sun::star::lang::EventObject& event)
		throw (com::sun::star::uno::RuntimeException) {
	// TODO: remove listener
}

void SAL_CALL ModifyBug::getSelection(Reference<XComponent> xComponent) {
	fakeGetSelection(xComponent);
	//Reference<XModel> xModel(xComponent, UNO_QUERY);
	//Reference<XController> xController = xModel->getCurrentController();
	//Reference<XSelectionSupplier> xSelectionSupplier(xController, UNO_QUERY);

	//xSelectionSupplier->getSelection(); // Including this line makes it hang on return from function!
}

void SAL_CALL ModifyBug::fakeGetSelection(Reference<XComponent> xComponent) {
	Reference<XTextDocument> xTextDocument(savedXComponent, UNO_QUERY);
	Reference<XText> xText = xTextDocument->getText();
	Reference<XTextRange> xTextRange = xText->getEnd();
}

void SAL_CALL ModifyBug::getSelections(Reference<XComponent> xComponent) {
	Reference<XModel> xModel(xComponent, UNO_QUERY);
	Reference<XController> xController = xModel->getCurrentController();
	Reference<XMultiSelectionSupplier> xMultiSelectionSupplier(xController, UNO_QUERY);
	// This will be NULL because only com::sun::star::awt::tree::TreeControl supports XMultiSelectionSupplier

	xMultiSelectionSupplier->getSelectionCount(); // Including this line makes it hang on return from function!
}

void SAL_CALL ModifyBug::modified(const com::sun::star::lang::EventObject& event)
		throw (com::sun::star::uno::RuntimeException) {
	getSelection(savedXComponent);
}

void SAL_CALL ModifyBug::windowResized(const com::sun::star::awt::WindowEvent& event)
		throw (com::sun::star::uno::RuntimeException) {
	getSelection(savedXComponent);
}

void SAL_CALL ModifyBug::windowMoved(const com::sun::star::awt::WindowEvent& event)
		throw (com::sun::star::uno::RuntimeException) {
	getSelection(savedXComponent);
}

void SAL_CALL ModifyBug::windowShown(const com::sun::star::lang::EventObject& event)
		throw (com::sun::star::uno::RuntimeException) {
	getSelection(savedXComponent);
}

void SAL_CALL ModifyBug::windowHidden(const com::sun::star::lang::EventObject& event)
		throw (com::sun::star::uno::RuntimeException) {
	getSelection(savedXComponent);
}

Reference<XTextDocument> ModifyBug::newXTextDocument(Reference<XInterface> desktop) {
	Reference<XComponentLoader> xComponentLoader(desktop, UNO_QUERY);
	Reference<XComponent> document = xComponentLoader->loadComponentFromURL(
			rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("private:factory/swriter")),
			rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("_blank")), 0, Sequence<PropertyValue>());
	Reference<XTextDocument> xTextDocument(document, UNO_QUERY);

	return xTextDocument;
}

void ModifyBug::run() {
	try {
		Reference<XComponentContext> xComponentContext(cppu::bootstrap());
		Reference<XMultiComponentFactory> xMultiComponentFactory(xComponentContext->getServiceManager(), UNO_QUERY);
		Reference<XInterface> desktop = xMultiComponentFactory->createInstanceWithContext(
				rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop")), xComponentContext);
		Reference<XDesktop> xDesktop(desktop, UNO_QUERY);

		{
			Reference<XTextDocument> xTextDocument = newXTextDocument(desktop);
			Reference<XText> xText = xTextDocument->getText();
			Reference<XComponent> xComponent(xTextDocument, UNO_QUERY);
			Reference<XModifiable> xModifiable(xComponent, UNO_QUERY);
			Reference<XModifyBroadcaster> xModifyBroadcaster(xComponent, UNO_QUERY);

			//Reference<XModifyListener> xModifyListener = new MyModifyListener(xComponent);
			Reference<XModifyListener> xModifyListener(new MyModifyListener(xComponent));


			savedXComponent = xComponent;
			//xModifyBroadcaster->addModifyListener(this);
			xModifyBroadcaster->addModifyListener(xModifyListener);

			Reference<XModel> xModel(xComponent, UNO_QUERY);
			Reference<XController> xController = xModel->getCurrentController();
			Reference<XFrame>  xFrame = xController->getFrame();
			Reference<XWindow> xWindow = xFrame->getContainerWindow();

			//xWindow->addWindowListener(this);

			getSelection(savedXComponent); // It (getSelection) can be called directly
			getSelection(savedXComponent); // and repeatedly

			xText->setString(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("hello"))); // It can _not_ be called indirectly
			xModifiable->setModified(sal_True); // It can't be called indirectly like this, either.

			xWindow->setVisible(sal_False);
			xWindow->setVisible(sal_True);

//			xModifyBroadcaster->removeModifyListener(this);
//			xWindow->removeWindowListener(this);
			savedXComponent = Reference<XComponent>();
			xTextDocument->dispose();
		}
		xDesktop->terminate();
	}
	catch (...) {
		std::cerr << "Exception..." << std::endl;
	}
}

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv) {
	ModifyBug modifyBug;
	
	modifyBug.run();
	return 0;
}
/*****************************************************************************/
