/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/BaselineExtractor.hpp#6 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EXTRACTOR__BASELINE_EXTRACTOR_HPP
#	define COM__HAATHI__WATERMARK__EXTRACTOR__BASELINE_EXTRACTOR_HPP
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
class BaselineExtractor: public Extractor {
	protected:
		void baseline(tesseract::ResultIterator* itr, PIX* pix, DiagonalBox<float>& diagonalBox);
	public:
		BaselineExtractor(int id, com::haathi::watermark::Tesseract& tesseract);
		~BaselineExtractor();

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
