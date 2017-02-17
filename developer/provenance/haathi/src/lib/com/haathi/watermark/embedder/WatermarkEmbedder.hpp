/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/embedder/WatermarkEmbedder.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EMBEDDER__WATERMARK_EMBEDDER_HPP
#	define COM__HAATHI__WATERMARK__EMBEDDER__WATERMARK_EMBEDDER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/SmartCursor.hpp"
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
class WatermarkEmbedder {
	public:
		WatermarkEmbedder();
		virtual ~WatermarkEmbedder();
		virtual void preWatermark() = 0;
		virtual int extract(com::haathi::document::SmartCursor& smartCursor) = 0;
		virtual void embed(com::haathi::document::SmartCursor& smartCursor, int watermark) = 0;
		virtual void unembed(com::haathi::document::SmartCursor& smartCursor) = 0;
		virtual void postWatermark() = 0;
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
