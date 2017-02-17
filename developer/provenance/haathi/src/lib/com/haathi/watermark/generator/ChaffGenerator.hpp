/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/generator/ChaffGenerator.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__GENERATOR__CHAFF_GENERATOR_HPP
#	define COM__HAATHI__WATERMARK__GENERATOR__CHAFF_GENERATOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "cryptoppBeg.hpp"
#include <cryptopp/osrng.h> // AutoSeededX917RNG
#include <cryptopp/des.h> // DES_EDE3
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
class ChaffGenerator: public WatermarkGenerator {
	protected:
		CryptoPP::AutoSeededX917RNG<CryptoPP::DES_EDE3> generator;

	public:
		ChaffGenerator(std::vector<std::string> seedInitializers,
				std::string versionId);
		virtual ~ChaffGenerator();

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
