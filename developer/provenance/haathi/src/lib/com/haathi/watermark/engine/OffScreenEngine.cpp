/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/OffScreenEngine.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/engine/OffScreenEngine.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/watermark/embedder/WatermarkEmbedder.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"

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
OffScreenEngine::OffScreenEngine(Reference<XController> xController,
		Reference<XText> xText, Reference<XComponent> xComponent,
		WatermarkSchemes& watermarkSchemes, WatermarkGenerator* watermarkGenerator,
		Watermarks& watermarks, WatermarkEmbedder& watermarkEmbedder,
		std::string versionId):
		SomewhereEngine(xController, xText, xComponent, watermarkSchemes,
		watermarkGenerator, watermarks, watermarkEmbedder, versionId),
		textHelper(xComponent) {
}

OffScreenEngine::~OffScreenEngine() {
}

void OffScreenEngine::watermarkSome(OUString documentState) {
	static const sal_Int32 halfScreen = 2; // Use at least 1500 for realistic document

	SmartTextViewCursor smartTextViewCursor(getXTextViewCursor(requiresTextViewCursor));
	SmartTextCursor smartTextCursor(getXTextCursor(!requiresTextViewCursor));
	SmartCursor& smartCursor(requiresTextViewCursor ?
			static_cast<SmartCursor&>(smartTextViewCursor) : 
			static_cast<SmartCursor&>(smartTextCursor));
	sal_Int32 cursorPosition = textHelper.getCursorPosition();
	sal_Int32 n = documentState.getLength();
	sal_Int32 left = std::max<sal_Int32>(0, cursorPosition - halfScreen);
	sal_Int32 right = std::min<sal_Int32>(cursorPosition + halfScreen, n);

	for (int i = 0; i < watermarkSchemes.count(); i++) {
		if (i == WatermarkSchemes::RUN_CHANNEL)
			continue;
		WatermarkScheme* pWatermarkScheme = watermarkSchemes.at(i);

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

		smartCursor.gotoRange(Range(x));
		underwatermark(smartCursor, i);
	}
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
