/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/DocumentTracker.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__DOCUMENT_TRACKER_HPP
#	define COM__HAATHI__DOCUMENT__DOCUMENT_TRACKER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/util/CloseVetoException.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/util/XCloseListener.hpp>
#include <cppuhelper/implbase1.hxx>
#include "sdkEnd.hpp"

#include <map>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
DocumentCloseListener
******************************************************************************/
class DocumentTracker;

class DocumentCloseListener: public cppu::WeakImplHelper1<
		com::sun::star::util::XCloseListener> {
	protected:
		DocumentTracker* documentTracker;
		int id;
	public:
		DocumentCloseListener(DocumentTracker* documentTracker, int id);
		virtual ~DocumentCloseListener();

		void SAL_CALL queryClosing(const com::sun::star::lang::EventObject& source, sal_Bool getsOwnership)
				throw (com::sun::star::util::CloseVetoException);
		void SAL_CALL notifyClosing(const com::sun::star::lang::EventObject& source)
				throw (com::sun::star::uno::RuntimeException);
		void SAL_CALL disposing(const com::sun::star::lang::EventObject& source)
				throw (com::sun::star::uno::RuntimeException);
};
/******************************************************************************
DocumentTracker
******************************************************************************/
class DocumentTracker {
	protected:
		static int nextId;

		std::map<int, Reference<com::sun::star::lang::XComponent> > xComponents;

	public:
		static DocumentTracker instance;

		void addDocument(Reference<com::sun::star::lang::XComponent> xComponent);
		void subDocument(int id);
		int getCount();
		Reference<com::sun::star::lang::XComponent> getXComponent();
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
