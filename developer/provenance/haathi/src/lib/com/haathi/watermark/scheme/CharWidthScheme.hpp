/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/CharWidthScheme.hpp#4 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__SCHEME__CHAR_WIDTH_SCHEME_HPP
#	define COM__HAATHI__WATERMARK__SCHEME__CHAR_WIDTH_SCHEME_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/WatermarkScheme.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include <map>
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
class CharWidthScheme: public WatermarkScheme {
	protected:
		std::vector<OUString> styleNames;
		com::haathi::watermark::utils::Watermark watermark;
		int currentBit;
		unsigned char reserveBit;
		bool hasReserveBit;

		int vis;

		std::map<int, int> styleToBitMap;
		std::map<sal_Unicode, int> symbolToStyleMaps[2];
		OUString usableSymbols;

		unsigned char nextWatermarkBit();

		bool usableSymbolFollows(com::haathi::document::SmartCursor& smartCursor);

	public:
		CharWidthScheme(int id, int vis, com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator);
		virtual std::string getAbbrev();
		virtual std::vector<WatermarkStyle> getWatermarkStyles();
		virtual int getWatermarkStyleIndex(com::haathi::document::SmartCursor& smartCursor);
		virtual void addWatermarkBits(std::vector<int>& watermarkBits, int index);
		virtual bool verifyIndex(com::haathi::document::SmartCursor& smartCursor,
				Reference<com::sun::star::style::XStyle>& xStyle, int index);
		virtual void reset();
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
