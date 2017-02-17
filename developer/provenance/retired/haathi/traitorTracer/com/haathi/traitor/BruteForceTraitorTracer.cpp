/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/retired/haathi/traitorTracer/com/haathi/traitor/BruteForceTraitorTracer.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/traitor/BruteForceTraitorTracer.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/traitor/BitsetComparer.hpp"

#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>
/******************************************************************************
Using
******************************************************************************/
using com::haathi::crypto::CryptoInfo;
using com::haathi::traitor::BitsetComparer;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace traitor {
/******************************************************************************
Class
******************************************************************************/
BruteForceTraitorTracer::BruteForceTraitorTracer(std::map<int, CryptoInfo>& _userDetails,
		std::map<int, std::string>& _documents,
		std::map<int, CryptoInfo>& _documentKeys,
		std::string& bitstream):
		userDetails(_userDetails), documents(_documents), documentKeys(_documentKeys), extractedBitStream(bitstream) {
}

std::map<int,int> BruteForceTraitorTracer::populateSuspectedTraitors() {
	std::vector< std::string > combinations;
//	unsigned long i = 0;

	BitsetComparer comparer;
	std::map<int, std::map<int,int> > userDocumentMatchingsMap;
	
	if (extractedBitStream.length() > 64 )
	{
		// Instead of push_back these all into separate objects, just keep track of the index
		// and start searches there.  Pass pointer to first one, for example
		// This is crap.
		for(size_t index = 0; index <= (extractedBitStream.length()-32);index++) //-63;index++
		{
			int window_size = index <= extractedBitStream.length() - 64 ? 64 : extractedBitStream.length() - index;
			std::string window = extractedBitStream.substr(index,window_size);
			combinations.push_back(window);
		}
	}
	else
	{
		std::cout << "Smaller than window size" << std::endl;
		combinations.push_back(extractedBitStream);
	}
	
	std::map<int,com::haathi::crypto::CryptoInfo>::iterator itr;
	std::map<int,int> usersMatch;
	std::map<int,int>::iterator matchItr;
	std::map<int,std::string>::iterator docItr;	
	for (itr = userDetails.begin(); itr != userDetails.end();++itr) {

		//..if((*itr).first == 2){
		std::cout << "User " << (*itr).first << std::endl;
			for(docItr = documents.begin();docItr != documents.end(); ++docItr){
				//if((*docItr).first == 204) {
				std::cout << "Document id :" << (*docItr).first << std::endl;
				std::vector<std::string> seedInitializers;
					seedInitializers.push_back(((com::haathi::crypto::CryptoInfo)(*itr).second).privateKey);
					seedInitializers.push_back(static_cast<std::ostringstream*>( &(std::ostringstream() << (*docItr).first) )->str());
					seedInitializers.push_back(static_cast<std::ostringstream*>( &(std::ostringstream() << (*itr).first) )->str());
				
					std::vector<int> matches = comparer.CompareBitByBit(seedInitializers, documentKeys[(*docItr).first].getSessionKey().getData(), combinations, 0.10f);
					if(matches.size() > 0)
					{
						int min_errors = *std::min_element(matches.begin(),matches.end());
						std::map<int, std::map<int,int> >::iterator it;
						it = userDocumentMatchingsMap.find((*itr).first);
						if(it == userDocumentMatchingsMap.end())
						{
							std::map<int,int> documentError;
							documentError[(*docItr).first] = min_errors;
							userDocumentMatchingsMap[(*itr).first] = documentError;
						}
						else
						{
							std::map<int,int> documentError = (*it).second;
							documentError[(*docItr).first] = min_errors;
							userDocumentMatchingsMap[(*itr).first] = documentError;
						}
					}
				//}
			}
		//}
			std::cout << "==============" << std::endl;
	}


	std::map<int, std::map<int,int> >::iterator it;
	for(it = userDocumentMatchingsMap.begin(); it != userDocumentMatchingsMap.end();++it)
	{
		std::map<int,int> documentErrors = (*it).second;
		int minError = 0;
		std::map<int,int>::iterator errorItr;

		for(errorItr = documentErrors.begin();errorItr != documentErrors.end();errorItr++)
		{
			if(errorItr == documentErrors.begin() )
			{
				minError = (*errorItr).second;
			}

			if((*errorItr).second < minError)
			{
				minError = (*errorItr).second;
			}
		}
		
		usersMatch[(*it).first] = minError;
	}
	 
	for(matchItr = usersMatch.begin();matchItr!=usersMatch.end();++matchItr){
		std::cout << "User - " << (*matchItr).first << ":" << (*matchItr).second << std::endl;
	}

	return usersMatch;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
