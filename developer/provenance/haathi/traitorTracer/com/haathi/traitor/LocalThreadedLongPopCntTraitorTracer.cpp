/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/traitorTracer/com/haathi/traitor/LocalThreadedLongPopCntTraitorTracer.cpp#4 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/traitor/LocalThreadedLongPopCntTraitorTracer.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Timer.hpp"

#ifdef _MSC_VER
#	include <intrin.h>
#endif

#ifdef __GNUC__
#	include <smmintrin.h>
#endif

#include <boost/thread.hpp>

#include <algorithm>
#include <cassert>
/******************************************************************************
Define
******************************************************************************/
#ifdef _MSC_VER
#	define POPCNT(x) (__popcnt(x.hiOrLo.hi) + __popcnt(x.hiOrLo.lo))
#	if defined(max)
#		undef max // Windows defines this and interferes with std::max
#	endif
#	if defined(min)
#		undef min // Windows defines this and interferes with std::min
#	endif
#endif

#ifdef __GNUC__
//#	define POPCNT(x) (_mm_popcnt_u32(*(((unsigned int *) &x) /*+ 0*/)) + _mm_popcnt_u32(*(((unsigned int *) &x) + 1)))
//#	define POPCNT(x) (__builtin_popcount()(*(((unsigned int *) &x) /*+ 0*/)) + __builtin_popcount()(*(((unsigned int *) &x) + 1)))
#endif
/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::Timer;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace traitor {
/******************************************************************************
Configuration
******************************************************************************/
// Assumed that size_of(unsigned long long) == WINDOW_SIZE
static const int WINDOW_SIZE = 64;
static const int WINDOW_BYTES = WINDOW_SIZE / 8;
static const double MAX_ERROR_RATE = 0.10;
static const int MAX_ERRORS = (int) (WINDOW_SIZE * MAX_ERROR_RATE);
static const int OVERLAP = WINDOW_SIZE - MAX_ERRORS;
static const int OVERHANG = WINDOW_SIZE - OVERLAP;
static const long long OVERLAP_MASK = 0x01LL << OVERLAP; // The resulting mask
static const long long OVERLAP_BITS = OVERLAP_MASK - 1; // Pre-mask OVERLAP times
/******************************************************************************
Helpers
******************************************************************************/
typedef struct {
	unsigned long hi;
	unsigned long lo;
} HiOrLo;

typedef union {
	unsigned long long hiAndLo;
	HiOrLo hiOrLo;
} HiAndLo;

class LocalSearcher {
	protected:
		static int count;

		LocalThreadedLongPopCntTraitorTheory* traitorTheory;

	public:
		static void setCount(int count) {
			LocalSearcher::count = count;
		}

		LocalSearcher(LocalThreadedLongPopCntTraitorTheory* traitorTheory): traitorTheory(traitorTheory) {
		}

		void operator()() {
			if (traitorTheory->extractedBitStreamLength < OVERLAP)
				return;
			for (int i = 0; i < count; i++)
				traitorTheory->search(i);
		}
};

int LocalSearcher::count = 50000;
/******************************************************************************
LongPopCntTraitorTheory
******************************************************************************/
LocalThreadedLongPopCntTraitorTheory::LocalThreadedLongPopCntTraitorTheory(Exhibit* exhibit, Theory* theory,
		const unsigned long long* extractedBitStream):
		GenericTraitorTheory(theory),
		watermarkId(exhibit->watermarkId), watermarkAbbrev(exhibit->watermarkAbbrev),
		extractedBitStream(extractedBitStream), extractedBitStreamLength(exhibit->watermarkLength),
		userId(theory->userId), documentId(theory->documentId), versionNo(theory->versionNo), versionId(theory->versionId) {
}

int bitAt(const unsigned long long* extractedBitStream, int pos) {
	return (extractedBitStream[pos / 64] & (0x01LL << (63 - pos % 64)) ? 1 : 0);
}

std::string LocalThreadedLongPopCntTraitorTheory::makeString(int start) {
	char buffer[WINDOW_SIZE + 1];
	buffer[WINDOW_SIZE] = '\0';

	int leading = std::max(-start, 0);
	int starting = std::max(start, 0);
	int remaining = WINDOW_SIZE - leading;
	int available = extractedBitStreamLength - starting;
	int running = std::min(available, remaining);
	int trailing = remaining - running;

	int pos = 0;
	for (int i = 0; i < leading; i++)
		buffer[pos++] = '-';
	for (int i = 0; i < running; i++)
		buffer[pos++] = '0' + bitAt(extractedBitStream, starting + i);
	for (int i = 0; i < trailing; i++)
		buffer[pos++] = '-';
	return std::string(buffer);
}

void LocalThreadedLongPopCntTraitorTheory::addMatch(int index, unsigned long long block, int position) {
	char tmpSeq[WINDOW_SIZE + 1];
	char* sequence = tmpSeq;

	for (unsigned long long mask = 0x8000000000000000LL; mask; mask >>= 1)
		*sequence++ = block & mask ? '1' : '0';
	*sequence = '\0';

	evidences.push_back(Evidence(index, std::string(tmpSeq, WINDOW_SIZE), makeString(position)));
}

void LocalThreadedLongPopCntTraitorTheory::getBlock(unsigned long long* block) {
	generateBlock((unsigned char*) block);
}

void LocalThreadedLongPopCntTraitorTheory::search(int index) {
	HiAndLo mismatches;
	int errorCount;

	unsigned long long unblockMask = 0xfffffffffffffffeLL;
	const unsigned long long* tmpExtractedBitStream = extractedBitStream;

	unsigned long long block;
	getBlock(&block);

	// Initialize
	unsigned long long blockMask = OVERLAP_MASK;
	unsigned long long watermark = block & OVERLAP_BITS;
	unsigned long long extracted = 0; // *tmpExtractedBitStream & OVERLAP_BITS;
	unsigned long long extractedBitMask = 0x8000000000000000LL;
	for (int i = 0; i < OVERLAP; i++) {
		extracted = (extracted << 1) | (*tmpExtractedBitStream & extractedBitMask ? 1 : 0);
		// This won't happen because onramp is too small
		if (!(extractedBitMask >>= 1)) {
			extractedBitMask = 0x8000000000000000LL;
			tmpExtractedBitStream++;
		}
	}

	// Onramp
	int end = std::min(WINDOW_SIZE, extractedBitStreamLength);
	for (int i = OVERLAP; i < end; i++) {
		mismatches.hiAndLo = extracted ^ watermark;
		errorCount = POPCNT(mismatches) + (WINDOW_SIZE - i);
		if (errorCount <= MAX_ERRORS)
			addMatch(index, block, i - WINDOW_SIZE);
		extracted = (extracted << 1) | (*tmpExtractedBitStream & extractedBitMask ? 1 : 0);
		// This won't happen because onramp is too small
		// Last time should get it
		if (!(extractedBitMask >>= 1)) {
			extractedBitMask = 0x8000000000000000LL;
			tmpExtractedBitStream++;
		}
		watermark |= block & blockMask;
		blockMask <<= 1;
	}
	// Highway
//	while (tmpExtractedBitStream != stop) {
	// Have done end so far and want to get to extractedBitStreamLength
	for (int i = 0; i != extractedBitStreamLength - end; i++) {
		mismatches.hiAndLo = extracted ^ watermark;
		errorCount = POPCNT(mismatches);
		if (errorCount <= MAX_ERRORS)
			addMatch(index, block, i);
		extracted = (extracted << 1) | (*tmpExtractedBitStream & extractedBitMask ? 1 : 0);
		if (!(extractedBitMask >>= 1)) {
			extractedBitMask = 0x8000000000000000LL;
			tmpExtractedBitStream++;
		}
	}
	// Offramp
	for (int i = 0; i <= OVERHANG; i++) {
		mismatches.hiAndLo = extracted ^ watermark;
		errorCount = POPCNT(mismatches) + i;
		if (errorCount <= MAX_ERRORS)
			addMatch(index, block, extractedBitStreamLength - WINDOW_SIZE + i);
		extracted <<= 1; // Add no more bits
		if (blockMask) {
			watermark |= block & blockMask;
			blockMask <<= 1;
		}
		else
			end--;
		watermark &= unblockMask;
		unblockMask <<= 1;
	}
}
/******************************************************************************
LocalThreadedLongPopCntTraitorTracer
******************************************************************************/
LocalThreadedLongPopCntTraitorTracer::LocalThreadedLongPopCntTraitorTracer(Exhibits& exhibits, Theories& theories) {
	// Make into static assert
	//assert(sizeof(mismatches) == 8);
	//assert(sizeof(mismatches.hiOrLo) == 8);

	for (Exhibits::iterator exhibitsItr = exhibits.begin(); exhibitsItr != exhibits.end(); ++exhibitsItr) {
		int watermarkLength = (*exhibitsItr)->watermarkLength;
		const char* watermarkBits = (*exhibitsItr)->watermarkBits;
		unsigned long long* extractedBitStream = new unsigned long long[watermarkLength / 64 + 1];

		for (int i = 0; i < watermarkLength; i += 64) {
			unsigned long long bits = 0;

			for (int j = 0; j < 64; j++) {
				unsigned char watermarkBit = i + j < watermarkLength ? watermarkBits[i + j] : '0';

				bits = (bits << 1) | (watermarkBit == '1' ? 1 : 0);
			}
			extractedBitStream[i / 64] = bits;
		}
		extractedBitStreams.push_back(extractedBitStream);
	}

	for (Theories::iterator theoriesItr = theories.begin(); theoriesItr != theories.end(); ++theoriesItr)
		for (size_t i = 0; i < exhibits.size(); i++)
			traitorTheories.push_back(new LocalThreadedLongPopCntTraitorTheory(exhibits[i], *theoriesItr, extractedBitStreams[i]));
}

void LocalThreadedLongPopCntTraitorTracer::trace(std::ostream& out, int count) {
	std::vector<boost::thread*> threads;
	LocalSearcher::setCount(count);

	Timer timer;
	timer.start();

	std::vector<LocalThreadedLongPopCntTraitorTheory*>::iterator traitorTheoryItr;
	for (traitorTheoryItr = traitorTheories.begin(); traitorTheoryItr != traitorTheories.end(); ++traitorTheoryItr) {
		threads.push_back(new boost::thread(LocalSearcher(*traitorTheoryItr)));
		//LocalSearcher localSearcher(*traitorTheoryItr);

		//localSearcher();
	}
	
	std::vector<boost::thread*>::iterator threadItr;
	for (threadItr = threads.begin(); threadItr != threads.end(); ++threadItr) {
		(*threadItr)->join();
		delete *threadItr;
	}

	timer.stop();	
	std::cout << "Inner search time: " << timer.getElapsedTime() << std::endl;

	out 
		<< "UserId" << "\t" << "DocumentId" << "\t" << "VersionNo"
		<< "\t" << "SchemeId" << "\t" << "SchemeAbbrev"
		<< "\t" << "count" << "\t" << "[idx, rnd, out]..."
		<< std::endl;

	for (traitorTheoryItr = traitorTheories.begin(); traitorTheoryItr != traitorTheories.end(); ++traitorTheoryItr) {
		out
			<< (*traitorTheoryItr)->userId
			<< "\t" << (*traitorTheoryItr)->documentId
			<< "\t" << (*traitorTheoryItr)->versionNo
			<< "\t" << (*traitorTheoryItr)->watermarkId
			<< "\t" << (*traitorTheoryItr)->watermarkAbbrev;

		Evidences& evidences = (*traitorTheoryItr)->evidences;
		Evidences::iterator evidencesItr;

		out << "\t" << evidences.size();

		for (evidencesItr = evidences.begin(); evidencesItr != evidences.end(); ++evidencesItr)
			out
				<< "\t" << evidencesItr->index
				<< "," << evidencesItr->rnd
				<< "," << evidencesItr->out;
		out << std::endl;
	}
}

LocalThreadedLongPopCntTraitorTracer::~LocalThreadedLongPopCntTraitorTracer() {
	std::vector<const unsigned long long*>::iterator extractedBitStreamsItr;
	for (extractedBitStreamsItr = extractedBitStreams.begin(); extractedBitStreamsItr != extractedBitStreams.end(); ++extractedBitStreamsItr)
		delete *extractedBitStreamsItr;

	std::vector<LocalThreadedLongPopCntTraitorTheory*>::iterator traitorTheoryItr;
	for (traitorTheoryItr = traitorTheories.begin(); traitorTheoryItr != traitorTheories.end(); ++traitorTheoryItr)
		delete *traitorTheoryItr;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
