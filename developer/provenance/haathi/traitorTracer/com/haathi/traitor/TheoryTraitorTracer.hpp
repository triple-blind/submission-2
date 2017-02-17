/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/traitorTracer/com/haathi/traitor/TheoryTraitorTracer.hpp#2 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TRAITOR__THEORY_TRAITOR_TRACER_HPP
#	define COM__HAATHI__TRAITOR__THEORY_TRAITOR_TRACER_HPP
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
TheoryTraitorTheory
******************************************************************************/
class TheoryTraitorTheory {
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
		inline void getBlock(unsigned long long* block);
		std::string makeString(int start);
		void addMatch(int index, unsigned long long block, int position);
	public:
		TheoryTraitorTheory(com::haathi::traitor::Exhibit* exhibit, com::haathi::traitor::Theory* theory,
				char* extractedBitStream);

		void search(int index, unsigned long long block);
};
/******************************************************************************
TheoryTraitorTracer
******************************************************************************/
class TheoryTraitorTracer: public GenericTraitorTracer {
	protected:
		std::vector<char*> extractedBitStreams;
		std::vector<TheoryTraitorTheory*> traitorTheories;
		std::vector<GenericTraitorTheory*> theories;
		int exhibitsCount;
	public:
		TheoryTraitorTracer(com::haathi::traitor::Exhibits& exhibits, com::haathi::traitor::Theories& theories);
		virtual ~TheoryTraitorTracer();

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
