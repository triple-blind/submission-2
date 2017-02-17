//
// Implementation of EncryptedProvenanceAuditor.hpp
//
// Author: Aaron Gibson
// Date: April 1, 2012
//
// This file actually implements the details of "EncryptedProvenanceAuditor.hpp".
//

// UNO includes
// In com::sun::star::beans
#include <com/sun/star/beans/PropertyValue.hpp>
// In com::sun::star::frame
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/frame/XLoadable.hpp>
// In com::sun::star::lang
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
// In com::sun::star::text
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
// In com::sun::star::uno
#include <com/sun/star/uno/XComponentContext.hpp>

// My includes
#include "com/haathi/auditor/AuditorDialog.hpp"
#include "com/haathi/auditor/EncryptedProvenanceAuditor.hpp"
#include "com/haathi/crypto/ProvenanceHelper.hpp"
#include "com/haathi/provenance/ProvenanceEvent.hpp"
#include "com/haathi/utils/ExtensionHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/utils/StreamLoader.hpp"
#include "cppJSON.hpp"

// UNO namespaces
using namespace com::sun::star::beans;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;

// My namespaces
using namespace com::haathi::crypto;
using namespace com::haathi::document;
using namespace com::haathi::provenance;
using namespace com::haathi::utils;

namespace com {
namespace haathi {
namespace auditor {
//	colors[0] = (Color) 0x00336699;
//	colors[1] = (Color) 0x00666666;
//	colors[2] = (Color) 0x00999999;
//	colors[3] = (Color) 0x00f000ff;

static Color getColor() {
	static int i = 0;
	switch (i++) {
	case 0:
		return (com::sun::star::util::Color) 0x00ffff00;
	case 1:
		return (com::sun::star::util::Color) 0x00adff2f;
	case 2:
		return (com::sun::star::util::Color) 0x0087cefa;
	case 3:
		return (com::sun::star::util::Color) 0x00b0c4de;
	case 4:
		return (com::sun::star::util::Color) 0x00ff00ff;
	case 5:
		return (com::sun::star::util::Color) 0x0040e0d0;
	case 6:
		return (com::sun::star::util::Color) 0x0098fb98;
	case 7:
		i = 0;
		return (com::sun::star::util::Color) 0x00f0e68c;
	}
	return 0xffffffff;
}

EncryptedProvenanceAuditor::EncryptedProvenanceAuditor(
		Reference<XComponentContext> xContext,
		Reference<XComponent> _xComponent,
		Reference<XMultiServiceFactory> _xMSF, std::string raw_provenance,
		com::haathi::document::DocumentMetaInfo info) :
		xComponent(_xComponent), xMSF(_xMSF), meta_info(info), user_color_map() {
	// Create the AuditorDialog first. This way, we can display error messages more readily.
	dialog = new AuditorDialog(xContext, this);

	// First, we construct the provenance chain from the parameter "provenance".
	cJSON* decrypted_chain;
	decrypted_chain = cJSON_Parse(raw_provenance.c_str());
	int arraySize = cJSON_GetArraySize(decrypted_chain);
	std::cout << "Provenance chain size : " << arraySize << std::endl;
	sal_Int32 i;
	for (i = 0; i < arraySize; ++i) {
		// Get the entry.
		ProvenanceEntry entry(
				cJSON_Print(cJSON_GetArrayItem(decrypted_chain, i)));

		// Push the entry back into the provenance_chain.
		provenance_chain.push_back(entry);
		// Validate the entry, now that it is in the provenance chain.
		if (!validateIntegrityOfEntry(i)) {
			std::cout << "Entry: " << i << " not valid..." << std::endl;
		}
	}

	// At this point, we should have the whole provenance chain.
	// Now that we have the provenance chain, we can create the events. 
	decryptProvenanceEvents();

	// Set the starting point of the audit.
	event_index = provenance_chain.size() - 1;
	verify_hash = validateIntegrityOfEntry(event_index);
	evolution_hash = validateEvolutionOfEntry(event_index);

	// Set the color highlighting here:
	std::map<std::string, com::sun::star::util::Color>::iterator itr;
	for (i = 0; i < events.size(); ++i) {
		itr = user_color_map.find(events[i]->getUser());

		// If we have a new user, assign them a new color.
		if (itr == user_color_map.end()) {
			user_color_map[events[i]->getUser()] = getColor();
		}
	}
	// All of the events have been created. Load them to the auditor dialog.
	dialog->setEventList(getEventList());
}

EncryptedProvenanceAuditor::~EncryptedProvenanceAuditor() {
}

void EncryptedProvenanceAuditor::run() {
	dialog->run();
}

bool EncryptedProvenanceAuditor::validateIntegrityOfEntry(
		sal_Int32 entryIndex) {
	std::string verified_data;
	ProvenanceHelper integrity_helper(meta_info.provenance_encryption);
	// First verify if the signature is correct
	ProvenanceEntry entry = provenance_chain[entryIndex];

	// Some temporary strings.
	std::string hex_decoded_str = integrity_helper.Base64Decode(
			entry.getChecksum());

	// Check if the signature is valid.
	bool isValidSignature = integrity_helper.VerifyDataSignature(
			hex_decoded_str, verified_data, entry.getPublicKey(),
			entry.getPublicKeySize());

	if (!isValidSignature) {
		return false;
	}
	// recompute the hash corresponding to the checksum and compare it 
	// with the value in the entry
	std::vector<std::string> checksum_components;
	std::string prev_checksum = "";

	if (entryIndex > 0) {
		prev_checksum = provenance_chain[entryIndex - 1].getChecksum();
	}

	checksum_components.push_back(
			entry.getPrincipalIdentifierJSONString());
	checksum_components.push_back(entry.getEncryptedEvent());
	checksum_components.push_back(entry.getDocumentHash());
	checksum_components.push_back(entry.getPublicKey());
	checksum_components.push_back(entry.getKeyInfoJSONString());
	//checksum_components.push_back( prev_checksum );

	std::string computed_hash = integrity_helper.GenerateHash(
			checksum_components);

	computed_hash.append(prev_checksum);

	computed_hash = integrity_helper.GenerateHash(computed_hash);

	if (computed_hash == verified_data) {
		return true;
	}
	return false;
}

int EncryptedProvenanceAuditor::validateIntegrity() {
	int result = 0;

	// Start at the first event, then iterate through all of them.
	for (result = 0; result < provenance_chain.size(); result++) {
		if (!validateIntegrityOfEntry(result)) {
			return result - 1;
		}
	}
	return result;
}

void EncryptedProvenanceAuditor::decryptProvenanceEvents() {
	try {
		// Here, we decrypt the provenance events. Note that we do NOT make any
		// assumption about the validity of the event. If the event cannot be decrypted,
		// then it won't be serialized properly. If it still serializes properly, then
		// we can detect the change when we verify the evolution.

		// Initialize some needed variables.
		ProvenanceHelper auditor_helper(meta_info.provenance_encryption);

		int auditorID = meta_info.user_info.userID;

		CryptoInfo keyInfo = meta_info.provenance_encryption;

		int i;

		// Initialize the vector to have the proper size.
		events = std::vector<ProvenanceEvent*>(provenance_chain.size());
		// Now, we decrypt each event.
		for (i = 0; i < provenance_chain.size(); ++i) {
			// Initialize some structures.
			ProvenanceEntry entry = provenance_chain[i];
//			std::cout << "Provenance Entry: " << provenance_chain[i].getJSONString() << std::endl;

			bool found;
			std::map<int, CryptoInfo>::iterator itr = entry.findKeyingMaterial(auditorID, &found);
			// Get the auditor's private key.
			std::string hex_decoded_str = auditor_helper.Base64Decode(itr->second.sessionKey);
			//	std::string sessionKey = auditor_helper.DecryptWithRSA( hex_decoded_str, keyInfo.privateKey,
			//		keyInfo.privateKeySize);

//			std::cout << "Hex Decoded str: " << hex_decoded_str << std::endl;
//			std::cout << "SessionKey Size: " << key_info[auditorID].sessionKeySize << std::endl;

			std::string sessionKey = auditor_helper.DecryptWithRSA(
					hex_decoded_str, meta_info.provenance_encryption.privateKey,
					meta_info.provenance_encryption.privateKeySize);

			std::string actualEventJSONString = auditor_helper.DecryptData(
					auditor_helper.Base64Decode(entry.getEncryptedEvent()),
					sessionKey, itr->second.sessionKeySize);

			// Now, we have decrypted the event. Deserialize it.
			std::stringstream stream(actualEventJSONString);

			boost::archive::text_iarchive ia(stream);
			ia >> events[i];

		}
	} catch (com::sun::star::uno::Exception & e1) {
		OUString message = OUSTRING("UNO Exception: ");
		message += e1.Message;
		message +=
				OUSTRING(
						"\nAn exception here most likely means that the provenance \
records have been tampered with!");
		dialog->showErrorMessageBox(OUSTRING("Decryption Error"),
				message);
	} catch (std::exception & e2) {
		OUString message = OUSTRING("std::exception: ");
		message += convertToOUString(e2.what());
		message +=
				OUSTRING(
						"\nAn exception here most likely means that the provenance \
records have been tampered with!");
		dialog->showErrorMessageBox(OUSTRING("Decryption Error"),
				message);
	} catch (...) {
		OUString message = OUSTRING("Unknown Exception");
		message +=
				OUSTRING(
						"\nAn exception here most likely means that the provenance \
records have been tampered with!");
		dialog->showErrorMessageBox(OUSTRING("Decryption Error"),
				message);
	}

}

bool EncryptedProvenanceAuditor::validateEvolutionOfEntry(sal_Int32 index) {
	ProvenanceEntry entry = provenance_chain[index];
	ProvenanceHelper auditor_helper;
	int auditorID = meta_info.user_info.userID;

	bool found;
	std::map<int, CryptoInfo>::iterator itr = entry.findKeyingMaterial(auditorID, &found);

	// Now, validate the document hash.
	Reference<XTextDocument> xtextdoc(xComponent, UNO_QUERY);
	Reference<XText> xtext = xtextdoc->getText();
	std::string finalDocument = convertToString(xtext->getString());
	std::string currentHash = auditor_helper.GenerateHash(finalDocument);
	std::string existingHash = "";

//	std::cout << "Event index: " << index << "Document State: " << finalDocument << std::endl;

	if (index > 0) {
		crypto::ProvenanceEntry previousEntry = provenance_chain[index - 1];
		existingHash = previousEntry.getDocumentHash();
	} else {
		existingHash = auditor_helper.GenerateHash(existingHash);
	}
	if (currentHash == existingHash)
		return true;
	return false;
}

bool EncryptedProvenanceAuditor::loadNextEvent() {
	bool result = false;
	try {
		if (event_index >= events.size() - 1) {
			return false;
		}
		result = loadCurrentEvent();
		updateDialog();
	} catch (com::sun::star::uno::Exception & e) {
		dialog->showErrorMessageBox(OUSTRING("UNO Exception"),
				convertToOUString(e.Message));
	} catch (std::exception & e1) {
		dialog->showErrorMessageBox(OUSTRING("Standard Exception"),
				convertToOUString(e1.what()));
	} catch (...) {
		dialog->showErrorMessageBox(OUSTRING("Unknown Error"),
				OUSTRING("I don't know what happened."));
	}
	return result;
}

bool EncryptedProvenanceAuditor::loadPreviousEvent() {
	bool result = false;
	try {
		if (event_index < 0) {
			return false;
		}
		result = unloadCurrentEvent();
		updateDialog();
	} catch (com::sun::star::uno::Exception & e) {
		dialog->showErrorMessageBox(OUSTRING("UNO Exception"),
				convertToOUString(e.Message));
	} catch (std::exception & e1) {
		dialog->showErrorMessageBox(OUSTRING("Standard Exception"),
				convertToOUString(e1.what()));
	} catch (...) {
		dialog->showErrorMessageBox(OUSTRING("Unknown Error"),
				OUSTRING("I don't know what happened."));
	}
	return result;
}

void EncryptedProvenanceAuditor::loadEventAt(sal_Int32 new_index) {
	try {
		if ((new_index < 0) || (new_index > events.size() - 1)) {
			// Out-of-bounds index... I could throw an error...
			return;
		}
		if (new_index == event_index) {
			return;
		}

		// We can't just jump around; we need to load/unload each event sequentially
		// to get the proper document state.
		while (event_index < new_index) {
			loadCurrentEvent();
		}
		while (event_index > new_index) {
			unloadCurrentEvent();
		}

		// Finally, update the AuditorDialog with the information for the currently
		// displayed event.
		updateDialog();
	} catch (com::sun::star::uno::Exception & e) {
		dialog->showErrorMessageBox(OUSTRING("UNO Exception"),
				convertToOUString(e.Message));
	} catch (std::exception & e1) {
		dialog->showErrorMessageBox(OUSTRING("Standard Exception"),
				convertToOUString(e1.what()));
	} catch (...) {
		dialog->showErrorMessageBox(OUSTRING("Unknown Error"),
				OUSTRING("I don't know what happened."));
	}
}

bool EncryptedProvenanceAuditor::loadCurrentEvent()
		throw (com::sun::star::uno::Exception, std::exception) {
	if (event_index >= events.size() - 1) {
		return false;
	}
//	verify_hash = validateIntegrityOfEntry(event_index);
//	evolution_hash = validateEvolutionOfEntry(event_index);

	event_index++;
	std::map<std::string, Color>::iterator itr;
	itr = user_color_map.find(events[event_index]->getUser());

	Color color = 0xffffffff;
	if (itr != user_color_map.end()) {
		color = itr->second;
	}

	Sequence<PropertyValue> props(1);
	props[0].Name = OUSTRING("BackgroundColor");
	props[0].Value <<= color;

	// ProvenanceEvents have a virtual function to load themselves to
	// the document. This allows for the auditor to not worry about what
	// type of provenance this is, since the "handleEvent()" function should
	// properly load the event regardless of type.
	events[event_index]->redo(xComponent, color);

	// Realistically, we shouldn't use these here because the audit must happen
	// from the most recent event back to the first event (we audit backwards
	// from the order the document was created in). This is what is specified
	// in the current scheme.
	//
	// Hypothetically, we could reverse the direction, but when Nitin and I
	// fiddled with this, there was an offset that never worked properly, so
	// we just ignored this, and simply saved the result from auditing backwards.
	return (verify_hash && evolution_hash);
}

bool EncryptedProvenanceAuditor::unloadCurrentEvent()
		throw (com::sun::star::uno::Exception, std::exception) {
	if (event_index < 0) {
		return false;
	}

	std::map<std::string, Color>::iterator itr;
	itr = user_color_map.find(events[event_index]->getUser());

	Color color = 0xffffffff;
	if (itr != user_color_map.end()) {
		color = itr->second;
	}

	Sequence<PropertyValue> props(1);
	props[0].Name = OUSTRING("BackgroundColor");
	props[0].Value <<= color;

	events[event_index]->undo(xComponent, color);

//	std::cout << "unloadEvent: " << *(events[event_index]) << std::endl;

	verify_hash = validateIntegrityOfEntry(event_index);
	evolution_hash = validateEvolutionOfEntry(event_index);

	event_index--;

	// We use these parameters here because we must audit from the most recent events
	// back to the first events. This is why we validate the Integrity and Evolution
	// in this function as opposed to the counterpart "loadCurrentEvent()".
	return (verify_hash && evolution_hash);
}

void EncryptedProvenanceAuditor::updateDialog() {
	std::map<std::string, Color>::iterator itr;
	itr = user_color_map.find(events[event_index]->getUser());

	Color color = 0xffffffff;
	if (itr != user_color_map.end()) {
		color = itr->second;
	}

	dialog->displayEvent(*(events[event_index]), verify_hash, evolution_hash,
			color);
}

Sequence<OUString> EncryptedProvenanceAuditor::getEventList() {
	sal_Int32 i;

	// The unicode for a checkmark is either 2713 or 2714 (both work)
	sal_Unicode checkmark = 0x2714;
	// This unicode "x" is prettier.
	sal_Unicode x_mark = 0x2717;
	Sequence<OUString> result(events.size());
	for (i = events.size() - 1; i >= 0; --i) {
		OUString entry;

		// Start at the "last" entry and work backwards.
		loadEventAt(i);
		if (verify_hash) {
			entry += OUString(checkmark);
		} else {
			entry += OUString(x_mark);
		}
		if (evolution_hash) {
			entry += OUString(checkmark);
		} else {
			entry += OUString(x_mark);
		}
		entry += OUSTRING(" ") + events[i]->printToTable();
		result[i] = entry;
	}

	return result;
}

void EncryptedProvenanceAuditor::auditProvenanceFromStream(
		Reference<XComponentContext> xContext, std::string document,
		std::string provenance, DocumentMetaInfo info) {
	// First, we create a new window to display the provenance document.
	// This involves "bootstrapping" to get an instance of the service factory.
	/*Reference<XComponentContext> xContext = getComponentContext();*/
	Reference<XMultiComponentFactory> xMCF(xContext->getServiceManager());

	// We require that the created document should be ReadOnly. The changes to this 
	// document will be made only by the extension to display the provenance.
	Sequence<PropertyValue> properties(1);
	properties[0].Name = OUSTRING("ReadOnly");
	properties[0].Value <<= sal_True;

	// This loads the document from a string, with the current context and service factory.
	Reference<XComponent> xComponent =
			com::haathi::utils::loadDocumentFromString(xContext, xMCF, document,
					properties);

	Reference<XModel> xModel(xComponent, UNO_QUERY);
	if (xModel.is())
		setTitle(xModel->getCurrentController()->getFrame(), info.document_title, true);

	// We need this type of factory to create the dialog.
	Reference<XMultiServiceFactory> xMSF(xMCF, UNO_QUERY);

	// Create the auditor.
	EncryptedProvenanceAuditor* auditor = new EncryptedProvenanceAuditor(
			xContext, xComponent, xMSF, provenance, info);
	auditor->create();
}

}
;
}
;
}
;

