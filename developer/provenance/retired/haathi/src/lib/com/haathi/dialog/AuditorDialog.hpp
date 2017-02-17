/*
 *  Created by Aaron Gibson on 3/17/12.
 */

#ifndef AuditorDialog_HPP
#define AuditorDialog_HPP

#include "com/haathi/dialog/BaseDialog.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"
//#include "com/haathi/auditor/ProvenanceAuditor.hpp"

#include <vector>
#include <string>

namespace com {
namespace haathi {
namespace auditor {

// Forward-declare this class
class EncryptedProvenanceAuditor;

class AuditorDialog: public com::haathi::dialog::BaseDialog {
	// Declare the friend classes here. (Listeners)
	friend class NextEventListener;
	friend class PreviousEventListener;

private:
	Reference<XListBox> event_listbox;
	Reference<XButton> next_button;
	Reference<XButton> prev_button;
	Reference<XButton> close_button;
	Reference<XButton> query_button;

	Reference<XPropertySet> username_field;
	Reference<XPropertySet> event_type_field;
	Reference<XPropertySet> event_description_field;
	Reference<XPropertySet> audit_description_field;
	Reference<XPropertySet> date_field;
	Reference<XPropertySet> time_field;

	Reference<XPropertySet> evolution_status;
	Reference<XPropertySet> hash_status;

	Reference<XGraphic> green_check_image;
	Reference<XGraphic> red_x_image;
	// The controller of the provenance-handling.
	EncryptedProvenanceAuditor* auditor;

public:
	AuditorDialog(Reference<XComponentContext> xContext,
			EncryptedProvenanceAuditor* _auditor);
	virtual ~AuditorDialog();

	void setEventList(const Sequence<OUString>& new_list);
	void displayEvent();
	void displayEvent(const com::haathi::provenance::ProvenanceEvent& event,
			bool hash_verify, bool hash_evolution,
			com::sun::star::util::Color color =
					(com::sun::star::util::Color) 0xffffffff);
	void displayEvent(const com::haathi::provenance::ProvenanceEvent& event,
			com::sun::star::util::Color color =
					(com::sun::star::util::Color) 0xffffffff);
};

}
;
}
;
}
;
// namespace com::haathi::auditor

#endif

