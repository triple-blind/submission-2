/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/generator/WatermarkGenerator.cpp#3 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
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
WatermarkGenerator::WatermarkGenerator(std::vector<std::string> seedInitializers) {
	CryptoPP::SHA256 hash;

	for (size_t index = 0; index < seedInitializers.size(); index++)
		hash.Update((byte*) seedInitializers[index].c_str(), seedInitializers[index].length());
	hash.Final(digest);
}

Watermark WatermarkGenerator::makeWatermark(byte* bytes, size_t size) {
	Watermark watermark;

	for (int i = 0; i < Watermark::SIZE; i++)
		watermark.set(i, shiftLeftAndGetBit(&bytes[0], size) != 0);
	return watermark;
}

Watermark WatermarkGenerator::makeWatermark(unsigned long long block) {
	// If this is treated like a number, the bits come out in a different order
	// However, start with high and be consistent, because the traitorTracer follows!
	unsigned long long mask = 0x8000000000000000LL;
	Watermark watermark;

	for (int i = 0; i < Watermark::SIZE; i++, mask >>= 1)
		watermark.set(i, block & mask ? 1 : 0);
	return watermark;
}

unsigned char WatermarkGenerator::shiftLeftAndGetBit(void *object, size_t size) {
	unsigned char lbit = (static_cast<unsigned char*>(object)[0]) & (1 << (CHAR_BIT - 1)) ? 1 : 0;

	for (unsigned char* data = static_cast<unsigned char*>(object); size--; ++data ) {
		unsigned char bit = size ? (data[1] & (1 << (CHAR_BIT - 1)) ? 1 : 0) : 0;
	      
	   	*data <<= 1;
	   	*data |= bit;
	}
	return lbit;
}

void WatermarkGenerator::fillSeed(byte seed[], std::string& versionId, int blockSize) {
	int rounds = (versionId.length() + blockSize - 1) / blockSize;

	for (int i = 0; i < blockSize; i++) {
		seed[i] = 0;

		for (int j = 0; j < rounds; j++) {
			int pos = i + j * blockSize;
			seed[i] += pos < (int) versionId.length() ? versionId[pos] : pos;
		}
	}
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
