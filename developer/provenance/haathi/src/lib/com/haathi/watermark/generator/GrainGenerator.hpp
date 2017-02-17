/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/generator/GrainGenerator.hpp#1 $

// See http://www.cryptopp.com/wiki/RandomNumberGenerator where there is
// an explanation of why earlier implementations with AutoSeededRandomPool
// would not deliver a reproducible sequence of values.  The suggested method
// of using CryptoPP::OFB_Mode<CryptoPP::AES>::Encryption threw exceptions.
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__GENERATOR__GRAIN_GENERATOR_HPP
#	define COM__HAATHI__WATERMARK__GENERATOR__GRAIN_GENERATOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/generator/ChaffGenerator.hpp"
#include "com/haathi/watermark/generator/KernelGenerator.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "cryptoppBeg.hpp"
#include <cryptopp/rng.h> // LC_RNG
#include "cryptoppEnd.hpp"
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
class GrainGenerator: public WatermarkGenerator {
	protected:
		// These are references so that the actual object can be
		// in a different generator and they can be shared.
		ChaffGenerator& rChaffGenerator;
		KernelGenerator& rKernelGenerator;
		CryptoPP::LC_RNG decisionGenerator;

		CryptoPP::LC_RNG makeDecisionGenerator(std::string versionId);

	public:
		GrainGenerator(std::vector<std::string> seedInitializer, std::string versionId,
			ChaffGenerator& rChaffGenerator, KernelGenerator& rKernelGenerator);
		virtual ~GrainGenerator();

		void reset(std::string versionId);
		com::haathi::watermark::utils::Watermark generate();
};
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
