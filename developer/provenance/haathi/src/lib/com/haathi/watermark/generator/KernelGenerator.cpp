/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/generator/KernelGenerator.cpp#3 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/generator/KernelGenerator.hpp"
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
KernelGenerator::KernelGenerator(std::vector<std::string> seedInitializers,
		 std::string versionId): WatermarkGenerator(seedInitializers) {
	reset(versionId);
}

void KernelGenerator::reset(std::string versionId) { // 36
	byte seed[CryptoPP::AES::DEFAULT_KEYLENGTH]; // 16

	fillSeed(seed, versionId, sizeof(seed));
	generator.Reseed(seed, sizeof(seed), digest, digest);
}

//Watermark KernelGenerator::generate() {
//	byte bytes[Watermark::SIZE / 8];
//
//	generator.GenerateBlock(bytes, sizeof(bytes));
//	return makeWatermark(bytes, sizeof(bytes));
//}

Watermark KernelGenerator::generate() {
	unsigned long long block;

	generator.GenerateBlock((unsigned char*) &block, sizeof(block));
	return makeWatermark(block);
}

KernelGenerator::~KernelGenerator() {
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
