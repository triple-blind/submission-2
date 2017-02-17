/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/Archive.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__UTILS__ARCHIVE_HPP
#	define COM__HAATHI__UTILS__ARCHIVE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "Config.hpp"

#include "boostBeg.hpp"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "boostEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Class
******************************************************************************/
#if ARCHIVE == ARCHIVE_CPP
	typedef boost::archive::text_iarchive HAATHI_IARCHIVE;
	typedef boost::archive::text_oarchive HAATHI_OARCHIVE;
#elif ARCHIVE == ARCHIVE_XML
	typedef boost::archive::xml_iarchive HAATHI_IARCHIVE;
	typedef boost::archive::xml_oarchive HAATHI_OARCHIVE;
#endif
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
