/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/utils/Watermarks.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/utils/Watermarks.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <algorithm>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::text::Range;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace utils {
/******************************************************************************
Class
******************************************************************************/
const int Watermarks::UNWATERMARKED = -1;

void Watermarks::resize(int size) {
	std::vector<int>::resize(size);
	std::fill(begin(), end(), UNWATERMARKED);
}

int Watermarks::safeAt(int pos) {
	return 0 <= pos && (unsigned) pos < size() ?  at(pos) : UNWATERMARKED;
}

void Watermarks::insertText(Range range) {
	insert(begin() + range.getLeft(), range.getWidth(), UNWATERMARKED);
}

void Watermarks::deleteText(Range range) {
	erase(begin() + range.getLeft(), begin() + range.getRight());
}

void Watermarks::replaceText(Range deleteRange, Range insertRange) {
	// assert(deleteRange.getLeft() == insertRange.getLeft());
	
	erase(begin() + deleteRange.getLeft(), begin() + deleteRange.getRight());
	insert(begin() + insertRange.getLeft(), insertRange.getWidth(), UNWATERMARKED);
}				
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
