/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/retired/haathi/traitorTracer/com/haathi/traitor/BruteForceTraitorTracer.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TRAITOR__BRUTE_FORCE_TRAITOR_TRACER_HPP
#	define COM__HAATHI__TRAITOR__BRUTE_FORCE_TRAITOR_TRACER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/crypto/CryptoInfo.hpp"
#include "com/haathi/traitor/GenericTraitorTracer.hpp"

#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace traitor {
/******************************************************************************
Class
******************************************************************************/
class BruteForceTraitorTracer: public GenericTraitorTracer {
	protected:
		std::map<int, com::haathi::crypto::CryptoInfo> &userDetails;
		std::map<int, std::string> documents;
		std::map<int, com::haathi::crypto::CryptoInfo> documentKeys;
		std::string extractedBitStream;
	public:
		BruteForceTraitorTracer(std::map<int, com::haathi::crypto::CryptoInfo>& userDetails,
				std::map<int, std::string>& documents,
				std::map<int, com::haathi::crypto::CryptoInfo>& documentKeys,
				std::string& bitstream);
		virtual std::map<int, int> populateSuspectedTraitors();
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
