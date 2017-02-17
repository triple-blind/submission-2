/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/WordSpacingScheme.hpp#4 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__SCHEME__WORD_SPACING_SCHEME_HPP
#	define COM__HAATHI__WATERMARK__SCHEME__WORD_SPACING_SCHEME_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/WatermarkScheme.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace scheme {
/******************************************************************************
Class
******************************************************************************/
class WordSpacingScheme: public WatermarkScheme {
	protected:
		std::vector<OUString> styleNames;
		com::haathi::watermark::utils::Watermark watermark;
		int currentBit;

		sal_Int16 kernings[2];

		unsigned char nextWatermarkBit();
		bool isEol(Reference<com::sun::star::text::XTextViewCursor> xTextViewCursor);

	public:
		WordSpacingScheme(int id, int vis, com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator);
		virtual std::string getAbbrev();
		virtual std::vector<WatermarkStyle> getWatermarkStyles();

		virtual bool requiresTextViewCursor() {
			return false;
		}

		virtual int getWatermarkStyleIndex(com::haathi::document::SmartCursor& smartCursor);
		virtual void addWatermarkBits(std::vector<int>& watermarkBits, int index);
		virtual bool verifyIndex(com::haathi::document::SmartCursor& smartCursor,
				Reference<com::sun::star::style::XStyle>& xStyle, int index);
		virtual void reset();
		virtual bool isWatermarkable(com::haathi::document::SmartCursor& smartCursor);
		virtual com::haathi::text::Range getSubrange(com::haathi::document::SmartCursor& smartCursor, com::haathi::text::Range range);
		virtual int getMinLength() {
			return com::haathi::watermark::utils::Watermark::SIZE / 1;
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
