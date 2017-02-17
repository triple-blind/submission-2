/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/SynonymEngine.cpp#6 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/engine/SynonymEngine.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/provenance/text/WatermarkTextProvenance.hpp"
#include "com/haathi/watermark/embedder/WatermarkEmbedder.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/WatermarkSchemes.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <rtl/ustrbuf.hxx>
#include "sdkEnd.hpp"

#include <algorithm>
/******************************************************************************
Define
******************************************************************************/
#define DEBUG_OFF 0
#define DEBUG_ON  1

#define DEBUG DEBUG_OFF
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::provenance;
using namespace com::haathi::text;
using namespace com::haathi::utils;
using namespace com::haathi::watermark::embedder;
using namespace com::haathi::watermark::generator;
using namespace com::haathi::watermark::scheme;
using namespace com::haathi::watermark::utils;

using namespace com::sun::star::beans;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::style;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;

using com::sun::star::text::XTextRange;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace engine {
/******************************************************************************
Class
******************************************************************************/
static const int COLORS = 8;

static const sal_Int32 colors[COLORS] = {
	0x00ffff00, 0x00adff2f, 0x0087cefa, 0x00b0c4de,
	0x00ff00ff, 0x0040e0d0, 0x0098fb98, 0x00f0e68c
};

SynonymEngine::SynonymEngine(Reference<XController> xController,
		Reference<XText> xText, Reference<XComponent> xComponent,
		WatermarkSchemes& watermarkSchemes, WatermarkGenerator* watermarkGenerator,
		Watermarks& watermarks, WatermarkEmbedder& watermarkEmbedder,
		std::string versionId, std::string dictionaryFilename, DocumentHistory& documentHistory,
		ProvenanceDocument& provenanceDocument):
		OffScreenEngine(xController, xText, xComponent, watermarkSchemes, watermarkGenerator,
		watermarks, watermarkEmbedder, versionId), textHelper(xComponent), documentHistory(documentHistory),
		provenanceDocument(provenanceDocument) {
	synonymScheme = boost::shared_ptr<SynonymScheme>(new SynonymScheme(SYNONYM_WATERMARK, watermarkGenerator, dictionaryFilename));
	round = 0;
}

SynonymEngine::~SynonymEngine() {
}

void color(SmartCursor& smartCursor, int round) {
	Reference<XTextRange> xTextRange = smartCursor.asXTextRange();
	Reference<XPropertySet> xPropertySet(xTextRange, UNO_QUERY);

	xPropertySet->setPropertyValue(OUSTRING("CharBackColor"), Any(colors[round]));
}

void SynonymEngine::rewatermark(sal_Int32 length) {
	watermarks.resize(length);
	OUString text = documentHistory.getOldText();

	SmartTextCursor smartTextCursor(getXTextCursor(true));
	SmartCursor& smartCursor(static_cast<SmartCursor&>(smartTextCursor));
	HideAndShowCursor hideAndShowCursor(smartCursor);
	rtl::OUStringBuffer wordBuffer;

	{
		SaveAndRestoreRange saveAndRestoreRange(xText, smartCursor);
		Reference<XTextCursor> completeSelectionCursor = getCompleteXTextCursor();
		Reference<XTextRangeCompare> xTextRangeCompare = getXTextRangeCompare();
		sal_Int32 pos = 0;
		bool inWord = false;
		smartCursor.gotoStart(sal_False);

		while (xTextRangeCompare->compareRegionEnds(smartCursor.getEnd(), completeSelectionCursor->getEnd()) != 0) {
			smartCursor.goRight(1, sal_True);
			
			sal_Unicode c = smartCursor.getCurrentCharacter();

			if (c == 0x0020 || c == 0x000d || c == 0x000a) {
				if (inWord) {
					inWord = false;

					OUString oldWord = wordBuffer.makeStringAndClear();
					bool watermarkable;
					OUString newWord = synonymScheme->watermarkWord(oldWord, &watermarkable);

					if (watermarkable) {
						smartCursor.collapseToStart();
						smartCursor.goLeft(oldWord.getLength(), sal_True);
						smartCursor.setCurrentText(newWord);
#if 0
						// Highlight the substitution
						color(smartCursor, round);
#endif
						if (oldWord.equals(newWord) == sal_False) {
							WatermarkTextProvenance* pWatermarkTextProvenance = new WatermarkTextProvenance(pos - oldWord.getLength(), oldWord, newWord);
							pWatermarkTextProvenance->track(watermarks);
							text = text.replaceAt(pos - oldWord.getLength(), oldWord.getLength(), newWord);
							documentHistory.setOldText(text);
							provenanceDocument.addProvenanceEvent(pWatermarkTextProvenance, false);

							sal_Int32 delta = newWord.getLength() - oldWord.getLength();
							pos += delta;
						}

						smartCursor.collapseToEnd();
						smartCursor.goRight(1, sal_True);
					}
				}
			}
			else
				if (inWord)
					wordBuffer.append(c);
				else {
					inWord = true;
					wordBuffer.append(c);
				}
			smartCursor.collapseToEnd();
			pos++;
		}
		// This does happen because trailing paragraph marker is not considered above
		if (inWord) {
			OUString oldWord = wordBuffer.makeStringAndClear();
			bool watermarkable;
			OUString newWord = synonymScheme->watermarkWord(oldWord, &watermarkable);

			if (oldWord.equals(newWord) == sal_False) {
				smartCursor.goLeft(oldWord.getLength(), sal_True);
				smartCursor.setCurrentText(newWord);
				color(smartCursor, round);

				WatermarkTextProvenance* pWatermarkTextProvenance = new WatermarkTextProvenance(pos - oldWord.getLength(), oldWord, newWord);
				pWatermarkTextProvenance->track(watermarks);
				text = text.replaceAt(pos - oldWord.getLength(), oldWord.getLength(), newWord);
				documentHistory.setOldText(text);
				provenanceDocument.addProvenanceEvent(pWatermarkTextProvenance, false);
			}
		}
	}
	round = (round + 1) % COLORS;
	OffScreenEngine::rewatermark(text.getLength());
}

void SynonymEngine::watermarkSome(OUString documentState) {
/*
	static const sal_Int32 halfScreen = 2; // Use at least 1500 for realistic document

	// If insert anything, need to base watermark it
	// first.  Be sure ot forward right stuff to insert runs and initial watermark

	SmartTextCursor smartTextCursor(getXTextCursor(true));
	SmartCursor& smartCursor(static_cast<SmartCursor&>(smartTextCursor));

	sal_Int32 cursorPosition = textHelper.getCursorPosition();
	sal_Int32 n = documentState.getLength();
	sal_Int32 left = std::max<sal_Int32>(0, cursorPosition - halfScreen);
	sal_Int32 right = std::min<sal_Int32>(cursorPosition + halfScreen, n);

	Range leftRange(0, left);
	leftRange = pWatermarkScheme->getSubrange(smartCursor, leftRange);
	sal_Int32 leftPrime = !leftRange.isValid() ? 0 : leftRange.getRight();

	Range rightRange(right, n);
	rightRange = pWatermarkScheme->getSubrange(smartCursor, rightRange);
	sal_Int32 nPrime = rightRange.isValid() ? rightRange.getRight() : right;

	Range singleRange(0, leftPrime - right + nPrime);
	if (!singleRange.isValid() || singleRange.getWidth() == 0)
		continue;

	// This one works on a closed interval [left, right], so right has to be tucked in
	sal_Int32 x = singleRange.getWidth() == 1 ?
		singleRange.getLeft() :
		decisionGenerator.GenerateWord32(singleRange.getLeft(), singleRange.getRight() - 1);
	if (x >= leftPrime)
		x += right - leftPrime;

	smartCursor.gotoRange(Range(x)); // start here and watermark, much as above
	
	// Will have to make changes to document state
	underwatermark(smartCursor, documentState);
	OffScreenEngine::watermarkSome(documentState);
	*/
	OffScreenEngine::watermarkSome(documentState);
}

std::string SynonymEngine::getWatermarkAbbrev(int watermarkId) {
	int synonymId = synonymScheme.get()->getId();

	if (synonymId == watermarkId)
		return synonymScheme.get()->getAbbrev();

	std::string abbrev = OffScreenEngine::getWatermarkAbbrev(watermarkId);

	if (!(synonymId & watermarkId))
		return abbrev;
	return abbrev + "+" + synonymScheme.get()->getAbbrev();
}

void SynonymEngine::addWatermarkBits(std::vector<int>& watermarkBits, OUString word) {
	size_t oldSize = watermarkBits.size();
	synonymScheme->addWatermarkBits(watermarkBits, word);
	size_t newSize = watermarkBits.size();
	if (oldSize == newSize)
		watermarkBits.push_back(-1); // No bit added
	watermarkBits.push_back(-2); // Separator
}

// Somehow combine with version above
std::vector<int> SynonymEngine::getWatermarkBits(int watermarkId) {
	if (synonymScheme.get()->getId() != watermarkId)
		return OffScreenEngine::getWatermarkBits(watermarkId);

	std::vector<int> watermarkBits;
	OUString text = documentHistory.getOldText();
	rtl::OUStringBuffer wordBuffer;

	{
		bool inWord = false;

		for (int pos = 0; pos < text.getLength(); pos++) {
			sal_Unicode c = text[pos];

			if (c == 0x0020 || c == 0x000d || c == 0x000a) {
				if (inWord) {
					inWord = false;
					OUString word = wordBuffer.makeStringAndClear();
					addWatermarkBits(watermarkBits, word);
				}
			}
			else
				if (inWord)
					wordBuffer.append(c);
				else {
					inWord = true;
					wordBuffer.append(c);
				}
		}
		if (inWord) {
			OUString word = wordBuffer.makeStringAndClear();
			addWatermarkBits(watermarkBits, word);
		}
	}
	return watermarkBits;
}

// Somehow combine with version above
std::vector<int> SynonymEngine::getWatermarkBits(int watermarkId, Reference<XTextRange> xTextRange) {
	if (synonymScheme.get()->getId() != watermarkId)
		return OffScreenEngine::getWatermarkBits(watermarkId, xTextRange);

	sal_Int32 begin = textHelper.getOffset(xTextRange);
	sal_Int32 end = textHelper.getEndOffset(xTextRange);

	std::vector<int> watermarkBits;
	OUString text = documentHistory.getOldText();
	rtl::OUStringBuffer wordBuffer;

	{
		bool inWord = false;

		for (int pos = begin; pos < end; pos++) {
			sal_Unicode c = text[pos];

			if (c == 0x0020 || c == 0x000d || c == 0x000a) {
				if (inWord) {
					inWord = false;
					OUString word = wordBuffer.makeStringAndClear();
					addWatermarkBits(watermarkBits, word);
				}
			}
			else
				if (inWord)
					wordBuffer.append(c);
				else {
					inWord = true;
					wordBuffer.append(c);
				}
		}
		if (inWord) {
			OUString word = wordBuffer.makeStringAndClear();
			addWatermarkBits(watermarkBits, word);
		}
	}
	return watermarkBits;
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
