/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/embedder/AttributeEmbedder.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EMBEDDER__ATTRIBUTE_EMBEDDER_HPP
#	define COM__HAATHI__WATERMARK__EMBEDDER__ATTRIBUTE_EMBEDDER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/SmartCursor.hpp"
#include "com/haathi/utils/NamesValuesHelper.hpp"
#include "com/haathi/watermark/embedder/WatermarkEmbedder.hpp"
#include "com/haathi/watermark/scheme/WatermarkStyle.hpp"

#include "sdkBeg.hpp"
#include "sdkEnd.hpp"

#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace embedder {
/******************************************************************************
Class
******************************************************************************/
class AttributeEmbedder: public WatermarkEmbedder {
	protected:
		std::vector<com::haathi::watermark::scheme::WatermarkStyle>& watermarkStyles;

	public:
		AttributeEmbedder(Reference<com::sun::star::lang::XComponent> xComponent,
				std::vector<com::haathi::watermark::scheme::WatermarkStyle>& watermarkStyles);

		void preWatermark();
		int extract(com::haathi::document::SmartCursor& smartCursor);
		void embed(com::haathi::document::SmartCursor& smartCursor, int watermark);
		void unembed(com::haathi::document::SmartCursor& smartCursor);
		void postWatermark();
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
