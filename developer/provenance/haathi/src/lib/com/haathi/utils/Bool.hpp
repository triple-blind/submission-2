/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance/haathi/src/lib/com/haathi/utils/Guard.hpp#8 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__BOOL_HPP
#	define COM__HAATHI__UTILS__BOOL_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include <sal/types.h>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
		};
	};
};

// The following will pollute the global namespace, but then the functions
// can be used in other header files without com::haathi::utils.
/******************************************************************************
Class
******************************************************************************/
inline bool isTrue(sal_Bool value) {
	return value == sal_True;
}

inline bool isFalse(sal_Bool value) {
	return value == sal_False;
}

inline sal_Bool asSalBool(bool value) {
	return value ? sal_True : sal_False;
}		
/******************************************************************************
Namespace
******************************************************************************/
//		};
//	};
//};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
