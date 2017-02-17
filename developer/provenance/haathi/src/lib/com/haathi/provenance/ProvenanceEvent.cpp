/******************************************************************************
Header
******************************************************************************/
// $Header$

/*
 *  Created by Aaron Gibson on 3/10/12.
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/ProvenanceEvent.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Guard.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::convertToOUString;
using com::haathi::utils::guard;
/******************************************************************************
Namespace
******************************************************************************/
int eventCount = 0;

namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
ProvenanceEvent
******************************************************************************/
ProvenanceEvent::ProvenanceEvent(): time(ProvenanceTime::getCurrentTime()) {
	eventCount++;
	guard(&ProvenanceEvent::toString, 160, 0, __FILE__, __LINE__);
}

void ProvenanceEvent::setUsername(std::string username) {
	this->username = username;
}

OUString ProvenanceEvent::toString() const {
	std::string result;

	result += "EventType: " + getEventType() + "\n";
	result += "Time: " + time.toString() + "\n";
	return convertToOUString(result);
}

ProvenanceEvent::ProvenanceEvent(const ProvenanceEvent& other):
		time(other.time), username(other.username) {
	eventCount++;
}

ProvenanceEvent::~ProvenanceEvent() {
	eventCount--;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};

// This macro allows the Boost serialization to actually serialize this abstract class.
// Note that any class that derives from com::haathi::provenance::ProvenanceEvent MUST include
// this macro "BOOST_CLASS_EXPORT" at the end of the source file to properly register with the
// serializer.
BOOST_CLASS_EXPORT(com::haathi::provenance::ProvenanceEvent);
//BOOST_CLASS_EXPORT(com::haathi::provenance::ProvenanceDocumentEvent);
//BOOST_CLASS_EXPORT(com::haathi::provenance::ProvenanceTextEvent);
/*****************************************************************************/
