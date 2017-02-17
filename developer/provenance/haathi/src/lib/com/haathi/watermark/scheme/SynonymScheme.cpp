/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/SynonymScheme.cpp#4 $
/******************************************************************************
Include self
******************************************************************************/
#include "Config.hpp"
// This must be placed this high in order to prevent warnings
#if OSYS == OSYS_WIN
//1>c:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\include\functional(143) : warning C4800: 'sal_Bool' : forcing value to bool 'true' or 'false' (performance warning)
#	pragma warning(disable: 4800)
#endif

#include "com/haathi/watermark/scheme/SynonymScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"

#include "boostBeg.hpp"
#include <boost/shared_ptr.hpp>
#include "boostEnd.hpp"

#include <fstream>
#include <map>
#include <sstream>
#include <vector>
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::utils;

using com::haathi::text::Range;
using com::haathi::watermark::generator::WatermarkGenerator;
using com::haathi::watermark::utils::SynonymDictionary;
using com::haathi::watermark::utils::Watermark;

using namespace com::sun::star::style;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace scheme {
/******************************************************************************
Class
******************************************************************************/
SynonymScheme::SynonymScheme(int id, WatermarkGenerator* watermarkGenerator, std::string dictionaryFilename):
		WatermarkScheme(id, watermarkGenerator), decisionGenerator(0) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	openLog("Syn");
#endif
	synonymDictionary.read(dictionaryFilename);
	styleNames.push_back(OUSTRING("Syn--"));
	reset();
}

std::string SynonymScheme::getAbbrev() {
	return std::string("Syn");
}

void SynonymScheme::reset() {
	currentBit = Watermark::SIZE;
}

std::vector<WatermarkStyle> SynonymScheme::getWatermarkStyles() {
	NamesValuesHelper emptyNamesValuesHelper;
	std::vector<WatermarkStyle> watermarkStyles;
	int index = 0;

	watermarkStyles.push_back(WatermarkStyle(styleNames[index++], emptyNamesValuesHelper));
	return watermarkStyles;
}

unsigned char SynonymScheme::nextWatermarkBit() {
	if (Watermark::SIZE <= currentBit) {
		watermark = watermarkGenerator->generate();
		currentBit = 0;
	}
	return watermark[currentBit++];
}

int SynonymScheme::getWatermarkStyleIndex(SmartCursor& smartCursor) {
	return 0;
}

bool SynonymScheme::verifyIndex(SmartCursor& smartCursor,
		 Reference<XStyle>& xStyle, int index) {
	return index == 0;
}

void SynonymScheme::addWatermarkBits(std::vector<int>& watermarkBits, int index) {
}

bool SynonymScheme::isWatermarkable(SmartCursor& smartCursor) {
	return false;
}

Range SynonymScheme::getSubrange(SmartCursor& smartCursor, Range range) {
	static int minLength = 0;
	sal_Int32 left = range.getLeft();
	sal_Int32 right = range.getRight();
	sal_Int32 width = range.getWidth();

	if (width < minLength)
		return Range();

	bool spaceFound = false;
	for (int pos = right - 1; pos >= left; pos--) {
		smartCursor.selectPos(pos);

		sal_Unicode c = smartCursor.getCurrentCharacter();

		if (c == 0x0020 || c == 0x000d || c == 0x000a) {
			spaceFound = true;
			right = pos;
			break;
		}
	}
	if (!spaceFound)
		return Range();

	int count = 0;
	sal_Int32 wordLength;
	bool inWord = false;

	for (int pos = right - 1; pos >= left; pos--) {
		smartCursor.selectPos(pos);

		sal_Unicode c = smartCursor.getCurrentCharacter();

		if (c == 0x0020 || c == 0x000d || c == 0x000a) {
			if (inWord) {
				smartCursor.goRight(wordLength - 1, sal_True);
				count += getBitCount(smartCursor);
				if (count >= Watermark::SIZE)
					return Range(left, pos + wordLength + 1);
				inWord = false;
			}
		}
		else
			if (inWord)
				wordLength++;
			else {
				inWord = true;
				wordLength = 1;
			}
	}

	if (inWord && left == 0) {
		smartCursor.goRight(wordLength - 1, sal_True);
		count += getBitCount(smartCursor);
		if (count >= Watermark::SIZE)
			return Range(left, left + 1);
	}

	return Range();
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

OUString SynonymScheme::watermarkWord(OUString word, bool* watermarkable) {
	SynonymDictionary::SharedSynonymGroup words = synonymDictionary.find(word);
	size_t wordCount = words->size();

	*watermarkable = wordCount != 0;
	if (wordCount == 0)
		return word; // no change

	int bitCount = log2(wordCount);
	int rollOver = pow2(bitCount);
	int index = 0;

	for (int i = 0; i < bitCount; i++)
		index = index * 2 + nextWatermarkBit();

	if (index + rollOver < (int) (words->size()))
		if (decisionGenerator.GenerateBit() != 0)
			index += rollOver;
	return words->at(index);
}

int SynonymScheme::getBitCount(com::haathi::document::SmartCursor& smartCursor) {
	OUString text = smartCursor.getCurrentText();
	SynonymDictionary::SharedSynonymGroup words = synonymDictionary.find(text);
	size_t wordCount = words->size();

	return wordCount == 0 ? 0 : log2(wordCount);
}

void SynonymScheme::addWatermarkBits(std::vector<int>& watermarkBits, OUString word) {
	SynonymDictionary::SharedSynonymGroup words = synonymDictionary.find(word);
	size_t wordCount = words->size();

	if (wordCount == 0)
		return;

	int bitCount = log2(wordCount);
	int rollOver = pow2(bitCount);

	SynonymDictionary::SynonymGroupIterator itr = std::find(words->begin(), words->end(), word);
	int index = std::distance(words->begin(), itr);
	
	index = index % rollOver;
	for (int mask = rollOver / 2; mask > 0; mask >>= 1)
		watermarkBits.push_back((index & mask) != 0 ? 1 : 0);
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
