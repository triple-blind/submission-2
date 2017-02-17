/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/TextDocumentListener.cpp#2 $

/*
 *  Created by Aaron Gibson on 8/2/11.
 */
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/document/TextDocumentListener.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "Config.hpp"

#include "com/haathi/listener/DropTarget.hpp"
#include "com/haathi/listener/FrameAction.hpp"
#include "com/haathi/listener/Key.hpp"
#include "com/haathi/listener/KeyHandler.hpp"
#include "com/haathi/listener/LinguService.hpp"
#include "com/haathi/listener/MouseHandler.hpp"
#include "com/haathi/listener/Refresh.hpp"
#include "com/haathi/listener/TopWindow.hpp"
#include "com/haathi/listener/UndoManager.hpp"
#include "com/haathi/listener/Window.hpp"
#include "com/haathi/listener/Window2.hpp"
#include "com/haathi/provenance/document/ListenDocumentProvenance.hpp"
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/Guard.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include "sdkBeg.hpp"
#include "sdkEnd.hpp"

#include <vector>
/******************************************************************************
Define
******************************************************************************/
#define DEBUG_OFF 0
#define DEBUG_ON  1

#if !defined(DEBUG)
#	define DEBUG DEBUG_OFF
#endif
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::listener;
using namespace com::haathi::provenance;
using namespace com::haathi::text;
using namespace com::haathi::utils;

using namespace com::sun::star::container;
using namespace com::sun::star::document;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
using namespace com::sun::star::util;
using namespace com::sun::star::view;

using namespace rtl;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class
******************************************************************************/		
TextDocumentListener::TextDocumentListener(Reference<XComponentContext> xComponentContext,
		Reference<XComponent> xComponent, ProvenanceDocument& provenanceDocument, TextHelper& textHelper, bool interactive):
		xComponentContext(xComponentContext), xComponent(xComponent),
		provenanceDocument(provenanceDocument), textHelper(textHelper),
		xModifiable(Reference<XModifiable>(xComponent, UNO_QUERY)), shouldListen(true) {
	addListeners(interactive);
	guard(&TextDocumentListener::modified, 62, 7390427, __FILE__, __LINE__);
}

TextDocumentListener::~TextDocumentListener() {
	std::vector<Reference<XEventListener> >::iterator it;

	for (it = undisposedEventListeners.begin(); it != undisposedEventListeners.end(); ++it)
		// After being disposed they then are correctly deleted.
		// TODO test again
		; // (*it)->disposing(EventObject());
}

void TextDocumentListener::addListeners(bool interactive) {
	if (!interactive) // Just skip these all together
		return;

	new SelectionChangeListener(xComponent, this); // working and used
	new UndoManagerListener(xComponent, NULL); // working
	new DocumentEventListener(xComponent, NULL); // not triggered, so not used
	if (interactive)
		new ModifyListener(xComponent, this); // working and used
	new PrintJobListener(xComponent, this); // working and used, not disposed but is deleted just the same?
	// This one causes a crash!
	//new RefreshListener(xComponent, NULL); // not triggered and causing crash
	new FrameActionListener(xComponent, NULL); // working, in particular on window close
	// This one would be disposed, but not until the XFrame does it, which is too late.

#if DEBUG == DEBUG_OFF
	undisposedEventListeners.push_back(static_cast<XEventListener*>(new TopWindowListener(xComponent, this)));
#endif

	// working, never disposed
	undisposedEventListeners.push_back(static_cast<XEventListener*>(new MouseHandlerListener(xComponent, NULL)));
	// working, never disposed
	undisposedEventListeners.push_back(static_cast<XEventListener*>(new KeyHandlerListener(xComponent, NULL)));

	new Window2Listener(xComponent, NULL); // working, but not triggered, may only be for dialogs, is disposed
	new WindowListener(xComponent, NULL); // working, disposed

	// After document destruction
	new KeyListener(xComponent, NULL); // not working, but see KeyHandler
	new DropTargetListener(xComponentContext, xComponent, NULL); // working, disposed
	// working, but not triggered, not disposed
	undisposedEventListeners.push_back(static_cast<XEventListener*>(new LinguServiceListener(xComponentContext, xComponent, NULL)));
}

void TextDocumentListener::listen(bool shouldListen) {
	this->shouldListen = shouldListen;
}

OUString TextDocumentListener::getString() {
	return textHelper.getString();
}

bool TextDocumentListener::isModified() {
	return isTrue(xModifiable->isModified());
}

void TextDocumentListener::record(std::string channel, std::string message, bool modify) {
	provenanceDocument.addProvenanceEvent(new ListenDocumentProvenance(channel, message), modify);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
