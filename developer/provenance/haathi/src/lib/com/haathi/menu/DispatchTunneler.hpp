/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/menu/DispatchTunneler.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__MENU__DISPATCH_TUNNELER_HPP
#	define COM__HAATHI__MENU__DISPATCH_TUNNELER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/util/URL.hpp>
#include <cppuhelper/implbase1.hxx>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace menu {
/******************************************************************************
Class
******************************************************************************/
class DispatchTunneler: public cppu::WeakImplHelper1<com::sun::star::frame::XDispatch> {
	protected:
		static int count;

		Reference<com::sun::star::frame::XDispatch> above;
		Reference<com::sun::star::frame::XDispatch> below;
		osl::Mutex& lock;

	public:
		DispatchTunneler(Reference<com::sun::star::frame::XDispatch> above, 
				Reference<com::sun::star::frame::XDispatch> below, osl::Mutex& lock):
				above(above), below(below), lock(lock) {
			count++;
		}

		virtual ~DispatchTunneler() {
			count--;
		}

		virtual void SAL_CALL dispatch(const com::sun::star::util::URL& url, const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& lArgs)
				throw (com::sun::star::uno::RuntimeException) {
			above->dispatch(url, lArgs);
			if (!below.is())
				return;
			// Especially needed for printing
			osl::MutexGuard guard(lock);
			below->dispatch(url, lArgs);
		}

		virtual void SAL_CALL addStatusListener(
				const Reference<com::sun::star::frame::XStatusListener>& xListener, const com::sun::star::util::URL& url)
				throw (com::sun::star::uno::RuntimeException) {
			if (!below.is())
				return;
			below->addStatusListener(xListener, url);
		}

		virtual void SAL_CALL removeStatusListener(
				const Reference<com::sun::star::frame::XStatusListener>& xListener, const com::sun::star::util::URL& url)
				throw (com::sun::star::uno::RuntimeException) {
			if (!below.is())
				return;
			below->removeStatusListener(xListener, url);
		}
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
