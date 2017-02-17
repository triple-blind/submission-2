/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/generator/WatermarkGenerator.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__GENERATOR__WATERMARK_GENERATOR_HPP
#	define COM__HAATHI__WATERMARK__GENERATOR__WATERMARK_GENERATOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "cryptoppBeg.hpp"
#include <cryptopp/osrng.h>
#include "cryptoppEnd.hpp"

#include <limits> // CHAR_BIT
#include <string>
#include <vector>

#ifndef CHAR_BIT
#	define CHAR_BIT 8
#endif
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
class WatermarkGenerator {
	protected:
		byte digest[CryptoPP::SHA256::DIGESTSIZE];

		unsigned char shiftLeftAndGetBit(void *object, size_t size);
		void fillSeed(byte seed[], std::string& versionId, int blockSize);
		com::haathi::watermark::utils::Watermark makeWatermark(byte* bytes, size_t size);
		com::haathi::watermark::utils::Watermark makeWatermark(unsigned long long block);

	public:
		WatermarkGenerator(std::vector<std::string> seedInitializers);
		virtual ~WatermarkGenerator() { };

		virtual void reset(std::string versionId) { };
		virtual com::haathi::watermark::utils::Watermark generate() = 0;
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
