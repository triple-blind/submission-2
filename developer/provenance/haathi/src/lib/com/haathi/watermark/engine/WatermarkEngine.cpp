/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/WatermarkEngine.cpp#5 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/engine/WatermarkEngine.hpp"
/******************************************************************************
Include others
******************************************************************************/
//#include "com/haathi/watermark/scheme/WatermarkScheme.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/text/XTextViewCursorSupplier.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::text;
using namespace com::haathi::utils;
using namespace com::haathi::watermark::generator;
using namespace com::haathi::watermark::scheme;
using namespace com::haathi::watermark::utils;

using namespace com::sun::star::beans;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
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
HideAndShowCursor
******************************************************************************/
void HideAndShowCursor::before() {
	smartCursor.setVisible(sal_False);
}

void HideAndShowCursor::after() {
	smartCursor.setVisible(sal_True);
}

HideAndShowCursor::HideAndShowCursor(SmartCursor& smartCursor): smartCursor(smartCursor) {
	before();
}

HideAndShowCursor::~HideAndShowCursor() {
	after();
}
/******************************************************************************
SaveAndRestoreRange
******************************************************************************/
void SaveAndRestoreRange::before(Reference<XText> xText) {
	Reference<XTextRange> xTextRange = smartCursor.asXTextRange();

	// Creating this by range doesn't seem to work
	xTextCursor = xText->createTextCursor();
	xTextCursor->gotoRange(xTextRange, sal_False);
}

void SaveAndRestoreRange::after() {
	Reference<XTextRange> xTextRange(xTextCursor, UNO_QUERY);

	smartCursor.gotoXTextRange(xTextRange);
}

SaveAndRestoreRange::SaveAndRestoreRange(Reference<XText> xText, SmartCursor& smartCursor): smartCursor(smartCursor) {
	before(xText);
}

SaveAndRestoreRange::~SaveAndRestoreRange() {
	after();
}
/******************************************************************************
WatermarkEngine
******************************************************************************/			
WatermarkEngine::WatermarkEngine(Reference<XController> xController, Reference<XText> xText,
		WatermarkSchemes& watermarkSchemes, WatermarkGenerator* watermarkGenerator):
		xController(xController), xText(xText), watermarkSchemes(watermarkSchemes),
		watermarkGenerator(watermarkGenerator) {
}

Reference<XTextViewCursor> WatermarkEngine::getXTextViewCursor(bool really) {
	if (!really)
		return Reference<XTextViewCursor>();

	Reference<XTextViewCursorSupplier> xViewCursorSupplier(xController, UNO_QUERY);
	Reference<XTextViewCursor> xTextViewCursor = xViewCursorSupplier->getViewCursor();

	return xTextViewCursor;
}

Reference<XTextCursor> WatermarkEngine::getXTextCursor(bool really) {
	if (!really)
		return Reference<XTextCursor>();

	Reference<XTextCursor> xTextCursor = xText->createTextCursor();

	return xTextCursor;
}

Reference<XTextCursor> WatermarkEngine::getCompleteXTextCursor() {
	Reference<XTextCursor> xTextCursor = getXTextCursor();

	xTextCursor->gotoStart(sal_False);
	xTextCursor->gotoEnd(sal_True);
	return xTextCursor;
}

Reference<XTextRangeCompare> WatermarkEngine::getXTextRangeCompare() {
	Reference<XTextRangeCompare> xTextRangeCompare(xText, UNO_QUERY);

	return xTextRangeCompare;
}

void WatermarkEngine::reset() {
	watermarkSchemes.reset();
}

WatermarkEngine::~WatermarkEngine() {
}

std::string WatermarkEngine::getWatermarkAbbrev(int watermarkId) {
	int count = watermarkSchemes.count();
	std::string abbrev;

	if (watermarkId == 0)
		return watermarkSchemes.getWatermarkAbbrev(watermarkId);

	for (int i = 0; i < count; i++)
		if (watermarkSchemes.at(i)->getId() & watermarkId) {
			if (!abbrev.empty())
				abbrev += "+";
			abbrev += watermarkSchemes.at(i)->getAbbrev();
		}
	return abbrev;
}

Watermark WatermarkEngine::generate() {
	return watermarkGenerator->generate();
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
