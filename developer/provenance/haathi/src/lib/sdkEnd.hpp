/*****************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/sdkEnd.hpp#1 $
*****************************************************************************/
//#ifndef SDK_END_HPP
//#	define SDK_END_HPP
/*****************************************************************************
Define internal values
*****************************************************************************/
#include "Config.hpp"

#if OSYS == OSYS_WIN
#	pragma warning(pop)
// Unfortunately, we would also need to wrap the implementation files inside
// an sdkBeg and sdkEnd because they also use exceptions.  Instead, just give
// up on the exception handling signatures for Windows.
#	pragma warning(disable: 4290)
#else
#	if OSYS != OSYS_MAC
#		pragma GCC diagnostic pop
#	endif
#endif

// Let every sdk user know about this class
using com::sun::star::uno::Reference;
using rtl::OUString;
/****************************************************************************/
//#endif
/****************************************************************************/
