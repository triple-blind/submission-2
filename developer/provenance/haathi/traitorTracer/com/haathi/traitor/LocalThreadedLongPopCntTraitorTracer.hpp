/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/traitorTracer/com/haathi/traitor/LocalThreadedLongPopCntTraitorTracer.hpp#2 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TRAITOR__LOCAL_THREADED_LONG_POP_CNT_TRAITOR_TRACER_HPP
#	define COM__HAATHI__TRAITOR__LOCAL_THREADED_LONG_POP_CNT_TRAITOR_TRACER_HPP
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
ThreadedLongPopCntTraitorTheory
******************************************************************************/
class LocalThreadedLongPopCntTraitorTheory: public GenericTraitorTheory {
	public:
		// From Exhibit
		int watermarkId;
		std::string& watermarkAbbrev;
		const unsigned long long* extractedBitStream;
		int extractedBitStreamLength;

		// From Theory
		std::string& userId;
		std::string& documentId;
		std::string& versionNo;
		std::string& versionId;

		// Collected evidence
		com::haathi::traitor::Evidences evidences;
	protected:
		inline void getBlock(unsigned long long* block);
		std::string makeString(int start);
		void addMatch(int index, unsigned long long block, int position);
	public:
		LocalThreadedLongPopCntTraitorTheory(com::haathi::traitor::Exhibit* exhibit, com::haathi::traitor::Theory* theory,
				const unsigned long long* extractedBitStream);

		void search(int index);
};
/******************************************************************************
ThreadedLongPopCntTraitorTracer
******************************************************************************/
class LocalThreadedLongPopCntTraitorTracer: public GenericTraitorTracer {
	protected:
		std::vector<const unsigned long long*> extractedBitStreams;
		std::vector<LocalThreadedLongPopCntTraitorTheory*> traitorTheories;
	public:
		LocalThreadedLongPopCntTraitorTracer(com::haathi::traitor::Exhibits& exhibits, com::haathi::traitor::Theories& theories);
		virtual ~LocalThreadedLongPopCntTraitorTracer();

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
