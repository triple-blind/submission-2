/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/traitorTracer/com/haathi/traitor/QuickTraitorTracer.hpp#7 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TRAITOR__QUICK_TRAITOR_TRACER_HPP
#	define COM__HAATHI__TRAITOR__QUICK_TRAITOR_TRACER_HPP
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
QuickTraitorTheory
******************************************************************************/
class QuickTraitorTheory: public GenericTraitorTheory {
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

		std::string itostr(int value);
		void generateBitStream(char* sequence);

		std::string makeString(int start);
		com::haathi::traitor::Evidence makeEvidence(int index, char* generatedBitStream, int start);

		bool matchBeg(const char* shortBits, int shortSize, const char* longBits, int start);
		bool matchMid(const char* shortBits, int shortSize, const char* longBits, int start, double errorInc);
		bool matchEnd(const char* shortBits, const char* longBits, int longSize, int start);
	public:
		QuickTraitorTheory(com::haathi::traitor::Exhibit* exhibit, com::haathi::traitor::Theory* theory);
		
		void search(int index);
		double getErrorRate();
};
/******************************************************************************
QuickTraitorTracer
******************************************************************************/
class QuickTraitorTracer: public GenericTraitorTracer {
	protected:
		std::vector<QuickTraitorTheory*> traitorTheories;
	public:
		QuickTraitorTracer(com::haathi::traitor::Exhibits& exhibits, com::haathi::traitor::Theories& theories);
		virtual ~QuickTraitorTracer();

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
