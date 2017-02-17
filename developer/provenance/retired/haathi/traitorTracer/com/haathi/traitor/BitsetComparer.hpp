#ifndef BITSETCOMPARER_HPP
#define BITSETCOMPARER_HPP

// System includes
#include <string>
#include <map>
#include <vector>
#include <bitset>

namespace com {
namespace haathi {
namespace traitor {

class BitsetComparer
{
	static int const WINDOW_SIZE = 64;
	unsigned char block[WINDOW_SIZE/8];
	int bitPosition ;
	int extractedNumber;
	std::vector<unsigned char> ConvertBlock();
	unsigned char Shiftl(void *object, size_t size);
	std::string HashGeneratedWindow();
	int GetMatch(std::string generatedSequence, std::string originalSequence,float confidencePercentage);
	public:
	BitsetComparer();
	int Compare(std::vector<std::string> seed,std::string documentKey,std::vector< std::bitset<64> > extractedBitset);
	std::vector<int> CompareBitByBit(std::vector<std::string> seed, std::string documentKey, std::vector< std::string > extractedBitset, float confidencePrecentage);
};

}
;
}
;
}
;
#endif

