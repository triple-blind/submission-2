// System includes
#include "com/haathi/crypto/ProvenanceHelper.hpp"
#include "com/haathi/provenance/ProvenanceTime.hpp"
#include "com/haathi/traitor/BitsetComparer.hpp"

#include <map>
#include <climits>
#include <sstream>
#include <algorithm>
#include <fstream>
//#include "com/haathi/utils/serialization.hpp"

using namespace std;
using namespace com::haathi::crypto;
using namespace com::haathi::provenance;

namespace com {
namespace haathi {
namespace traitor {

BitsetComparer::BitsetComparer()
{
	bitPosition = 7;
	extractedNumber = 0;
}


unsigned char BitsetComparer::Shiftl(void *object, size_t size)
{
   unsigned char *data;
   unsigned char lbit = 0;
   lbit = (static_cast<unsigned char*>(object)[0]) & ( 1 << (CHAR_BIT -1))?1:0;
   for ( data = static_cast<unsigned char*>(object); size--; ++data )
   {
      unsigned char bit = 0;
      if ( size )
      {
         bit = data[1] & (1 << (CHAR_BIT - 1)) ? 1 : 0;
      }
      
      *data <<= 1;
      *data  |= bit;
   }
   return lbit;
}

std::vector<unsigned char> BitsetComparer::ConvertBlock()
{
	std::vector<unsigned char> sequence;
	int count = 0;
//	ofstream myfile("/home/nitin/Documents/blocks.txt");
	while(count < WINDOW_SIZE+8)
	{
		if(!(bitPosition >=0)) {
			sequence.push_back(extractedNumber);
			bitPosition = 7;
			extractedNumber = 0;
		}
		else{
			unsigned bit = Shiftl(block,WINDOW_SIZE/8);
			extractedNumber = extractedNumber | (bit << bitPosition);
			bitPosition--;
		}
		count++;
//		myfile << bitPosition << ":"<< extractedNumber << std::endl;
	}
	
	bitPosition = 7;
	extractedNumber = 0;
//	myfile.close();
	return sequence;
}

string BitsetComparer::HashGeneratedWindow()
{
	CryptoPP::SHA256 hash;
	byte digest[CryptoPP::SHA256::DIGESTSIZE];
	hash.CalculateDigest(digest, (byte*) block,
			WINDOW_SIZE);

	CryptoPP::HexEncoder encoder;
	std::string item_hash;
	encoder.Attach(new CryptoPP::StringSink(item_hash));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return item_hash;
}

int BitsetComparer::Compare(std::vector<std::string> seed,std::string documentKey,std::vector< std::bitset<64> > extractedBitset)
{
	std::map<string,int> generatedBlocks;
	std::map<string,int>::iterator itr;
	int count = 0;
	CryptoPP::AutoSeededX917RNG<CryptoPP::AES>* _prng;
		CryptoPP::SHA256 hasher;
		byte digest[CryptoPP::SHA256::DIGESTSIZE];
	
		for (size_t index = 0; index < seed.size(); index++) {
			hasher.Update((byte*) seed[index].c_str(),
				seed[index].length());
		}

		hasher.Final(digest);

		
	_prng = new CryptoPP::AutoSeededX917RNG<CryptoPP::AES>(false,false);

	_prng->Reseed((byte*) ProvenanceHelper::base64Decode(documentKey).c_str(),16,digest,digest);

	_prng->GenerateBlock(block,WINDOW_SIZE);
	string hash = HashGeneratedWindow();
	itr = generatedBlocks.find(hash);
//	ofstream myfile("/home/nitin/Documents/combinations.txt");
	while(itr == generatedBlocks.end() && count < 10)
	{
		generatedBlocks[hash] = 1;
		stringstream bitString;
		//vector<unsigned char> bitStream = ConvertBlock();
		/*
		for(size_t i = 0; i < bitStream.size()*8;i++)
   		{
       			unsigned char bit = Shiftl(&bitStream[0], bitStream.size());
			bit == 1?bitString<<"1":bitString<<"0";
       		}*/

		for(size_t i = 0; i < WINDOW_SIZE;i++)
		{
			unsigned char bit = Shiftl(&block,WINDOW_SIZE/8);
			bit == 1 ? bitString << "1" : bitString << "0";
		}
	
		string extractedBits = bitString.str();
		std::cerr << "Block : " << extractedBits << std::endl;
		bitset<64> bitSet(extractedBits);
		count++;
		vector< bitset<64> >::iterator result =  std::find(extractedBitset.begin(),extractedBitset.end(),bitSet);
		if(result != extractedBitset.end())
		{
			cout << "Found a match" << endl;		
			count++;

			if(count == (int) extractedBitset.size())
				break;
		}
		_prng->GenerateBlock(block,WINDOW_SIZE);
		hash = HashGeneratedWindow();
		itr = generatedBlocks.find(hash);
			
	}
//	myfile.close();
	return count;
}

int BitsetComparer::GetMatch(std::string generatedSequence, std::string originalSequence,float confidencePercentage)
{
	std::vector<int> matches;
	int maxAllowedErrors = static_cast<int>((confidencePercentage * originalSequence.length()));	

	std::map<std::string, int>::iterator iter;
	unsigned int index = 0;
	int bitErrorCount = 0;

	while(index < originalSequence.length() )
	{
		string original = originalSequence.substr(index,1);
		string candidate = generatedSequence.substr(index,1);
		if(atoi(original.c_str()) != atoi(candidate.c_str()))
		{
			bitErrorCount++;
		}
		if(bitErrorCount > maxAllowedErrors)
		{
			break;
		}
		index++;
	}

	if(index != originalSequence.length())
	{
		bitErrorCount = -1;
	}
	
	return bitErrorCount;	
}

std::vector<int>  BitsetComparer::CompareBitByBit(std::vector<std::string> seed,std::string documentKey, std::vector<std::string > extractedBitset, float confidencePrecentage)
{
	std::map<string,int> generatedBlocks;
	std::map<string,int>::iterator itr;
//	int count = 0;
	std::vector<int> matches;
	CryptoPP::AutoSeededX917RNG<CryptoPP::AES>* _prng;
	CryptoPP::SHA256 hasher;
	byte digest[CryptoPP::SHA256::DIGESTSIZE];
	
	for (size_t index = 0; index < seed.size(); index++) {
		hasher.Update((byte*) seed[index].c_str(),
		seed[index].length());
	}

	hasher.Final(digest);

	std::cerr << "Document key : " << documentKey << std::endl;
	_prng = new CryptoPP::AutoSeededX917RNG<CryptoPP::AES>(false,false);

	// Generate all possible combinations
	int iterations = 0;

	while(iterations != (int) extractedBitset.size())
	{
		std::cerr << "Iteration " << iterations << " of " << extractedBitset.size() << " at "
				<< ProvenanceTime::getCurrentTime().printDateAndMillisecondsTime() << "." << std::endl;


		string candidateToMatch = extractedBitset[iterations];

		_prng->Reseed((byte*) ProvenanceHelper::base64Decode(documentKey).c_str(),16,digest,digest);

		_prng->GenerateBlock(block,WINDOW_SIZE/8);

		std::stringstream sequence;
		for(int i = 0; i < WINDOW_SIZE;i++)
		{
			unsigned char bit = Shiftl(&block[0], WINDOW_SIZE/8);
			bit == 1?sequence<<"1":sequence<<"0";
		}
		string generatedSequence = sequence.str();
		itr = generatedBlocks.find(generatedSequence);
		long int generatedSequenceCount = 0;

		while(generatedSequenceCount < 50000)
		{
			int match = GetMatch(generatedSequence,candidateToMatch,confidencePrecentage);
			if(match >= 0)
			{
				std::cerr << "Found a match - " << generatedSequence << " and " << candidateToMatch << std::endl;
				std::cerr << "Bit errors = " << match << std::endl;
				std::cerr << "iteration (where in the document) = " << iterations << endl;
				std::cerr << "generatedSequenceCount (where in the prng) = " << generatedSequenceCount << endl;
				matches.push_back(match);
				break;
			}
			_prng->GenerateBlock(block,WINDOW_SIZE/8);
			std::stringstream seq;
			for(int i = 0; i < WINDOW_SIZE;i++)
			{
				unsigned char bit = Shiftl(&block[0], WINDOW_SIZE/8);
				bit == 1?seq<<"1":seq<<"0";
			}
			generatedSequence  = seq.str();
			generatedSequenceCount++;
		}
		iterations++;

	}

	delete _prng;
	return matches;
}

}
;
}
;
}
;

