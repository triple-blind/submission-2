/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/text/Range.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/text/Range.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace text {
/******************************************************************************
Class
******************************************************************************/
Range Range::invalidRange;

Range::Range() {
	left = right = -1;
}

Range::Range(sal_Int32 pos) {
	this->left = pos;
	this->right = pos;
}

Range::Range(sal_Int32 left, sal_Int32 right) {
	if (left <= right) {
		this->left = left;
		this->right = right;
	}
	else {
		this->left = right;
		this->right = left;
	}
}

bool Range::operator<(const Range& other) const {
	return left < other.left || (left == other.left && right < other.right);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
