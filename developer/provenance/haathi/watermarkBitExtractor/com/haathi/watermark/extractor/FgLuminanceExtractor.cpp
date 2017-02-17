/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/FgLuminanceExtractor.cpp#15 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/extractor/FgLuminanceExtractor.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <iostream>
#include <map>
#include <vector>
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
FgLuminanceExtractor::FgLuminanceExtractor(int id, Tesseract& tesseract): Extractor(id, tesseract) {
}

void FgLuminanceExtractor::collectAveColors(ResultIterator* itr, float* rAve, float* gAve, int *maskedCount) {
	BoundingBox<int> boundingBox = getBoundingBox(itr);
	if (boundingBox.left == boundingBox.right)
		return;
	int extractedCount;
	PIX* extractedPix = copyPix(tesseract.getPix(), boundingBox, &extractedCount);
	PIX* maskPix = makeMaskPix(extractedPix);

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	//pixWrite("extractedPix.png", extractedPix, IFF_PNG);
	//pixWrite("maskPix.png", maskPix, IFF_PNG);
#endif

	int count = 0;
	// Assume the totals will also fit into l_uint32
	l_uint32 rTotal = 0;
	l_uint32 gTotal = 0;
	l_uint32 mask;
	Color color;

	for (l_uint32 y = 0; y < maskPix->h; y++)
		for (l_uint32 x = 0; x < maskPix->w; x++) {
			pixGetPixel(maskPix, x, y, &mask);
			if (mask == 0) {
				pixGetRGBPixel(extractedPix, x, y, &color.red, &color.green, &color.blue);

				// Make sure that pixel also approximates black (and isn't too close to edge)
				if ((color.red + color.green + color.blue) / 3 < 64) {
					rTotal += color.red;
					gTotal += color.green;
					count++;
				}
				else {
					count = count + 1 - 1;
				}
			}
		}

	*maskedCount = count;
	if (count == 0)
		*rAve = *gAve = 0;
	else {
		*rAve = (float) rTotal / count;
		*gAve = (float) gTotal / count;
	}

	pixDestroy(&maskPix);
	pixDestroy(&extractedPix);
}

static inline void pushBackLetter(std::string letter, std::vector<std::string>& letters,
		std::vector<float>& rAves, std::vector<float>& gAves) {
	letters.push_back(letter);
	rAves.push_back(0);
	gAves.push_back(0);
}

void FgLuminanceExtractor::extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage) {
	static std::string endOfWord(" ");
	static std::string endOfLine(" "); // Assume word wrapping, otherwise '\n'
	static std::string endOfPara("\r");
	static std::string endOfBlock(" "); // Page more likely ends in space than endOfPara
	static std::string hyphen("-");

	float rDocTotal = 0; // Document total averages
	float gDocTotal = 0; // Document total averages
	int docCount = 0; // Count of letters contributing to totals

	std::map<std::string, float> rTotals;
	std::map<std::string, float> gTotals;
	std::map<std::string, int> counts;

	std::vector<std::string> letters;
	std::vector<float> rAves; // Sequential letter averages
	std::vector<float> gAves; // Sequential letter averages

	ResultIterator* itr = tesseract.getIterator();

	while (!itr->Empty(RIL_BLOCK)) {
		if (itr->Empty(RIL_WORD)) {
			// Non-text block
			itr->Next(RIL_WORD);
			continue;
		}	

		while (true) { // Paragraph
			while (true) { // Line
				while (true) { // Word
					std::string letter = tesseract.nextLetter(itr);

					float rAve = 0;
					float gAve = 0;
					int maskedCount = 0;

					collectAveColors(itr, &rAve, &gAve, &maskedCount);
					letters.push_back(letter);
					rAves.push_back(rAve);
					gAves.push_back(gAve);

					if (maskedCount != 0) {
						rDocTotal += rAve;
						gDocTotal += gAve;
						docCount++;

						std::map<std::string, float>::iterator it;
						
						it = rTotals.find(letter);
						if (it != rTotals.end()) {
							it->second += rAve;
							counts[letter]++;
						}
						else {
							rTotals[letter] = rAve;
							counts[letter] = 1;
						}

						it = gTotals.find(letter);
						if (it != gTotals.end())
							it->second += gAve;
						else
							gTotals[letter] = gAve;
					}
					if (itr->IsAtFinalElement(RIL_WORD, RIL_SYMBOL))
						break;
					itr->Next(RIL_SYMBOL);
				}
				if (itr->IsAtFinalElement(RIL_TEXTLINE, RIL_SYMBOL))
					break;
				// Finish processing word
				pushBackLetter(endOfWord, letters, rAves, gAves);
				itr->Next(RIL_SYMBOL);
			}
			if (itr->IsAtFinalElement(RIL_PARA, RIL_SYMBOL))
				break;
			// Finish processing line
			pushBackLetter(endOfLine, letters, rAves, gAves);
			itr->Next(RIL_SYMBOL);
		}
		if (itr->IsAtFinalElement(RIL_BLOCK, RIL_SYMBOL)) {
			itr->Next(RIL_SYMBOL);
			// Want to avoid endOfPara at end of page
			if (!itr->Empty(RIL_BLOCK))
				pushBackLetter(endOfPara, letters, rAves, gAves);
			continue;
		}
		// Finish processing paragraph
		pushBackLetter(endOfPara, letters, rAves, gAves);
		itr->Next(RIL_SYMBOL);
	}
	if (!lastPage)
		pushBackLetter(endOfBlock, letters, rAves, gAves);

	for (size_t i = 0; i < letters.size(); i++) {
		std::string letter = letters[i];

		if (letter.empty())
			continue; // No letter output, no bits output

		// These don't get watermarked
		if (letter == endOfWord || letter == endOfPara /*|| letter == hyphen*/) {
			bits << "--";
			continue;
		}
		if (letter == hyphen) {
			bits << "--";
			continue;
		}

		// Could not measure
		if (rAves[i] == 0 && gAves[i] == 0) {
			bits << "??";
			continue;
		}

		int letterCount = counts.find(letter)->second;
		float rAve;
		float gAve;

		if (letterCount == 1) {
			rAve = rDocTotal / docCount;
			gAve = gDocTotal / docCount;
		}
		else {
			rAve = rTotals[letter] / letterCount;
			gAve = gTotals[letter] / letterCount;
		}
		bits << (rAve < rAves[i] ? '1' : '0');
		bits << (gAve < gAves[i] ? '1' : '0');
	}
}

FgLuminanceExtractor::~FgLuminanceExtractor() {
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
