/*
 *  ProvenanceAuditor.cpp
 *  
 *
 *  Created by Aaron Gibson on 3/17/12.
 *  Copyright 2012 Aaron Gibson. All rights reserved.
 *
 */

// The obvious include.
#include "com/haathi/auditor/ProvenanceAuditor.hpp"
#include "com/haathi/auditor/AuditorDialog.hpp"
// Used to create the initial XComponentContext.
#include <cppuhelper/bootstrap.hxx>

// In com::sun::star::beans
#include <com/sun/star/beans/PropertyValue.hpp>
// In com::sun::star::frame
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XStorable.hpp>
// In com::sun::star::lang
#include <com/sun/star/lang/XMultiComponentFactory.hpp>

// Utilities
#include "com/haathi/utils/extension_helper.hpp"
#include "com/haathi/utils/string_wrapper.hpp"
#include "com/haathi/utils/serialization.hpp"

// namespaces
using namespace com::sun::star::beans;
using namespace com::sun::star::frame;
using namespace com::sun::star::uno;

// my namespaces
using namespace com::haathi::provenance;

namespace com {
namespace haathi {
namespace auditor {

//==========================================================================
// Constructors and Destructors
//==========================================================================
ProvenanceAuditor::ProvenanceAuditor(Reference<XComponent> _xComponent,
		Reference<XMultiServiceFactory> xMSF) :
		document(_xComponent), event_index(0), eventLoaded(false) {
	dialog = new AuditorDialog(xMSF, this);

	// Everything has been constructed.
}

ProvenanceAuditor::~ProvenanceAuditor() {
	// Make sure this process closes?
	dialog->close();
	delete dialog;
}
//==========================================================================
// Virtual Functions from Superclasses
//==========================================================================
void ProvenanceAuditor::run() {
	dialog->run();
}

//==========================================================================
// Member Functions
//==========================================================================

bool ProvenanceAuditor::loadNextEvent() {
	bool result = false;
	if (event_index >= getEventCount() - 1) {
		return false;
	}
	if (!eventLoaded) {
		result = loadCurrentEvent();
		eventLoaded = true;
		updateDialog();
		return result;
	}
	event_index++;
	// Now, load the event at the current index.
	result = loadCurrentEvent();

	updateDialog();
	return result;
}

bool ProvenanceAuditor::loadPreviousEvent() {
	bool result = false;
	if (!eventLoaded) {
		return false;
	}
	if (event_index == 0) {
		eventLoaded = false;
		result = unloadCurrentEvent();
		updateDialog();
		return result;
	}
	// Undo the 'current' event.
	result = unloadCurrentEvent();
	if (result) {
		// This sets us back to the previous index, so we lower the event_index.
		event_index--;
	}

	updateDialog();
	return result;
}

void ProvenanceAuditor::loadEventAt(sal_Int32 new_index) {
	if ((event_index < 0) || (event_index > getEventCount())) {
		// Out-of-bounds index... I could throw an error...
		return;
	}
	if (new_index == event_index) {
		return;
	}

	while (event_index < new_index) {
		if (!loadNextEvent()) {
			return;
		}
		// Pause for a short time? We can implement this later.
	}
	while (event_index > new_index) {
		if (!loadPreviousEvent()) {
			return;
		}
	}

	// Finally, update the dialog.
	updateDialog();
}

/*
 void ProvenanceAuditor::updateDialog() const
 {
 const ProvenanceEvent* event_ptr = getCurrentEvent();
 dialog->displayEvent(*event_ptr);	
 }

 std::vector< std::string > ProvenanceAuditor::getEventList() const
 {
 std::vector< std::string > result;
 std::vector< ProvenanceEvent* >::const_iterator itr;
 for(itr = events.begin(); itr != events.end(); ++itr){
 // Pushback the short description of each event.
 result.push_back((*itr)->toString());
 }
 return result;
 }
 */
}
;
}
;
}
;
// namespace com::haathi::auditor

