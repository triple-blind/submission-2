/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/utils/SynonymDictionary.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__UTILS__SYNONYM_DICTIONARY_HPP
#	define COM__HAATHI__WATERMARK__UTILS__SYNONYM_DICTIONARY_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"

#include "boostBeg.hpp"
#include <boost/shared_ptr.hpp>
#include "boostEnd.hpp"

#include <map>
#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace utils {
/******************************************************************************
Class
******************************************************************************/
class SynonymDictionary {
	protected:
		typedef std::vector<OUString> SynonymGroup;
	public:
		typedef SynonymGroup::iterator SynonymGroupIterator;
		typedef boost::shared_ptr<SynonymGroup> SharedSynonymGroup;
	protected:
		typedef std::map<OUString, SharedSynonymGroup> SynonymMap;
		typedef SynonymMap::iterator iterator;

		static SharedSynonymGroup emptySynonymGroup;

		SynonymMap synonymMap;
	public:
		SynonymDictionary();
		bool read(std::string filename);
		SharedSynonymGroup find(OUString word);
};
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
