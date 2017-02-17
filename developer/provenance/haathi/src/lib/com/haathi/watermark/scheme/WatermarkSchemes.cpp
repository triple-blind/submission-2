/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/WatermarkSchemes.cpp#4 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/WatermarkSchemes.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/document/SmartCursor.hpp"
#include "com/haathi/watermark/utils/Watermarks.hpp"

#include "com/haathi/watermark/scheme/BaselineScheme.hpp"
#include "com/haathi/watermark/scheme/BgLuminanceScheme.hpp"
#include "com/haathi/watermark/scheme/CharWidthScheme.hpp"
#include "com/haathi/watermark/scheme/FgLuminanceScheme.hpp"
#include "com/haathi/watermark/scheme/RunScheme.hpp"
#include "com/haathi/watermark/scheme/UnderlineColorScheme.hpp"
#include "com/haathi/watermark/scheme/UserAttributesScheme.hpp"
#include "com/haathi/watermark/scheme/VisitedStyleScheme.hpp"
#include "com/haathi/watermark/scheme/WordSpacingScheme.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::document::SmartCursor;
using com::haathi::watermark::generator::WatermarkGenerator;
using com::haathi::watermark::utils::Watermarks;
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
WatermarkSchemes::WatermarkSchemes(WatermarkGenerator* watermarkGenerator, std::string dictionaryFilename, int schemes, int vis) {
	runScheme = new RunScheme(RUN_WATERMARK, watermarkGenerator);
	watermarkSchemes.push_back(runScheme);

	if (schemes & FG_LUMINANCE_WATERMARK)
		watermarkSchemes.push_back(new FgLuminanceScheme(FG_LUMINANCE_WATERMARK, vis, watermarkGenerator));
	if (schemes & BG_LUMINANCE_WATERMARK)
		watermarkSchemes.push_back(new BgLuminanceScheme(BG_LUMINANCE_WATERMARK, vis, watermarkGenerator));
	// This one is so different that a SynonymEngine is required
//	if (schemes & SYNONYM_WATERMARK)
//		watermarkSchemes.push_back(new SynonymScheme(SYNONYM_WATERMARK, watermarkGenerator));
	if (schemes & BASELINE_WATERMARK)
		 watermarkSchemes.push_back(new BaselineScheme(BASELINE_WATERMARK, vis, watermarkGenerator));
	if (schemes & CHAR_WIDTH_WATERMARK)
		watermarkSchemes.push_back(new CharWidthScheme(CHAR_WIDTH_WATERMARK, vis, watermarkGenerator));
	if (schemes & WORD_SPACING_WATERMARK)
		watermarkSchemes.push_back(new WordSpacingScheme(WORD_SPACING_WATERMARK, vis, watermarkGenerator));

//	watermarkSchemes.push_back(new VisitedStyleScheme(watermarkGenerator));
//	watermarkSchemes.push_back(new UnderlineColorScheme(watermarkGenerator));
//	watermarkSchemes.push_back(new UserAttributesScheme(watermarkGenerator));

	watermarkStyles.push_back(WatermarkStyle()); // One's place
	for (std::vector<WatermarkScheme*>::reverse_iterator it = watermarkSchemes.rbegin();
			it != watermarkSchemes.rend(); ++it) {
		std::vector<WatermarkStyle> newWatermarkStyles((*it)->getWatermarkStyles());

		placeValues.push_back(watermarkStyles.size());
		watermarkStyles = join(newWatermarkStyles, watermarkStyles);
	}
	placeValues.push_back(watermarkStyles.size());
	std::reverse(placeValues.begin(), placeValues.end());
}

std::vector<WatermarkStyle> WatermarkSchemes::join(std::vector<WatermarkStyle>& lefts, std::vector<WatermarkStyle>& rights) {
	std::vector<WatermarkStyle> watermarkStyles;

	for (std::vector<WatermarkStyle>::iterator leftsIt = lefts.begin(); leftsIt != lefts.end(); ++leftsIt)
		for (std::vector<WatermarkStyle>::iterator rightsIt = rights.begin(); rightsIt != rights.end(); ++rightsIt)
			watermarkStyles.push_back((*leftsIt).merge(*rightsIt));
	return watermarkStyles;
}

bool WatermarkSchemes::requiresTextViewCursor() {
	for (std::vector<WatermarkScheme*>::iterator it = watermarkSchemes.begin();
			it != watermarkSchemes.end(); ++it)
		if ((*it)->requiresTextViewCursor())
			return true;
	return false;
}

void WatermarkSchemes::reset() {
	for (std::vector<WatermarkScheme*>::iterator it = watermarkSchemes.begin();
			it != watermarkSchemes.end(); ++it)
		(*it)->reset();
}

void WatermarkSchemes::resetRun(int width, int leftRun, int rightRun) {
	runScheme->reset(width, leftRun, rightRun);
}

WatermarkSchemes::~WatermarkSchemes() {
	for (std::vector<WatermarkScheme*>::iterator it = watermarkSchemes.begin(); 
			it != watermarkSchemes.end(); ++it)
		delete *it;
}

int WatermarkSchemes::getWatermark(SmartCursor& smartCursor) {
	int watermark = 1; // skip the default

	for (size_t i = 0; i < watermarkSchemes.size(); i++)
		watermark += placeValues[i + 1] * watermarkSchemes[i]->getWatermarkStyleIndex(smartCursor);
	return watermark;
}

int WatermarkSchemes::getWatermarkIndexRun(SmartCursor& smartCursor) {
	return runScheme->getWatermarkStyleIndex(smartCursor);
}

int WatermarkSchemes::getWatermarkIndexChannel(int watermark, int channel) {
	if (watermark == Watermarks::UNWATERMARKED)
		return Watermarks::UNWATERMARKED;
	return ((watermark - 1) % placeValues[channel]) / placeValues[channel + 1];
}

int WatermarkSchemes::getWatermarkIndexId(int watermark, int id) {
	for (size_t i = 0; i < watermarkSchemes.size(); i++)
		if (watermarkSchemes[i]->getId() == id)
			return getWatermarkIndexChannel(watermark, i);
	throw std::exception("Illegal argument");
}

int WatermarkSchemes::combineChannel(int oldWatermark, int newWatermarkIndex, int channel) {
	int watermark = oldWatermark == Watermarks::UNWATERMARKED ? 1 :
			oldWatermark - getWatermarkIndexChannel(oldWatermark, channel) * placeValues[channel + 1];

	watermark += newWatermarkIndex * placeValues[channel + 1];
	return watermark;
}

std::string WatermarkSchemes::getWatermarkAbbrev(int watermarkId) {
	if (watermarkId == 0)
		return runScheme->getAbbrev();

	std::vector<WatermarkScheme*>::iterator it;
	for (it = watermarkSchemes.begin(); it != watermarkSchemes.end(); ++it) {
		if ((*it)->getId() == watermarkId)
			return (*it)->getAbbrev();
	}
	return std::string();
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
