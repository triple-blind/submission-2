/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/NowhereEngine.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__ENGINE__NOWHERE_ENGINE_HPP
#	define COM__HAATHI__WATERMARK__ENGINE__NOWHERE_ENGINE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/IDocumentStateTracker.hpp"
#include "com/haathi/watermark/engine/IWatermarkEngine.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace engine {
/******************************************************************************
Class
******************************************************************************/
class NowhereEngine: public IWatermarkEngine, public com::haathi::document::IDocumentStateTracker {
	public:
		NowhereEngine();
		~NowhereEngine();

		void reset(std::string versionId) { };
		void watermark(com::haathi::text::Range range, rtl::OUString documentState) { };
		void rewatermark(sal_Int32 length) { };
		void verifyWatermark(sal_Int32 length) { };
		void unwatermark(sal_Int32 length) { };
		com::haathi::document::IDocumentStateTracker* getDocumentStateTracker() { return this; };

		void insertText(com::haathi::text::Range range) { };
		void deleteText(com::haathi::text::Range range) { };
		void replaceText(com::haathi::text::Range deleteRange, com::haathi::text::Range insertRange) { };

		virtual std::string getWatermarkAbbrev(int watermarkId) { return std::string(); }
		virtual std::vector<int> getWatermarkBits(int watermarkId) { return std::vector<int>(); }
		virtual std::vector<int> getWatermarkBits(int watermarkId, Reference<com::sun::star::text::XTextRange> xTextRange) {
			return std::vector<int>();
		}
		virtual com::haathi::watermark::utils::Watermark generate() {
			return com::haathi::watermark::utils::Watermark();
		}
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
