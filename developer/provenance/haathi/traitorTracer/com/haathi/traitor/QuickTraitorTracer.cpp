/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/traitorTracer/com/haathi/traitor/QuickTraitorTracer.cpp#13 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/traitor/QuickTraitorTracer.hpp"
/******************************************************************************
Include others
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
QuickTraitorTheory
******************************************************************************/
static const int WINDOW_SIZE = 64;
//static int const OVERLAP = WINDOW_SIZE / 2; // Minimum size of window overlap
// We're now assuming that non-overlapping bytes are incorrect.
// Since there can only be 0.10 * 64 or six wrong, the other 58 must be right.
static const int OVERLAP = 58; // Minimum size of window overlap
//static int const OVERHANG = WINDOW_SIZE - OVERLAP; // Maximum size of window overhang
//static const double MAX_ERROR_RATE = 0.10;
// Always used now.  Rate is irrelevant.
static const int MAX_ERROR = 6;

// TODO: pass in maxErrorRate based on SchemeId

QuickTraitorTheory::QuickTraitorTheory(Exhibit* exhibit, Theory* theory):
		GenericTraitorTheory(theory),
		watermarkId(exhibit->watermarkId), watermarkAbbrev(exhibit->watermarkAbbrev),
		extractedBitStream(exhibit->watermarkBits), extractedBitStreamLength(exhibit->watermarkLength),
		userId(theory->userId), documentId(theory->documentId), versionNo(theory->versionNo), versionId(theory->versionId) {
	errorRate = 0;
}

void QuickTraitorTheory::generateBitStream(char* sequence) {
	unsigned long long block;
	generateBlock((unsigned char*) &block);

	for (unsigned long long mask = 0x8000000000000000LL; mask; mask >>= 1)
		*sequence++ = block & mask ? '1' : '0';
	*sequence = '\0';
}

bool QuickTraitorTheory::matchBeg(const char* shortBits, int shortSize, const char* longBits, int start) {
	int error = -start;
	int stop = start + shortSize;
	const char* pLong = longBits;
	const char* pStop = pLong + stop;
	const char* pShort = shortBits - start;

	while (pLong != pStop)
		if (*pLong++ != *pShort++)
			if (++error > MAX_ERROR)
				return false;
	return true;
}

bool QuickTraitorTheory::matchMid(const char* shortBits, int shortSize, const char* longBits, int start, double errorInc) {
	int error = 0;
	const char* pLong = longBits + start;
	const char* pStop = pLong + shortSize;
	const char* pShort = shortBits;

	while (pLong != pStop)
		if (*pLong++ != *pShort++)
			if (++error > MAX_ERROR)
				return false;

//	std::cerr << "Found a match at position " << start << " - " << shortBits << std::endl;
	return true;
}

bool QuickTraitorTheory::matchEnd(const char* shortBits, const char* longBits, int longSize, int start) {
	int error = WINDOW_SIZE - (longSize - start);
	const char* pLong = longBits + start;
	const char* pStop = longBits + longSize;
	const char* pShort = shortBits;

	while (pLong != pStop)
		if (*pLong++ != *pShort++)
			if (++error > MAX_ERROR)
				return false;
	return true;
}

std::string QuickTraitorTheory::makeString(int start) {
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
		buffer[pos++] = extractedBitStream[starting + i];
	for (int i = 0; i < trailing; i++)
		buffer[pos++] = '-';

	if (pos != WINDOW_SIZE) {
		int i = 10;
	}

	return std::string(buffer);
}

Evidence QuickTraitorTheory::makeEvidence(int index, char* generatedBitStream, int start) {
	bool isLongBitStream = WINDOW_SIZE <= extractedBitStreamLength;

	return Evidence(index, generatedBitStream, makeString(isLongBitStream ? start : -start));
}

void QuickTraitorTheory::search(int index) {
	if (extractedBitStreamLength < OVERLAP)
		return;

	char generatedBitStream[WINDOW_SIZE + 1];

	//	static int count = 0;
	
	const char* shortBits;
	const char* longBits;
	int shortSize;
	int longSize;

	generateBitStream(generatedBitStream);

//	matchCount = 0;
//	errorRate = 0;
//	int beg, mid, end;
//	beg = mid = end = 0;

	// The normal case where there is plenty of data
	bool isLongBitStream = WINDOW_SIZE <= extractedBitStreamLength;

	if (isLongBitStream) {
		shortBits = generatedBitStream;
		shortSize = WINDOW_SIZE;
		longBits = extractedBitStream;
		longSize = extractedBitStreamLength;
	}
	else {
		shortBits = extractedBitStream;
		shortSize = extractedBitStreamLength;
		longBits = generatedBitStream;
		longSize = WINDOW_SIZE;
	}

	int windowSize = shortSize;
	int overlap = OVERLAP;
	int overhang = windowSize - overlap;
	double errorInc = 1.0 / shortSize;

//	std::cerr << "Loop " << count << std:: endl;
//	count++;

	for (int start = -overhang; start < 0; start++/*, beg++*/)
		if (matchBeg(shortBits, shortSize, longBits, start))
			evidences.push_back(makeEvidence(index, generatedBitStream, start));
	for (int start = 0; start < longSize - windowSize + 1; start++/*, mid++*/)
		if (matchMid(shortBits, shortSize, longBits, start, errorInc))
			evidences.push_back(makeEvidence(index, generatedBitStream, start));
	for (int start = std::max(1, longSize - windowSize + 1); start <= longSize - overlap; start++/*, end++*/)
		if (matchEnd(shortBits, longBits, longSize, start))
			evidences.push_back(makeEvidence(index, generatedBitStream, start));

//	std::cout << beg << "\t" << mid << "\t" << end << std::endl;
}
/******************************************************************************
QuickTraitorTracer
******************************************************************************/
QuickTraitorTracer::QuickTraitorTracer(Exhibits& exhibits, Theories& theories) {
	for (Theories::iterator theoriesItr = theories.begin(); theoriesItr != theories.end(); ++theoriesItr)
		for (Exhibits::iterator exhibitsItr = exhibits.begin(); exhibitsItr != exhibits.end(); ++exhibitsItr)
			traitorTheories.push_back(new QuickTraitorTheory(*exhibitsItr, *theoriesItr));
}

void QuickTraitorTracer::trace(std::ostream& out, int count) {
	std::vector<QuickTraitorTheory*>::iterator traitorTheoryItr;

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

QuickTraitorTracer::~QuickTraitorTracer() {
	std::vector<QuickTraitorTheory*>::iterator traitorTheoryItr;

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
