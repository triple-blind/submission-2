/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/traitorTracer/com/haathi/traitor/GenericTraitorTracer.hpp#8 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TRAITOR__GENERIC_TRAITOR_TRACER_HPP
#	define COM__HAATHI__TRAITOR__GENERIC_TRAITOR_TRACER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/traitor/Theory.hpp"

#include <cryptopp/des.h> // DES_EDE3
#include <cryptopp/osrng.h> // AutoSeededX917RNG
#include <cryptopp/rng.h> // LC_RNG

#include <iostream>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace traitor {
/******************************************************************************
QuickTraitorTheory
******************************************************************************/
class GenericTraitorTheory {
	protected:
		CryptoPP::AutoSeededX917RNG<CryptoPP::AES> kernelGenerator;
		CryptoPP::AutoSeededX917RNG<CryptoPP::DES_EDE3> chaffGenerator;
		CryptoPP::LC_RNG decisionGenerator;

		std::string itostr(int value);
		void updateHasher(CryptoPP::SHA256& hasher, std::string& seed);
		void fillSeed(byte seed[], std::string& versionId, int blockSize);
	public:
		GenericTraitorTheory(com::haathi::traitor::Theory* theory);		
		virtual ~GenericTraitorTheory() { }

		void generateBlock(unsigned char* block);
};
/******************************************************************************
GenericTraitorTracer
******************************************************************************/
class GenericTraitorTracer {
	public:
		virtual void trace(std::ostream& out, int count) = 0;
		virtual ~GenericTraitorTracer() { };
};
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
