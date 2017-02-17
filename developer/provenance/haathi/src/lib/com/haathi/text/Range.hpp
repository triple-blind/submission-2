/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/text/Range.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TEXT__RANGE_HPP
#	define COM__HAATHI__TEXT__RANGE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "sdkBeg.hpp"
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace text {
/******************************************************************************
Class
******************************************************************************/
class Range {
	protected:
		sal_Int32 left;
		sal_Int32 right;

	public:
		static Range invalidRange;

		Range();
		Range(sal_Int32 pos);
		Range(sal_Int32 left, sal_Int32 right);

		bool operator<(const Range& other) const;

		inline void setPos(sal_Int32 pos) {
			left = right = pos;
		}

		inline sal_Int32 getLeft() {
			return left;
		}

		inline void setLeft(sal_Int32 left) {
			this->left = left;
		}

		inline sal_Int32 getRight() {
			return right;
		}

		inline void setRight(sal_Int32 right) {
			this->right = right;
		}

		// This implies [left, right)
		inline sal_Int32 getWidth() {
			return right - left;
		}

		inline bool isValid() {
			return isValidLeft() && isValidRight() && isValidPos();
		}

		inline bool isValidLeft() {
			return 0 <= left;
		}

		inline bool isValidRight() {
			return 0 <= right;
		}

		// If both sides are included, then right could be less than left
		// to indicate an empty range.
		inline bool isValidPos() {
			return left <= right;
		}

		inline bool isInvalid() {
			return !isValid();
		}

		inline void invalidate() {
			left = right = -1;
		}

		inline void invalidateLeft() {
			left = -1;
		}

		inline void invalidateRight() {
			right = -1;
		}

		inline void shift(sal_Int32 offset) {
			left += offset;
			right += offset;
		}

		inline void expand(sal_Int32 amount) {
			right += amount;
		}
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
