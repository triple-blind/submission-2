/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/document/DocumentHistory.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__DOCUMENT__DOCUMENT_HISTORY_HPP
#	define COM__HAATHI__DOCUMENT__DOCUMENT_HISTORY_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/DocumentState.hpp"

#include "sdkBeg.hpp"
#include "sdkEnd.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
/******************************************************************************
Class
******************************************************************************/
class DocumentHistory {
	protected:
		com::haathi::document::DocumentState states[2];
		int oldStateIndex;
		int newStateIndex;

		inline OUString getText(int stateIndex) {
			return states[stateIndex].getText();
		}

		inline void setText(OUString text, int stateIndex) {
			states[stateIndex].setText(text);
		}

		inline com::haathi::document::Selections& getSelectionsForUpdate(int stateIndex) {
			states[stateIndex].clearSelections();
			return states[stateIndex].getSelections();
		}

		inline int getSelectionSize(int stateIndex) {
			return states[stateIndex].getSelectionSize();
		}

		inline int getSelectionLength(int stateIndex) {
			return states[stateIndex].getSelectionLength();
		}

	public:
		DocumentHistory();

		inline void advance() {
			oldStateIndex ^= 1;
			newStateIndex ^= 1;
		}

		inline OUString getOldText() {
			return getText(oldStateIndex);
		}

		inline void setOldText(OUString text) {
			setText(text, oldStateIndex);
		}

		inline OUString getNewText() {
			return getText(newStateIndex);
		}

		inline void setNewText(OUString text) {
			setText(text, newStateIndex);
		}

		inline com::haathi::document::Selections& getOldSelectionsForUpdate() {
			return getSelectionsForUpdate(oldStateIndex);
		}

		inline com::haathi::document::Selections& getNewSelectionsForUpdate() {
			return getSelectionsForUpdate(newStateIndex);
		}

		inline int getOldSelectionSize() {
			return getSelectionSize(oldStateIndex);
		}

		inline int getNewSelectionSize() {
			return getSelectionSize(newStateIndex);
		}

		inline int getOldSelectionLength() {
			return getSelectionLength(oldStateIndex);
		}

		inline int getNewSelectionLength() {
			return getSelectionLength(newStateIndex);
		}

		inline com::haathi::document::Selections& getOldSelections() {
			states[oldStateIndex].sortSelections();
			return states[oldStateIndex].getSelections();
		}

		inline sal_Int32 getOldLeft() {
			return states[oldStateIndex].getSelections()[0].getLeft();
		}

		inline sal_Int32 getNewLeft() {
			return states[newStateIndex].getSelections()[0].getLeft();
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
