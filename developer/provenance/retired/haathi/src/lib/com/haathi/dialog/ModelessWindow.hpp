/*!
 @file BaseWindow.hpp
 
 @author Aaron Gibson
 @date September 14, 2012
 @brief This header implements a base class for modeless windows in UNO.
 
 This header defines the classes necessary to implement a modeless dialog in UNO. The original
 "dialog" class, com::haathi::dialog::BaseDialog, is outdated because it is modal. There was a 
 hack that made the dialog appear modeless by creating an invisible window as a parent, but this
 ultimately is not ideal, and it is very buggy. 
 
 The classes defined here are "authentic" in the sense that they are modeless windows in the same
 manner as the OpenOffice/LibreOffice document windows themselves. The controls, like buttons, 
 labels, etc. are still implemented with the familiar UnoDialogControlModel services as documented
 in the UNO API; however, the modeless nature of this dialog requires an approach that could not 
 simply be resolved in the BaseDialog class, and so it was reimplemented here.
 */

// In com::sun::star::awt
#include <com/sun/star/awt/XActionListener.hpp>
#include <com/sun/star/awt/XAdjustmentListener.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XDateField.hpp>
#include <com/sun/star/awt/XDialog.hpp>
#include <com/sun/star/awt/ScrollBarOrientation.hpp>
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
// components
#include <com/sun/star/awt/XButton.hpp>
#include <com/sun/star/awt/XCheckBox.hpp>
#include <com/sun/star/awt/XComboBox.hpp>
#include <com/sun/star/awt/XListBox.hpp>
#include <com/sun/star/awt/XNumericField.hpp>
#include <com/sun/star/awt/XPatternField.hpp>
#include <com/sun/star/awt/XProgressBar.hpp>
#include <com/sun/star/awt/XScrollBar.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
#include <com/sun/star/awt/XTimeField.hpp>

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

using namespace rtl;
using namespace com::sun::star::awt;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;
using namespace com::sun::star::graphic;
using namespace com::sun::star::lang;
using namespace com::sun::star::util;
using namespace com::sun::star::uno;

namespace com {
namespace haathi {
namespace window {

class ModelessWindow {
private:
	// I don't feel like implementing these at the moment.
	ModelessWindow(const ModelessWindow& other);
	ModelessWindow& operator=(const ModelessWindow& other);

	/*!
	 * This is a helper function that adds the component to the window.
	 */
	void linkAndAddToWindow(Reference<XInterface> model,
			Reference<XInterface> control);

protected:
	Reference<XMultiComponentFactory> xMCF;
	Reference<XComponentContext> xContext;

	Reference<XWindow> xWindow;
	Reference<XFrame> xFrame;
	Reference<XWindowPeer> xWindowPeer;
	Reference<XToolkit> xToolkit;

public:
	// Constructors and the like
	ModelessWindow(Reference<XMultiComponentFactory> serviceFactory,
			Reference<XComponentContext> context);
	ModelessWindow(Reference<XMultiComponentFactory> serviceFactory,
			Reference<XComponentContext> context, WindowDescriptor descriptor);
	virtual ~ModelessWindow();

	/*!
	 * This function will set this window's visibility.
	 * @param visible -- If true, the window will be set visible;
	 * 					if false, the window will be hidden.
	 */
	void setVisible(sal_Bool visible);

	/*!
	 * This function will return whether this window is currently visible or not.
	 * @return -- true if this window is visible; false otherwise.
	 */
	sal_Bool isVisible() const;

	void redraw();

	// Member functions
	/*!
	 * insertImage Function
	 * 
	 * This function will insert an image into this instance of ModelessWindow, using the
	 * internal Toolkit, Window, and Frame.
	 * 
	 * This function requires the coordinates (of the top-left corner) and the size of this
	 * component to add it. It will adjust the image accordingly, unless otherwise specified
	 * otherwise with additional properties. Note that the coordinates and sizes are all 
	 * relative to this instance of the "ModelessWindow".
	 * 
	 * This image component can be made to behave like a button, though that is not covered 
	 * here. The key-value pairs, specified in "additionalPropNames" and "additionalPropValues"
	 * respectively, are always added last, meaning they will always overwrite the default
	 * behaviour.
	 * 
	 * @param[in] posx -- the x-coordinate to place the top-left corner of this image.
	 * @param[in] posy -- the y-coordinate to place the top-left corner of this image.
	 * @param[in] height -- the height of this image component.
	 * @param[in] width -- the width of this image component.
	 * @param[in] name -- a UNIQUE name to identify this component.
	 * @param[in] graphic -- the image to load in the component
	 * @param[optional] additionalPropNames -- the names of any custom properties.
	 * @param[optional] additionalPropValues -- the values of any custom properties;
	 * 				must be specified in the same order as "additionalPropNames".
	 * @result -- A reference to the created Image component's "XPropertySet".
	 */
	Reference<XPropertySet> insertImage(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, rtl::OUString name,
			Reference<XGraphic> graphic,
			Sequence<rtl::OUString> additionalPropNames =
					Sequence<rtl::OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	/*!
	 * insertCheckbox() Function
	 * 
	 * This function will insert a checkbox to this instance of ModelessWindow.
	 * 
	 * As usual, the coordinates must be specified, as well as the size, though the size
	 * should generally have a height and width of "12".
	 * 
	 * If "enable_tristate" is true, then TODO
	 * 
	 * @param[in] posx -- the x-coordinate to place the checkbox.
	 * @param[in] posy -- the y-coordinate to place the checkbox.
	 * @param[in] height -- the height of the checkbox, usually 12.
	 * @param[in] width -- the width of the checkbox, usually 12.
	 * @param[in] name -- a UNIQUE name to identify this checkbox.
	 * @param[in] enable_tristate -- TODO
	 * @param[optional] additionalPropNames -- the names of any custom properties.
	 * @param[optional] additionalPropValues -- the values of any custom properties;
	 * 				must be specified in the same order as "additionalPropNames".
	 * @result -- a reference to the created checkbox.
	 */
	Reference<XCheckBox> insertCheckbox(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name, OUString label,
			sal_Bool enable_tristate, Sequence<OUString> additionalPropNames =
					Sequence<OUString>(), Sequence<Any> additionalPropValues =
					Sequence<Any>());

	/*!
	 * insertButton() Function
	 * 
	 * This function will add a button to this instance of ModelessWindow at the 
	 * specified location. This function also allows for adding "ActionListeners"
	 * to the button that are called anytime the button is pressed. The returned 
	 * reference of "XButton" allows for adding listeners after creation as well.
	 * 
	 * The full list of optional properties (and their expected types) are listed below:
	 * (short) Align,
	 * (com::sun::star::util::Color) BackgroundColor, 
	 * (bool) DefaultButton, 
	 * (bool) Enabled,
	 * (bool) FocusOnClick,
	 * (FontDescriptor) FontDescriptor,
	 * (short) FontEmphasisMark,
	 * (short) FontRelief,
	 * (OUString) HelpText,
	 * (OUString) HelpURL, 
	 * (short) ImageAlign,
	 * (short) ImagePosition,
	 * (OUString) imageURL,
	 * (com::sun::star::graphic::XGraphic) Graphic, 
	 * (OUString) label, 
	 * (bool) MultiLine,
	 * (bool) Printable,
	 * (short) PushButtonType,
	 * (bool) Repeat,
	 * (long) RepeatDelay,
	 * (short) State,
	 * (bool) Tabstop,
	 * (com::sun::star::util::Color) TextColor,
	 * (com::sun::star::util::Color) TextLineColor,
	 * (bool) Toggle,
	 * (com::sun::star::style::VerticalAlignment) VerticalAlign
	 * 
	 * @param[in] listener -- the listener to call when the button is clicked.
	 * @param[in] posx -- the x-coordinate of the button.
	 * @param[in] posy -- the y-coordinate of the button.
	 * @param[in] height -- the height of the button; standard size is 15.
	 * @param[in] width -- the width of the button.
	 * @param[in] name -- a UNIQUE name to identify this button.
	 * @param[in] label -- the text to display in the button.
	 * @param[optional] additionalPropNames -- the names of any custom properties.
	 * @param[optional] additionalPropValues -- the values of any custom properties;
	 * 				must be specified in the same order as "additionalPropNames".
	 * @result -- a reference to the created button.
	 */
	Reference<XButton> insertButton(Reference<XActionListener> listener,
			sal_Int32 posx, sal_Int32 posy, sal_Int32 height, sal_Int32 width,
			OUString name, OUString label,
			Sequence<OUString> additionalPropNames = Sequence<OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	/*!
	 * insertListBox() Function
	 * 
	 * This function will insert a ListBox (a table from which to select values) to 
	 * this instance of "ModelessWindow". The created ListBox allows for a table of
	 * one-lined entries, as well as multiple selection, but does not (yet?) allow for
	 * more complicated entries with images, or customized text.
	 * 
	 * The listbox will initially display the items in the list "list_items", but the
	 * entries can be modified later with the returned reference. The returned reference
	 * also allows for getting the selected items (if no items are selected, then it will
	 * return an index of -1).
	 * 
	 * @param[in] posx -- the x-coordinate of the listbox.
	 * @param[in] posy -- the y-coordinate of the listbox.
	 * @param[in] height -- the height of the listbox. If too small, this will allow for scrolling.
	 * @param[in] width -- the width of the listbox. If too small, this will allow for scrolling.
	 * @param[in] name -- a UNIQUE name to identify this listbox.
	 * @param[in] list_items -- a list of entries to display in the listbox.
	 * @param[in] multi_select -- If true, the listbox will allow for selecting multiple items.
	 * @param[optional] additionalPropNames -- the names of any additional properties to specify.
	 * @param[optional] additionalPropValues -- the values of the additional properties in the same
	 * 		order as "additionalPropNames".
	 * @return -- A reference to the listbox, which has functions to get and set items and selections.
	 */
	Reference<XListBox> insertListBox(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<OUString> list_items, sal_Bool multi_select,
			Sequence<OUString> additionalPropNames = Sequence<rtl::OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	Reference<XComboBox> insertComboBox(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<rtl::OUString> item_list,
			Sequence<rtl::OUString> additionalPropNames =
					Sequence<rtl::OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	Reference<XPropertySet> insertTextLabel(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name, OUString label,
			sal_Int16 alignment, Sequence<OUString> additionalPropNames =
					Sequence<OUString>(), Sequence<Any> additionalPropValues =
					Sequence<Any>());

	Reference<XTextComponent> insertTextField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			OUString initialText, Sequence<OUString> additionalPropNames =
					Sequence<OUString>(), Sequence<Any> additionalPropValues =
					Sequence<Any>());

	Reference<XTextComponent> insertPasswordField(sal_Int32 posx,
			sal_Int32 posy, sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<OUString> additionalPropNames = Sequence<OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	Reference<XScrollBar> insertScrollBar(
			Reference<XAdjustmentListener> listener, sal_Int32 posx,
			sal_Int32 posy, sal_Int32 length, OUString name,
			sal_Bool isHorizontal, sal_Int32 minScrollValue,
			sal_Int32 maxScrollValue, sal_Int32 initScrollValue,
			sal_Int32 lineIncrement, sal_Int32 blockIncrement,
			sal_Int32 thickness /* = 8*/,
			Sequence<OUString> additionalPropNames = Sequence<OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	Reference<XNumericField> insertNumericField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			sal_Bool spin_enable, double minVal, double maxVal, double initVal,
			double stepVal, sal_Int16 decimal_count,
			Sequence<OUString> additionalPropNames = Sequence<OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	Reference<XPatternField> insertPatternField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			OUString literal_mask, OUString edit_mask, sal_Bool strict,
			Sequence<OUString> additionalPropNames = Sequence<OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	Reference<XDateField> insertFixedDateField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			sal_Int16 date_format, sal_Int32 date,
			Sequence<OUString> additionalPropNames = Sequence<OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	Reference<XTimeField> insertFixedTimeField(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			sal_Int16 time_formate, sal_Int32 time,
			Sequence<OUString> additionalPropNames = Sequence<OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	Reference<XTextComponent> insertFileControl(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name,
			Sequence<OUString> additionalPropNames = Sequence<OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

	Reference<XProgressBar> insertProgressBar(sal_Int32 posx, sal_Int32 posy,
			sal_Int32 height, sal_Int32 width, OUString name, sal_Int32 begin,
			sal_Int32 end,
			Sequence<OUString> additionalPropNames = Sequence<OUString>(),
			Sequence<Any> additionalPropValues = Sequence<Any>());

};
// class ModelessWindow

}
;
}
;
}
;
// namespace com::haathi::window

