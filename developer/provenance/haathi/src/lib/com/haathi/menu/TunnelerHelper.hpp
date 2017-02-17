/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/menu/TunnelerHelper.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__MENU__TUNNELER_HELPER_HPP
#	define COM__HAATHI__MENU__TUNNELER_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/Logger.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
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
template<class T> struct TunnelerData {
	OUString name;
	void (T::*function)(int subCommand);
	int subCommand;
};

void showPlainCounts(com::haathi::utils::Logger& logger);

template<class T> class TunnelerHelper {
	protected:
		TunnelerData<T>* tunnelerData;

		void showCounts(com::haathi::utils::Logger& logger) {
			showPlainCounts(logger);
		}

	public:
		TunnelerHelper(TunnelerData<T>* tunnelerData): tunnelerData(tunnelerData) {
		}

		int getTunnelerIndex(const com::sun::star::util::URL& url) {
			for (int i = 0; tunnelerData[i].name.getLength() != 0; i++)
				if (url.Complete.equals(tunnelerData[i].name))
					return i;
			return -1;
		}

		Reference<com::sun::star::frame::XDispatch> queryDispatch(const com::sun::star::util::URL& url,
				Reference<com::sun::star::frame::XDispatch> xDispatch) {
			int tunnelerIndex = getTunnelerIndex(url);

			return tunnelerIndex >= 0 ? xDispatch : 0;
		}

		bool dispatch(T* dispatcher, const com::sun::star::util::URL& url, com::haathi::utils::Logger& logger) {
			int tunnelerIndex = getTunnelerIndex(url);

			if (tunnelerIndex < 0)
				return false;
			if (tunnelerData[tunnelerIndex].function != 0) {
				showCounts(logger);
				logger << "Dispatching " << tunnelerData[tunnelerIndex].name << "\n";
				(dispatcher->*(tunnelerData[tunnelerIndex].function))(tunnelerData[tunnelerIndex].subCommand);
				showCounts(logger);
			}
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
