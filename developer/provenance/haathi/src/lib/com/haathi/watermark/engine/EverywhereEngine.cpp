/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/EverywhereEngine.cpp#4 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/engine/EverywhereEngine.hpp"
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
EverywhereEngine::EverywhereEngine(Reference<XController> xController,
		Reference<XText> xText, Reference<XComponent> xComponent,
		WatermarkSchemes& watermarkSchemes, WatermarkGenerator* watermarkGenerator,
		Watermarks& watermarks, WatermarkEmbedder& watermarkEmbedder):
		WatermarkEngine(xController, xText, watermarkSchemes, watermarkGenerator),
		watermarkEmbedder(watermarkEmbedder),
		watermarks(watermarks), xComponent(xComponent) {
	requiresTextViewCursor = watermarkSchemes.requiresTextViewCursor();
}

EverywhereEngine::~EverywhereEngine() {
}

void EverywhereEngine::reset(std::string versionId) {
	watermarkGenerator->reset(versionId);
	WatermarkEngine::reset();
}

void EverywhereEngine::preWatermark(sal_Int32 length) {
	watermarks.resize(length);
	watermarkEmbedder.preWatermark();
}

void EverywhereEngine::postWatermark(sal_Int32 length) {
	watermarkEmbedder.postWatermark();
}

void EverywhereEngine::overwatermark(SmartCursor& smartCursor) {
	if (!WatermarkScheme::isWatermarkable(smartCursor.getCurrentCharacter()))
		return;

	int watermark = watermarkSchemes.getWatermark(smartCursor);

	watermarks[smartCursor.getLeft()] = watermark;
	watermarkEmbedder.embed(smartCursor, watermark); 
}

void EverywhereEngine::underwatermark(SmartCursor& smartCursor) {
	if (!WatermarkScheme::isWatermarkable(smartCursor.getCurrentCharacter()))
		return;

	int oldWatermark = watermarks[smartCursor.getLeft()];
	int newWatermarkIndex = watermarkSchemes.getWatermarkIndexRun(smartCursor);
	int watermark = watermarkSchemes.combineRun(oldWatermark, newWatermarkIndex);

	watermarks[smartCursor.getLeft()] = watermark;
	watermarkEmbedder.embed(smartCursor, watermark); 
}

void EverywhereEngine::watermark(Range range, OUString documentState) {
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
			smartCursor.selectPos(i); // TODO: if a viewCursor is really needed, this leaves it in the wrong place
			overwatermark(smartCursor);
		}
	}
}

void EverywhereEngine::rewatermark(sal_Int32 length) {
	preWatermark(length);

	SmartTextViewCursor smartTextViewCursor(getXTextViewCursor(requiresTextViewCursor));
	SmartTextCursor smartTextCursor(getXTextCursor(!requiresTextViewCursor));
	SmartCursor& smartCursor(requiresTextViewCursor ?
			static_cast<SmartCursor&>(smartTextViewCursor) : 
			static_cast<SmartCursor&>(smartTextCursor));
	HideAndShowCursor hideAndShowCursor(smartCursor);
	SaveAndRestoreRange saveAndRestoreRange(xText, smartCursor);

	traverse(smartCursor, &EverywhereEngine::overwatermark);
}

void EverywhereEngine::unwatermark(SmartCursor& smartCursor) {
	if (!WatermarkScheme::isWatermarkable(smartCursor.getCurrentCharacter()))
		return;

	watermarks[smartCursor.getLeft()] = -1;
	watermarkEmbedder.unembed(smartCursor);
}

void EverywhereEngine::unwatermark(sal_Int32 length) {
	SmartTextCursor smartCursor(getXTextCursor()); // only ever requires an XTextCursor
	HideAndShowCursor hideAndShowCursor(smartCursor);
	SaveAndRestoreRange saveAndRestoreRange(xText, smartCursor);

	traverse(smartCursor, &EverywhereEngine::unwatermark);
	postWatermark(length);
}

void EverywhereEngine::verifyWatermark(SmartCursor& smartCursor) {
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

void EverywhereEngine::verifyWatermark(sal_Int32 length) {
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

	traverse(smartCursor, &EverywhereEngine::verifyWatermark);
}

void EverywhereEngine::traverse(SmartCursor& smartCursor, TextAction action) {
	Reference<XTextCursor> completeSelectionCursor = getCompleteXTextCursor();
	Reference<XTextRangeCompare> xTextRangeCompare = getXTextRangeCompare();
	smartCursor.gotoStart(sal_False);

	while (xTextRangeCompare->compareRegionEnds(smartCursor.getEnd(), completeSelectionCursor->getEnd()) != 0) {
		smartCursor.goRight(1, sal_True);
		(*this.*action)(smartCursor);
		smartCursor.collapseToEnd();
	}
}

std::vector<int> EverywhereEngine::getWatermarkBits(int watermarkId) {
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

std::vector<int> EverywhereEngine::getWatermarkBits(int watermarkId, Reference<XTextRange> xTextRange) {
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
