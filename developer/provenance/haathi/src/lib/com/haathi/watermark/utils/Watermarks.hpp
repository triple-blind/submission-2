/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/utils/Watermarks.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__WATERMARKS_HPP
#	define COM__HAATHI__WATERMARK__WATERMARKS_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/IDocumentStateTracker.hpp"

#include <vector>
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
class Watermarks: public std::vector<int>, public com::haathi::document::IDocumentStateTracker {
	public:
		static const int UNWATERMARKED;

		virtual ~Watermarks() { };

		void resize(int size);
		int safeAt(int pos);

		void insertText(com::haathi::text::Range range);
		void deleteText(com::haathi::text::Range range);
		void replaceText(com::haathi::text::Range deleteRange,
				com::haathi::text::Range insertRange);
};
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
