/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/utils/SynonymDictionary.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__UTILS__SYNONYM_DICTIONARY_HPP
#	define COM__HAATHI__WATERMARK__UTILS__SYNONYM_DICTIONARY_HPP
/******************************************************************************
Include
******************************************************************************/
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
		typedef std::vector<std::string> SynonymGroup;
	public:
		typedef SynonymGroup::iterator SynonymGroupIterator;
		typedef boost::shared_ptr<SynonymGroup> SharedSynonymGroup;
	protected:
		typedef std::map<std::string, SharedSynonymGroup> SynonymMap;
		typedef SynonymMap::iterator iterator;

		static SharedSynonymGroup emptySynonymGroup;

		SynonymMap synonymMap;
	public:
		SynonymDictionary();
		bool read(std::string filename);
		SharedSynonymGroup find(std::string word);
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
