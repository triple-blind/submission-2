/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/ProvenanceEventHelper.hpp#1 $

/*
 *  Created by Aaron Gibson on 3/10/12.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__PROVENANCE_EVENT_HELPER_HPP
#	define COM__HAATHI__PROVENANCE__PROVENANCE_EVENT_HELPER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "Config.hpp"

#include "sdkBeg.hpp"
#include "sdkEnd.hpp"

#include "boostBeg.hpp"
#include <boost/shared_ptr.hpp>
#include "boostEnd.hpp"

#include <string>
#include <memory>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
Class
******************************************************************************/
class ProvenanceEvent;

#if SHARED_EVENT_PTR == SHARED_EVENT_PTR_OFF
typedef ProvenanceEvent* ProvenanceEventPtr;
#else
typedef boost::shared_ptr<ProvenanceEvent> ProvenanceEventPtr;
#endif

class ProvenanceEventHelper {
	protected:
		ProvenanceEventPtr pProvenanceEvent;
		std::string documentState;

	public:
		inline ProvenanceEventHelper(ProvenanceEvent* provenanceEvent, std::string documentState):
				pProvenanceEvent(provenanceEvent), documentState(documentState) {
		}

		inline ~ProvenanceEventHelper() {
			// Do not delete provenanceEvent here!
		}

		inline std::string getDocumentState() {
			return documentState;
		}

		inline ProvenanceEventPtr getProvenanceEvent() {
			return pProvenanceEvent;
		}
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
