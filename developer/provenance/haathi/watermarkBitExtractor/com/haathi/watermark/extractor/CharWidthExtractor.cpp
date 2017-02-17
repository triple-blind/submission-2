/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/CharWidthExtractor.cpp#11 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/extractor/CharWidthExtractor.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

#include <string.h>
/******************************************************************************
Define
******************************************************************************/
#define SCHEME_VIS_HI 3
#define SCHEME_VIS_MD 2
#define SCHEME_VIS_LO 1

#define SCHEME_VIS_NO 0
#define SCHEME_VIS_DB -1 // debug
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
//static const char* usableSymbols = "abcdefghijknopqrstuvwxyz"; // Times New Roman 12
static const char* usableSymbolsHi = "abcdeghknopqsuvxyz"; // Ariel 12
static const char* usableSymbolsMd = "abcdeghknopqsuvxyz"; // Ariel 12
static const char* usableSymbolsLo = "cfkmrstvwxyz"; // Ariel 12

CharWidthExtractor::CharWidthExtractor(int id, Tesseract& tesseract): Extractor(id, tesseract) {
	// For compiler
	bitPosition = 0;
	extractedNumber = 0;
}

struct ExpandedMeasure {
	ExpandedMeasure(): narrow(0), wide(0) { }
	ExpandedMeasure(float narrow, float wide): narrow(narrow), wide(wide) { }
	float narrow;
	float wide;
};

static std::map<std::string, float> calcNormalMeasures(std::vector<std::string> normalChars, std::vector<int> normalWidths) {
	std::map<std::string, float> normalMeasures;

	{
		// Go through vector and add all chars to map
		std::vector<std::string>::iterator it;
		for (it = normalChars.begin(); it != normalChars.end(); ++it)
			if (normalMeasures.find(*it) == normalMeasures.end())
				normalMeasures[*it] = 0;
	}

	{
		// For each char, find count and sum in order to calculate average
		std::map<std::string, float>::iterator it;
		for (it = normalMeasures.begin(); it != normalMeasures.end(); ++it) {
			std::string c = it->first;
			int count = 0;
			int sum = 0;
			
			for (size_t i = 0; i < normalChars.size(); i++)
				if (normalChars[i] == c) {
					count++;
					sum += normalWidths[i];
				}
			it->second = (float) sum / count;
		}
	}
	return normalMeasures;
}

struct LessBySecond {
	template <typename LHS, typename RHS>
	bool operator()(const LHS& lhs, const RHS& rhs) const {
		return lhs.second < rhs.second;
	}
};

static std::map<std::string, ExpandedMeasure> calcExpandedMeasures(std::vector<std::string> expandedChars, std::vector<int> expandedWidths,
		std::map<std::string, float> normalMeasures) {
	std::map<std::string, ExpandedMeasure> expandedMeasures;

	{
		// Go through vector and add all chars to map
		std::vector<std::string>::iterator it;
		for (it = expandedChars.begin(); it != expandedChars.end(); ++it)
			if (normalMeasures.find(*it) == normalMeasures.end() && expandedMeasures.find(*it) == expandedMeasures.end())
				expandedMeasures[*it] = ExpandedMeasure(0, 0);
	}

	{
		// For each char, count the various widths
		std::map<std::string, ExpandedMeasure>::iterator it;
		for (it = expandedMeasures.begin(); it != expandedMeasures.end(); ++it) {
			std::map<int, int> counts;
			std::string c = it->first;
			
			for (size_t i = 0; i < expandedChars.size(); i++)
				if (expandedChars[i] == c) {
					int width = expandedWidths[i];
			
					if (counts.find(width) == counts.end())
						counts[width] = 1;
					else
						counts[width] = counts[width] + 1;
				}

			int prevWidth;
			int prevCount;

			int mode1;
			{
				// Find the first mode and must have at least one!
				std::map<int, int>::iterator it = std::max_element(counts.begin(), counts.end(), LessBySecond());

				mode1 = it->first;
				prevWidth = it->first;
				prevCount = it->second;
				counts.erase(it);
			}

			int mode2;
			{
				if (!counts.empty()) {
					// Find the first mode and must have at least one!
					std::map<int, int>::iterator it = std::max_element(counts.begin(), counts.end(), LessBySecond());

					mode2 = it->first;
				}
				else
					mode2 = mode1;
			}
			// Add the erased one back in before next calculation
			counts[prevWidth] = prevCount;
		
			int narrow = mode1 < mode2 ? mode1 : mode2;
			int wide = mode1 >= mode2 ? mode1 : mode2;
			float fNarrow;
			float fWide;

			{
				int count = 0;
				int sum = 0;

				std::map<int, int>::iterator it;
				for (it = counts.begin(); it != counts.end(); ++it) {
					if (it->first <= narrow) {
						count += it->second;
						sum += (it->first * it->second);
					}
				}
				fNarrow = (float) sum / count;
			}

			{
				int count = 0;
				int sum = 0;

				std::map<int, int>::iterator it;
				for (it = counts.begin(); it != counts.end(); ++it) {
					if (wide <= it->first) {
						count += it->second;
						sum += (it->first * it->second);
					}
				}
				fWide = (float) sum / count;
			}

			expandedMeasures[c] = ExpandedMeasure(fNarrow, fWide);
		}
	}
	return expandedMeasures;
}

// Returns -1 for narrow, 0 for ambiguous, and 1 for wide
static int getBit(std::string c, int width, std::map<std::string, float>& normalMeasures, std::map<std::string, ExpandedMeasure>& expandedMeasures) {

	{
		std::map<std::string, float>::iterator it = normalMeasures.find(c);

		if (it != normalMeasures.end()) {
			if (width == it->second)
				return 0;
			return width > it->second ? 1 : -1;
		}
	}

	{
		std::map<std::string, ExpandedMeasure>::iterator it = expandedMeasures.find(c);

		if (it != expandedMeasures.end()) {
			float narrow = (it->second).narrow;
			float wide = (it->second).wide;
			float fWidth = (float) width;

			if (fWidth == narrow && fWidth == wide)
				return 0;
			if (fWidth <= narrow)
				return -1;
			if (fWidth >= wide)
				return 1;
			if (fWidth - narrow == wide - fWidth)
				return 0;
			return fWidth - narrow > wide - fWidth ? 1 : -1;
		}
	}
	// Never to get here
	return 0;
}

static inline void pushBackLetter(std::string letter, std::vector<std::string>& letters,
		std::vector<int>& widths, std::vector<int>& indexes) {
	letters.push_back(letter);
	widths.push_back(0);
	indexes.push_back(indexes.size());
}

void CharWidthExtractor::extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage) {
	const char* usableSymbols;

	if (vis == SCHEME_VIS_HI)
		usableSymbols = usableSymbolsHi;
	else if (vis == SCHEME_VIS_MD)
		usableSymbols = usableSymbolsMd;
	else if (vis == SCHEME_VIS_LO)
		usableSymbols = usableSymbolsLo;
	else
		throw std::runtime_error("Bad visibility value.");

	static std::string endOfWord(" ");
	static std::string endOfLine(" "); // Assume word wrapping, otherwise '\n'
	static std::string endOfPara("\r");
	static std::string endOfBlock(" "); // Page more likely ends in space than endOfPara

	std::vector<std::string> allChars;
	std::vector<int> allWidths;
	std::vector<int> otherIndexes;

	std::vector<std::string> normalChars;
	std::vector<int> normalWidths;

	std::vector<std::string> expandedChars;
	std::vector<int> expandedWidths;

	// Need to keep one in reserve because of pairs
	std::string prevChar;
	int prevWidth = 0;
	int prevIndex = 0;
	bool prevFound;

	const char glyphs[] = { '0', '?', '1' };

	ResultIterator* itr = tesseract.getIterator();

	while (!itr->Empty(RIL_BLOCK)) {
		if (itr->Empty(RIL_WORD)) {
			// Non-text block
			itr->Next(RIL_WORD);
			continue;
		}	

		while (true) { // Paragraph
			while (true) { // Line
				prevFound = false;
				while (true) { // Word
					std::string letter = tesseract.nextLetter(itr);

					if (!letter.empty()) {
						char c = *letter.c_str();
						bool usableSymbol = strchr(usableSymbols, c) != NULL;

						allChars.push_back(letter);
						otherIndexes.push_back(otherIndexes.size());

						if (usableSymbol) {
							BoundingBox<int> boundingBox = getBoundingBox(itr);
							int width = boundingBox.right - boundingBox.left;

							allWidths.push_back(width);
							if (!prevFound) {
								prevChar = c;
								prevWidth = width;
								prevIndex = allChars.size() - 1;
								prevFound = true;
							}
							else {
								expandedChars.push_back(prevChar);
								expandedWidths.push_back(prevWidth);

								expandedChars.push_back(letter);
								expandedWidths.push_back(width);

								otherIndexes[prevIndex] = allChars.size() - 1;
								otherIndexes[allChars.size() - 1] = prevIndex;

								prevFound = false;
							}
						}
						else
							allWidths.push_back(0);
					}
					if (itr->IsAtFinalElement(RIL_WORD, RIL_SYMBOL))
						break;
					itr->Next(RIL_SYMBOL);
				}
				if (prevFound) {
					normalChars.push_back(prevChar);
					normalWidths.push_back(prevWidth);
				}
				if (itr->IsAtFinalElement(RIL_TEXTLINE, RIL_SYMBOL))
					break;
				// Finish processing word
				pushBackLetter(endOfWord, allChars, allWidths, otherIndexes);
				itr->Next(RIL_SYMBOL);
			}
			if (itr->IsAtFinalElement(RIL_PARA, RIL_SYMBOL))
				break;
			// Finish processing line
			pushBackLetter(endOfLine, allChars, allWidths, otherIndexes);
			itr->Next(RIL_SYMBOL);
		}
		if (itr->IsAtFinalElement(RIL_BLOCK, RIL_SYMBOL)) {
			itr->Next(RIL_SYMBOL);
			if (!itr->Empty(RIL_BLOCK))
				pushBackLetter(endOfPara, allChars, allWidths, otherIndexes);
			continue;
		}
		// Finish processing paragraph
		pushBackLetter(endOfPara, allChars, allWidths, otherIndexes);
		itr->Next(RIL_SYMBOL);
	}
	if (!lastPage)
		pushBackLetter(endOfBlock, allChars, allWidths, otherIndexes);

	// If three possible - + *, should have six arrangements
	// -+*, -*+, +-*, +*-, *-+, *+- instead of just one bit
	// Base 3?
	// If four and only +-, ++-- +-+- +--+, could have 2.5 bits
	//                      --++ -+-+ -++-  what about . though?

	std::map<std::string, float> normalMeasures = calcNormalMeasures(normalChars, normalWidths);
	std::map<std::string, ExpandedMeasure> expandedMeasures = calcExpandedMeasures(expandedChars, expandedWidths, normalMeasures);

	for (size_t i = 0; i < allChars.size(); i++) {
		if (otherIndexes[i] == i)
			bits << '-';
		else {
			int b1 = getBit(allChars[i], allWidths[i], normalMeasures, expandedMeasures);
			int b2 = getBit(allChars[otherIndexes[i]], allWidths[otherIndexes[i]], normalMeasures, expandedMeasures);

			// Use parity to fix some ambiguity
			if (b1 == 0 && b2 != 0)
				b1 = -b2;
			if (b1 != 0 && b2 == 0)
				b2 = -b1;
//			if (b1 + b2 != 0)
//				; // std::cerr << "Parity is bad!" << "\t" << allChars[i] << "\t" << allChars[otherIndexes[i]] << std::endl;

			bits << glyphs[b1 + 1];
		}
	}
}

CharWidthExtractor::~CharWidthExtractor() {
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
