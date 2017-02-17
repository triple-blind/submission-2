/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/BgLuminanceExtractor.hpp#6 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EXTRACTOR__BG_LUMINANCE_EXTRACTOR_HPP
#	define COM__HAATHI__WATERMARK__EXTRACTOR__BG_LUMINANCE_EXTRACTOR_HPP
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
class BgLuminanceExtractor: public Extractor {
	protected:
		void collectAveColors(tesseract::ResultIterator* itr,
				int top, int bottom, int left, int right,
				float* rAve, float* gAve, int *maskedCount);

	public:
		BgLuminanceExtractor(int id, com::haathi::watermark::Tesseract& tesseract);
		~BgLuminanceExtractor();

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
