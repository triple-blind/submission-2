/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkTest/watermarkTest.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/

/******************************************************************************
Include others
******************************************************************************/
#include <catch/catch.hpp>

#include "com/haathi/watermark/generator/ChaffGenerator.cpp"
#include "com/haathi/watermark/generator/GrainGenerator.cpp"
#include "com/haathi/watermark/generator/KernelGenerator.cpp"

#include "sdkBeg.hpp"
#include "sdkEnd.hpp"

#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::watermark::generator::ChaffGenerator;
using com::haathi::watermark::generator::GrainGenerator;
using com::haathi::watermark::generator::KernelGenerator;
using com::haathi::watermark::generator::WatermarkGenerator;
using com::haathi::watermark::utils::Watermark;
/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
Content
******************************************************************************/
class GeneratorBuilder {
protected:
		WatermarkGenerator *watermarkGenerator;
	public:
		GeneratorBuilder(): watermarkGenerator(NULL) {
		};

		virtual ~GeneratorBuilder() {
			//if (watermarkGenerator != NULL)
				// delete watermarkGenerator;
			watermarkGenerator = NULL;
		};

		virtual WatermarkGenerator* build(	std::vector<std::string> seedInitializers, std::string versionId) = 0;
};

class KernelGeneratorBuilder: public GeneratorBuilder {
	public:
		WatermarkGenerator* build(std::vector<std::string> seedInitializers, std::string versionId) {
			return watermarkGenerator = new KernelGenerator(seedInitializers, versionId);
		}
};

class ChaffGeneratorBuilder: public GeneratorBuilder {
	public:
		WatermarkGenerator* build(std::vector<std::string> seedInitializers, std::string versionId) {
			return watermarkGenerator = new ChaffGenerator(seedInitializers, versionId);
		}
};

class GrainGeneratorBuilder: public GeneratorBuilder {
	protected:
		ChaffGeneratorBuilder chaffGeneratorBuilder;
		KernelGeneratorBuilder kernelGeneratorBuilder;

		ChaffGenerator *chaffGenerator;
		KernelGenerator *kernelGenerator;
	public:
		GrainGeneratorBuilder(): chaffGenerator(NULL), kernelGenerator(NULL) {
		}

		WatermarkGenerator* build(std::vector<std::string> seedInitializers, std::string versionId) {
			// These will just leak in this test
			chaffGenerator = dynamic_cast<ChaffGenerator*>(chaffGeneratorBuilder.build(seedInitializers, versionId));
			kernelGenerator = dynamic_cast<KernelGenerator*>(kernelGeneratorBuilder.build(seedInitializers, versionId));

			return watermarkGenerator = new GrainGenerator(seedInitializers, versionId, *chaffGenerator, *kernelGenerator);
		}
};

void testSameSeeds(GeneratorBuilder* generatorBuilder) {
	std::string versionId = "0123456789abcdef01234567"; // Should be 24 characters or more

	std::vector<std::string> seedInitializers;
	seedInitializers.push_back("This is a test.");

	WatermarkGenerator* generator1 = generatorBuilder->build(seedInitializers, versionId);
	WatermarkGenerator* generator2 = generatorBuilder->build(seedInitializers, versionId);

	Watermark watermark1 = generator1->generate();
	Watermark watermark2 = generator2->generate();

	if (watermark1 != watermark2) {
		WatermarkGenerator* generator3 = generatorBuilder->build(seedInitializers, versionId);
		WatermarkGenerator* generator4 = generatorBuilder->build(seedInitializers, versionId);

		Watermark watermark3 = generator3->generate();
		Watermark watermark4 = generator4->generate();

		if (watermark3 != watermark4) {
			int foo = 10;
		}

		delete generator4;
		delete generator3;
	}

	REQUIRE(watermark1 == watermark2);

	delete generator2;
	delete generator1;
}

template<class T>
void testSameSeeds() {
	SECTION(typeid(T).name(), "") {
		T* builder = new T();

		testSameSeeds(builder);
		delete builder;
	}
}

TEST_CASE("Same seeds produce same values", "[WatermarkGenerator]") {
	testSameSeeds<KernelGeneratorBuilder>();
	testSameSeeds<ChaffGeneratorBuilder>();
	testSameSeeds<GrainGeneratorBuilder>();
}

void testDifferentSeeds(GeneratorBuilder* generatorBuilder) {
	std::string versionId1 = "0123456789abcdef01234567";
	std::string versionId2 = "0123456789ABCDEF01234567";

	std::vector<std::string> seedInitializers;
	seedInitializers.push_back("This is a test.");

	WatermarkGenerator* generator1 = generatorBuilder->build(seedInitializers, versionId1);
	WatermarkGenerator* generator2 = generatorBuilder->build(seedInitializers, versionId2);

	Watermark watermark1 = generator1->generate();
	Watermark watermark2 = generator2->generate();

	REQUIRE(watermark1 != watermark2);

	delete generator2;
	delete generator1;
}

template<class T>
void testDifferentSeeds() {
	SECTION(typeid(T).name(), "") {
		T* builder = new T();

		testDifferentSeeds(builder);
		delete builder;
	}
}

TEST_CASE("Different seeds produce different values", "[WatermarkGenerator]") {
	testDifferentSeeds<KernelGeneratorBuilder>();
	testDifferentSeeds<ChaffGeneratorBuilder>();
	testDifferentSeeds<GrainGeneratorBuilder>();
}

void testSequenceVaries(GeneratorBuilder* generatorBuilder) {
	std::string versionId = "0123456789abcdef01234567";

	std::vector<std::string> seedInitializers;
	seedInitializers.push_back("This is a test.");

	WatermarkGenerator* generator = generatorBuilder->build(seedInitializers, versionId);

	Watermark watermark1 = generator->generate();
	Watermark watermark2 = generator->generate();

	REQUIRE(watermark1 != watermark2);

	delete generator;
}

template<class T>
void testSequenceVaries() {
	SECTION(typeid(T).name(), "") {
		T* builder = new T();

		testSequenceVaries(builder);
		delete builder;
	}
}

TEST_CASE("Sequence varies as new values are produced", "[WatermarkGenerator]") {
	testSequenceVaries<KernelGeneratorBuilder>();
	testSequenceVaries<ChaffGeneratorBuilder>();
	testSequenceVaries<GrainGeneratorBuilder>();
}

void testSameReseeding(GeneratorBuilder* generatorBuilder) {
	std::string versionId = "0123456789abcdef01234567";

	std::vector<std::string> seedInitializers;
	seedInitializers.push_back("This is a test.");

	WatermarkGenerator* generator = generatorBuilder->build(seedInitializers, versionId);

	Watermark watermarkA1 = generator->generate();
	Watermark watermarkA2 = generator->generate();

	generator->reset(versionId);

	Watermark watermarkB1 = generator->generate();
	Watermark watermarkB2 = generator->generate();

	REQUIRE(watermarkA1 == watermarkB1);
	REQUIRE(watermarkA2 == watermarkB2);

	delete generator;
}

template<class T>
void testSameReseeding() {
	SECTION(typeid(T).name(), "") {
		T* builder = new T();

		testSameReseeding(builder);
		delete builder;
	}
}

TEST_CASE("Same reseeding produces same sequence", "[WatermarkGenerator]") {
	testSameReseeding<KernelGeneratorBuilder>();
	testSameReseeding<ChaffGeneratorBuilder>();
	testSameReseeding<GrainGeneratorBuilder>();
}
/******************************************************************************
Namespace
******************************************************************************/

/*****************************************************************************/
