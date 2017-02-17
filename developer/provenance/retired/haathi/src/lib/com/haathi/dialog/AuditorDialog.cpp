// $Header: //MyDataDepot/Projects/provenance/haathi/src/lib/com/haathi/auditor/AuditorDialog.cpp#3 $

/*
 *  Created by Aaron Gibson on 3/17/12.
 */

// The obvious include
#include "com/haathi/auditor/AuditorDialog.hpp"

// We also have to include this because it wasn't declared in the header.
#include "com/haathi/auditor/EncryptedProvenanceAuditor.hpp"

#include "com/haathi/utils/ExtensionHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

using namespace com::haathi::dialog;
using namespace com::haathi::provenance;

namespace com {
namespace haathi {
namespace auditor {

//=============================================================
// Helper classes
//=============================================================
class NextEventListener: public ActionListenerHelper {
private:
	EncryptedProvenanceAuditor* auditor;
public:
	NextEventListener(EncryptedProvenanceAuditor* _auditor) :
			auditor(_auditor) {
	}
	virtual ~NextEventListener() {
	}

	// XEventListener
	virtual void SAL_CALL disposing(const EventObject& eventOb) throw (RuntimeException) {
	}

	// XActionListener
	virtual void SAL_CALL actionPerformed(const ActionEvent& event)
			throw (RuntimeException) {
		auditor->loadNextEvent();
//			event_listbox->selectItemPos(auditor->getCurrentIndex());
	}
};

class PreviousEventListener: public ActionListenerHelper {
private:
	EncryptedProvenanceAuditor* auditor;
public:
	PreviousEventListener(EncryptedProvenanceAuditor* _auditor) :
			auditor(_auditor) {
	}
	virtual ~PreviousEventListener() {
	}

	// XEventListener
	virtual void SAL_CALL disposing(const EventObject& eventOb) throw (RuntimeException) {
	}

	// XActionListener
	virtual void SAL_CALL actionPerformed(const ActionEvent& event)
			throw (RuntimeException) {
		auditor->loadPreviousEvent();
//			event_listbox->selectItemPos(auditor->getCurrentIndex());
	}
};

class EventListboxItemListener: public ItemListenerHelper {
private:
	EncryptedProvenanceAuditor * auditor;
	Reference<XListBox> xListBox;
public:
	EventListboxItemListener(Reference<XListBox> _xListBox,
			EncryptedProvenanceAuditor * _auditor) :
			auditor(_auditor) {
		xListBox = _xListBox;
	}

	virtual ~EventListboxItemListener() {
	}

	// XEventListener
	virtual void SAL_CALL disposing(const EventObject& event) throw (RuntimeException) {
	}
	// XItemListener
	virtual void SAL_CALL itemStateChanged(const ItemEvent& event)
			throw (RuntimeException) {
		sal_Int32 index = (sal_Int32) xListBox->getSelectedItemPos();
		auditor->loadEventAt(index);
	}
};
//=============================================================
// Constructors and Destructor
//=============================================================
AuditorDialog::AuditorDialog(Reference<XComponentContext> xContext,
		EncryptedProvenanceAuditor* _auditor) :
		BaseDialog(xContext, sal_False ), auditor(_auditor) {
	Any val;
	setWindowProperties(0, 0, 300, 150, OUSTRING("Auditor"));

//		val <<= com::haathi::utils::getGraphicFromFile(
//			OUSTRING("/images/auditor_background.jpg"));
//		windowProps->setPropertyValue(OUSTRING("Graphic"), val);

// Initialize the images
	green_check_image = getGraphicFromFile("/images/green_checkmark.png");
	red_x_image = getGraphicFromFile("/images/red_x.png");
	Sequence<OUString> labelNames(1);
	Sequence<Any> labelValues(1);

	// Initialize all of the items in the dialog.
	// Event Type field
	insertFixedText(0, 20, 50, OUSTRING("EventTypeLabel"),
			OUSTRING("Event Type: "), HORIZONTAL_ALIGN_LEFT);
	event_type_field = insertFixedText(50, 20, 100,
			OUSTRING("EventTypeField"), OUSTRING("NA"),
			HORIZONTAL_ALIGN_LEFT);

	// Username field.
	insertFixedText(0, 32, 50, OUSTRING("UsernameLabel"),
			OUSTRING("Username: "), HORIZONTAL_ALIGN_LEFT);
	username_field = insertFixedText(50, 32, 100,
			OUSTRING("UsernameField"),
			OUSTRING("<No Event Selected>"), HORIZONTAL_ALIGN_LEFT);

	// Date field
	insertFixedText(0, 45, 50, OUSTRING("DateLabel"),
			OUSTRING("Date: "), HORIZONTAL_ALIGN_LEFT);
	date_field = insertFixedText(50, 45, 100, OUSTRING("DateField"),
			OUSTRING("< No event selected >"), HORIZONTAL_ALIGN_LEFT);

	// Time field
	insertFixedText(0, 60, 50, OUSTRING("TimeLabel"),
			OUSTRING("Time: "), HORIZONTAL_ALIGN_LEFT);
	time_field = insertFixedText(50, 60, 100, OUSTRING("TimeField"),
			OUSTRING("< No event selected >"), HORIZONTAL_ALIGN_LEFT);

	// Event Description Field.
	insertFixedText(0, 75, 50, OUSTRING("EventDescriptionLabel"),
			OUSTRING("Event Description:"), HORIZONTAL_ALIGN_LEFT);
	event_description_field = insertFixedText(0, 90, 30, 150,
			OUSTRING("EventDescriptionField"),
			OUSTRING("<No Event Selected>"), HORIZONTAL_ALIGN_LEFT);
	// Audit Description field
	insertFixedText(0, 120, 150, OUSTRING("AuditDescriptionLabel"),
			OUSTRING("Auditor Information:"), HORIZONTAL_ALIGN_LEFT);

	Sequence<OUString> auditProps(1);
	Sequence<Any> auditVals(1);
	auditProps[0] = OUSTRING("MultiLine");
	auditVals[0] <<= sal_True;
	audit_description_field = insertFixedText(0, 135, 50, 150,
			OUSTRING("AuditDescriptionField"),
			OUSTRING("<No Event Selected>"), HORIZONTAL_ALIGN_LEFT,
			auditProps, auditVals);
	// Insert Buttons
	NextEventListener* next_listener = new NextEventListener(auditor);
	Reference<XActionListener> xnext_listener =
			static_cast<XActionListener*>(next_listener);
	insertButton(xnext_listener, 50, 288, 50, OUSTRING("NextButton"),
			OUSTRING("Next Event"));

	PreviousEventListener* prev_listener = new PreviousEventListener(auditor);
	Reference<XActionListener> xprev_listener =
			static_cast<XActionListener*>(prev_listener);
	insertButton(xprev_listener, 0, 288, 50,
			OUSTRING("PreviousButton"),
			OUSTRING("Previous Event"));

	// Insert ListBox
	// Create the event list first.
	// kwa: The events list is not yet ready.  It will be filled in later.
	Sequence<OUString> events_oustring; //  = auditor->getEventList();
	event_listbox = insertListBox(0, 170, 100, 150,
			OUSTRING("EventListbox"), events_oustring);
	EventListboxItemListener* listbox_listener = new EventListboxItemListener(
			event_listbox, auditor);
	Reference<XItemListener> item_listener =
			static_cast<XItemListener*>(listbox_listener);
	event_listbox->addItemListener(item_listener);

	/*		// Insert the hash verification image and label.
	 insertFixedText(0, 220, 50, OUSTRING("HashLabel"), OUSTRING("Hash:"),
	 HORIZONTAL_ALIGN_CENTER);
	 insertFixedText(50, 220, 50, OUSTRING("EvolutionLabel"), OUSTRING("Evolution:"),
	 HORIZONTAL_ALIGN_CENTER);
	 hash_status = insertImage(0, 232, 50, 50, OUSTRING("HashImage"),
	 red_x_image);
	 evolution_status = insertImage(50, 232, 50, 50, OUSTRING("EvolutionImage"),
	 red_x_image);
	 */repaint();
}

AuditorDialog::~AuditorDialog() {
	// Nothing to do?
}

void AuditorDialog::setEventList(const Sequence<OUString>& new_events) {
	event_listbox->removeItems(0, event_listbox->getItemCount());
	event_listbox->addItems(new_events, 0);
}

//=============================================================
// Member Functions
//=============================================================
void AuditorDialog::displayEvent(const ProvenanceEvent & event,
		com::sun::star::util::Color color) {
	Any val;
	OUString text_str = OUSTRING("Label");

	// Set the EventType.
	val <<= convertToOUString(event.getEventType());
	event_type_field->setPropertyValue(text_str, val);

	// Set the username
	// Remember that we must also set the background color, based on the userlist.
	// Also, set the alpha to opaque so that the color actually appears.
	color &= 0x00ffffff;
	val <<= color;
	username_field->setPropertyValue(OUSTRING("BackgroundColor"), val);
	val <<= convertToOUString(event.getUser());
	username_field->setPropertyValue(text_str, val);

	// Set the date and time.
	val <<= convertToOUString(event.getTime().printDate());
	date_field->setPropertyValue(text_str, val);

	val <<= convertToOUString(event.getTime().printTime());
	time_field->setPropertyValue(text_str, val);

	// Set the event description.
	val <<= convertToOUString(event.getEventDescription());
	event_description_field->setPropertyValue(text_str, val);

	// Repaint the entire dialog to make sure all of the fields have properly updated.
	repaint();
}

void AuditorDialog::displayEvent(const ProvenanceEvent& event, bool hash_verify,
		bool hash_evolution, com::sun::star::util::Color color) {
	displayEvent(event, color);

	Any val;
	OUString status;
	if (hash_verify && hash_evolution) {
		status = OUSTRING("Good -- Event matches with current state.");
	} else if (!hash_verify && hash_evolution) {
		status =
				OUSTRING(
						"Inegrity Error -- The integrity of the event has been compromised. \
Someone may have changed the credentials (username, for instance) of the event, but not the event itself.");
	} else {
		status =
				OUSTRING(
						"Evolution Error -- The event does not match what is present in the \
document. The document evolution and consequently the integrity have all been compromised.");
	}

	val <<= status;
	audit_description_field->setPropertyValue(OUSTRING("Label"), val);
}
}
;
}
;
}
;
// namespace com::haathi;::auditor

