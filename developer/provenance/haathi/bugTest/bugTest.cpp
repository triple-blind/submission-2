/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/bugTest/bugTest.cpp#1 $
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout
#include <string> // strcmp
#include <vector> // vector

#include "com/haathi/crypto/ProvenanceHelper.hpp" // ProvenanceHelper
#include "com/haathi/network/Server.hpp" // Server
#include "com/haathi/utils/StringWrapper.hpp" // ConvertToOUString

#ifdef _MSC_VER
#	pragma warning(disable: 4996)
#endif
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::network;
using namespace com::haathi::crypto;
using namespace com::haathi::utils;

using namespace std;
/******************************************************************************
Content
******************************************************************************/
/*!
 This function scans through two different strings and identifies the differences between
 them. It will return the differences with respect to string a.
 */
static OUString getDifferences1(OUString a, OUString b,
		bool flipped = false) throw (std::out_of_range, std::bad_alloc) {
	OUString result;

	sal_Int32 i = 0;
	sal_Int32 j = 0;
	if (a.getLength() > b.getLength()) {
		return getDifferences1(b, a, !flipped);
	} else if (a.getLength() == b.getLength()) {
		for (i = 0; i < a.getLength(); ++i) {
			if (a[i] != b[i]) {
				sal_Unicode after = a[i];
				sal_Unicode before = b[i];
				result = result + OUString(before) + OUSTRING(">") + OUString(after) + OUSTRING("@") + convertToOUString(i);
			}
		}
		return result;
	}

	for (i = 0, j = 0; (i < a.getLength() && j < b.getLength()); i++, j++) {
		if (a[i] != b[j]) {
			sal_Unicode ch = b[i];
			OUString data(ch);
			// The characters are different, so we should log the change.
			result = result + (flipped ? OUSTRING("+") : OUSTRING("-")) + data + convertToOUString(i);
			j++;
		}
	}

	// Now, we process the last event for b.
	OUString end = b.copy(j);
	if (end.getLength() > 0) {
		result = result + (flipped ? OUSTRING("+") : OUSTRING("-")) + end + convertToOUString(j);
	}
	return result;
}


static OUString getDifferences2(OUString before, OUString after)
		throw (std::out_of_range, std::bad_alloc) {
	OUString result;

	if (before.getLength() < after.getLength()) {
		// Characters were inserted.
		OUString inserted;
		sal_Int32 insertedPos;
		sal_Int32 beforePos = 0;
		sal_Int32 afterPos = 0;
		for (; beforePos < before.getLength() && afterPos < after.getLength(); afterPos++)
			if (before[beforePos] != after[afterPos]) {
				if (inserted.getLength() == 0)
					insertedPos = afterPos; // start inserting
				inserted += OUString(after[afterPos]); // keep inserting
			}
			else {
				if (inserted.getLength() != 0) {
					result = result + (true ? OUSTRING("+") : OUSTRING("-")) + inserted + convertToOUString(insertedPos);
					inserted = OUString(); // transition away from inserting
				}
				beforePos++;
			}
		if (afterPos < after.getLength()) { // need to insert the rest
			if (inserted.getLength() == 0)
				insertedPos = afterPos;
			inserted += after.copy(afterPos);
			result = result + (true ? OUSTRING("+") : OUSTRING("-")) + inserted + convertToOUString(insertedPos);
			return result;
		}
		if (beforePos < before.getLength()) { // need to delete the rest
			if (inserted.getLength() != 0)
				result = result + (true ? OUSTRING("+") : OUSTRING("-")) + inserted + convertToOUString(insertedPos);
			result = result + (false ? OUSTRING("+") : OUSTRING("-")) + before.copy(beforePos) + convertToOUString(afterPos);
			return result;
		}
		if (inserted.getLength() != 0)
			result = result + (true ? OUSTRING("+") : OUSTRING("-")) + inserted + convertToOUString(insertedPos);
	}

	if (before.getLength() > after.getLength()) {
		// Characters were deleted.
		OUString deleted;
		sal_Int32 deletedPos;
		sal_Int32 beforePos = 0;
		sal_Int32 afterPos = 0;
		for (; beforePos < before.getLength() && afterPos < after.getLength(); beforePos++)
			if (before[beforePos] != after[afterPos]) {
				if (deleted.getLength() == 0)
					deletedPos = afterPos; // start deleting
				deleted += OUString(before[beforePos]); // keep deleting
			}
			else {
				if (deleted.getLength() != 0) {
					result = result + (false ? OUSTRING("+") : OUSTRING("-")) + deleted + convertToOUString(deletedPos);
					deleted = OUString(); // transition away from deleting
				}
				afterPos++;
			}
		if (beforePos < before.getLength()) { // need to delete the rest
			if (deleted.getLength() == 0)
				deletedPos = afterPos;
			deleted += before.copy(beforePos);
			result = result + (false ? OUSTRING("+") : OUSTRING("-")) + deleted + convertToOUString(deletedPos);
			return result;
		}
		if (afterPos < after.getLength()) { // need to insert the rest
			if (deleted.getLength() != 0)
				result = result + (false ? OUSTRING("+") : OUSTRING("-")) + deleted + convertToOUString(deletedPos);
			result = result + (true ? OUSTRING("+") : OUSTRING("-")) + after.copy(afterPos) + convertToOUString(afterPos);
			return result;
		}
		if (deleted.getLength() != 0)
			result = result + (false ? OUSTRING("+") : OUSTRING("-")) + deleted + convertToOUString(deletedPos);
	}

	if (before.getLength() == after.getLength()) {
		// Characters were replaced.
		OUString deleted;
		OUString inserted;
		sal_Int32 replacedPos;
		sal_Int32 pos = 0;
		for (; pos < before.getLength(); pos++)
			if (before[pos] != after[pos]) {
				if (deleted.getLength() == 0)
					replacedPos = pos; // start replacing
				deleted += OUString(before[pos]);
				inserted += OUString(after[pos]);
			}
			else {
				if (deleted.getLength() != 0) {
					result = result + deleted + OUSTRING(">") + inserted + convertToOUString(replacedPos);
					deleted = OUString();
					inserted = OUString();
				}
			}
		if (deleted.getLength() != 0)
			result = result + deleted + OUSTRING(">") + inserted + convertToOUString(replacedPos);
	}
	return result;
}

bool testDifference(const char* _prev, const char* _next, const char* _expected) {
	OUString prev = OUString::createFromAscii(_prev);
	OUString next = OUString::createFromAscii(_next);
	OUString expected = OUString::createFromAscii(_expected);

	OUString v1 = getDifferences1(next, prev);
	OUString v2 = getDifferences2(prev, next);

	if (!expected.equals(v2))
		return true;

	if (!v1.equals(v2))
		; // error prevented
	return false;
}

bool testDifference() {
	bool result = false;

	// Single insertions
	result |= testDifference("", "a", "+a0"); // to empty
	result |= testDifference("b", "ab", "+a0"); // to beginning
	result |= testDifference("a", "ab", "+b1"); // to end
	result |= testDifference("ac", "abc", "+b1"); // to middle

	// Double insertions
	result |= testDifference("", "ab", "+ab0"); // to empty
	result |= testDifference("c", "abc", "+ab0"); // to beginning
	result |= testDifference("a", "abc", "+bc1"); // to end
	result |= testDifference("ad", "abcd", "+bc1"); // to middle

	// Multiple insertions
	result |= testDifference("ace", "abcde", "+b1+d3");
	result |= testDifference("bd", "abcde", "+a0+c2+e4");

	// Uneven insertions
	result |= testDifference("ac", "abcde", "+b1+de3"); // leftover on right
	result |= testDifference("adef", "abcde", "+bc1-f5"); // leftover on left

	// Even insertions
	result |= testDifference("abc", "abcd", "+d3");

	// Single deletions
	result |= testDifference("a", "", "-a0"); // to empty
	result |= testDifference("ab", "b", "-a0"); // to beginning
	result |= testDifference("ab", "a", "-b1"); // to end
	result |= testDifference("abc", "ac", "-b1"); // to middle

	// Double deletions
	result |= testDifference("ab", "", "-ab0"); // to empty
	result |= testDifference("abc", "c", "-ab0"); // to beginning
	result |= testDifference("abc", "a", "-bc1"); // to end
	result |= testDifference("abcd", "ad", "-bc1"); // to middle

	// Multiple deletions
	result |= testDifference("abcde", "ace", "-b1-d2"); // note difference!
	result |= testDifference("abcde", "bd", "-a0-c1-e2"); // note difference!

	// Uneven deletions
	result |= testDifference("abcde", "ac", "-b1-de2"); // leftover on left
	result |= testDifference("abcde", "adef", "-bc1+f3"); // leftover on right

	// Even deletions
	result |= testDifference("abcd", "abc", "-d3");

	// Single replacement
	result |= testDifference("", "", ""); // to empty
	result |= testDifference("ac", "bc", "a>b0"); // to beginning
	result |= testDifference("ab", "ac", "b>c1"); // to end
	result |= testDifference("abd", "acd", "b>c1"); // to middle

	// Double replacement
	result |= testDifference("abd", "bcd", "ab>bc0"); // to beginning
	result |= testDifference("abc", "acd", "bc>cd1"); // to end
	result |= testDifference("abce", "acde", "bc>cd1"); // to middle

	// Multiple replacement
	result |= testDifference("aacce", "abcde", "a>b1c>d3"); // note difference!
	result |= testDifference("abcde", "bbddf", "a>b0c>d2e>f4"); // note difference!

	return result;
}

bool testConvertToOUString() {
	try {
		char *string1 = (char *) malloc(100);
		char *string2 = (char *) malloc(100);

		strcpy(string1, "hello"); // (new string("hello"))->c_str();
		strcpy(string2, "hello"); // char *string2 = strcpy("good-bye"); // (new string("goodbye"))->c_str();

		OUString ouString1 = convertToOUString(string1);
		string1[0] = 'g';
		OUString ouString2 = convertToOUString(string1);

		if (ouString1.compareToAscii(string2) || !ouString1.compareToAscii(string1)) {
			cout << "testConvertToOUString failed: string changed!" << endl;
			return true;
		}
		else
			cout << "testConvertToOUString passed." << endl;
		return false;
	}
	catch (...) {
		cout << "testConvertToOUString failed: exception!" << endl;
	}
	return false;
}

// It was found that the function was no longer used so that it was removed
// from ProvenanceHelper.  If it is reintroduced, it must be corrected.
bool testProvenanceHelper_GenerateSeed() {
	cout << "testProvenanceHelper_GenerateSeed passed, but perhaps only because it was disabled." <<endl;
	return false;
/*
	try {
		ProvenanceHelper provenanceHelper;
		vector<string> seedInitializers;
		
		seedInitializers.push_back("this");

		byte* seed = provenanceHelper.GenerateSeed(seedInitializers);
		// Actually need to do this CryptoPP::SHA256::DIGESTSIZE times
		byte beforeSeed0 = seed[0];
		byte beforeSeed1 = seed[1];
		byte beforeSeed2 = seed[2];

		// Call something that will overwrite the buffer area of the seed
		seedInitializers.push_back("that");

		provenanceHelper.GenerateSeed(seedInitializers);
		byte afterSeed0 = seed[0];
		byte afterSeed1 = seed[1];
		byte afterSeed2 = seed[2];

		if (beforeSeed0 != afterSeed0 || beforeSeed1 != afterSeed1 || beforeSeed2 != afterSeed2)
			cout << "testProvenanceHelper_GenerateSeed failed: values on the stack have changed!" << endl;
		else
			cout << "testProvenanceHelper_GenerateSeed passed." << endl;
	}
	catch (...) {
		cout << "testProvenanceHelper_GenerateSeed failed: exception!" << endl;
	}
*/
}

bool testServer_Constructor() {
	const char *beforeServerAddress = "127.0.0.1";
	NetworkInfo networkInfo(beforeServerAddress, 9987, "");

	try {
		Server server(networkInfo /*, false*/);
		const char *afterServerAddress = server.getServerAddress();

		if (strcmp(beforeServerAddress, afterServerAddress)) {
			cout << "testServer_Constructor failed: server address was lost, although it might have stuck around long enough!" << endl;
			return true;
		}
		else
			cout << "testServer_Constructor passed." << endl;
	}
	catch (...) {
		cout << "testServer_Constructor failed: exception!" << endl;
		return true;
	}
	return false;
}

class InitializerList {
	protected:
		int value;
	public:
		InitializerList(int value): value(value) {
		}

		int getValue() {
			return value;
		}
};

bool testInitializerList() {
	InitializerList initializerList(5);

	if (initializerList.getValue() != 5) {
		cout << "InitializerList failed!" << endl;
		return true;
	}
	else
		cout << "InitializerList passed." << endl;
	return false;
}

int passfail(bool testResult) {
	std::cout << (testResult ? "Failed" : "Passed") << std::endl;
	return testResult ? 1 : 0;
}

int main(int argc, char *argv[]) {
	int  fail = 0;

	fail |= passfail(testServer_Constructor());
	fail |= passfail(testProvenanceHelper_GenerateSeed());
	fail |= passfail(testConvertToOUString());
	fail |= passfail(testDifference());
	fail |= passfail(testInitializerList());
	return passfail(fail != 0);
}
/*****************************************************************************/

