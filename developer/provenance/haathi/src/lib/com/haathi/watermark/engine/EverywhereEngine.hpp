/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/EverywhereEngine.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__ENGINE__EVERYWHERE_ENGINE_HPP
#	define COM__HAATHI__WATERMARK__ENGINE__EVERYWHERE_ENGINE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/engine/WatermarkEngine.hpp"
#include "com/haathi/watermark/utils/Watermarks.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XComponent.hpp>
#include "sdkEnd.hpp"

#include <vector>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Forward declarations
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace embedder {
				class WatermarkEmbedder;
			};
			namespace generator {
				class WatermarkGenerator;
			};
		};
	};
};
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
class EverywhereEngine: public WatermarkEngine {
	protected:
		typedef void (EverywhereEngine::*TextAction)(com::haathi::document::SmartCursor& smartCursor);

		com::haathi::watermark::embedder::WatermarkEmbedder& watermarkEmbedder;
		com::haathi::watermark::utils::Watermarks& watermarks;
		Reference<com::sun::star::lang::XComponent> xComponent;

		void preWatermark(sal_Int32 length);
		void postWatermark(sal_Int32 length);

		void overwatermark(com::haathi::document::SmartCursor& smartCursor);
		void underwatermark(com::haathi::document::SmartCursor& smartCursor);
		void rewatermark(com::haathi::document::SmartCursor& smartCursor);
		void unwatermark(com::haathi::document::SmartCursor& smartCursor);
		void verifyWatermark(com::haathi::document::SmartCursor& smartCursor);
		void traverse(com::haathi::document::SmartCursor& smartCursor, TextAction action);

	public:
		EverywhereEngine(Reference<com::sun::star::frame::XController> xController,
				Reference<com::sun::star::text::XText> xText,
				Reference<com::sun::star::lang::XComponent> xComponent,
				com::haathi::watermark::scheme::WatermarkSchemes& watermarkSchemes,
				com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator,
				com::haathi::watermark::utils::Watermarks& watermarks,
				com::haathi::watermark::embedder::WatermarkEmbedder& watermarkEmbedder);
		virtual ~EverywhereEngine();	

		void watermark(com::haathi::text::Range range, rtl::OUString documentState);
		void rewatermark(sal_Int32 length);
		void unwatermark(sal_Int32 length);
		void verifyWatermark(sal_Int32 length);
		void reset(std::string versionId);

		virtual std::vector<int> getWatermarkBits(int watermarkId);
		virtual std::vector<int> getWatermarkBits(int watermarkId, Reference<com::sun::star::text::XTextRange> xTextRange);
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
