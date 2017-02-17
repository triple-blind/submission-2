/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/menu/DispatchHelper.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__MENU__DISPATCH_HELPER_HPP
#	define COM__HAATHI__MENU__DISPATCH_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/Logger.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "cppJSON.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/frame/FeatureStateEvent.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/util/URL.hpp>
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
template<class T> struct DispatchData {
	OUString name;
	void (T::*function)();
	bool (T::*isEnabled)();
};

void showPlainCounts(com::haathi::utils::Logger& logger);

template<class T> class DispatchHelper {
	protected:
		DispatchData<T>* dispatchData;

		void showCounts(com::haathi::utils::Logger& logger) {
			showPlainCounts(logger);
		}

		int getDispatchIndex(const com::sun::star::util::URL& url) {
			for (int i = 0; dispatchData[i].name.getLength() != 0; i++)
				if (url.Complete.equals(dispatchData[i].name))
					return i;
			return -1;
		}

	public:
		DispatchHelper(DispatchData<T>* dispatchData):
				dispatchData(dispatchData) {
		}

		Reference<com::sun::star::frame::XDispatch> queryDispatch(const com::sun::star::util::URL& url, Reference<com::sun::star::frame::XDispatch> xDispatch) {
			int dispatchIndex = getDispatchIndex(url);

			return dispatchIndex >= 0 ? xDispatch : 0;
		}

		bool canDispatch(T* dispatcher, const com::sun::star::util::URL& url) {
			int dispatchIndex = getDispatchIndex(url);

			return dispatchIndex >= 0 && (dispatcher->*dispatchData[dispatchIndex].isEnabled)();
		}

		bool dispatch(T* dispatcher, const com::sun::star::util::URL& url, com::haathi::utils::Logger& logger) {
			int dispatchIndex = getDispatchIndex(url);

			if (dispatchIndex < 0)
				return false;
			showCounts(logger);
			logger << "Dispatching " << dispatchData[dispatchIndex].name << "\n";
			(dispatcher->*(dispatchData[dispatchIndex].function))();
			showCounts(logger);
			return true;
		}

		bool addStatusListener(T* dispatcher, const com::sun::star::util::URL& url, com::sun::star::frame::FeatureStateEvent& featureStateEvent) {
			int dispatchIndex = getDispatchIndex(url);

			if (dispatchIndex < 0)
				return false;
			featureStateEvent.FeatureURL = url;
			featureStateEvent.IsEnabled = asSalBool((dispatcher->*(dispatchData[dispatchIndex].isEnabled))());
			featureStateEvent.Requery = sal_False; // sal_True does not work
			return true;
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
