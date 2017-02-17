// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/dialog/BaseDialog.hpp#1 $

//
// UnoWindowBase.hpp
//
// Author: Aaron Gibson
// Date: August 17, 2011
//

#ifndef BASEDIALOG_HPP
#define BASEDIALOG_HPP

#include <string> // std::string
// System includes
// In com::sun::star::awt
#include <com/sun/star/awt/XActionListener.hpp>
#include <com/sun/star/awt/XAdjustmentListener.hpp>
#include <com/sun/star/awt/XCheckBox.hpp>
#include <com/sun/star/awt/XComboBox.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XDateField.hpp>
#include <com/sun/star/awt/XDialog.hpp>
#include <com/sun/star/awt/XNumericField.hpp>
#include <com/sun/star/awt/XPatternField.hpp>
#include <com/sun/star/awt/XProgressBar.hpp>
#include <com/sun/star/awt/XScrollBar.hpp>
#include <com/sun/star/awt/ScrollBarOrientation.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
#include <com/sun/star/awt/XTimeField.hpp>
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
// components
#include <com/sun/star/awt/XButton.hpp>
#include <com/sun/star/awt/XListBox.hpp>
// In com::sun::star::beans
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
// In com::sun::star::container
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
// In com::sun::star::frame
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
// In com::sun::star::graphic
#include <com/sun/star/graphic/XGraphic.hpp>
// In com::sun::star::lang
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
// In com::sun::star::util
#include <com/sun/star/util/Color.hpp>
#include <com/sun/star/util/URL.hpp>
// In com::sun::star::uno
#include <com/sun/star/uno/XComponentContext.hpp>

// My includes
#include "com/haathi/dialog/ListenerTypedefs.hpp"
#include "com/haathi/utils/Debug.hpp"

// namespaces
using namespace com::sun::star::awt;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;
using namespace com::sun::star::graphic;
using namespace com::sun::star::lang;
using namespace com::sun::star::util;
using namespace com::sun::star::uno;

// For OUString...
using namespace rtl;
using namespace cppu;

namespace com {
namespace haathi {
namespace dialog {

// Forward-declare classes
class LoadPageListener;

class BaseDialog {
private:
protected:
	OUString getURLToExtensionRoot();

	// These are the context component context and factoryReference< XToolkit > xToolkit;
	Reference<XMultiComponentFactory> xMCF;
	Reference<XComponentContext> xContext;
	// Instance variables
//	Reference<XMultiServiceFactory> remoteMSF; // Remote service manager.
	Reference<XMultiServiceFactory> xMSF; // Local service manager
	Reference<XInterface> baseModel;
	Reference<XInterface> base;
	Reference<XPropertySet> windowProps; // Properties for THIS dialog.
	Reference<XNameContainer> xNameContainer;
	Reference<XControlContainer> xControlContainer;
	// These are used for getting the window properties and the associated listeners.
	Reference<XControl> xControl;
	Reference<XWindow> xWindow;
	Reference<XWindowPeer> xWindowPeer;
	Reference<XToolkit> xToolkit;

	/*!
	 This stores whether this instance of "BaseDialog" is modal or not.
	 If it is modal, this will block all other actions on the same ServiceFactory.
	 If it is NOT modal, this will exist as a standalone window.
	 */
	sal_Bool isModal;

	// This stores the return result.
	sal_Int16 dialog_result;

	// This stores the currently loaded page or "Step" in the dialog.
	sal_Int16 page;

	// Copy Constructor and Assignment operator are protected in case the subclasses 
	// want to implement 'public' versions of them.
	/*!
	 The assignment operator passes a reference of everything here. (Not a hard copy).
	 @param other -- the other instance to assign over.
	 @return -- this instance of BaseDialog.
	 */
	BaseDialog& operator=(const BaseDialog& other);

	/*!
	 The Copy Constructor will pass a reference of everything here. (Not a hard copy).
	 @param other -- the other instance to copy.
	 */
	BaseDialog(const BaseDialog& other);

	/*!
	 This function will add the control to the dialog.
	 */
//	void 
public:
	// Constructors and Destructors
	/*!
	 This constructor creates an instance of an BaseDialog. It requires a remote 
	 XMultiServiceFactory to function properly; everything else that is required is
	 automatically handled by this call.
	 @param _xMSF -- the remove XMultiServiceFactory to create this window.
	 @param modal -- indicates whether this dialog should take focus from its parent.
	 By default, this parameter is assumed true, if not specified.
	 */

	BaseDialog(Reference<XComponentContext> xContext, sal_Bool model = sal_True);
	BaseDialog(Reference<XComponentContext> xContext,
			sal_Int32 height, sal_Int32 width, OUString title);

	void initialize(Reference<XMultiServiceFactory> _xMSF);

	// Functions from ExtensionHelper
	std::string getSystemFilePathToExtensionFile(const char* file);
	Reference<XGraphic> getGraphicFromFile(const char* file);

	/*!
	 This destructor destroys the window. I don't call dispose() anywhere, and this might
	 be needed... Check this when debugging.
	 */
	virtual ~BaseDialog();

	//==================================================
	// Initialization-eque Functions
	//==================================================

	void setWindowProperties(sal_Int32 height, sal_Int32 width,
			OUString title, sal_Bool closeable = true);

	/*!
	 This function sets the size and title of the main window component. Note that the position
	 parameters are with respect to the 'Parent' context of this window. The position shouldn't
	 matter too much anyway, but just for reference.

	 The 'units' for each of the parameters are 'MA' which is based on the current font.
	 
	 @param xpos -- the x-position of the window when instantiated.
	 @param ypos -- the y-position of the window when instantiated.
	 @param height -- the height of the window
	 @param width -- the width of the window
	 @param title -- the name of the window (this displays on the topbar-part of the window)
	 @param closeable -- if false, the window won't close when the 'x' in the corner is clicked.
	 */
	void setWindowProperties(sal_Int32 xpos, sal_Int32 ypos, sal_Int32 height,
			sal_Int32 width, OUString title, sal_Bool closeable = true);

	/*!
	 This function must be called after all of the components are added. It will setup the controls
	 for the window and execute it so that the window is active. The window, by default, will also
	 become visible, but this can be changed by explicitly setting the parameter 'visible'.
	 
	 This function will also return an instance of DialogResult to indicate how the dialog was exitted.

	 @param visible -- (optional; true by default) true makes the window visible, 
	 false hides the window. Note that in general, this option should NOT be specified.
	 @result -- the value returned by the 'execute()' function of XDialog, indicating how the dialog was 
	 exited.
	 */
	virtual sal_Int16 run(sal_Bool visible = sal_True);

	virtual sal_Int16 execute(sal_Bool visible = true) {
		return run(visible);
	}

	//=========================================================
	// Helper Functions, Getters, and Setters
	//=========================================================	
	/*!
	 This function will close the dialog.
	 */
	void close();

	/*!
	 This function will close the dialog. It is equivalent to calling "close()".
	 */
	void endExecute();

	/*!
	 This function will force a redraw of the dialog.
	 */
	void repaint() {
		xWindowPeer->invalidate(8);
	}

	/*!
	 This function will set the return value for this dialog.
	 */
	void setReturnValue(sal_Int16 value) {
		dialog_result = value;
	}

	/*!
	 This function will load the given page. This function isn't needed in a single-page
	 dialog.
	 
	 @param page -- the number (step) of the page to load.
	 */
	void loadPage(sal_Int16 _page);

	sal_Int16 getCurrentPage() const {
		return page;
	}

	//=========================================================
	// Insertion Functions
	//=========================================================
	Reference<XPropertySet> insertImageOnPage(sal_Int16 page, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 height, sal_Int32 width, OUString name,
			Reference<XGraphic> graphic,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

	Reference<XPropertySet> insertImage(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			Reference<XGraphic> graphic,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertImageOnPage(0, posx, posy, height, width, name, graphic,
				names, values);
	}

	/*! 
	 This function will create a checkbox in this window at the specified location.
	 */
	Reference<XCheckBox> insertCheckboxOnPage(sal_Int16 page, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 height, sal_Int32 width, OUString name,
			OUString label, sal_Bool enable_tristate, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>());

	Reference<XCheckBox> insertCheckboxOnPage(sal_Int16 page, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 width, OUString name, OUString label,
			sal_Bool enable_tristate,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertCheckboxOnPage(page, posx, posy, 12, width, name, label,
				enable_tristate, names, values);
	}

	Reference<XCheckBox> insertCheckbox(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 width, OUString name, OUString label,
			sal_Bool enable_tristate,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertCheckboxOnPage(0, posx, posy, 12, width, name, label,
				enable_tristate, names, values);
	}

	Reference<XCheckBox> insertCheckbox(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name, OUString label,
			sal_Bool enable_tristate,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertCheckboxOnPage(0, posx, posy, height, width, name, label,
				enable_tristate, names, values);
	}

	/*!
	 This function will create a button in this window at the specified location. The '_listener'
	 parameter is an XActionListener that indicates what to do when the button is pressed.
	 The position parameters specify the position inside of the main window where the button is
	 actually inserted. 

	 There are two other parameters: name and label. The 'name' parameter identifies the button
	 and acts more as a 'key' or 'hash' to find the button in the list. The 'label' parameter is 
	 what is actually displayed on the button itself.

	 Note that the default value of the height of the button should remain 14. This is the default
	 value. I don't give the freedom to change this; maybe this can be done later, but right now,
	 it doesn't matter.

	 @param _listener -- the XActionListener that is notified when the button is clicked.
	 @param posx -- the x-position of the button inside of the current window.
	 @param posy -- the y-position of the button inside of the current window.
	 @param width -- the width of the button
	 @param name -- a unique identifier string of this button.
	 @param label -- the text displayed on the button itself.
	 @param names -- the names of any additional properties to pass in. This is optional.
	 @param values -- the values of the respective properties defined in 'names'. This is optional.
	 @return -- a reference to the created button.
	 */
	Reference<XButton> insertButton(Reference<XActionListener> _listener,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
			OUString label, Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertButtonOnPage(0, _listener, posx, posy, 12, width, name,
				label, names, values);
	}

	Reference<XButton> insertButtonOnPage(sal_Int16 page,
			Reference<XActionListener> _listener, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 width, OUString name, OUString label,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertButtonOnPage(page, _listener, posx, posy, 12, width, name,
				label, names, values);
	}

	/*!
	 This function will create a button in this window at the specified location. The '_Listener'
	 parameter is an XActionListener that indicates what to do when the button is pressed. The 
	 position parameters specify the position inside of the main window where the button is actually 
	 inserted. 

	 The 'name' parameter must be a unique string that identifies this button in the given window.
	 The 'label' parameter is the text that the button actually displays.

	 This method allows the user to specify other properties
	 of the button, such as images, background color, etc. The full list of properties that can be 
	 edited are listed below where the argument in parenthesis indicates the data-type of the expected
	 value for that property. Here's the list:
	 (short) Align, (com::sun::star::util::Color) BackgroundColor, 
	 (bool) DefaultButton, 
	 (bool) Enabled,
	 (bool) FocusOnClick, 
	 (FontDescriptor) FontDescriptor, 
	 (short) FontEmphasisMark, 
	 (short) FontRelief,
	 (OUString) HelpText, 
	 (OUString) HelpURL, 
	 (short) ImageAlign, 
	 (short) ImagePosition, 
	 (OUString) imageURL, 
	 (com::sun::star::graphic::XGraphic) Graphic, 
	 (OUString) label, 
	 (bool) MultiLine, 
	 (bool) Printable, 
	 (short) PushButtonType, 
	 (bool) Repeat, 
	 (long) RepeatDelay,
	 (short) State, 
	 (bool) Tabstop, 
	 (com::sun::star::util::Color) TextColor,
	 (com::sun::star::util::Color) TextLineColor, 
	 (bool) Toggle, 
	 (com::sun::star::style::VerticalAlignment) VerticalAlign

	 @param _listener -- the XActionListener that is notified when the button is clicked.
	 @param posx -- the x-position of the button inside of the current window.
	 @param posy -- the y-position of the button inside of the current window.
	 @param width -- the width of the button
	 @param name -- a unique identifier string of this button.
	 @param label -- the text displayed on the button itself.
	 @param propNames -- the sequence of property names
	 @param propValues -- the sequence of property values for the respective names.
	 @return -- a reference to the created button.
	 
	 */
	Reference<XButton> insertButton(Reference<XActionListener> _listener,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, OUString label, Sequence<OUString> propNames =
					Sequence<OUString>(),
			Sequence<Any> propValues = Sequence<Any>()) {
		return insertButtonOnPage(0, _listener, posx, posy, height, width, name,
				label, propNames, propValues);
	}

	Reference<XButton> insertButtonOnPage(sal_Int16 page,
			Reference<XActionListener> _listener, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 height, sal_Int32 width, OUString name,
			OUString label, Sequence<OUString> propNames = Sequence<OUString>(),
			Sequence<Any> propValues = Sequence<Any>());
	/*!
	 This function will create a listbox that lists all of the items (OUStrings) passed in.
	 Listeners can also be passed in to identify the selection of the current value.
	 Other stuff can probably be handled as well, but I don't do that now.
	 
	 @param posx -- the x-position of the listbox in the window
	 @param posy -- the y-position of the listbox in the window
	 @param height -- the height of the listbox in the window
	 @param width -- the width of the listbox in the window
	 @param name -- a UNIQUE name identifying this component
	 @param list_items -- a 'sequence' of all of the items to display in the listbox.
	 @param multi_select -- (optional) true allows multiple items to be selected.
	 @result -- a reference to the created listbox.
	 */
	Reference<XListBox> insertListBoxOnPage(sal_Int16 page, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<OUString> list_items, sal_Bool multi_select,
			Sequence<OUString> otherProps = Sequence<OUString>(),
			Sequence<Any> otherVals = Sequence<Any>());

	Reference<XListBox> insertListBox(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<OUString> list_items, sal_Bool multi_select = sal_False ) {
		return insertListBoxOnPage((sal_Int16) 0, posx, posy, height, width,
				name, list_items, multi_select, Sequence<OUString>(),
				Sequence<Any>());
	}

	Reference<XListBox> insertListBox(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<OUString> list_items, sal_Bool multi_select,
			Sequence<OUString> otherProps, Sequence<Any> otherVals) {
		return insertListBoxOnPage((sal_Int16) 0, posx, posy, height, width,
				name, list_items, multi_select, otherProps, otherVals);
	}

	Reference<XListBox> insertListBoxOnPage(sal_Int16 page, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<OUString> list_items, sal_Bool multi_select = sal_False ) {
		return insertListBoxOnPage(page, posx, posy, height, width, name,
				list_items, multi_select, Sequence<OUString>(), Sequence<Any>());
	}

	//---------------------------------------
	// Insert ComboBox Functions
	//---------------------------------------	
	Reference<XComboBox> insertComboBoxOnPage(sal_Int16 page, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<OUString> item_list,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

	Reference<XComboBox> insertComboBoxOnPage(sal_Int16 page, sal_Int32 posx,
			sal_Int32 posy, sal_Int16 width, OUString name,
			Sequence<OUString> item_list,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertComboBoxOnPage(page, posx, posy, 12, width, name,
				item_list, names, values);
	}

	Reference<XComboBox> insertComboBox(sal_Int16 page, sal_Int32 posx,
			sal_Int32 posy, sal_Int16 width, OUString name,
			Sequence<OUString> item_list,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertComboBoxOnPage(0, posx, posy, 12, width, name, item_list,
				names, values);
	}

	Reference<XComboBox> insertComboBox(sal_Int16 page, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 height, sal_Int16 width, OUString name,
			Sequence<OUString> item_list,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertComboBoxOnPage(0, posx, posy, height, width, name,
				item_list, names, values);
	}

	//---------------------------------------
	// Insert TextField Functions
	//---------------------------------------

	// These are used for setting the alignments.
#define HORIZONTAL_ALIGN_LEFT 0
#define HORIZONTAL_ALIGN_CENTER 1
#define HORIZONTAL_ALIGN_RIGHT 2
	/*!
	 This function will create a 'label' of text. This text is meant to function as a title or
	 as an informative text block. This text can be formatted by passing in a sequence of the
	 additional properties in 'names' and 'values' respectively. The types of changes (with the
	 required 'value' type in parenthesis) are:
	 (short) Align -- specifies the horizontal alignment of the text in the control.
	 (com::sun::star::util::Color) BackgroundColor -- specifies the background color of the control.
	 (short) Border -- specifies the border style of the control.
	 (long) BorderColor -- specifies the color of the Border, if present
	 (bool) enabled -- determines whether the control is enabled or disabled
	 (FontDescriptor) FontDescriptor -- specifies the font attributes of the text in the control.  
	 (short) FontEmphasisMark -- specifies the ::com::sun::star::text::FontEmphasis value of the text.  
	 (short) FontRelief -- specifies the ::com::sun::star::text::FontRelief value of the text.  
	 (string) HelpText -- specifies the help text of the control.  
	 (string) HelpURL -- specifies the help URL of the control.  
	 (string) Label -- specifies the label of the control.  
	 (boolean) MultiLine -- specifies that the text may be displayed on more than one line.
	 (short) TabIndex -- specifies when this component is accessible with the "tab" key.
	 (boolean) Printable -- specifies that the control will be printed with the document.  
	 (com::sun::star::util::Color) TextColor -- specifies the text color (RGB) of the control.  
	 (com::sun::star::util::Color) TextLineColor -- specifies the text line color (RGB) of the control.  
	 (com::sun::star::style::VerticalAlignment) VerticalAlign
	 @param posx -- the x-position of the text field.
	 @param posy -- the y-position of the text field.
	 @param height -- the height of the text field.
	 @param width -- the width of the text field.
	 @param name -- a UNIQUE name identifying this component.
	 @param label -- the text that is displayed in the field.
	 @param alignment -- sets the alignment of the text (i.e. Centered, left-justified, right-justified)
	 @param names -- (optional) names of any additional properties for the text, (notably things like font,
	 color and border type)
	 @param values -- (optional) values of the respective properties specified by names.
	 @result -- the XPropertySet for the text. This allows for changing the text displayed.
	 */
	Reference<XPropertySet> insertFixedText(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name, OUString label,
			sal_Int16 alignment, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertFixedTextOnPage(0, posx, posy, height, width, name, label,
				alignment, names, values);
	}

	Reference<XPropertySet> insertFixedText(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 width, OUString name, OUString label, sal_Int16 alignment,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertFixedTextOnPage(0, posx, posy, 12, width, name, label,
				alignment, names, values);
	}

	Reference<XPropertySet> insertFixedTextOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
			OUString label, sal_Int16 alignment, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertFixedTextOnPage(page, posx, posy, 12, width, name, label,
				alignment, names, values);
	}

	Reference<XPropertySet> insertFixedTextOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, OUString label, sal_Int16 alignment,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

	/*!
	 This function is the same as the other "insertTextField()" function, only this will assume that 
	 the height is 12 by default.
	 @param posx -- the x-position of the text field.
	 @param posy -- the y-position of the text field.
	 @param width -- the width of the text field.
	 @param name -- a UNIQUE name identifying this component.
	 @param tab_index -- a value that indicates the 'tab' button priority of this text field.
	 @param label -- the text that is displayed in the field.
	 @param alignment -- sets the alignment of the text (i.e. Centered, left-justified, right-justified)
	 @param names -- (optional) names of any additional properties for the text.
	 @param values -- (optional) values of the respective properties specified by names.
	 @result -- the XPropertySet for the text. This allows for changing the text displayed.
	 */

	/*!
	 This function will create an editable text field and return a handle to actually get the
	 text. This handle also allows for adding an "XTextListener" and/or an "XFocusListener".
	 
	 By default, the constructed text field will have a height of 12. (Whatever "12" means in
	 OpenOffice units...)
	 
	 Additional properties can be passed to create more customizable text fields. They are listed
	 below (parenthesis indicate the datatype required):
	 EchoChar (short or uchar) -- the character displayed as the user types; used for password entry.
	 HardLineBreaks (boolean) -- specifies if hard line breaks are included in the text.
	 HideInactiveSelection (boolean) -- inidicates if selected text remains selected after focus loss.
	 MaxTextLen (short) -- the length of the largest text field; 0 indicates no limitation.
	 MultiLine (boolean) -- indicates whether to display multiple lines of text.
	 LineEndFormat (short) -- A unicode character (short) indicating newlines in MultiLine contexts.
	 ReadOnly (boolean) -- indicates whether the text field is read-only.
	 Text (string) -- the text currently in the text field.
	 VScroll (boolean) -- indicates whether to allow a vertical scrollbar in MultiLine contexts.
	 HScroll (boolean) -- indicates whether to allow a horizontal scrollbar in MultiLine contexts.
	 
	 @param posx -- the x-coordinate of the top-left of the text field.
	 @param posy -- the y-coordinate of the top-left of the text field.
	 @param width -- the width (length) of the resulting text field.
	 @param name -- a name for this component; the name MUST be unique!
	 @param initialText -- the initial text to display in the text field.
	 @param names -- (optional) the names of any additional properties to set.
	 @param values -- (optional) the values of any additional properties to set.
	 @result -- A reference to an XTextComponent that allows for getting the text as well as
	 adding any listeners.
	 */
	Reference<XTextComponent> insertTextFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, OUString initialText, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>());

	Reference<XTextComponent> insertTextField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 width, OUString name, OUString initialText,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertTextFieldOnPage(0, posx, posy, 12, width, name,
				initialText, names, values);
	}

	Reference<XTextComponent> insertTextFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
			OUString initialText, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertTextFieldOnPage(page, posx, posy, 12, width, name,
				initialText, names, values);
	}

	Reference<XTextComponent> insertTextField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			OUString initialText, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertTextFieldOnPage(0, posx, posy, height, width, name,
				initialText, names, values);
	}

	/*!
	 This function will create a password field that will "hide" what is being typed.
	 The resulting "XTextComponent" otherwise works the same as any other XTextComponents, notably
	 the ones defined above with the "insertTextField" functions.
	 
	 @param posx -- the x-coordinate of the top-left of the text field.
	 @param posy -- the y-coordinate of the top-left of the text field.
	 @param width -- the width (length) of the resulting text field.
	 @param name -- a name for this component; the name MUST be unique!
	 @param names -- (optional) the names of any additional properties to set.
	 @param values -- (optional) the values of any additional properties to set.
	 @result -- A reference to an XTextComponent that holds the password.
	 */
	Reference<XTextComponent> insertPasswordFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

	Reference<XTextComponent> insertPasswordField(sal_Int32 posx,
			sal_Int32 posy, sal_Int32 width, OUString name,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertPasswordFieldOnPage(0, posx, posy, width, name, names,
				values);
	}

	/*
	 Reference< XComboBox > insertComboBoxOnPage(sal_Int16 page, sal_Int32 posx, sal_Int32 posy, 
	 sal_Int32 height, sal_Int32 width, OUString name, sal_Bool enable_autocomplete,
	 sal_Bool read_only, sal_Bool dropdown,
	 Sequence< OUString > names = Sequence< OUString >()
	 Sequence< Any > values = Sequence< Any >());
	 */
	/*!
	 This function will insert a scrollbar component. The orientation of the scrollbar is handled by the
	 'isHorizontal' parameter: if true, this creates a horizontal scrollbar; if false, this creates a 
	 vertical scrollbar. As with all components, a unique identifier must be present for each component, 
	 passed into this function in the parameter 'name'. 
	 
	 The scrollbar has various positions and increments of these positions.
	 The minScrollValue is the value of the scrollbar when the bar is at the very 'top' or 'left'
	 depending on the orientation of the scrollbar. Similarly, the maxScrollValue is when the bar
	 is at the very 'bottom' or 'right'. The initialScrollValue indicates where the scrollbar is
	 initially set and MUST be a value between the max- and minScrollValue. The lineIncrement and 
	 blockIncrement parameters specify how much the scrollbar should move when performing various 
	 actions, such as clicking the arrow button to move the scrollbar, or dragging the bar along. 
	 The default thickness of the scrollbar should be the same as a line, which is typically
	 a value of '8'.
	 
	 Finally, an XAdjustmentListener can be added for when the scrollbar changes value.
	 @param posx -- the x-coordinate of the scrollbar
	 @param posy -- the y-coordinate of the scrollbar
	 @param length -- the length of the scrollbar, independent of its orientation.
	 @param name -- a UNIQUE identifier for this component.
	 @param isHorizontal -- if true, a Horizontal scrollbar is created. if false, a vertical one.
	 @param minScrollValue -- the value of the scrollbar in its 'minimum' position.
	 @param maxScrollValue -- the value of the scrollbar in its 'maximum' position.
	 @param initScrollValue -- the initial position of the scrollbar.
	 @param lineIncrement -- the change in value when an arrow is clicked.
	 @param blockIncrement -- the change in value when space between the bar and the arrow is clicked.
	 @param thickness -- the thickness of the scrollbar.
	 @param names -- a Sequence of names of any additional properties to pass in.
	 @param values -- a Sequence of values for the respective names of any properties to pass in.
	 @result -- a reference to the XScrollBar created, giving access to the value of the slider.
	 */
	Reference<XScrollBar> insertScrollBar(
			Reference<XAdjustmentListener> _listener, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 length, OUString name,
			sal_Bool isHorizontal, sal_Int32 minScrollValue,
			sal_Int32 maxScrollValue, sal_Int32 initScrollValue,
			sal_Int32 lineIncrement, sal_Int32 blockIncrement,
			sal_Int32 thickness = 8,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

//	Reference< XScrollBar > insertScrollBar(sal_Int16 page, Reference< XAdjustmentListener > _listener, 
//		sal_Int32 posx, sal_Int32 posy, sal_Int32 length, OUString name, sal_Bool isHorizontal, 
//		sal_Int32 minScrollValue, sal_Int32 maxScrollValue, sal_Int32 initScrollValue, 
//		sal_Int32 lineIncrement, sal_Int32 blockIncrement,sal_Int32 thickness = 8,
//		Sequence< OUString > names = Sequence< OUString >(), Sequence< Any > values = Sequence< Any >());

	// TODO, implement these functions...
	/*!
	 This function will create a line that is either horizontal or vertical and of the given thickness.
	 The position of the line indicates the top-left point on the line.
	 @param posx -- The x-coordinate of the top-left point of the line.
	 @param posy -- The y-coordinate of the top-left point of the line.
	 @param thickness -- the thickness of the line.
	 @param length -- the length of the line
	 @param name -- a UNIQUE identifier of this component.
	 @param isHorizontal 
	 -- If this is true, the line will be aligned horizontally. If this is false, the line will be
	 aligned vertically. Note that the thickness and length parameters are independent of orientation.
	 @param color -- the Color of the line. If not specified, the line is assumed to be black.
	 @return -- an XPropertySet giving access to all properties of the created line.
	 */
	Reference<XPropertySet> insertLine(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 thickness, sal_Int32 length, OUString name,
			sal_Bool isHorizontal, Color color = 0xff000000);

	// TODO -- Finish this.
	// Also, note that this field can be used in conjunction with a spin listener (a field where
	// there are little up-down arrows).
	/*!
	 This function will create a date field that allows for the entry of a date.
	 */
//	Reference< XPropertySet > insertDateField(sal_Int32 posx, sal_Int32 posy, sal_Int32 height,
//		sal_Int32 width, OUString name, sal_Int32 date, sal_Int16 dateFormat, sal_Int32 dateMin, 
//		sal_Int32 dateMax);
//	Reference< XPropertySet > insertTimeField();
	Reference<XNumericField> insertNumericFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, sal_Bool spin_enable, double minVal, double maxVal,
			double initVal, double stepVal, sal_Int16 decimal_count,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

	Reference<XNumericField> insertNumericFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
			sal_Bool spin_enable, double minVal, double maxVal, double initVal,
			double stepVal, sal_Int16 decimal_count, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertNumericFieldOnPage(page, posx, posy, 12, width, name,
				spin_enable, minVal, maxVal, initVal, stepVal, decimal_count,
				names, values);
	}

	Reference<XNumericField> insertNumericField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			sal_Bool spin_enable, double minVal, double maxVal, double initVal,
			double stepVal, sal_Int16 decimal_count, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertNumericFieldOnPage(0, posx, posy, height, width, name,
				spin_enable, minVal, maxVal, initVal, stepVal, decimal_count,
				names, values);
	}

	Reference<XNumericField> insertNumericField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 width, OUString name, sal_Bool spin_enable, double minVal,
			double maxVal, double initVal, double stepVal,
			sal_Int16 decimal_count,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertNumericFieldOnPage(0, posx, posy, 12, width, name,
				spin_enable, minVal, maxVal, initVal, stepVal, decimal_count,
				names, values);
	}

	/*!
	 This function will create a text field that only allows certain patterns for input.
	 The text that is displayed in the text field by default is indicated by literal_mask,
	 while the properties of each character location (i.e, which characters are allowed at
	 that spot) are stored in the edit_mask. 
	 Note also that there is a "strict" parameter.
	 This boolean indicates whether the component should enforce the edit_mask immediately,
	 or after the component loses focus.
	 
	 Visit: http://wiki.services.openoffice.org/wiki/Documentation/DevGuide/GUI/Pattern_Field
	 for the details about the edit mask as well as the available properties.
	 
	 @param page -- the page to display this dialog on.
	 @param posx -- the x-coordinate for this control.
	 @param posy -- the y-coordinate for this control.
	 @param height -- the height for this control. (optional)
	 @param width -- the width for this control.
	 @param name -- a unique name identifying this control.
	 @param literal_mask -- the text to display in the field.
	 @param edit_mask -- the properties for each character location.
	 @param strict -- a boolean indicating when to enforce the edit_mask properties.
	 @param names (optional) -- any additional properties to specify.
	 @param values (optional) -- any additional properties to specify, in correspondence with "names".
	 */
	Reference<XPatternField> insertPatternFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, OUString literal_mask, OUString edit_mask,
			sal_Bool strict, Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

	Reference<XPatternField> insertPatternFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
			OUString literal_mask, OUString edit_mask, sal_Bool strict,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertPatternFieldOnPage(0, posx, posy, 12, width, name,
				literal_mask, edit_mask, strict, names, values);
	}

	Reference<XPatternField> insertPatternField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			OUString literal_mask, OUString edit_mask, sal_Bool strict,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertPatternFieldOnPage(0, posx, posy, height, width, name,
				literal_mask, edit_mask, strict, names, values);
	}

	Reference<XPatternField> insertPatternField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 width, OUString name, OUString literal_mask,
			OUString edit_mask, sal_Bool strict, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertPatternFieldOnPage(0, posx, posy, 12, width, name,
				literal_mask, edit_mask, strict, names, values);
	}

	/*!
	 This function will create a fixed "DateField" that displays the specified date. 
	 This control allows for the date to be changed by the programmer, but NOT the user.
	 
	 This function allows for specifying the usual requirements for a dialog control, such
	 as position, size, and the unique identifier name. The date format must also be specified.
	 The following properties are available for "date_format":
	 0: system short
	 1: system short YY
	 2: system short YYYY
	 3: system long
	 4: short DDMMYY
	 5: short MMDDYY
	 6: short YYMMDD
	 7: short DDMMYYYY
	 8: short MMDDYYYY
	 9: short YYYYMMDD
	 10: short YYMMDD DIN5008
	 11: short YYYYMMDD DIN5008
	 The date must also be passed in based on the format. 
	 
	 Other Notes:
	 The 'height' of this control increases the height, but it does NOT change the font size
	 that is used to display this. Setting the font size requires passing in a customized
	 "FontDescriptor" property in propNames and propVals.
	 
	 @param page -- the page of the dialog to insert the control
	 @param posx -- the x-coordinate of the top-left corner of this control
	 @param posy -- the y-coordinate of the top-left corner of this control
	 @param height -- the height of the control (by default, this should be 12)
	 @param width -- the width of the control
	 @param date_format -- the format with which to display the date.
	 @param date -- the initial date to display.
	 @param names -- the names of additional properties to be set for this control.
	 @param values -- the values of additional properties to be set for this control.
	 @result -- an instance of XDateField that allows for changing the date.
	 
	 @seealso insertFixedTextOnPage()
	 */
	Reference<XDateField> insertFixedDateFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, sal_Int16 date_format, sal_Int32 date,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

	Reference<XDateField> insertFixedDateFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
			sal_Int16 date_format, sal_Int32 date, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertFixedDateFieldOnPage(page, posx, posy, 12, width, name,
				date_format, date, names, values);
	}

	Reference<XDateField> insertFixedDateField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			sal_Int16 date_format, sal_Int32 date, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertFixedDateFieldOnPage(0, posx, posy, height, width, name,
				date_format, date, names, values);
	}

	Reference<XDateField> insertFixedDateField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 width, OUString name, sal_Int16 date_format,
			sal_Int32 date, Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertFixedDateFieldOnPage(0, posx, posy, 12, width, name,
				date_format, date, names, values);
	}

	/*!
	 This function will create a text field to display a time. This function does not let the user 
	 edit this field, only the programmer. For editing the time in the field, use the function
	 insertTimeFieldOnPage() and its derivatives.
	 
	 The format of this time field (as specified by time_format) is as follows:
	 0: 24h short
	 1: 24h long
	 2: 12h short
	 3: 12h long
	 4: Duration short
	 5: Duration long
	 
	 Also, as in most other text fields available in this class, the height is by default 12. If
	 the time_field is made larger, then the font still needs to be changed, which can be done by 
	 explicitly passing in a FontDescriptor Property into the Sequences names and values.
	 
	 @param page -- the Page on the dialog to place this field.
	 @param posx -- the x-coordinate of the top-left coordinate of this control.
	 @param posy -- the y-coordinate of the top-left coordinate of this control.
	 @param height -- the height of this control.
	 @param width -- the width of this control.
	 @param name -- a UNIQUE name identifying this component.
	 @param time_format -- a short indicating how to display the date.
	 @param names -- the names of any additional properties to set.
	 @param values -- the values of any additional properties to set.
	 @result -- an instance of XTimeField that allows for changing the displayed time.
	 
	 @seealso insertFixedDateFieldOnPage();
	 */
	Reference<XTimeField> insertFixedTimeFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, sal_Int16 time_format, sal_Int32 time,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

	Reference<XTimeField> insertFixedTimeFieldOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
			sal_Int16 time_format, sal_Int32 time, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertFixedTimeFieldOnPage(page, posx, posy, 12, width, name,
				time_format, time, names, values);
	}

	Reference<XTimeField> insertFixedTimeField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			sal_Int16 time_format, sal_Int32 time, Sequence<OUString> names =
					Sequence<OUString>(), Sequence<Any> values =
					Sequence<Any>()) {
		return insertFixedTimeFieldOnPage(0, posx, posy, height, width, name,
				time_format, time, names, values);
	}
	Reference<XTimeField> insertFixedTimeField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 width, OUString name, sal_Int16 time_format,
			sal_Int32 time, Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertFixedTimeFieldOnPage(0, posx, posy, 12, width, name,
				time_format, time, names, values);
	}

	//=========================================================
	// Extra Dialog Functions
	//=========================================================
	/*!
	 This function will create a special text field for file-selection. If prompted, a Window will
	 come up that allows the user to select the file as opposed to type in the path. This field 
	 otherwise behaves like any arbitrary text field.
	 
	 @param posx -- the x-coordinate of the top-left corner of the text field.
	 @param posy -- the y-coordinate of the top-left corner of the text field.
	 @param width -- the width of the text field.
	 @param name -- the name of the text field.
	 @param names -- (optional) the names of any additional properties to pass.
	 @param values -- (optional) the values of the additional properties to pass.
	 @return -- a reference to the XTextComponent for the constructed text field.
	 */
	Reference<XTextComponent> insertFileControlOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>());

	Reference<XTextComponent> insertFileControl(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertFileControlOnPage(0, posx, posy, height, width, name,
				names, values);
	}

	Reference<XTextComponent> insertFileControl(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 width, OUString name,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertFileControlOnPage(0, posx, posy, 12, width, name, names,
				values);
	}

	Reference<XTextComponent> insertFileControlOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 width, OUString name,
			Sequence<OUString> names = Sequence<OUString>(),
			Sequence<Any> values = Sequence<Any>()) {
		return insertFileControlOnPage(page, posx, posy, 12, width, name, names,
				values);
	}

	Reference<XProgressBar> insertProgressbarOnPage(sal_Int16 page,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, sal_Int32 begin, sal_Int32 end,
			Sequence<OUString> propNames = Sequence<OUString>(),
			Sequence<Any> propVals = Sequence<Any>());

	//=========================================================
	// MessageBox Functions
	//=========================================================

	void showErrorMessageBox(OUString title, OUString message);

	void showErrorMessageBox(sal_Int32 posx, sal_Int32 posy, sal_Int32 height,
			sal_Int32 width, OUString title, OUString message);

	void showInfoMessageBox(OUString title, OUString message);
	/*
	 TODO -- Here are all of the messagebox types...
	 infobox -- A message box to inform the user about a certain event. Always has only "OK" button.
	 warningbox -- A message to warn the user about a certain problem.
	 errorbox -- A message box to provide an error message to the user.
	 querybox -- A message box to query information from the user.
	 messbox -- A normal message box.

	 TODO -- Here are all of the button combinations supposedly supported by OpenOffice:
	 
	 BUTTONS_OK 	specifies a message with "OK" button.
	 BUTTONS_OK_CANCEL 	specifies a message box with "OK" and "CANCEL" button.  
	 BUTTONS_YES_NO 	specifies a message box with "YES" and "NO" button.  
	 BUTTONS_YES_NO_CANCEL 	specifies a message box with "YES", "NO" and "CANCEL" button.  
	 BUTTONS_RETRY_CANCEL 	specifies a message box with "RETRY" and "CANCEL" button.  
	 BUTTONS_ABORT_IGNORE_RETRY 	specifies a message box with "ABORT", "IGNORE" and "RETRY" button.  
	 DEFAULT_BUTTON_OK 	specifies that OK is the default button.  
	 DEFAULT_BUTTON_CANCEL 	specifies that CANCEL is the default button.  
	 DEFAULT_BUTTON_RETRY 	specifies that RETRY is the default button.  
	 DEFAULT_BUTTON_YES 	specifies that YES is the default button.  
	 DEFAULT_BUTTON_NO 	specifies that NO is the default button.  
	 DEFAULT_BUTTON_IGNORE 	specifies that IGNORE is the default button. 
	 */

	static Reference<XActionListener> getLoadPageListener(BaseDialog* dialog,
			sal_Int16 page);
};

//=========================================================
// Helper Classes
//=========================================================	
class LoadPageListener: public ActionListenerHelper {
protected:
	BaseDialog* dialog;
	sal_Int16 page;
public:
	LoadPageListener(BaseDialog* _dialog, sal_Int16 _page) :
			dialog(_dialog), page(_page) {
	}
	virtual ~LoadPageListener() {
	}

	// XEventListener
	virtual void SAL_CALL disposing(const EventObject& eventOb) throw (RuntimeException) {
	}

	// XActionListener
	virtual void SAL_CALL actionPerformed(const ActionEvent& event)
			throw (RuntimeException) {
		dialog->loadPage(page);
	}
};

}
;
}
;
}
;
// namespace com::haathi::dialog
#endif

