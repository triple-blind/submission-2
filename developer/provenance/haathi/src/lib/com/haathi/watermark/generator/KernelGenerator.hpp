/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/generator/KernelGenerator.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__GENERATOR__KERNEL_GENERATOR_HPP
#	define COM__HAATHI__WATERMARK__GENERATOR__KERNEL_GENERATOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/Key.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "cryptoppBeg.hpp"
#include <cryptopp/osrng.h> // AutoSeededX917RNG
#include <cryptopp/aes.h> // AES
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
class KernelGenerator: public WatermarkGenerator {
	protected:
		CryptoPP::AutoSeededX917RNG<CryptoPP::AES> generator;

	public:
		KernelGenerator(std::vector<std::string> seedInitializer, std::string versionId);
		virtual ~KernelGenerator();

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
