/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/BgLuminanceExtractor.cpp#11 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/extractor/BgLuminanceExtractor.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#if defined(max)
#	undef max
#endif

#if defined(min)
#	undef min
#endif
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
BgLuminanceExtractor::BgLuminanceExtractor(int id, Tesseract& tesseract): Extractor(id, tesseract) {
}

void BgLuminanceExtractor::collectAveColors(ResultIterator* itr,
		int top, int bottom, int left, int right,
		float* rAve, float* gAve, int *maskedCount) {
	BoundingBox<int> boundingBox = getBoundingBox(itr);

	boundingBox.top = top; // Expand from symbol to line values
	boundingBox.bottom = bottom;

	// Done like this so as to avoid pixels exactly in the middle via integer division truncation
	int leftDec = (boundingBox.left - left) / 2;
	if (leftDec < 0)
		return;

	leftDec = std::max(0, std::min(leftDec - 1, 3)); // -1 for separation
	boundingBox.left = boundingBox.left  - leftDec;

	int rightInc = (right - boundingBox.right) / 2;
	if (rightInc < 0)
		return;

	rightInc = std::max(0, std::min(rightInc - 1, 3)); // -1 for separation
	boundingBox.right = boundingBox.right + rightInc;

	if (boundingBox.right - boundingBox.left < 0)
		return;

	int extractedCount;
	PIX* extractedPix = copyPix(tesseract.getPix(), boundingBox, &extractedCount);
	PIX* maskPix = makeMaskPix(extractedPix);

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	pixWrite("extractedPix.png", extractedPix, IFF_PNG);
	pixWrite("maskPix.png", maskPix, IFF_PNG);
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
			if (mask == 0xff) {
				pixGetRGBPixel(extractedPix, x, y, &color.red, &color.green, &color.blue);
				
				// Make sure that pixel also approximates white (and isn't too close to edge)
				if ((color.red + color.green + color.blue) / 3 > 192) {
					rTotal += color.red;
					gTotal += color.green;
					count++;
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

void BgLuminanceExtractor::extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage) {
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
	int lineNo = 0;

	while (!itr->Empty(RIL_BLOCK)) {
		if (itr->Empty(RIL_WORD)) {
			// Non-text block
			itr->Next(RIL_WORD);
			continue;
		}	

		while (true) { // Paragraph
			std::vector<int> lefts;
			std::vector<int> rights;

			while (true) { // Line, first pass
				while (true) { // Word
					std::string letter = tesseract.nextLetter(itr);

					if (!letter.empty()) {
						BoundingBox<int> boundingBox;
						itr->BoundingBoxInternal(RIL_SYMBOL, &boundingBox.left, &boundingBox.top, &boundingBox.right, &boundingBox.bottom);

						lefts.push_back(boundingBox.left);
						rights.push_back(boundingBox.right);
					}
					if (itr->IsAtFinalElement(RIL_WORD, RIL_SYMBOL))
						break;
					itr->Next(RIL_SYMBOL);
				}
				if (itr->IsAtFinalElement(RIL_TEXTLINE, RIL_SYMBOL))
					break;
				// Finish processing word
				lefts.push_back(-1);
				rights.push_back(-1);
				itr->Next(RIL_SYMBOL);
			}
			for (size_t i = 0; i < lefts.size(); i++) {
				if (lefts[i] < 0 && 0 <= i - 1)
					lefts[i] = rights[i - 1];
				if (rights[i] < 0 && i + 1 < lefts.size())
					rights[i] = lefts[i + 1];
			}
			if (lefts.size() == 1) {
				int kwa = 9;
				//alert("Lefts.size is 1 in BgLuminanceExtractor.");
			}

			if (lefts.size() > 0) {
				itr->RestartRow();
				int count = 0;
				BoundingBox<int> boundingBox;
				itr->BoundingBoxInternal(RIL_TEXTLINE, &boundingBox.left, &boundingBox.top, &boundingBox.right, &boundingBox.bottom);

				while (true) { // Line, second pass
					while (true) { // Word
						std::string letter = tesseract.nextLetter(itr);

						if (!letter.empty()) {
							float rAve = 0;
							float gAve = 0;
							int maskedCount = 0;

							// This second condition should not be required.
							int left = 0 <= count - 1 && count - 1 < (int) rights.size() ? rights[count - 1] : boundingBox.left;
							int right = 0 <= count + 1 && count + 1 < (int) lefts.size() ? lefts[count + 1] : boundingBox.right;
							collectAveColors(itr, boundingBox.top, boundingBox.bottom, left, right,
									&rAve, &gAve, &maskedCount);

							letters.push_back(letter);
							rAves.push_back(rAve);
							gAves.push_back(gAve);
							count++;

							if (maskedCount != 0) {
								rDocTotal += rAve;
								gDocTotal += gAve;
								docCount++;

								std::map<std::string, float>::iterator it;
								
								it = rTotals.find(letter);
								if (it != rTotals.end()) {
									it->second += rAve;
									counts.find(letter)->second++;
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
						}
						if (itr->IsAtFinalElement(RIL_WORD, RIL_SYMBOL))
							break;
						itr->Next(RIL_SYMBOL);
					}
					if (itr->IsAtFinalElement(RIL_TEXTLINE, RIL_SYMBOL))
						break;
					// Finish processing word
					pushBackLetter(endOfWord, letters, rAves, gAves);
					count++;
					itr->Next(RIL_SYMBOL);
				}
			}
			else
			{ int kwa = 9; }
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

		// Spaces would be OK except that they can't be distinguished
		// from CR or LF which we don't want to have marked, so don't put them in.

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

BgLuminanceExtractor::~BgLuminanceExtractor() {
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
