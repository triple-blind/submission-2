/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/SynonymExtractor.cpp#6 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/extractor/SynonymExtractor.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::watermark;
using namespace tesseract;

using com::haathi::watermark::utils::SynonymDictionary;
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
SynonymExtractor::SynonymExtractor(int id, Tesseract& tesseract,
		std::string pathToSynDict): Extractor(id, tesseract) {
	synonymDictionary.read(pathToSynDict);
}

char *getWord(ResultIterator* itr) {
	return itr->GetUTF8Text(RIL_WORD);
}

int log2(int value) {
	int result = -1;

	while (value) {
		value >>= 1;
		result++;
	}
	return result;
}

int pow2(int value) {
	int result = 1;

	while (value) {
		result <<= 1;
		value--;
	}
	return result;
}

// See void SynonymScheme::addWatermarkBits(std::vector<int>& watermarkBits, OUString word) {
void SynonymExtractor::addWatermarkBits(std::stringstream& bits, std::string word) {
	SynonymDictionary::SharedSynonymGroup words = synonymDictionary.find(word);
	size_t wordCount = words->size();

	if (wordCount == 0)
		bits << '-';
	else {
		int bitCount = log2(wordCount);
		int rollOver = pow2(bitCount);

		SynonymDictionary::SynonymGroupIterator itr = std::find(words->begin(), words->end(), word);
		int index = std::distance(words->begin(), itr);
		
		index = index % rollOver;
		for (int mask = rollOver / 2; mask > 0; mask >>= 1)
			bits << ((index & mask) != 0 ? '1' : '0');
	}
	bits << ' ';
}

void SynonymExtractor::extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage) {
	ResultIterator* itr = tesseract.getIterator();

	while (!itr->Empty(RIL_BLOCK)) {
		if (itr->Empty(RIL_WORD)) {
			// Non-text block
			itr->Next(RIL_WORD);
			continue;
		}	

		while (true) { // Paragraph
			while (true) { // Line
				std::string word;

				while (true) { // Word
					std::string letter = tesseract.nextLetter(itr);
					word += letter;

					if (itr->IsAtFinalElement(RIL_WORD, RIL_SYMBOL))
						break;
					itr->Next(RIL_SYMBOL);
				}
				addWatermarkBits(bits, word);

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
}

SynonymExtractor::~SynonymExtractor() {
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
