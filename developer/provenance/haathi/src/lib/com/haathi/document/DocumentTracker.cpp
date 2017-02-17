/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/DocumentTracker.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/document/DocumentTracker.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <com/sun/star/frame/XModel.hpp>
/******************************************************************************
Using
******************************************************************************/
using com::sun::star::frame::XFrame;
using com::sun::star::frame::XModel;
using com::sun::star::lang::EventObject;
using com::sun::star::lang::XComponent;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::util::XCloseable;
using com::sun::star::util::XCloseListener;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
DocumentCloseListener
******************************************************************************/
DocumentCloseListener::DocumentCloseListener(DocumentTracker* documentTracker, int id):
		documentTracker(documentTracker), id(id) {
}

DocumentCloseListener::~DocumentCloseListener() {
}

void SAL_CALL DocumentCloseListener::queryClosing(const com::sun::star::lang::EventObject& source, sal_Bool getsOwnership)
		throw (com::sun::star::util::CloseVetoException) {
}

void SAL_CALL DocumentCloseListener::notifyClosing(const EventObject& source)
		throw (com::sun::star::uno::RuntimeException) {
	if (documentTracker != NULL)
		documentTracker->subDocument(id);
	documentTracker = NULL;
}

void SAL_CALL DocumentCloseListener::disposing(const com::sun::star::lang::EventObject& source)
		throw (com::sun::star::uno::RuntimeException) {
	notifyClosing(source);
//	delete this;
}
/******************************************************************************
DocumentTracker
******************************************************************************/
void DocumentTracker::addDocument(Reference<XComponent> xComponent) {
	Reference<XModel> xModel(xComponent, UNO_QUERY);
	Reference<XFrame> xFrame = xModel->getCurrentController()->getFrame();
	Reference<XCloseable> xCloseable(xFrame, UNO_QUERY);
	DocumentCloseListener* documentCloseListener = new DocumentCloseListener(this, nextId);
	Reference<XCloseListener> xCloseListener = static_cast<XCloseListener*>(documentCloseListener);

	xCloseable->addCloseListener(xCloseListener);
	xComponents[nextId++] = xComponent;
}

void DocumentTracker::subDocument(int id) {
	std::map<int, Reference<XComponent> >::iterator it = xComponents.find(id);

	if (it != xComponents.end())
		xComponents.erase(it);
}

int DocumentTracker::getCount() {
	return xComponents.size();
}

Reference<XComponent> DocumentTracker::getXComponent() {
	if (xComponents.size() == 0)
		return Reference<XComponent>();
	return xComponents.begin()->second;
}

DocumentTracker DocumentTracker::instance;
int DocumentTracker::nextId = 0;
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
