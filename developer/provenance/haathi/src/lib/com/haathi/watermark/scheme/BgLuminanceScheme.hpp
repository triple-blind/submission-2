/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/BgLuminanceScheme.hpp#2 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__SCHEME__BG_LUMINANCE_SCHEME_HPP
#	define COM__HAATHI__WATERMARK__SCHEME__BG_LUMINANCE_SCHEME_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/WatermarkScheme.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/util/Color.hpp>
#include "sdkEnd.hpp"

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
class BgLuminanceScheme: public WatermarkScheme {
	protected:
		std::vector<OUString> styleNames;
		com::haathi::watermark::utils::Watermark watermark;
		int currentBit;

		int vis;

		void setColorBit(com::sun::star::util::Color* color, unsigned char value, int n);
		com::sun::star::util::Color makeColor(unsigned char bit1, unsigned char bit2);
		unsigned char nextWatermarkBit();

	public:
		BgLuminanceScheme(int id, int vis, com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator);
		virtual std::string getAbbrev();
		virtual std::vector<WatermarkStyle> getWatermarkStyles();
		virtual int getWatermarkStyleIndex(com::haathi::document::SmartCursor& smartCursor);
		virtual void addWatermarkBits(std::vector<int>& watermarkBits, int index);
		virtual bool verifyIndex(com::haathi::document::SmartCursor& smartCursor,
				Reference<com::sun::star::style::XStyle>& xStyle, int index);
		virtual void reset();
		virtual bool isWatermarkable(com::haathi::document::SmartCursor& smartCursor);
		virtual com::haathi::text::Range getSubrange(com::haathi::document::SmartCursor& smartCursor, com::haathi::text::Range range);
		virtual int getMinLength() {
			return com::haathi::watermark::utils::Watermark::SIZE / 2;
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
