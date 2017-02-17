/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/cryptoppTest/cryptoppTest.cpp#1 $

// See http://www.cryptopp.com/wiki/HexEncoder
/******************************************************************************
Include others
******************************************************************************/
#include <iostream> // cout

#ifdef __GNUC__
#	ifndef __llvm__
// My version is 4.2 and doesn't understand this
#		pragma GCC diagnostic push
#	endif
#	pragma GCC diagnostic ignored "-Wunused-variable"
// Unfortunately, this is not sufficient here
#	pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include <cryptopp/hex.h> // HexEncoder
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
using namespace CryptoPP;
/******************************************************************************
Content
******************************************************************************/
bool cryptoppTest(int argc,char *argv[])
{
	byte decoded[] = { 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00 };
	std::string expected = "FFEEDDCCBBAA99887766554433221100";

	HexEncoder encoder;
	encoder.Put(decoded, sizeof(decoded));
	encoder.MessageEnd();

	std::string actual;
	word64 size = encoder.MaxRetrievable();
	if (size)
	{
		actual.resize((int) size);		
		encoder.Get((byte*) actual.data(), actual.size());
	}

	std::cout << "Does '" << actual << "'=='" << expected << "'?" << std::endl;
	return expected != actual;
}
/*****************************************************************************/
