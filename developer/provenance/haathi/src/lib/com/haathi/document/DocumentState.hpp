/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/DocumentState.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__DOCUMENT_STATE_HPP
#	define COM__HAATHI__DOCUMENT__DOCUMENT_STATE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/text/Range.hpp"

#include "sdkBeg.hpp"
#include "sdkEnd.hpp"

#include <algorithm>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Selections
******************************************************************************/
class Selections: public std::vector<com::haathi::text::Range> {
	public:
		void sortAll();
		void add(com::haathi::text::Range range);
};

inline void Selections::sortAll() {
	std::sort(begin(), end());
}

inline void Selections::add(com::haathi::text::Range range) {
	push_back(range);
}
/******************************************************************************
DocumentState
******************************************************************************/
class DocumentState {
	protected:
		Selections selections;
		OUString text;

	public:
		inline int getSelectionSize() {
			return selections.size();
		}

		inline void clearSelections() {
			selections.clear();
		}

		int getSelectionLength();

		inline Selections& getSelections() {
			return selections;
		}

		inline void addSelection(com::haathi::text::Range selection) {
			selections.add(selection);
		}

		inline void setText(OUString text) {
			this->text = text;
		}

		inline OUString getText() {
			return text;
		}

		inline sal_Int32 getTextLength() {
			return text.getLength();
		}

		inline void sortSelections() {
			selections.sortAll();
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
