/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/SynonymExtractor.hpp#4 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EXTRACTOR__SYNONYM_EXTRACTOR_HPP
#	define COM__HAATHI__WATERMARK__EXTRACTOR__SYNONYM_EXTRACTOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/extractor/Extractor.hpp"
#include "com/haathi/watermark/utils/SynonymDictionary.hpp"

#include <sstream>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace extractor {
/******************************************************************************
Class
******************************************************************************/
class SynonymExtractor: public Extractor {
	protected:
		com::haathi::watermark::utils::SynonymDictionary synonymDictionary;
		void addWatermarkBits(std::stringstream& text, std::string word);
	public:
		SynonymExtractor(int id, com::haathi::watermark::Tesseract& tesseract, std::string pathToSynDict);
		~SynonymExtractor();

		virtual void extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage);
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
