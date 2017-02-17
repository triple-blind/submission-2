/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/watermark/scheme/ChaffWatermarkScheme.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__CHAFF_WATERMARK_SCHEME_HPP
#	define COM__HAATHI__WATERMARK__CHAFF_WATERMARK_SCHEME_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/WatermarkScheme.hpp"
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
/******************************************************************************
Class
******************************************************************************/
class ChaffWatermarkScheme: public WatermarkScheme {
	protected:

	public:
		ChaffWatermarkScheme(com::haathi::document::ProvenanceDocument* provenanceDocument);
		virtual void setBlockToEmbed(std::bitset<DEFAULT_BLOCK_SIZE> block);
		virtual bool isWatermarkEmbedded() ;
		virtual short generateWatermark(com::sun::star::uno::Reference<com::sun::star::text::XTextViewCursor>& xTextViewCursor);
		virtual void resetState();
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

