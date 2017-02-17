/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/SmartCursor.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__SMART_CURSOR_HPP
#	define COM__HAATHI__DOCUMENT__SMART_CURSOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/text/Range.hpp"
#include "com/haathi/utils/Bool.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/text/XTextViewCursor.hpp>
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
SmartCursorInterface
******************************************************************************/
class SmartCursor {
	// These methods need to be concrete in order to pass as variable without
	// pointer or reference
	public:
		virtual ~SmartCursor() {
		}

		virtual sal_Bool gotoRange(com::haathi::text::Range range) = 0;
		virtual Reference<com::sun::star::beans::XPropertySet> getXPropertySet() = 0;
		
		virtual Reference<com::sun::star::text::XTextCursor> asXTextCursor() {
			return Reference<com::sun::star::text::XTextCursor>();
		}

		virtual Reference<com::sun::star::text::XTextViewCursor> asXTextViewCursor() {
			return Reference<com::sun::star::text::XTextViewCursor>();
		}

		virtual Reference<com::sun::star::text::XTextRange> asXTextRange() = 0;

		virtual void gotoStart(sal_Bool bExpand) = 0;
		virtual void gotoEnd(sal_Bool bExpand) = 0;
		virtual Reference<com::sun::star::text::XTextRange> getEnd() = 0;
		virtual sal_Bool selectPos(sal_Int32 pos) = 0;
		virtual sal_Bool goRight(sal_Int32 nCount, sal_Bool bExpand) = 0;
		virtual sal_Bool goLeft(sal_Int32 nCount, sal_Bool bExpand) = 0;
		virtual void collapseToStart() = 0;
		virtual void collapseToEnd() = 0;
		virtual sal_Unicode getCurrentCharacter() = 0;
		virtual OUString getCurrentText() = 0;
		virtual void setCurrentText(OUString text) = 0;
		virtual void gotoXTextRange(Reference<com::sun::star::text::XTextRange> xTextRange) = 0;
		virtual sal_Int32 getLeft() = 0;
		virtual sal_Int32 getRight() = 0;
		virtual sal_Int32 getWidth() = 0;

		virtual void setVisible(sal_Bool visible) {
		}
};
/******************************************************************************
SmartCursorTemplate
******************************************************************************/
template<class T>
class SmartCursorTemplate: public SmartCursor {
	protected:
		Reference<T> xCursor;
		com::haathi::text::Range range;

		virtual void makeRangeValid() {
			// Could do different things based on left or right being valid
			// and where start and end are in relation
			xCursor->gotoStart(sal_False);
			range.setPos(0);
		}

	public:
		SmartCursorTemplate<T>(Reference<T> xCursor) {
			this->xCursor = xCursor;
		}

		virtual sal_Bool gotoRange(com::haathi::text::Range range) {
			if (this->range.isInvalid())
				makeRangeValid();
			collapseToStart();
			sal_Int32 right = range.getLeft() - this->getLeft();
			if (isFalse(goRight(right, sal_False)))
				return sal_False;
			if (range.getWidth() != 0)
				return goRight(range.getWidth(), sal_True);
			return sal_True;
		}

		virtual sal_Bool selectPos(sal_Int32 pos) {
			return gotoRange(com::haathi::text::Range(pos, pos + 1));
		}

		virtual Reference<com::sun::star::beans::XPropertySet> getXPropertySet() {
			Reference<com::sun::star::beans::XPropertySet> xPropertySet(xCursor, com::sun::star::uno::UNO_QUERY);

			return xPropertySet;
		}

		virtual void gotoStart(sal_Bool bExpand) {
			xCursor->gotoStart(bExpand);
			if (!bExpand)
				range.setPos(0);
			else
				range.setRight(0);
		}

		virtual void gotoEnd(sal_Bool bExpand) {
			xCursor->gotoEnd(bExpand);
			if (!bExpand)
				range.setPos(-1);
			else
				range.setRight(-1);
		}

		virtual Reference<com::sun::star::text::XTextRange> getEnd() {
			return xCursor->getEnd();
		}

		virtual sal_Bool goLeft(sal_Int32 nCount, sal_Bool bExpand) {
			if (nCount < 0)
				return goRight(-nCount, bExpand);

			const sal_Int16 maxJump = 32767;

			while (nCount > 0) {
				sal_Int16 jump = nCount > maxJump ? maxJump : (sal_Int16) nCount;

				if (isFalse(xCursor->goLeft(jump, bExpand))) {
					if (!bExpand)
						range.invalidate();
					else
						range.invalidateRight();
					return sal_False;
				}
				if (!bExpand)
					if (range.isValidLeft())
						range.setPos(range.getLeft() - jump);
					else
						;
				else
					if (range.isValidLeft())
						range.setLeft(range.getLeft() - jump);
					else
						;
				nCount -= jump;
			}
			return sal_True;
		}

		virtual sal_Bool goRight(sal_Int32 nCount, sal_Bool bExpand) {
			if (nCount < 0)
				return goLeft(-nCount, bExpand);

			const sal_Int16 maxJump = 32767;

			while (nCount > 0) {
				sal_Int16 jump = nCount > maxJump ? maxJump : (sal_Int16) nCount;

				if (isFalse(xCursor->goRight(jump, bExpand))) {
					if (!bExpand)
						range.invalidate();
					else
						range.invalidateRight();
					return sal_False;
				}
				if (!bExpand)
					if (range.isValidLeft())
						range.setPos(range.getLeft() + jump);
					else
						;
				else
					if (range.isValidRight())
						range.setRight(range.getRight() + jump);
					else
						;
				nCount -= jump;
			}
			return sal_True;
		}

		virtual void collapseToStart() {
			xCursor->collapseToStart();
			range.setRight(range.getLeft());
		}

		virtual void collapseToEnd() {
			xCursor->collapseToEnd();
			range.setLeft(range.getRight());
		}

		virtual sal_Unicode getCurrentCharacter() {
			return xCursor->getString()[0];
		}

		virtual OUString getCurrentText() {
			return xCursor->getString();
		}

		virtual void setCurrentText(OUString text) {
			sal_Int32 oldWidth = range.getWidth();

			xCursor->setString(text);
			range.expand(text.getLength() - oldWidth);
		}

		virtual Reference<com::sun::star::text::XTextRange> asXTextRange() {
			Reference<com::sun::star::text::XTextRange> xTextRange(xCursor, com::sun::star::uno::UNO_QUERY);

			return xTextRange;
		};

		virtual sal_Int32 getLeft() {
			return range.getLeft();
		}

		virtual sal_Int32 getRight() {
			return range.getRight();
		}

		virtual sal_Int32 getWidth() {
			return range.getWidth();
		}
};
/******************************************************************************
SmartTextCursor
******************************************************************************/
class SmartTextCursor: public SmartCursorTemplate<com::sun::star::text::XTextCursor> {
	public:
		SmartTextCursor(Reference<com::sun::star::text::XTextCursor> xTextCursor):
				SmartCursorTemplate<com::sun::star::text::XTextCursor>(xTextCursor) {
		}

		SmartTextCursor(Reference<com::sun::star::text::XTextViewCursor> xTextViewCursor):
		SmartCursorTemplate<com::sun::star::text::XTextCursor>(Reference<com::sun::star::text::XTextCursor>()) {
				//xTextViewCursor->getText()->createTextCursor()) {
			// This does not work for some reason!  Avoid this constructor.
			xCursor = xTextViewCursor->getText()->createTextCursor();
		}

		Reference<com::sun::star::text::XTextCursor> asXTextCursor() {
			return xCursor;
		}

		void gotoXTextRange(Reference<com::sun::star::text::XTextRange> xTextRange) {
		}
};
/******************************************************************************
SmartTextViewCursor
******************************************************************************/
class SmartTextViewCursor: public SmartCursorTemplate<com::sun::star::text::XTextViewCursor> {
	public:
		SmartTextViewCursor(Reference<com::sun::star::text::XTextViewCursor> xTextViewCursor):
				SmartCursorTemplate<com::sun::star::text::XTextViewCursor>(xTextViewCursor) {
		}

		Reference<com::sun::star::text::XTextCursor> asXTextCursor() {
			Reference<com::sun::star::text::XTextCursor> xTextCursor(xCursor, com::sun::star::uno::UNO_QUERY);

			return xTextCursor;
		}

		Reference<com::sun::star::text::XTextViewCursor> asXTextViewCursor() {
			return xCursor;
		}

		void setVisible(sal_Bool visible) {
			xCursor->setVisible(visible);
		}

		void gotoXTextRange(Reference<com::sun::star::text::XTextRange> xTextRange);
};
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
