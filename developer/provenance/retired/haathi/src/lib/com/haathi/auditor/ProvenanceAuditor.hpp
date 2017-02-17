// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/auditor/ProvenanceAuditor.hpp#1 $

/*
 *  ProvenanceAuditor.h
 *  
 *
 *  Created by Aaron Gibson on 3/17/12.
 *  Copyright 2012 Aaron Gibson. All rights reserved.
 *
 */

#ifndef ProvenanceAuditor_HPP
#define ProvenanceAuditor_HPP

// UNO includes
// In com::sun::star::lang
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
// In com::sun::star::uno;
#include <com/sun/star/uno/Reference.hxx>

// In osl
#include <osl/thread.hxx>

// My includes
#include "com/haathi/provenance/ProvenanceEvent.hpp"

#include "com/haathi/utils/StringWrapper.hpp"

// System includes
#include <vector>
#include <string>

// Namespaces
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

namespace com {
namespace haathi {
namespace auditor {

// Forward-declare the "AuditorDialog" class.
class AuditorDialog;

class ProvenanceAuditor: protected osl::Thread {
private:
protected:
	Reference<XComponent> document;

	sal_Int32 event_index;
	bool eventLoaded;

	AuditorDialog* dialog;

	virtual bool loadCurrentEvent() = 0;
	virtual bool unloadCurrentEvent() = 0;

public:
	ProvenanceAuditor(Reference<XComponent> xComponent,
			Reference<XMultiServiceFactory> xMSF);
	virtual ~ProvenanceAuditor();

	virtual void SAL_CALL run();

	void loadEventAt(sal_Int32 new_index);
	bool loadNextEvent();
	bool loadPreviousEvent();

	virtual void updateDialog() const = 0;
	virtual int getEventCount() const = 0;
	virtual std::vector<std::string> getEventList() const = 0;
};

}
;
}
;
}
;
// namespace com::haathi::auditor

#endif

