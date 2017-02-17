/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/retired/haathi/src/lib/com/haathi/watermark/scheme/DualWatermarkScheme.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__DUAL_WATERMARK_SCHEME_HPP
#	define COM__HAATHI__WATERMARK__DUAL_WATERMARK_SCHEME_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/NewLineTracker.hpp"
#include "com/haathi/watermark/WatermarkScheme.hpp"
/******************************************************************************
Class
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
/******************************************************************************
Class
******************************************************************************/
// This combines luminance and word spacing
			
class DualWatermarkScheme: public WatermarkScheme {
	private:
		int _embeddingCriteria;
		unsigned char _currentByte;
		short _previousBit;
		NewLineTracker newLineTracker;

	protected:
		void addCharacterStyles(OUString styleName, rtl::OUString propertyName1, com::sun::star::uno::Any value1,
				OUString propertyName2, com::sun::star::uno::Any value2);
	
	public:
		DualWatermarkScheme(com::haathi::document::ProvenanceDocument* provenanceDocument);
		virtual void setBlockToEmbed(std::bitset<DEFAULT_BLOCK_SIZE> block);
		virtual bool isWatermarkEmbedded();
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
