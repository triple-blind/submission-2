/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/SomewhereEngine.cpp#4 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/engine/SomewhereEngine.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/text/TextHelper.hpp"
#include "com/haathi/watermark/embedder/WatermarkEmbedder.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
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
SomewhereEngine::SomewhereEngine(Reference<XController> xController,
		Reference<XText> xText, Reference<XComponent> xComponent,
		WatermarkSchemes& watermarkSchemes, WatermarkGenerator* watermarkGenerator,
		Watermarks& watermarks, WatermarkEmbedder& watermarkEmbedder,
		std::string versionId):
		WatermarkEngine(xController, xText, watermarkSchemes, watermarkGenerator),
		watermarkEmbedder(watermarkEmbedder),
		watermarks(watermarks), decisionGenerator(0), xComponent(xComponent) {
	decisionGenerator = makeDecisionGenerator(versionId);
	requiresTextViewCursor = watermarkSchemes.requiresTextViewCursor();
}

SomewhereEngine::~SomewhereEngine() {
}

// See also GrainGenerator
CryptoPP::LC_RNG SomewhereEngine::makeDecisionGenerator(std::string versionId) {
	int hash = 0;

	for (size_t i = 0; i < versionId.size(); i++)
		hash += versionId[i];
	return CryptoPP::LC_RNG(hash);
}

void SomewhereEngine::reset(std::string versionId) {
	decisionGenerator = makeDecisionGenerator(versionId);
	watermarkGenerator->reset(versionId);
	WatermarkEngine::reset();
}

void SomewhereEngine::preWatermark(sal_Int32 length) {
	watermarks.resize(length);
	watermarkEmbedder.preWatermark();
}

void SomewhereEngine::postWatermark(sal_Int32 length) {
	watermarkEmbedder.postWatermark();
}

void SomewhereEngine::overwatermark(SmartCursor& smartCursor) {
	int oldWatermark = watermarks[smartCursor.getLeft()];
	int watermark = watermarkSchemes.getWatermark(smartCursor);

	if (oldWatermark == watermark)
		return;
	watermarks[smartCursor.getLeft()] = watermark;

	// Only do if allowed
	if (WatermarkScheme::isWatermarkable(smartCursor.getCurrentCharacter()))
		watermarkEmbedder.embed(smartCursor, watermark); 
}

void SomewhereEngine::underwatermark(SmartCursor& smartCursor, int channel) {
	WatermarkScheme* pWatermarkScheme = watermarkSchemes.at(channel);
	int minLength = pWatermarkScheme->getMinLength();
	int count = 0;
	int newWatermarkIndex;

	for (int pos = smartCursor.getLeft(); count < minLength; pos++) {
		smartCursor.selectPos(pos);

		sal_Unicode c = smartCursor.getCurrentCharacter();
		newWatermarkIndex = pWatermarkScheme->getWatermarkStyleIndex(smartCursor);
		// If can include a watermark, then count it.  Could be that there
		// was a non-zero index there before, but this time around it's different.
		if (newWatermarkIndex != 0)
			count++;

		int oldWatermark = watermarks[smartCursor.getLeft()];
		int watermark = watermarkSchemes.combineChannel(oldWatermark, newWatermarkIndex, channel);

		if (oldWatermark == watermark)
			continue;
		watermarks[smartCursor.getLeft()] = watermark;

		// Only do if allowed
		if (WatermarkScheme::isWatermarkable(c))
			watermarkEmbedder.embed(smartCursor, watermark); 
	}
}

void SomewhereEngine::underwatermark(SmartCursor& smartCursor) {
	int oldWatermark = watermarks[smartCursor.getLeft()];
	int newWatermarkIndex = watermarkSchemes.getWatermarkIndexRun(smartCursor);
	int watermark = watermarkSchemes.combineRun(oldWatermark, newWatermarkIndex);

	watermarks[smartCursor.getLeft()] = watermark;
	// Only do if allowed
	if (WatermarkScheme::isWatermarkable(smartCursor.getCurrentCharacter()))
		watermarkEmbedder.embed(smartCursor, watermark); 
}

void SomewhereEngine::watermarkRun(Range range, OUString documentState) {
	if (range.isInvalid())
		return;

	int leftIndexRun = watermarkSchemes.getWatermarkIndexRun(watermarks.safeAt(range.getLeft() - 1));
	int rightIndexRun = watermarkSchemes.getWatermarkIndexRun(watermarks.safeAt(range.getRight()));
	int width = range.getWidth();

	if (width == 0) {
		if (leftIndexRun != rightIndexRun || rightIndexRun == -1)
			// There was a deletion, but the resulting neighbors are still in different runs
			return;

		// Change the character to the right, but just the run
		rightIndexRun = watermarkSchemes.getWatermarkIndexRun(watermarks.safeAt(range.getRight() + 1));
		watermarkSchemes.resetRun(1, leftIndexRun, rightIndexRun);

		SmartTextViewCursor smartTextViewCursor(getXTextViewCursor(requiresTextViewCursor));
		SmartTextCursor smartTextCursor(getXTextCursor(!requiresTextViewCursor));
		SmartCursor& smartCursor(requiresTextViewCursor ?
				static_cast<SmartCursor&>(smartTextViewCursor) : 
				static_cast<SmartCursor&>(smartTextCursor));

		smartCursor.selectPos(range.getLeft());
		underwatermark(smartCursor);
	}
	else {
		watermarkSchemes.resetRun(width, leftIndexRun, rightIndexRun);

		SmartTextViewCursor smartTextViewCursor(getXTextViewCursor(requiresTextViewCursor));
		SmartTextCursor smartTextCursor(getXTextCursor(!requiresTextViewCursor));
		SmartCursor& smartCursor(requiresTextViewCursor ?
				static_cast<SmartCursor&>(smartTextViewCursor) : 
				static_cast<SmartCursor&>(smartTextCursor));

		for (int i = range.getLeft(); i < range.getRight(); i++) {
			smartCursor.selectPos(i);
			underwatermark(smartCursor);
		}
	}
}

void SomewhereEngine::watermarkSome(OUString documentState) {
	SmartTextViewCursor smartTextViewCursor(getXTextViewCursor(requiresTextViewCursor));
	SmartTextCursor smartTextCursor(getXTextCursor(!requiresTextViewCursor));
	SmartCursor& smartCursor(requiresTextViewCursor ?
			static_cast<SmartCursor&>(smartTextViewCursor) : 
			static_cast<SmartCursor&>(smartTextCursor));

	for (int i = 0; i < watermarkSchemes.count(); i++) {
		if (i == WatermarkSchemes::RUN_CHANNEL)
			continue;
		WatermarkScheme* pWatermarkScheme = watermarkSchemes.at(i);
		Range range = pWatermarkScheme->getSubrange(smartCursor, Range(0, documentState.getLength()));

		if (!range.isValid())
			continue;

		sal_Int32 pos = decisionGenerator.GenerateWord32(range.getLeft(), range.getRight() - 1);

		smartCursor.gotoRange(Range(pos));
		underwatermark(smartCursor, i);
	}
}

void SomewhereEngine::watermark(Range range, OUString documentState) {
	if (range.isInvalid())
		return;
	watermarkRun(range, documentState);
	watermarkSome(documentState);
}

void SomewhereEngine::rewatermark(SmartCursor& smartCursor) {
	int watermark = watermarkSchemes.getWatermark(smartCursor);

	watermarks[smartCursor.getLeft()] = watermark;
	// Only do if allowed
	if (WatermarkScheme::isWatermarkable(smartCursor.getCurrentCharacter()))
		watermarkEmbedder.embed(smartCursor, watermark); 
}

void SomewhereEngine::rewatermark(sal_Int32 length) {
	preWatermark(length);

	SmartTextViewCursor smartTextViewCursor(getXTextViewCursor(requiresTextViewCursor));
	SmartTextCursor smartTextCursor(getXTextCursor(!requiresTextViewCursor));
	SmartCursor& smartCursor(requiresTextViewCursor ?
			static_cast<SmartCursor&>(smartTextViewCursor) : 
			static_cast<SmartCursor&>(smartTextCursor));
	HideAndShowCursor hideAndShowCursor(smartCursor);
	SaveAndRestoreRange saveAndRestoreRange(xText, smartCursor);

	traverse(smartCursor, &SomewhereEngine::rewatermark);
}

void SomewhereEngine::unwatermark(SmartCursor& smartCursor) {
	watermarks[smartCursor.getLeft()] = -1;

	// Only do if allowed
	if (WatermarkScheme::isWatermarkable(smartCursor.getCurrentCharacter()))
		watermarkEmbedder.unembed(smartCursor);
}

void SomewhereEngine::unwatermark(sal_Int32 length) {
	SmartTextCursor smartCursor(getXTextCursor()); // only ever requires an XTextCursor
	HideAndShowCursor hideAndShowCursor(smartCursor);
	SaveAndRestoreRange saveAndRestoreRange(xText, smartCursor);

	traverse(smartCursor, &SomewhereEngine::unwatermark);
	postWatermark(length);
}

void SomewhereEngine::verifyWatermark(SmartCursor& smartCursor) {
/*
	int index = watermarkEmbedder.unembed(smartCursor);

	if (index <= 0)
		return;
	index--;
	for (size_t i = 0; i < watermarkSchemes.size(); i++) {
		int tmpIndex = index / placeValues[i];

		if (!watermarkSchemes[i]->verifyIndex(smartCursor, Reference<XStyle>(), tmpIndex))
			std::cerr << "Could not verify index!" << std::endl;
		index -= placeValues[i] * tmpIndex;
	}
	*/
}

void SomewhereEngine::verifyWatermark(sal_Int32 length) {
#if DEBUG == DEBUG_OFF
	return;
#endif
	SmartTextViewCursor smartTextViewCursor(getXTextViewCursor(requiresTextViewCursor));
	SmartTextCursor smartTextCursor(getXTextCursor(!requiresTextViewCursor));
	SmartCursor& smartCursor(requiresTextViewCursor ?
			static_cast<SmartCursor&>(smartTextViewCursor) : 
			static_cast<SmartCursor&>(smartTextCursor));
	HideAndShowCursor hideAndShowCursor(smartCursor);
	SaveAndRestoreRange saveAndRestoreRange(xText, smartCursor);

	traverse(smartCursor, &SomewhereEngine::verifyWatermark);
}

void SomewhereEngine::traverse(SmartCursor& smartCursor, TextAction action) {
	Reference<XTextCursor> completeSelectionCursor = getCompleteXTextCursor();
	Reference<XTextRangeCompare> xTextRangeCompare = getXTextRangeCompare();
	smartCursor.gotoStart(sal_False);

	while (xTextRangeCompare->compareRegionEnds(smartCursor.getEnd(), completeSelectionCursor->getEnd()) != 0) {
		smartCursor.goRight(1, sal_True);
		(*this.*action)(smartCursor);
		smartCursor.collapseToEnd();
	}
}

std::vector<int> SomewhereEngine::getWatermarkBits(int watermarkId) {
	std::vector<int> watermarkBits;
	WatermarkScheme* watermarkScheme = NULL;
	int count = watermarkSchemes.count();

	for (int i = 0; i < count; i++)
		if (watermarkSchemes.at(i)->getId() == watermarkId) {
			watermarkScheme = watermarkSchemes.at(i);
			break;
		}
	if (watermarkScheme == NULL)
		return watermarkBits;

	Watermarks::iterator it;
	for (it = watermarks.begin(); it != watermarks.end(); ++it) {
		int index = watermarkSchemes.getWatermarkIndexId(*it, watermarkId);

		watermarkScheme->addWatermarkBits(watermarkBits, index);
	}
	return watermarkBits;
}

std::vector<int> SomewhereEngine::getWatermarkBits(int watermarkId, Reference<XTextRange> xTextRange) {
	std::vector<int> watermarkBits;
	WatermarkScheme* watermarkScheme = NULL;
	int count = watermarkSchemes.count();

	for (int i = 0; i < count; i++)
		if (watermarkSchemes.at(i)->getId() == watermarkId) {
			watermarkScheme = watermarkSchemes.at(i);
			break;
		}
	if (watermarkScheme == NULL)
		return watermarkBits;

	TextHelper textHelper(xComponent);
	sal_Int32 begin = textHelper.getOffset(xTextRange);
	sal_Int32 end = textHelper.getEndOffset(xTextRange);

	for (sal_Int32 i = begin; i < end; i++) {
		int index = watermarkSchemes.getWatermarkIndexId(watermarks[i], watermarkId);

		watermarkScheme->addWatermarkBits(watermarkBits, index);
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
