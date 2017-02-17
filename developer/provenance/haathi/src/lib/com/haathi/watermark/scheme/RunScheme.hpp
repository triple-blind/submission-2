/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/RunScheme.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__SCHEME__RUN_SCHEME_HPP
#	define COM__HAATHI__WATERMARK__SCHEME__RUN_SCHEME_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/scheme/WatermarkScheme.hpp"

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
class RunScheme: public WatermarkScheme {
	protected:
		std::vector<OUString> styleNames;
		int run;

	public:
		RunScheme(int id, com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator);
		virtual std::string getAbbrev();
		virtual std::vector<WatermarkStyle> getWatermarkStyles();
		virtual int getWatermarkStyleIndex(com::haathi::document::SmartCursor& smartCursor);
		virtual int getWatermarkStyleIndex();
		virtual void addWatermarkBits(std::vector<int>& watermarkBits, int index);
		virtual bool verifyIndex(com::haathi::document::SmartCursor& smartCursor,
				Reference<com::sun::star::style::XStyle>& xStyle, int index);
		virtual void reset();
		virtual void reset(int n);
		virtual int reset(int width, int leftRun, int rightRun);
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
