/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/BothwhereEngine.cpp#7 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/engine/BothwhereEngine.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::text;
using namespace com::haathi::watermark::utils;

using namespace com::sun::star::text;
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
BothwhereEngine::BothwhereEngine(EverywhereEngine& everywhereEngine, SomewhereEngine& somewhereEngine):
		everywhereEngine(everywhereEngine), somewhereEngine(somewhereEngine) {
}

BothwhereEngine::~BothwhereEngine() {
}

void BothwhereEngine::reset(std::string versionId) {
	everywhereEngine.reset(versionId);
	somewhereEngine.reset(versionId);
}

void BothwhereEngine::watermark(Range range, OUString documentState) {
	everywhereEngine.watermark(range, documentState);
	// Background watermarking is disabled. TODO: reenable it
	//somewhereEngine.watermarkSome(documentState);
}

void BothwhereEngine::rewatermark(sal_Int32 length) {
	somewhereEngine.rewatermark(length); // To catch synonyms
}

void BothwhereEngine::unwatermark(sal_Int32 length) {
	everywhereEngine.unwatermark(length);
}

void BothwhereEngine::verifyWatermark(sal_Int32 length) {
// copy from elsewhere
}

std::string BothwhereEngine::getWatermarkAbbrev(int watermarkId) {
	return somewhereEngine.getWatermarkAbbrev(watermarkId);
}

std::vector<int> BothwhereEngine::getWatermarkBits(int watermarkId) {
	return somewhereEngine.getWatermarkBits(watermarkId); // To catch synonyms
}

std::vector<int> BothwhereEngine::getWatermarkBits(int watermarkId, Reference<XTextRange> xTextRange) {
	return somewhereEngine.getWatermarkBits(watermarkId, xTextRange); // To catch synonyms
}

Watermark BothwhereEngine::generate() {
	return somewhereEngine.generate();
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
