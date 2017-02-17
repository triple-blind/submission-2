/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/utils/SynonymDictionary.cpp#3 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/utils/SynonymDictionary.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <stdlib.h>

#include <fstream>
#include <sstream>
/******************************************************************************
Using
******************************************************************************/

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
SynonymDictionary::SharedSynonymGroup SynonymDictionary::emptySynonymGroup(new SynonymGroup());

SynonymDictionary::SynonymDictionary() {
}

bool SynonymDictionary::read(std::string filename) {
	try {
		std::ifstream input(filename.c_str());
		std::string line;

		while (std::getline(input, line)) {
			if (line.empty())
				break;

			SharedSynonymGroup words(new SynonymGroup());
			std::istringstream iss(line);

			do {
				std::string word;

				iss >> word;
				if (word.size() > 0)
					words->push_back(word);
			} while (iss);

			SynonymGroup::iterator it;
			for (it = words->begin(); it != words->end(); ++it)
				synonymMap[*it] = words;
		}
	}
	catch (...) {
		return false;
	}
	return true;
}

SynonymDictionary::SharedSynonymGroup SynonymDictionary::find(std::string word) {
	iterator it = synonymMap.find(word);

	if (it == synonymMap.end())
		return emptySynonymGroup;
	return it->second;
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
