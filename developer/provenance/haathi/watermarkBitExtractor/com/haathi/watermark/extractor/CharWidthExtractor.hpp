/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/CharWidthExtractor.hpp#4 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EXTRACTOR__CHAR_WIDTH_EXTRACTOR_HPP
#	define COM__HAATHI__WATERMARK__EXTRACTOR__CHAR_WIDTH_EXTRACTOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/extractor/Extractor.hpp"
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
class CharWidthExtractor: public Extractor {
	private:
		int bitPosition;
		unsigned char extractedNumber;

	public:
		CharWidthExtractor(int id, com::haathi::watermark::Tesseract& tesseract);
		~CharWidthExtractor();

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
