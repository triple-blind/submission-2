/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/text/TextHelper.cpp#1 $

/*
Breaks as found when getString is called directly on xText have been found to be these:

       Paragraph  Line
Win    \r\n       \n
Mac    \n         \n
Linux  \n         \n

Therefore it seems that only the paragraph markers need to be corrected and all changed to \r.
*/
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/text/TextHelper.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/Bool.hpp"
#include "com/haathi/utils/Timer.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/text/XParagraphCursor.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/text/XTextRangeCompare.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <rtl/ustrbuf.hxx>
#include "sdkEnd.hpp"
/******************************************************************************
Using
******************************************************************************/
using com::haathi::text::Range;
using com::haathi::text::Selection;
using com::haathi::utils::TimeKeeper;
using com::haathi::utils::textHelperTimer;
using com::haathi::utils::Timer;

using com::sun::star::container::XIndexAccess;
using com::sun::star::frame::XModel;
using com::sun::star::frame::XController;
using com::sun::star::text::XParagraphCursor;
using com::sun::star::text::XTextDocument;
using com::sun::star::text::XText;
using com::sun::star::text::XTextCursor;
using com::sun::star::text::XTextRange;
using com::sun::star::text::XTextRangeCompare;
using com::sun::star::uno::Any;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::lang::XComponent;
using com::sun::star::view::XSelectionSupplier;
using rtl::OUStringBuffer;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace text {
/******************************************************************************
Class
******************************************************************************/
TextHelper::TextHelper(Reference<com::sun::star::lang::XComponent> xComponent) {
	Reference<XTextDocument> xTextDocument(xComponent, UNO_QUERY);
	xText = xTextDocument->getText();

	Reference<XModel> xModel(xComponent, UNO_QUERY);
	Reference<XController> xController = xModel->getCurrentController();
	xSelectionSupplier = Reference<XSelectionSupplier>(xController, UNO_QUERY);
}

sal_Int32 TextHelper::getWidth(Reference<XTextRange> xTextRange) {
	TimeKeeper timeKeeper(textHelperTimer);

	Reference<XTextRangeCompare> xTextRangeCompare(xText, UNO_QUERY);
	Reference<XTextCursor> xTextCursor = xText->createTextCursorByRange(xTextRange);
	Reference<XParagraphCursor> xParagraphCursor(xTextCursor, UNO_QUERY);

	xParagraphCursor->collapseToStart();
	xParagraphCursor->gotoEndOfParagraph(sal_True);
	if (xTextRangeCompare->compareRegionEnds(Reference<XTextRange>(xParagraphCursor, UNO_QUERY), xTextRange) <= 0)
		return xTextRange->getString().getLength();

	sal_Int32 width = xParagraphCursor->getString().getLength() + 1;

	while (true) {
		xParagraphCursor->gotoNextParagraph(sal_False);
		if (xTextRangeCompare->compareRegionEnds(Reference<XTextRange>(xParagraphCursor, UNO_QUERY), xTextRange) <= 0)
			return width; // Quit after skipping over cr
		xParagraphCursor->gotoEndOfParagraph(sal_True);
		if (xTextRangeCompare->compareRegionEnds(Reference<XTextRange>(xParagraphCursor, UNO_QUERY), xTextRange) <= 0)
			break;
		width += xParagraphCursor->getString().getLength() + 1;
	}
	xParagraphCursor->gotoRange(xTextRange, sal_False);
	xParagraphCursor->collapseToEnd();
	xParagraphCursor->gotoStartOfParagraph(sal_True);
	width += xParagraphCursor->getString().getLength();
	return width;
}

// This is really getStartOffset
sal_Int32 TextHelper::getOffset(Reference<XTextRange> xTextRange) {
	// TODO need to make sure range is right way around if want _left_ offset
	Reference<XTextCursor> xTextCursor = xText->createTextCursorByRange(xTextRange);

	xTextCursor->collapseToStart();
	xTextCursor->gotoStart(sal_True);
	return getWidth(Reference<XTextRange>(xTextCursor, UNO_QUERY));
}

sal_Int32 TextHelper::getEndOffset(Reference<XTextRange> xTextRange) {
	Reference<XTextCursor> xTextCursor = xText->createTextCursorByRange(xTextRange);

	xTextCursor->collapseToEnd();
	xTextCursor->gotoStart(sal_True);
	return getWidth(Reference<XTextRange>(xTextCursor, UNO_QUERY));
}

OUString TextHelper::getString() {
	TimeKeeper timeKeeper(textHelperTimer);

	// Use previous cursor and maybe just set range to current one
	Reference<XTextCursor> xTextCursor = xText->createTextCursor();
	xTextCursor->collapseToStart();
	Reference<XParagraphCursor> xParagraphCursor(xTextCursor, UNO_QUERY);
	OUStringBuffer buffer;
	bool isMore;

	do { // The document must have have at least one paragraph by design.
		xParagraphCursor->gotoEndOfParagraph(sal_True);
		buffer.append(xParagraphCursor->getString());
		isMore = isTrue(xParagraphCursor->gotoNextParagraph(sal_False));
		if (isMore)
			buffer.append((sal_Unicode) '\r');  // Close the paragraph with \r.
	} while (isMore);
	OUString string = buffer.makeStringAndClear();

	return string;
}

OUString TextHelper::getString(Reference<XTextRange> xTextRange) {
	return getStringFromTextRange(xText, xTextRange);
}

OUString TextHelper::getStringFromTextCursor(Reference<XTextCursor> xTextCursor) {
	Reference<XTextRange> xTextRange(xTextCursor, UNO_QUERY);

	return getStringFromTextRange(xTextCursor->getText(), xTextRange);
}

OUString TextHelper::getStringFromTextRange(Reference<XText> xText, Reference<XTextRange> xTextRange) {
	TimeKeeper timeKeeper(textHelperTimer);

	Reference<XTextCursor> xTextCursor = xText->createTextCursorByRange(xTextRange);
	Reference<XParagraphCursor> xParagraphCursor(xTextCursor, UNO_QUERY);
	Reference<XTextRangeCompare> xTextRangeCompare(xText, UNO_QUERY);

	xParagraphCursor->collapseToStart();
	xParagraphCursor->gotoEndOfParagraph(sal_True);
	if (xTextRangeCompare->compareRegionEnds(Reference<XTextRange>(xParagraphCursor, UNO_QUERY), xTextRange) <= 0)
		return xTextRange->getString(); // Selection was smaller than a paragraph

	OUStringBuffer buffer;
	buffer.append(xParagraphCursor->getString());
	buffer.append((sal_Unicode) '\r');

	while (true) {
		xParagraphCursor->gotoNextParagraph(sal_False);
		if (xTextRangeCompare->compareRegionEnds(Reference<XTextRange>(xParagraphCursor, UNO_QUERY), xTextRange) <= 0)
			return buffer.makeStringAndClear(); // Quit after skipping over cr
		xParagraphCursor->gotoEndOfParagraph(sal_True);
		if (xTextRangeCompare->compareRegionEnds(Reference<XTextRange>(xParagraphCursor, UNO_QUERY), xTextRange) <= 0)
			break;
		buffer.append(xParagraphCursor->getString()); // Add complete paragraph.
		buffer.append((sal_Unicode) '\r');
	}
	xParagraphCursor->gotoRange(xTextRange, sal_False);
	xParagraphCursor->collapseToEnd();
	xParagraphCursor->gotoStartOfParagraph(sal_True);
	buffer.append(xParagraphCursor->getString()); // Add final, partial paragraph.
	return buffer.makeStringAndClear();
}

std::vector<Selection> TextHelper::getSelections() {
	std::vector<Selection> selections;
	Any anySelection = xSelectionSupplier->getSelection();

	Reference<XIndexAccess> xIndexAccess;
	anySelection >>= xIndexAccess;
	sal_Int32 count = xIndexAccess->getCount();

	for (int i = 0; i < count; i++) {
		Any anyIndex = xIndexAccess->getByIndex(i);
		Reference<XTextRange> xTextRange;
		anyIndex >>= xTextRange;

		sal_Int32 offset = getOffset(xTextRange);
		OUString text = getStringFromTextRange(xText, xTextRange);
		selections.push_back(Selection(offset, text));
	}
	return selections;
}

sal_Int32 TextHelper::getCursorPosition() {
	Any anySelection = xSelectionSupplier->getSelection();

	Reference<XIndexAccess> xIndexAccess;
	anySelection >>= xIndexAccess;

	sal_Int32 count = xIndexAccess->getCount();

	Any anyIndex = xIndexAccess->getByIndex(count - 1);
	Reference<XTextRange> xTextRange;
	anyIndex >>= xTextRange;

	sal_Int32 offset = getEndOffset(xTextRange);
	return offset;
}

void TextHelper::fillRanges(std::vector<Range>& ranges) {
	Any anySelection = xSelectionSupplier->getSelection();
	Reference<XIndexAccess> xIndexAccess;
	anySelection >>= xIndexAccess;

	// Sometimes, when the window is closing, there are no more selections.
	if (xIndexAccess.is() == sal_False)
		return;

	sal_Int32 count = xIndexAccess->getCount();

	ranges.clear();
	for (int i = 0; i < count; i++) {
		Any anyIndex = xIndexAccess->getByIndex(i);
		Reference<XTextRange> xTextRange;
		anyIndex >>= xTextRange;

		sal_Int32 offset = getOffset(xTextRange->getStart());
		sal_Int32 width = getWidth(xTextRange->getEnd());

		ranges.push_back(Range(offset, offset + width));
	}
}

void TextHelper::setString(const OUString& ouString) {
	xText->setString(ouString);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
