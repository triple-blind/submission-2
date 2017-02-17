/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/traitorTracer/com/haathi/traitor/LongPopCntTraitorTracer.cpp#18 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/traitor/LongPopCntTraitorTracer.hpp"
/******************************************************************************
Include others
******************************************************************************/
#ifdef _MSC_VER
#	include <intrin.h>
#	define POPCNT(x) (__popcnt(x.hiOrLo.hi) + __popcnt(x.hiOrLo.lo))
#endif

#ifdef __GNUC__
//#	include <smmintrin.h>
//#	define POPCNT(x) (_mm_popcnt_u32(*(((unsigned int *) &x) /*+ 0*/)) + _mm_popcnt_u32(*(((unsigned int *) &x) + 1)))
//#	define POPCNT(x) (__builtin_popcount()(*(((unsigned int *) &x) /*+ 0*/)) + __builtin_popcount()(*(((unsigned int *) &x) + 1)))
#	define POPCNT(x) x
#endif
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace traitor {
/******************************************************************************
LongPopCntTraitorTheory
******************************************************************************/
// Assumed that size_of(unsigned long long) == WINDOW_SIZE
static const int WINDOW_SIZE = 64;
static const int WINDOW_BYTES = WINDOW_SIZE / 8;
//static const int OVERLAP = WINDOW_SIZE / 2; // Minimum size of watermark overlap
static const int OVERLAP = 58;
static const int OVERHANG = WINDOW_SIZE - OVERLAP;
static const double MAX_ERROR_RATE = 0.10;
static const int MAX_ERRORS = (int) (WINDOW_SIZE * MAX_ERROR_RATE);

static const long long OVERLAP_BITS = 0x03ffffffffffffffLL; // Pre-mask OVERLAP times
static const long long OVERLAP_MASK = 0x0400000000000000LL; // The resulting mask

#define R(x) 1.0 / x##.0
static double reciprocals[WINDOW_SIZE + 1] = {
	   0,  R( 1), R( 2), R( 3), R( 4), R( 5), R( 6), R( 7), R( 8), R( 9),
	R(10), R(11), R(12), R(13), R(14), R(15), R(16), R(17), R(18), R(19),
	R(20), R(21), R(22), R(23), R(24), R(25), R(26), R(27), R(28), R(29),
	R(30), R(31), R(32), R(33), R(34), R(35), R(36), R(37), R(38), R(39),
	R(40), R(41), R(42), R(43), R(44), R(45), R(46), R(47), R(48), R(49),
	R(50), R(51), R(52), R(53), R(54), R(55), R(56), R(57), R(58), R(59),
	R(60), R(61), R(62), R(63), R(64)
};

#define H(x) x##.0 / 64.0
static double sixtyfourths[WINDOW_SIZE + 1] = {
	   0,  H( 1), H( 2), H( 3), H( 4), H( 5), H( 6), H( 7), H( 8), H( 9),
	H(10), H(11), H(12), H(13), H(14), H(15), H(16), H(17), H(18), H(19),
	H(20), H(21), H(22), H(23), H(24), H(25), H(26), H(27), H(28), H(29),
	H(30), H(31), H(32), H(33), H(34), H(35), H(36), H(37), H(38), H(39),
	H(40), H(41), H(42), H(43), H(44), H(45), H(46), H(47), H(48), H(49),
	H(50), H(51), H(52), H(53), H(54), H(55), H(56), H(57), H(58), H(59),
	H(60), H(61), H(62), H(63), H(64)
};

LongPopCntTraitorTheory::LongPopCntTraitorTheory(Exhibit* exhibit, Theory* theory, char* extractedBitStream):
		GenericTraitorTheory(theory),
		watermarkId(exhibit->watermarkId), watermarkAbbrev(exhibit->watermarkAbbrev),
		extractedBitStream(extractedBitStream), extractedBitStreamLength(exhibit->watermarkLength),
		userId(theory->userId), documentId(theory->documentId), versionNo(theory->versionNo), versionId(theory->versionId) {
	errorRate = 0;
}

std::string LongPopCntTraitorTheory::makeString(int start) {
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
		buffer[pos++] = '0' + extractedBitStream[starting + i];
	for (int i = 0; i < trailing; i++)
		buffer[pos++] = '-';

	if (pos != WINDOW_SIZE) {
		int i = 10;
	}

	return std::string(buffer);
}

void LongPopCntTraitorTheory::addMatch(int index, unsigned long long block, int position) {
//	std::cerr << matchCount << "\t" << position << std::endl;
//	errorRate += tmpErrorRate; // need to decode block

	char tmpSeq[WINDOW_SIZE + 1];
	char* sequence = tmpSeq;

	for (unsigned long long mask = 0x8000000000000000LL; mask; mask >>= 1)
		*sequence++ = block & mask ? '1' : '0';
	*sequence = '\0';

	evidences.push_back(Evidence(index, std::string(tmpSeq, WINDOW_SIZE), makeString(position)));
}

void LongPopCntTraitorTheory::getBlock(unsigned long long* block) {
	generateBlock((unsigned char*) block);
}

typedef struct {
	unsigned long hi;
	unsigned long lo;
} HiOrLo;

typedef union {
	unsigned long long hiAndLo;
	HiOrLo hiOrLo;
} HiAndLo;

#include <cassert>

void LongPopCntTraitorTheory::search(int index) {
	if (extractedBitStreamLength < OVERLAP)
		return;

	HiAndLo mismatches;
	int errorCount;
//	double tmpErrorRate;

	unsigned long long unblockMask = 0xfffffffffffffffeLL;
	const char* tmpExtractedBitStream = extractedBitStream;

	unsigned long long block;
	getBlock(&block);

	// Make into static assert
	//assert(sizeof(mismatches) == 8);
	//assert(sizeof(mismatches.hiOrLo) == 8);

	// Initialize
	unsigned long long blockMask = OVERLAP_MASK;
	unsigned long long watermark = block & OVERLAP_BITS;
	unsigned long long extracted = 0;
	for (int i = 0; i < OVERLAP; i++)
		extracted = (extracted << 1) | *tmpExtractedBitStream++;

	// Onramp
	int end = std::min(WINDOW_SIZE, extractedBitStreamLength);
	for (int i = OVERLAP; i < end; i++) {
		mismatches.hiAndLo = extracted ^ watermark;
		errorCount = POPCNT(mismatches) + (WINDOW_SIZE - i);
		if (errorCount <= MAX_ERRORS)
			addMatch(index, block, i - WINDOW_SIZE);
		extracted = (extracted << 1) | *tmpExtractedBitStream++;
		watermark |= block & blockMask;
		blockMask <<= 1;
	}
	// Highway
	const char* stop = extractedBitStream + extractedBitStreamLength;
	//for (int i = WINDOW_SIZE; i < extractedBitStreamLength; i++) {
	while (tmpExtractedBitStream != stop) {
		mismatches.hiAndLo = extracted ^ watermark;
		errorCount = POPCNT(mismatches);
		if (errorCount <= MAX_ERRORS)
			addMatch(index, block, tmpExtractedBitStream - extractedBitStream - WINDOW_SIZE);
		extracted = (extracted << 1) | *tmpExtractedBitStream++;
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
LongPopCntTraitorTracer
******************************************************************************/
LongPopCntTraitorTracer::LongPopCntTraitorTracer(Exhibits& exhibits, Theories& theories) {
	for (Exhibits::iterator exhibitsItr = exhibits.begin(); exhibitsItr != exhibits.end(); ++exhibitsItr) {
		int watermarkLength = (*exhibitsItr)->watermarkLength;
		const char* watermarkBits = (*exhibitsItr)->watermarkBits;
		char* extractedBitStream = new char[watermarkLength + 1];
		
		// Convert these from '1' to 1.  Could this be done in place?
		for (int i = 0; i < watermarkLength; i++)
			extractedBitStream[i] = (watermarkBits[i] == '1' ? 1 : 0);
		extractedBitStreams.push_back(extractedBitStream);
	}

	for (Theories::iterator theoriesItr = theories.begin(); theoriesItr != theories.end(); ++theoriesItr)
		for (size_t i = 0; i < exhibits.size(); i++)
			traitorTheories.push_back(new LongPopCntTraitorTheory(exhibits[i], *theoriesItr, extractedBitStreams[i]));
}
/*
LongPopCntTraitorTracer::LongPopCntTraitorTracer(std::map<int, CryptoInfo>& _userDetails,
		std::map<int, std::string>& _documents,
		std::map<int, CryptoInfo>& _documentKeys,
		std::string& bitstream):
		userDetails(_userDetails), documents(_documents), documentKeys(_documentKeys),
		extractedBitStreamLength((int) bitstream.length()) {

	const char* tmpExtractedBitStream = bitstream.c_str();
	extractedBitStream = new char[extractedBitStreamLength + 1];
	for (int i = 0; i < extractedBitStreamLength; i++)
		extractedBitStream[i] = (tmpExtractedBitStream[i] == '1' ? 1 : 0);

	for (std::map<int, CryptoInfo>::iterator userDetailsItr = userDetails.begin(); userDetailsItr != userDetails.end(); ++userDetailsItr) {
		int userId = userDetailsItr->first;
		std::string userPrivateKey = userDetailsItr->second.privateKey;

		// Insert them once here for use later
		traitors.insert(std::pair<int, int>(userId, 0));
		for (std::map<int, std::string>::iterator documentsItr = documents.begin(); documentsItr != documents.end(); ++documentsItr) {
			int documentId = documentsItr->first;
			// Because of this we are only checking the most recent version, not all of them
			std::string versionId = ProvenanceHelper::base64Decode(documentKeys[documentsItr->first].getSessionKey().getData());

			traitorTheories.push_back(new LongPopCntTraitorTheory(userId, userPrivateKey, documentId, versionId,
					extractedBitStream, extractedBitStreamLength));
		}
	}
}
*/

/*
// TODO: Has to run over all encoding types, which hasn't so far
std::map<int, int> LongPopCntTraitorTracer::populateSuspectedTraitors() {
	std::map<int, int> bestTraitors;

	// There is either nothing to do or insufficient data for the theories
	if (!traitorTheories.size() || extractedBitStreamLength < OVERLAP)
		return bestTraitors;

	std::vector<LongPopCntTraitorTheory*>::iterator traitorTheoryItr;
	// How far we search might depend on what is found
	for (int i = 0; i < 50000; i++)
		for (traitorTheoryItr = traitorTheories.begin(); traitorTheoryItr != traitorTheories.end(); ++traitorTheoryItr)
			(*traitorTheoryItr)->search();
	// Tally the match counts for each of the users
	for (traitorTheoryItr = traitorTheories.begin(); traitorTheoryItr != traitorTheories.end(); ++traitorTheoryItr)
		traitors[(*traitorTheoryItr)->getUserId()] += (*traitorTheoryItr)->getMatchCount();

	int maxMatchCount = std::max_element(traitors.begin(), traitors.end(), traitors.value_comp())->second;

	// Get the most likely traitor(s) based on most matches, no tie breaking based on errorRate
	for (std::map<int, int>::iterator traitorsItr = traitors.begin(); traitorsItr != traitors.end(); ++traitorsItr)
		if (traitorsItr->second == maxMatchCount)
			bestTraitors.insert(*traitorsItr);
	return traitors;	
}
*/

void LongPopCntTraitorTracer::trace(std::ostream& out, int count) {
	std::vector<LongPopCntTraitorTheory*>::iterator traitorTheoryItr;

	for (int i = 0; i < count; i++)
		for (traitorTheoryItr = traitorTheories.begin(); traitorTheoryItr != traitorTheories.end(); ++traitorTheoryItr)
			(*traitorTheoryItr)->search(i);

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

LongPopCntTraitorTracer::~LongPopCntTraitorTracer() {
	std::vector<char*>::iterator extractedBitStreamsItr;
	for (extractedBitStreamsItr = extractedBitStreams.begin(); extractedBitStreamsItr != extractedBitStreams.end(); ++extractedBitStreamsItr)
		delete *extractedBitStreamsItr;

	std::vector<LongPopCntTraitorTheory*>::iterator traitorTheoryItr;
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
