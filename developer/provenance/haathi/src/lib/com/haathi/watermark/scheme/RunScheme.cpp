/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/RunScheme.cpp#2 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/RunScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::utils;
using namespace com::haathi::watermark::generator;

using namespace com::sun::star::style;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
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
RunScheme::RunScheme(int id, WatermarkGenerator* watermarkGenerator):
		WatermarkScheme(id, NULL) {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	openLog("Run");
#endif
	styleNames.push_back(OUSTRING("WM0"));
	styleNames.push_back(OUSTRING("WM1"));
	styleNames.push_back(OUSTRING("WM2"));
	reset();
}

std::string RunScheme::getAbbrev() {
	return std::string("WM");
}

void RunScheme::reset() {
	run = 0;
}

void RunScheme::reset(int n) {
	run = n % 3;
}

int RunScheme::reset(int width, int leftRun, int rightRun) {
	if (leftRun == -1 && rightRun == -1)
		reset(0);
	else if (rightRun == -1)
		reset(leftRun + 1);
	else if (leftRun == -1)
		reset(rightRun - width);
	else {
		bool avail[] = {true, true, true};
		avail[leftRun] = false;
		avail[(rightRun - width + 1) % 3] = false;
		if (avail[0])
			reset(0);
		else if (avail[1])
			reset(1);
		else reset(2);
	}
	return run;
}

std::vector<WatermarkStyle> RunScheme::getWatermarkStyles() {
	NamesValuesHelper emptyNamesValuesHelper;
	std::vector<WatermarkStyle> watermarkStyles;

	for (std::vector<OUString>::iterator it = styleNames.begin(); it != styleNames.end(); ++it)
		watermarkStyles.push_back(WatermarkStyle(*it, emptyNamesValuesHelper));
	return watermarkStyles;
}

int RunScheme::getWatermarkStyleIndex(com::haathi::document::SmartCursor& smartCursor) {
	int index = getWatermarkStyleIndex();

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	std::vector<int> bits;
	bits.push_back(index);
	writeLogIn(smartCursor, bits);
#endif
	return index;
}

int RunScheme::getWatermarkStyleIndex() {
	int index = run;

	run = (run + 1) % 3;
	return index;
}

bool RunScheme::verifyIndex(com::haathi::document::SmartCursor& smartCursor,
		 Reference<XStyle>& xStyle, int index) {
// TODO: actual do some checking
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	writeLogOut(smartCursor, getWatermarkBits(smartCursor, xStyle, index));
#endif
	return true;
}

void RunScheme::addWatermarkBits(std::vector<int>& watermarkBits, int index) {
	watermarkBits.push_back(index); // This won't necessarily be 0 or 1
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
