/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/WordSpacingExtractor.cpp#12 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/extractor/WordSpacingExtractor.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <stdlib.h>

#include <algorithm>
#include <locale.h>
#include <mbstring.h>
#include <string>
#include <vector>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::watermark;
using namespace tesseract;
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
WordSpacingExtractor::WordSpacingExtractor(int id, Tesseract& tesseract): Extractor(id, tesseract) {
}

void WordSpacingExtractor::extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage) {
	std::vector<int> lengths;
	std::vector<int> gaps;
	int aveGap;

	{
		ResultIterator* itr = tesseract.getIterator();
		int totalCount = 0;
		int gapCount = 0;
		int totalGap = 0;
		int left, top, right, bottom;
		int prevRight;

		while (!itr->Empty(RIL_BLOCK)) {
			if (itr->Empty(RIL_WORD)) {
				// Non-text block
				itr->Next(RIL_BLOCK);
				continue;
			}

			while (true) { // Paragraph
				while (true) { // Line
					std::string word;

					itr->BoundingBoxInternal(RIL_SYMBOL, &left, &top, &right, &bottom);
					if (totalCount > 0) {
						int gap = -prevRight + left;

						gaps.push_back(gap);
						if (gap > 0) {
							totalGap += gap;
							gapCount++;
						}
					}
					while (true) { // Word
						std::string letter = tesseract.nextLetter(itr);
						word += letter;

						if (itr->IsAtFinalElement(RIL_WORD, RIL_SYMBOL))
							break;
						itr->Next(RIL_SYMBOL);
					}
					// Finish processing word
					lengths.push_back(word.length());
					itr->BoundingBoxInternal(RIL_SYMBOL, &left, &top, &right, &bottom);
					prevRight = right;
					totalCount++;

					if (itr->IsAtFinalElement(RIL_TEXTLINE, RIL_SYMBOL))
						break;
					itr->Next(RIL_SYMBOL);
				}
				if (itr->IsAtFinalElement(RIL_PARA, RIL_SYMBOL))
					break;
				itr->Next(RIL_SYMBOL);
			}
			itr->Next(RIL_SYMBOL);
		}
		aveGap = totalGap / gapCount;
	}

	for (std::vector<int>::size_type i = 0; i < lengths.size() - 1; i++) {
		int gap = gaps[i];

		for (int j = 0; j < lengths[i]; j++)
			bits << '-';
		bits << (gap > 0 ? (gap < aveGap ? '0' : '1') : '?');
	}
	for (int j = 0; j < lengths[lengths.size() - 1]; j++)
		bits << '-';
	if (!lastPage)
		bits << '?';
}

WordSpacingExtractor::~WordSpacingExtractor() {
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
