/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/WordSpacingExtractor.hpp#5 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EXTRACTOR__WORD_SPACING_EXTRACTOR_HPP
#	define COM__HAATHI__WATERMARK__EXTRACTOR__WORD_SPACING_EXTRACTOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/extractor/Extractor.hpp"

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
class WordSpacingExtractor: public Extractor {
	protected:

	public:
		WordSpacingExtractor(int id, com::haathi::watermark::Tesseract& tesseract);
		~WordSpacingExtractor();

		virtual void extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage);

		virtual bool verifyCounts(int charCount, int bitCount) {
			return charCount == bitCount;
		}
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
