/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/traitorTracer/com/haathi/traitor/LongPopCntTraitorTracer.hpp#11 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TRAITOR__LONG_POP_CNT_TRAITOR_TRACER_HPP
#	define COM__HAATHI__TRAITOR__LONG_POP_CNT_TRAITOR_TRACER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/traitor/GenericTraitorTracer.hpp"

#include "com/haathi/traitor/Evidence.hpp"
#include "com/haathi/traitor/Exhibit.hpp"

#include <iostream>
#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace traitor {
/******************************************************************************
LongPopCntTraitorTheory
******************************************************************************/
class LongPopCntTraitorTheory: public GenericTraitorTheory {
	public:
		// From Exhibit
		int watermarkId;
		std::string& watermarkAbbrev;
		const char* extractedBitStream;
		int extractedBitStreamLength;

		// From Theory
		std::string& userId;
		std::string& documentId;
		std::string& versionNo;
		std::string& versionId;

		// Collected evidence
		com::haathi::traitor::Evidences evidences;
	protected:
		double errorRate;

		inline void getBlock(unsigned long long* block);
		std::string makeString(int start);
		void addMatch(int index, unsigned long long block, int position);
	public:
		LongPopCntTraitorTheory(com::haathi::traitor::Exhibit* exhibit, com::haathi::traitor::Theory* theory,
				char* extractedBitStream);

		void search(int index);
		double getErrorRate();
};
/******************************************************************************
LongPopCntTraitorTracer
******************************************************************************/
class LongPopCntTraitorTracer: public GenericTraitorTracer {
	protected:
		std::vector<char*> extractedBitStreams;
		std::vector<LongPopCntTraitorTheory*> traitorTheories;
	public:
		LongPopCntTraitorTracer(com::haathi::traitor::Exhibits& exhibits, com::haathi::traitor::Theories& theories);
		virtual ~LongPopCntTraitorTracer();

		virtual void trace(std::ostream& out, int count);
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
