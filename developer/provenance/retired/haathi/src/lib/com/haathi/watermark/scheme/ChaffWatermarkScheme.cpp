/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/watermark/scheme/ChaffWatermarkScheme.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/ChaffWatermarkScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/document/ProvenanceDocument.hpp"
#include "com/haathi/watermark/WatermarkHelper.hpp"

#include "cryptoppBeg.hpp"
#include <cryptopp/osrng.h>
#include <cryptopp/des.h>
#include "cryptoppEnd.hpp"
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;

using namespace com::sun::star::text;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
/******************************************************************************
Class
******************************************************************************/
ChaffWatermarkScheme::ChaffWatermarkScheme(ProvenanceDocument* provenanceDocument):
		WatermarkScheme(provenanceDocument) {
	resetState();
}

void ChaffWatermarkScheme::resetState() {
	_currentBit = DEFAULT_BLOCK_SIZE;
}

bool ChaffWatermarkScheme::isWatermarkEmbedded() {
	return _currentBit < 0;
}

void ChaffWatermarkScheme::setBlockToEmbed(std::bitset<DEFAULT_BLOCK_SIZE> blockToEmbed) {
 	CryptoPP::AutoSeededX917RNG<CryptoPP::DES_EDE3> rng;
   	byte pcbScratch[DEFAULT_BLOCK_SIZE];

	rng.GenerateBlock(pcbScratch, DEFAULT_BLOCK_SIZE);
	_blockToEmbed.reset();
	for (int i = DEFAULT_BLOCK_SIZE-1; i >= 0; i--) {
		unsigned char bit = WatermarkHelper::shiftLeftAndGetBit(&pcbScratch[0], DEFAULT_BLOCK_SIZE/8);
		_blockToEmbed.set(i, bit != 0);
	}
	_currentBit = DEFAULT_BLOCK_SIZE;
}

short ChaffWatermarkScheme::generateWatermark(Reference<XTextViewCursor>& xTextViewCursor) {
	if (isWatermarkEmbedded())
		return 0;
	if (isSpace(xTextViewCursor))
		return 0;

	unsigned char bit1, bit2;
	Color color = makeColor(xTextViewCursor, &bit1, &bit2);
	OUString colorKey = convertToOUString(color);
	OUString styleName;

	std::map<OUString,OUString>* documentStyleMap = provenanceDocument->getStyle();
	std::map<OUString,OUString>::iterator it = documentStyleMap->find(colorKey);
	if (it == documentStyleMap->end()) {
		styleName = OUSTRING("clru") + colorKey;
		documentStyleMap->insert(std::pair<OUString,OUString>(colorKey, styleName));				
		addCharacterStyle(styleName, OUSTRING("CharColor"), Any(color));
	}
	else
		styleName = it->second;

	applyWatermark(xTextViewCursor, styleName);
	return 2;
}	
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
