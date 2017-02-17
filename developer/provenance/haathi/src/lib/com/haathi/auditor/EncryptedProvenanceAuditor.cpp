/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/auditor/EncryptedProvenanceAuditor.cpp#1 $

//
// Implementation of EncryptedProvenanceAuditor.hpp
//
// Author: Aaron Gibson
// Date: April 1, 2012
//
// This file actually implements the details of "EncryptedProvenanceAuditor.hpp".
//
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/auditor/EncryptedProvenanceAuditor.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/crypto/ProvenanceHelper.hpp"
#include "com/haathi/document/DocumentStore.hpp"
#include "com/haathi/document/TextDocumentListener.hpp"
#include "com/haathi/extension/ExtensionHelper.hpp"
#include "com/haathi/text/TextHelper.hpp"
#include "com/haathi/utils/HaathiException.hpp"
#include "com/haathi/utils/PropertyValuesHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include "sdkEnd.hpp"

#include "cppJSON.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::crypto::CryptoInfo;
using com::haathi::crypto::ProvenanceEntry;
using com::haathi::crypto::ProvenanceHelper;
using com::haathi::dialog::MessageBoxProvider;
using com::haathi::document::DocumentMetaInfo;
using com::haathi::document::DocumentStore;
using com::haathi::extension::ExtensionHelper;
using com::haathi::provenance::ProvenanceEvent;
using com::haathi::text::TextHelper;
using com::haathi::utils::convertToString;
using com::haathi::utils::HAATHI_IARCHIVE;
using com::haathi::utils::HaathiException;
using com::haathi::utils::PropertyValuesHelper;

using com::sun::star::lang::XComponent;
using com::sun::star::uno::Any;
using com::sun::star::uno::Exception;
using com::sun::star::uno::XComponentContext;
using com::sun::star::util::Color;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace auditor {
/******************************************************************************
EventSummary
******************************************************************************/
const OUString EventSummary::trueMark(0x2713);
const OUString EventSummary::falseMark(0x2717);
const OUString EventSummary::questionMark = OUSTRING("?");
const OUString EventSummary::blank(0x20);
const OUString EventSummary::empty;

EventSummary::EventSummary() {
	initialize(false, false, false, false, false, 0);
}

EventSummary::EventSummary(bool auditor) {
	initialize(auditor, false, false, false, false, 0);
}

EventSummary::EventSummary(bool auditor, bool decoded) {
	initialize(auditor, decoded, false, false, false, 0);
}

EventSummary::EventSummary(bool auditor, bool decoded, bool rsaDecrypted) {
	initialize(auditor, decoded, rsaDecrypted, false, false, 0);
}

EventSummary::EventSummary(bool auditor, bool decoded, bool rsaDecrypted,
		bool plainDecrypted) {
	initialize(auditor, decoded, rsaDecrypted, plainDecrypted, false, 0);
}

EventSummary::EventSummary(bool auditor, bool decoded, bool rsaDecrypted,
	   bool plainDecrypted, bool deserialized) {
	initialize(auditor, decoded, rsaDecrypted, plainDecrypted, deserialized, 0);
}

EventSummary::EventSummary(bool auditor, bool decoded, bool rsaDecrypted,
		bool plainDecrypted, bool deserialized, ProvenanceEvent* provenanceEvent) {
	initialize(auditor, decoded, rsaDecrypted, plainDecrypted, deserialized, provenanceEvent);
}

void EventSummary::initialize(bool auditor, bool decoded, bool rsaDecrypted,
	   bool plainDecrypted, bool deserialized, ProvenanceEvent* provenanceEvent) {
	this->auditor = auditor;
	this->decoded = decoded;
	this->rsaDecrypted = rsaDecrypted;
	this->plainDecrypted = plainDecrypted;
	this->deserialized = deserialized;
	this->provenanceEvent = provenanceEvent;

	integrity = false;
	forwardEvent = false;
	forwardDocument = false;
	backwardEvent = false;
	backwardDocument = false;

	color = 0;
}

void EventSummary::setColor(Color color) {
	this->color = color;
}

void EventSummary::setIntegrity(bool integrity) {
	this->integrity = integrity;
}

void EventSummary::setForwardEvent(bool event) {
	forwardEvent = event;
}

void EventSummary::setForwardDocument(bool document) {
	forwardDocument = document;
}

void EventSummary::setBackwardEvent(bool event) {
	backwardEvent = event;
}

void EventSummary::setBackwardDocument(bool document) {
	backwardDocument = document;
}

OUString EventSummary::toOUString() {
	rtl::OUStringBuffer result;

	result.append(auditor ? trueMark : falseMark);
	result.append(integrity ? trueMark : falseMark);

	// TODO: three-state? Assuming could be decrypted, etc. might need ?
	result.append(forwardEvent ? trueMark : falseMark);
	result.append(forwardDocument ? trueMark : falseMark);

	result.append(backwardEvent ? trueMark : falseMark);
	result.append(backwardDocument ? trueMark : falseMark);

	result.append(provenanceEvent ? blank + provenanceEvent->printToTable() : empty);
	return result.makeStringAndClear();
}

OUString EventSummary::getIntegrityStatus() {
	rtl::OUStringBuffer result;

	result.append(integrity ? trueMark : falseMark);
	result.append(OUSTRING(" integrity"));
	return result.makeStringAndClear();
}

OUString EventSummary::getAuditorStatus() {
	rtl::OUStringBuffer result;

	result.append(auditor ? trueMark : falseMark);
	result.append(OUSTRING(" auditor"));
	return result.makeStringAndClear();
}

OUString EventSummary::getBackwardEventStatus() {
	rtl::OUStringBuffer result;

	result.append(backwardEvent ? trueMark : falseMark);
	result.append(OUSTRING(" event"));
	return result.makeStringAndClear();
}

OUString EventSummary::getBackwardDocumentStatus() {
	rtl::OUStringBuffer result;

	result.append(backwardDocument ? trueMark : falseMark);
	result.append(OUSTRING(" document"));
	return result.makeStringAndClear();
}

OUString EventSummary::getForwardEventStatus() {
	rtl::OUStringBuffer result;

	result.append(forwardEvent ? trueMark : falseMark);
	result.append(OUSTRING(" event"));
	return result.makeStringAndClear();
}

OUString EventSummary::getForwardDocumentStatus() {
	rtl::OUStringBuffer result;

	result.append(forwardDocument ? trueMark : falseMark);
	result.append(OUSTRING(" document"));
	return result.makeStringAndClear();
}
/******************************************************************************
EncryptedProvenanceAuditor
******************************************************************************/
static cJSON* newDecryptedChain(std::vector<std::string>& provenances, DocumentMetaInfo& documentMetaInfo) {
	cJSON* provenanceArray = cJSON_CreateArray();

	std::vector<std::string>::iterator it;
	for (it = provenances.begin(); it != provenances.end(); ++it) {
#if ENCRYPT_PROVENANCE == ENCRYPT_PROVENANCE_ON
		std::string outerProvenance = ProvenanceHelper::decryptData(documentMetaInfo.documentEncryptionKey,
					ProvenanceHelper::base64Decode(*it));
		cJSON* innerProvenances = cJSON_Parse(outerProvenance.c_str());
#else
		cJSON* innerProvenances = cJSON_Parse(it->c_str());
#endif
		if (!innerProvenances)
			continue;
		cppJSON innerJson(innerProvenances);

		while (cJSON_GetArraySize(innerProvenances) > 0) {
			cJSON* item = cJSON_DetachItemFromArray(innerProvenances, 0);

			cJSON_AddItemToArray(provenanceArray, item);
		}
	}
	return provenanceArray;
}

EncryptedProvenanceAuditor::EncryptedProvenanceAuditor(
		Reference<XComponent> xComponent,
		std::vector<std::string>& provenances,
		DocumentMetaInfo documentMetaInfo, MessageBoxProvider* messageBoxProvider):
		xComponent(xComponent), textHelper(xComponent), documentMetaInfo(documentMetaInfo),
		messageBoxProvider(messageBoxProvider) {

	colorIndex = 0;
	eventIndex = 0;
	highlighted = false;

	cJSON* decryptedChain = 0;	
	try {
		decryptedChain = newDecryptedChain(provenances, documentMetaInfo);
		if (decryptedChain == 0)
			throw HaathiException("Could not parse provenance chain");
	}
	catch (HaathiException& haathiException) {
		messageBoxProvider->showErrorMessageBox(haathiException.getMessage());
		return;
	}

	try {
		fillProvenanceChain(decryptedChain);
		cJSON_Delete(decryptedChain);
	}
	catch (...) {
		cJSON_Delete(decryptedChain);
		messageBoxProvider->showErrorMessageBox("Could not fill provenance chain.");
		return;
	}

	fillProvenanceEvents();
	fillUserColorMap();
	eventIndex = provenanceChain.size() - 1;

	if (!validateForwardEvolutionOfEntry(eventIndex))
		messageBoxProvider->showErrorMessageBox("The document's last recorded hash value does not match its contents.\nIt has likely been manipulated!");

	fillEventSummaries();
}

EncryptedProvenanceAuditor::~EncryptedProvenanceAuditor() {
	unsigned int size;
	
	size = provenanceEvents.size();
	for (unsigned int i = 0; i < size; ++i)
		if (provenanceEvents[i])
			delete provenanceEvents[i];
	size = provenanceChain.size();
	for (unsigned int i = 0; i < size; ++i)
		if (provenanceChain[i])
			delete provenanceChain[i];
}

void EncryptedProvenanceAuditor::fillProvenanceChain(cJSON* decryptedChain) {
	int size = cJSON_GetArraySize(decryptedChain);

	for (sal_Int32 i = 0; i < size; ++i) {
		char* string = cJSON_GetArrayItem(decryptedChain, i)->valuestring;
		//cJSON* item = cJSON_Parse(string);
		// If it is given the string, it will free the cJSON*
		provenanceChain.push_back(new ProvenanceEntry(string));
	}
}

void EncryptedProvenanceAuditor::fillProvenanceEvents() {
	ProvenanceHelper auditorHelper(documentMetaInfo.provenanceEncryption);
	CryptoInfo keyInfo = documentMetaInfo.provenanceEncryption;
	int auditorId = documentMetaInfo.userInfo.userId;

	provenanceEvents = std::vector<ProvenanceEvent*>(provenanceChain.size());
	eventSummaries = std::vector<EventSummary>(provenanceEvents.size());

	for (unsigned int i = 0; i < provenanceChain.size(); ++i) {
		// This can be broken down even finer
		ProvenanceEntry* provenanceEntry = provenanceChain[i];

		std::string sessionKey1;
		int sessionKeySize;
		try {
			bool found;
			std::map<int, CryptoInfo>::iterator itr = provenanceEntry->findKeyingMaterial(auditorId, &found);
			if (!found)
				throw HaathiException("Auditor not found");
			sessionKey1 = itr->second.getSessionKey().getData();
			sessionKeySize = itr->second.getSessionKey().getSize();
		}
		catch (HaathiException&) {
			eventSummaries[i] = EventSummary(false);
			continue;
		}

		std::string hexDecodedStr;
		try {
			hexDecodedStr = auditorHelper.base64Decode(sessionKey1);
		}
		catch (...) {
			eventSummaries[i] = EventSummary(true, false);
			continue;
		}

		std::string sessionKey2;
		try {
			sessionKey2 = auditorHelper.decryptWithRSA(
					hexDecodedStr, documentMetaInfo.provenanceEncryption.privateKey,
					documentMetaInfo.provenanceEncryption.privateKeySize);
		}
		catch (...) {
			eventSummaries[i] = EventSummary(true, true, false);
			continue;
		}

		std::string actualEventJSONString;
		try {
			actualEventJSONString = auditorHelper.decryptData(
					auditorHelper.base64Decode(provenanceEntry->getEncryptedEvent()),
					sessionKey2, sessionKeySize);
		}
		catch (...) {
			eventSummaries[i] = EventSummary(true, true, true, false);
			continue;
		}

		try {
			std::stringstream stream(actualEventJSONString);
			HAATHI_IARCHIVE ia(stream);
			ia >> BOOST_SERIALIZATION_NVP(provenanceEvents[i]);
		}
		catch (...) {
			eventSummaries[i]= EventSummary(true, true, true, true, false);
			continue;
		}
		eventSummaries[i] = EventSummary(true, true, true, true, true, provenanceEvents[i]);
	}
}

Color EncryptedProvenanceAuditor::nextColor() {
	static const sal_Int32 colors[] = {
		0x00ffff00, 0x00adff2f, 0x0087cefa, 0x00b0c4de,
		0x00ff00ff, 0x0040e0d0, 0x0098fb98, 0x00f0e68c
	};
	static const int size = sizeof(colors) / sizeof(Color);

	colorIndex = colorIndex % size;
	return (Color) colors[colorIndex++];
}

void EncryptedProvenanceAuditor::fillUserColorMap() {
	std::map<std::string, Color>::iterator itr;

	for (unsigned int i = 0; i < provenanceEvents.size(); ++i) {
		if (!provenanceEvents[i])
			continue;
		itr = userColorMap.find(provenanceEvents[i]->getUser()); // not unique, need ID
		// If we have a new user, assign them a new color.
		if (itr == userColorMap.end())
			userColorMap[provenanceEvents[i]->getUser()] = nextColor();
	}
}

void EncryptedProvenanceAuditor::fillEventSummaries() {
	for (size_t i = 0; i < provenanceEvents.size(); ++i) {
		eventSummaries[i].setColor(findColor(i));
		eventSummaries[i].setIntegrity(validateIntegrityOfEntry(i));
	}
	for (int i = provenanceEvents.size() - 1; i >= 0; --i) {
		eventSummaries[i].setBackwardEvent(undoCurrentEvent(false));
		eventSummaries[i].setBackwardDocument(validateBackwardEvolutionOfEntry(i));
	}
	textHelper.setString(OUString());
	for (size_t i = 0; i < provenanceEvents.size(); ++i) {
		eventSummaries[i].setForwardEvent(redoCurrentEvent(i == provenanceEvents.size() - 1));
		eventSummaries[i].setForwardDocument(validateForwardEvolutionOfEntry(i));
	}
}

bool EncryptedProvenanceAuditor::validateIntegrityOfEntry(sal_Int32 index) {
	ProvenanceHelper integrityHelper(documentMetaInfo.provenanceEncryption);
	// First verify if the signature is correct
	ProvenanceEntry* provenanceEntry = provenanceChain[index];

	// Some temporary strings.
	std::string hexDecodedStr = integrityHelper.base64Decode(provenanceEntry->getChecksum());

	// Check if the signature is valid.
	std::string verifiedData;
	bool isValidSignature = integrityHelper.verifyDataSignature(
			hexDecodedStr, verifiedData, provenanceEntry->getPublicKey(),
			provenanceEntry->getPublicKeySize());
	if (!isValidSignature)
		return false;

	// recompute the hash corresponding to the checksum and compare it 
	// with the value in the entry
	std::vector<std::string> checksumComponents;
	std::string prevChecksum = index > 0 ? provenanceChain[index - 1]->getChecksum() : "";

	checksumComponents.push_back(provenanceEntry->getPrincipalIdentifier());
	checksumComponents.push_back(provenanceEntry->getEncryptedEvent());
	checksumComponents.push_back(provenanceEntry->getDocumentHash());
	checksumComponents.push_back(provenanceEntry->getPublicKey());
	checksumComponents.push_back(provenanceEntry->getKeyingMaterialString());

	std::string computedHash = integrityHelper.generateHash(checksumComponents);
	computedHash.append(prevChecksum);
	computedHash = integrityHelper.generateHash(computedHash);
	return computedHash == verifiedData;
}

bool EncryptedProvenanceAuditor::validateForwardEvolutionOfEntry(sal_Int32 index) {
	try {
		std::string documentState = convertToString(textHelper.getString());
		std::string documentHash = ProvenanceHelper::generateHash(documentState);
		std::string existingHash = provenanceChain[index]->getDocumentHash();

		return documentHash == existingHash;
	}
	catch (...) {
		return false;
	}
}

bool EncryptedProvenanceAuditor::validateBackwardEvolutionOfEntry(sal_Int32 index) {
	try {
		std::string documentState = convertToString(textHelper.getString());
		std::string documentHash = ProvenanceHelper::generateHash(documentState);
		std::string existingHash = index > 0 ?
				provenanceChain[index - 1]->getDocumentHash() :
				ProvenanceHelper::generateHash(std::string());
		
		return documentHash == existingHash;
	}
	catch (...) {
		return false;
	}
}

bool EncryptedProvenanceAuditor::isEventIndexValid(int eventIndex) {
	return 0 <= eventIndex && eventIndex < (sal_Int32) provenanceEvents.size();
}

bool EncryptedProvenanceAuditor::isProvenanceEventValid() {
	return isEventIndexValid(eventIndex) && provenanceEvents[eventIndex] != 0;
}

void EncryptedProvenanceAuditor::loadEventAt(sal_Int32 index) {
	if (!isEventIndexValid(index))
		return;
	try {
		while (eventIndex < index)
			redoCurrentEvent(eventIndex + 1 == index);
		while (index < eventIndex)
			undoCurrentEvent(eventIndex - 1 == index);
	}
	catch (Exception& exception) {
		messageBoxProvider->showErrorMessageBox(exception);
	}
	catch (std::exception& exception) {
		messageBoxProvider->showErrorMessageBox(exception);
	}
	catch (...) {
		messageBoxProvider->showErrorMessageBox("I don't know what happened.");
	}
}

Color EncryptedProvenanceAuditor::findColor(sal_Int32 index) {
	Color color = 0xffffffff;

	if (provenanceEvents[index] == 0)
		return color;
	std::map<std::string, Color>::iterator itr;
	itr = userColorMap.find(provenanceEvents[index]->getUser());

	if (itr != userColorMap.end())
		color = itr->second;
	return color;
}

bool EncryptedProvenanceAuditor::redoCurrentEvent(bool highlight) throw (Exception, std::exception) {
	bool result = false;

	// Take care of unhighlighting the current event
	if (isProvenanceEventValid() && highlighted) {
		provenanceEvents[eventIndex]->undo(xComponent);
		provenanceEvents[eventIndex]->redo(xComponent);
		highlighted = false;
	}
	eventIndex++;
	if (isProvenanceEventValid()) {
		if (highlight)
			result = provenanceEvents[eventIndex]->redo(xComponent, findColor(eventIndex));
		else
			result = provenanceEvents[eventIndex]->redo(xComponent);
		highlighted = highlight;
	}
	return result;
}

bool EncryptedProvenanceAuditor::undoCurrentEvent(bool highlight) throw (Exception, std::exception) {
	bool result = false;

	if (isProvenanceEventValid())
		result = provenanceEvents[eventIndex]->undo(xComponent);
	eventIndex--;
	// Take care of highlighting the previous event
	if (isProvenanceEventValid() && highlight) {
		provenanceEvents[eventIndex]->undo(xComponent);
		provenanceEvents[eventIndex]->redo(xComponent, findColor(eventIndex));
		highlighted = true;
	}
	return result;
}

Reference<XComponent> EncryptedProvenanceAuditor::createProvenanceFromStream(
		Reference<XComponentContext> xComponentContext, std::string document,
		std::string title) {
	// Uncomment the ReadOnly to be able to see the actual characters
	PropertyValuesHelper props(1);
	props.add("ReadOnly", Any(sal_True));
//	props.add("ReadOnly", Any(sal_False));

	Reference<XComponent> xComponent = DocumentStore(xComponentContext)
			.loadDocumentFromString(document, props.get());
	ExtensionHelper::setTitle(xComponent, title, true);
	return xComponent;
}

std::vector<EventSummary>* EncryptedProvenanceAuditor::getEventSummaries() {
	return &eventSummaries;
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
