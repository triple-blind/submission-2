/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/BaselineExtractor.cpp#10 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/extractor/BaselineExtractor.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
#include <numeric>
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
BaselineExtractor::BaselineExtractor(int id, Tesseract& tesseract): Extractor(id, tesseract) {
}

static inline void pushBackLetter(std::string letter, std::vector<std::string>& letters,
		std::vector<float>& values) {
	letters.push_back(letter);
	values.push_back(0);
}

static float typeset2(DiagonalBox<int>& baselineDiagonalBox, BoundingBox<int>& symbolBoundingBox) {
	float x = (float) (symbolBoundingBox.right + symbolBoundingBox.left) / 2;
	float dy = (float) (baselineDiagonalBox.y2 - baselineDiagonalBox.y1);
	float dx = (float) (baselineDiagonalBox.x2 - baselineDiagonalBox.x1);
	float m = dy / dx;
	float y = baselineDiagonalBox.y1 + (x - baselineDiagonalBox.x1) * m;

	return symbolBoundingBox.bottom - y;
}

static float typeset(DiagonalBox<float>& baselineDiagonalBox, BoundingBox<int>& symbolBoundingBox) {
	// We're assuming a horizontal baseline now
	return (float) symbolBoundingBox.bottom - baselineDiagonalBox.y1;
}

int countHitsOctant0(PIX* pix, int x1, int y1, int x2, int y2) {
	static int white = 64 * 3;
	int hits = 0;
	Color color;

	int dx = x2 - x1;
	int dx2 = dx << 1;
	int dy2 = (y2 - y1) << 1;
	int d = dy2 - dx;
	int x = x1;
	int y = y1;

	for (; x == x1; x++) {
		pixGetRGBPixel(pix, x, y, &color.red, &color.green, &color.blue);
		if ((color.red + color.green + color.blue) < white)
			hits++;
		if (d > 0) {
			y++;
			d -= dx2;
		}
	}

	for (; x <= x2; x++) {
		pixGetRGBPixel(pix, x, y, &color.red, &color.green, &color.blue);
		if ((color.red + color.green + color.blue) < white)
			hits++;
		d += dy2;
		if (d > 0) {
			y++;
			d -= dx2;
		}
	}
	return hits;
}

int countsHitsOctant7(PIX* pix, int x1, int y1, int x2, int y2) {
	y1 = -y1;
	y2 = -y2;

	static int white = 64 * 3;
	int hits = 0;
	Color color;

	int dx = x2 - x1;
	int dx2 = dx << 1;
	int dy2 = (y2 - y1) << 1;
	int d = dy2 - dx;
	int x = x1;
	int y = y1;

	for (; x == x1; x++) {
		pixGetRGBPixel(pix, x, -y, &color.red, &color.green, &color.blue);
		if ((color.red + color.green + color.blue) < white)
			hits++;
		if (d > 0) {
			y++;
			d -= dx2;
		}
	}

	for (; x <= x2; x++) {
		pixGetRGBPixel(pix, x, -y, &color.red, &color.green, &color.blue);
		if ((color.red + color.green + color.blue) < white)
			hits++;
		d += dy2;
		if (d > 0) {
			y++;
			d -= dx2;
		}
	}
	return hits;
}

// See https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int countHits(PIX* pix, int x1, int y1, int x2, int y2) {
	// Assume they are almost horizontal
	if (y2 >= y1)
		return countHitsOctant0(pix, x1, y1, x2, y2);
	else
		return countsHitsOctant7(pix, x1, y1, x2, y2);
}
/*
void BaselineExtractor::baseline2(ResultIterator* itr, PIX* pix, DiagonalBox<int>& diagonalBox) {
	// For diagnostics
	itr->Baseline(RIL_TEXTLINE, &diagonalBox.x1, &diagonalBox.y1,
			&diagonalBox.x2, &diagonalBox.y2);

	int largestY = INT_MIN;
	int smallestY = INT_MAX;
	int left;
	int right;

	BoundingBox<int> symbolBoundingBox;
	itr->BoundingBoxInternal(RIL_SYMBOL, &symbolBoundingBox.left, &symbolBoundingBox.top,
			&symbolBoundingBox.right, &symbolBoundingBox.bottom);
	left = symbolBoundingBox.left;
	largestY = smallestY = symbolBoundingBox.bottom;

	while (true) {
		char *symbol = itr->GetUTF8Text(RIL_SYMBOL);

		// Skip these characters which are too low or too high.
		if (strchr("gjpqy'\"-=^~", *symbol) == NULL) {
			itr->BoundingBoxInternal(RIL_SYMBOL, &symbolBoundingBox.left, &symbolBoundingBox.top,
					&symbolBoundingBox.right, &symbolBoundingBox.bottom);

			largestY = std::max(largestY, symbolBoundingBox.bottom);
			smallestY = std::min(smallestY, symbolBoundingBox.bottom);
			right = symbolBoundingBox.right;
		}
		delete[] symbol;
		if (itr->IsAtFinalElement(RIL_TEXTLINE, RIL_SYMBOL))
			break;
		itr->Next(RIL_SYMBOL);
	}
	itr->RestartRow();
	
	int bestCount = INT_MIN;
	int bestY1;
	int bestY2;

	for (int y1 = smallestY; y1 <= largestY; y1++)
		for (int y2 = smallestY; y2 <= largestY; y2++) {
			int count = countHits(pix, left, y1, right, y2);

			if (count > bestCount) {
				bestCount = count;
				bestY1 = y1;
				bestY2 = y2;
			}
		}

	diagonalBox.x1 = left;
	diagonalBox.y1 = bestY1;
	diagonalBox.x2 = right;
	diagonalBox.y2 = bestY2;
}
*/
void BaselineExtractor::baseline(ResultIterator* itr, PIX* pix, DiagonalBox<float>& diagonalBox) {
	// Assuming that the baselines are horizontal now
	std::vector<int> bottoms;

	// For diagnostics
	DiagonalBox<int> tmpDiagonalBox;
	itr->Baseline(RIL_TEXTLINE, &tmpDiagonalBox.x1, &tmpDiagonalBox.y1,
			&tmpDiagonalBox.x2, &tmpDiagonalBox.y2);

	while (true) {
		char *symbol = tesseract.nextSymbol(itr);

		// Skip these characters which are too low or too high.
		if (strchr("gjpqy'\"-=^~,", *symbol) == NULL) {
			BoundingBox<int> symbolBoundingBox;
			itr->BoundingBoxInternal(RIL_SYMBOL, &symbolBoundingBox.left, &symbolBoundingBox.top,
					&symbolBoundingBox.right, &symbolBoundingBox.bottom);

			bottoms.push_back(symbolBoundingBox.bottom);
		}
		delete[] symbol;
		if (itr->IsAtFinalElement(RIL_TEXTLINE, RIL_SYMBOL))
			break;
		itr->Next(RIL_SYMBOL);
	}
	itr->RestartRow();

	std::sort(bottoms.begin(), bottoms.end());

//	float bestY = (float) bottoms[bottoms.size() / 2];
//	if (bottoms.size() % 2 == 0)
//		bestY = bestY / 2 + (float) bottoms[bottoms.size() / 2 - 1];
	float bestY = (float) std::accumulate(bottoms.begin(), bottoms.end(), 0) / bottoms.size();

	// Should be float
	diagonalBox.x1 = (float) tmpDiagonalBox.x1;
	diagonalBox.y1 = bestY;
	diagonalBox.x2 = (float) tmpDiagonalBox.x2;
	diagonalBox.y2 = bestY;
}

void BaselineExtractor::extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage) {
	static std::string endOfWord(" ");
	static std::string endOfLine(" "); // Assume word wrapping, otherwise '\n'
	static std::string endOfPara("\r");
	static std::string endOfBlock(" "); // Page more likely ends in space than endOfPara
	static std::string hyphen("-");

	std::map<std::string, float> totals;
	std::map<std::string, int> counts;

	std::vector<std::string> letters;
	std::vector<float> values;

	ResultIterator* itr = tesseract.getIterator();
	PIX* pix = tesseract.getPix();
	
	while (!itr->Empty(RIL_BLOCK)) {
		if (itr->Empty(RIL_WORD)) {
			// Non-text block
			itr->Next(RIL_WORD);
			continue;
		}	

		while (true) { // Paragraph
			//DiagonalBox<int> baselineDiagonalBox;
			//baseline2(itr, pix, baselineDiagonalBox);

			DiagonalBox<float> baselineDiagonalBox;
			baseline(itr, pix, baselineDiagonalBox);

			while (true) { // Line
				while (true) { // Word
					std::string letter = tesseract.nextLetter(itr);

					if (!letter.empty()) {
						BoundingBox<int> symbolBoundingBox;
						itr->BoundingBoxInternal(RIL_SYMBOL, &symbolBoundingBox.left, &symbolBoundingBox.top,
								&symbolBoundingBox.right, &symbolBoundingBox.bottom);

						//float value2 = typeset(baselineDiagonalBox, symbolBoundingBox);
						float value = typeset(baselineDiagonalBox, symbolBoundingBox);
						letters.push_back(letter);
						values.push_back(value);

						std::map<std::string, float>::iterator it;
						
						it = totals.find(letter);
						if (it != totals.end()) {
							it->second += value;
							counts[letter]++;
						}
						else {
							totals[letter] = value;
							counts[letter] = 1;
						}
					}
					if (itr->IsAtFinalElement(RIL_WORD, RIL_SYMBOL))
						break;
					itr->Next(RIL_SYMBOL);
				}
				if (itr->IsAtFinalElement(RIL_TEXTLINE, RIL_SYMBOL))
					break;
				// Finish processing word
				pushBackLetter(endOfWord, letters, values);
				itr->Next(RIL_SYMBOL);
			}

			//pixRenderLine(tesseract.getPix(), (int) baselineDiagonalBox.x1, (int) baselineDiagonalBox.y1,
			//		(int) baselineDiagonalBox.x2, (int) baselineDiagonalBox.y2, 1 , L_CLEAR_PIXELS);

			if (itr->IsAtFinalElement(RIL_PARA, RIL_SYMBOL))
				break;
			// Finish processing line
			pushBackLetter(endOfLine, letters, values);
			itr->Next(RIL_SYMBOL);
		}
		if (itr->IsAtFinalElement(RIL_BLOCK, RIL_SYMBOL)) {
			itr->Next(RIL_SYMBOL);
			// Want to avoid endOfPara at end of page
			if (!itr->Empty(RIL_BLOCK))
				pushBackLetter(endOfPara, letters, values);
			continue;
		}
		// Finish processing paragraph
		pushBackLetter(endOfPara, letters, values);
		itr->Next(RIL_SYMBOL);
	}
	if (!lastPage)
		pushBackLetter(endOfBlock, letters, values);

	//pixWrite("baselines.png", tesseract.getPix(), IFF_PNG);

	for (size_t i = 0; i < letters.size(); i++) {
		std::string letter = letters[i];

		// Spaces would be OK except that they can't be distinguished
		// from CR or LF which we don't want to have marked, so don't put them in.

		// These don't get watermarked
		if (letter == endOfWord || letter == endOfPara) {
			bits << '-';
			continue;
		}
		if (letter == hyphen) {
			bits << '-';
			continue;
		}

		int count = counts[letter];

		if (count == 1)
			bits << '?'; // Would like to do better here
		else
			// Backwards because escapement raises letter and lowers y value
			bits << (totals[letter] / count < values[i] ? '0' : '1');
	}
}

BaselineExtractor::~BaselineExtractor() {
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
