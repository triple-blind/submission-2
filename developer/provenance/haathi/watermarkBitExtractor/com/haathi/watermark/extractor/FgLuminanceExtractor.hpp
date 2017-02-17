/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/FgLuminanceExtractor.hpp#7 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EXTRACTOR__FG_LUMINANCE_EXTRACTOR_HPP
#	define COM__HAATHI__WATERMARK__EXTRACTOR__FG_LUMINANCE_EXTRACTOR_HPP
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
class FgLuminanceExtractor: public Extractor {
	protected:
		void collectAveColors(tesseract::ResultIterator* itr, float* rAve, float* gAve, int *maskedCount);

	public:
		FgLuminanceExtractor(int id, com::haathi::watermark::Tesseract& tesseract);
		~FgLuminanceExtractor();

		virtual void extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage);

		virtual bool verifyCounts(int charCount, int bitCount) {
			return 2 * charCount == bitCount;
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
