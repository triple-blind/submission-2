/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/UserAttributesScheme.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__SCHEME__USER_ATTRIBUTES_SCHEME_HPP
#	define COM__HAATHI__WATERMARK__SCHEME__USER_ATTRIBUTES_SCHEME_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/WatermarkScheme.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/container/XNameContainer.hpp>
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
class UserAttributesScheme: public WatermarkScheme {
	protected:
		std::vector<OUString> styleNames;
		com::haathi::watermark::utils::Watermark watermark;
		int currentBit;

		unsigned char nextWatermarkBit();

	public:
		UserAttributesScheme(int id, com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator);
		virtual std::string getAbbrev();
		virtual std::vector<WatermarkStyle> getWatermarkStyles();
		virtual int getWatermarkStyleIndex(com::haathi::document::SmartCursor& smartCursor);
		virtual void addWatermarkBits(std::vector<int>& watermarkBits, int index);
		bool verifyIndex(com::haathi::document::SmartCursor& smartCursor,
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
