/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/generator/GrainGenerator.cpp#1 $

// Imagine someone loads a file and has it watermarked, then does copy and paste
// and extracts the watermark.  Then does this again with the same document again
// and again.  If the watermark bits stay the same but the decision changes,
// then it would be possible to conclude which were watermark bytes and which not.
// Assuming that chaff does not match anything.  Why does chaff have to be different?
// So don't know everything that is a watermark?
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/generator/GrainGenerator.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <algorithm>
#include <exception>
#include <iostream>
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
GrainGenerator::GrainGenerator(std::vector<std::string> seedInitializers, std::string versionId, 
		ChaffGenerator& rChaffGenerator, KernelGenerator& rKernelGenerator):
		WatermarkGenerator(seedInitializers),
		rChaffGenerator(rChaffGenerator),
		rKernelGenerator(rKernelGenerator),
		decisionGenerator(0) {
	rChaffGenerator.reset(versionId);
	rKernelGenerator.reset(versionId);
	decisionGenerator = makeDecisionGenerator(versionId);
}

CryptoPP::LC_RNG GrainGenerator::makeDecisionGenerator(std::string versionId) {
	int hash = 0;

	for (size_t i = 0; i < versionId.size(); i++)
		hash += versionId[i];
	return CryptoPP::LC_RNG(hash);
}

void GrainGenerator::reset(std::string versionId) {
	decisionGenerator = makeDecisionGenerator(versionId);
	rChaffGenerator.reset(versionId);
	rKernelGenerator.reset(versionId);
}

Watermark GrainGenerator::generate() {
	byte randomNumber = decisionGenerator.GenerateByte();
	bool chaff = 100 < randomNumber && randomNumber < 150;

	return chaff ? rChaffGenerator.generate() : rKernelGenerator.generate();
}

GrainGenerator::~GrainGenerator() {
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
