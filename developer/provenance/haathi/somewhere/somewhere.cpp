/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/somewhere/somewhere.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream>
#include <map>
#include <vector>

#ifdef __GNUC__
#	ifndef __llvm__
// My version is 4.2 and doesn't understand this
#		pragma GCC diagnostic push
#	endif
#	pragma GCC diagnostic ignored "-Wunused-variable"
// Unfortunately, this is not sufficient here
#	pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include <cryptopp/rng.h> // LC_RNG
#ifdef __GNUC__
#	ifndef __llvm__
// My version is 4.2 and doesn't understand this
#		pragma GCC diagnostic pop
#	else
// Go ahead and fake using it
		std::string (*dummy)(const wchar_t*, bool) = &CryptoPP::StringNarrow;
#	endif
// This doesn't not work for version 4.2 llvm, so see hack above
#	pragma GCC diagnostic ignored "-Wunused-function"
#endif
/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Content
******************************************************************************/
void run(int threshold) {
	const int documentSize = 1024;
	const int watermarkSize = 32;
	CryptoPP::LC_RNG random(42);
	std::vector<int> document(documentSize, -1);
	std::map<int, int> counts;
	int round = 0;
	int fullCount = 0;
	counts[-1] = documentSize;
	int countSum = 0;

	while (round < 10000 + 1000) {
		int pos = random.GenerateWord32(0, documentSize - watermarkSize - 1);

		for (int i = pos; i < pos + watermarkSize; i++) {
			int oldWatermark = document[i];
			int oldCount = counts[oldWatermark];

			document[i] = round;
			if (oldCount == 1)
				counts.erase(oldWatermark);
			else {
				counts[oldWatermark] = oldCount - 1;
				// oldCount was threshold and just now reduced one more
				if (oldCount == threshold && oldWatermark != -1)
					fullCount--;
			}
		}
		counts[round] = watermarkSize;
		fullCount++;
//		std::cout
//				<< round << "\t"
//				<< fullCount
//				<< std::endl;

		if (10000 <= round)
			countSum += fullCount;
		round++;
	}
	std::cout << "For " << threshold << ", ave. of last 1000 is " << (float) countSum / 1000 << std::endl;

	std::map<int, int>::iterator it;
	int total = 0;
	for (it = counts.begin(); it != counts.end(); ++it)
		total += it->second;

	if (total != documentSize)
		std::cerr << "This isn't expected!" << std::endl;
}

int main(int argc, char *argv[]) {
	for (int threshold = 32; threshold >= 20; threshold--)
		run(threshold);
}
/*****************************************************************************/

