/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/Tesseract.cpp#7 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/Tesseract.hpp"
#include "com/haathi/watermark/utils/UtfConverter.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <iostream>
#include <string>
/******************************************************************************
Using
******************************************************************************/
using com::haathi::watermark::utils::UtfConverter;

using namespace tesseract;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
/******************************************************************************
Class
******************************************************************************/
Tesseract::Tesseract(std::string inputPath) {
	pix = pixRead(inputPath.c_str());
	pixScaleResolution(pix, 3.0,3.0);

	api.SetVariable("tessedit_create_hocr", "true");
	api.Init(NULL, "eng");
	api.SetPageSegMode(tesseract::PSM_AUTO);

	api.SetImage(pix);
}

void Tesseract::recognize() {
	api.Recognize(NULL);
}

char *Tesseract::nextSymbol(ResultIterator* itr) {
	// This will need to be deleted
	return itr->GetUTF8Text(RIL_SYMBOL);
}

std::string Tesseract::nextLetter(ResultIterator* itr) {
	static std::string empty;

	char *symbol = nextSymbol(itr);
	if (symbol == NULL)
		return empty;
	if (*symbol == '\0')
		std::cerr << "Read empty symbol!" << std::endl; // what to return?

	std::string multiLetter(symbol);
	delete[] symbol;

	if (multiLetter.empty())
		return multiLetter;

	std::string singleLetter = UtfConverter::toSingle(multiLetter);
	return singleLetter;
}

void Tesseract::extractText(std::stringstream& text, bool lastPage) {
	static char endOfWord(' ');
	static char endOfLine(' '); // Assume word wrapping, otherwise '\n'
	static char endOfPara('\r');
	static char endOfBlock(' '); // Page more likely ends in space than endOfPara
	static PageIteratorLevel level(RIL_SYMBOL);

	ResultIterator* itr = getIterator();

	while (!itr->Empty(RIL_BLOCK)) {
		if (itr->Empty(RIL_WORD)) {
			// Non-text block
			itr->Next(RIL_WORD);
			continue;
		}	

		while (true) { // Paragraph
			while (true) { // Line
				// To be more efficient, this could be done by word.
				while (true) { // Word
					// Process the symbol
					std::string letter = nextLetter(itr);
					text << letter;

					if (itr->IsAtFinalElement(RIL_WORD, level))
						break;
					// Finish processing symbols
					itr->Next(RIL_SYMBOL);
				}
				if (itr->IsAtFinalElement(RIL_TEXTLINE, level))
					break;
				// Finish processing word
				text << endOfWord;
				itr->Next(level);
			}
			if (itr->IsAtFinalElement(RIL_PARA, level))
				break;
			// Finish processing line
			text << endOfLine;
			itr->Next(level);
		}
		if (itr->IsAtFinalElement(RIL_BLOCK, level)) {
			itr->Next(level);
			// Want to avoid endOfPara at end of page
			if (!itr->Empty(RIL_BLOCK))
				text << endOfPara;
			continue;
		}
		// Finish processing paragraph
		text << endOfPara;
		itr->Next(level);
	}
	if (!lastPage)
		text << endOfBlock;
}

Tesseract::~Tesseract() {
	api.Clear();
	api.End();
	pixDestroy(&pix);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
