/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/generator/ChaffGenerator.cpp#3 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/generator/ChaffGenerator.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::watermark::utils::Watermark;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace generator {
/******************************************************************************
Class
******************************************************************************/
ChaffGenerator::ChaffGenerator(std::vector<std::string> seedInitializers,
		std::string versionId): WatermarkGenerator(seedInitializers) {
	reset(versionId);
}

void ChaffGenerator::reset(std::string versionId) { // 36
	byte seed[CryptoPP::DES_EDE3::DEFAULT_KEYLENGTH]; // 24

	fillSeed(seed, versionId, sizeof(seed));
	generator.Reseed(seed, sizeof(seed), digest, digest);
}

//Watermark ChaffGenerator::generate() {
//	byte bytes[Watermark::SIZE / 8];
//
//	generator.GenerateBlock(bytes, sizeof(bytes));
//	return makeWatermark(bytes, sizeof(bytes));
//}

Watermark ChaffGenerator::generate() {
	unsigned long long block;

	generator.GenerateBlock((unsigned char*) &block, sizeof(block));
	return makeWatermark(block);
}

ChaffGenerator::~ChaffGenerator() {
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
