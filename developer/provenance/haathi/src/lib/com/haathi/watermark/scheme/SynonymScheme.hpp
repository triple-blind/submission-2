/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/SynonymScheme.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__SCHEME__SYNONYM_SCHEME_HPP
#	define COM__HAATHI__WATERMARK__SCHEME__SYNONYM_SCHEME_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/WatermarkScheme.hpp"
#include "com/haathi/watermark/utils/SynonymDictionary.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/util/Color.hpp>
#include "sdkEnd.hpp"

#include "cryptoppBeg.hpp"
#include <cryptopp/rng.h> // LC_RNG
#include "cryptoppEnd.hpp"

#include <string>
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
class SynonymScheme: public WatermarkScheme {
	protected:
		std::vector<OUString> styleNames;
		com::haathi::watermark::utils::Watermark watermark;
		int currentBit;
		CryptoPP::LC_RNG decisionGenerator;
		com::haathi::watermark::utils::SynonymDictionary synonymDictionary;

		unsigned char nextWatermarkBit();
	public:
		SynonymScheme(int id, com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator,
				std::string dictionaryFilename);
		virtual std::string getAbbrev();
		virtual std::vector<WatermarkStyle> getWatermarkStyles();
		virtual int getWatermarkStyleIndex(com::haathi::document::SmartCursor& smartCursor);
		virtual void addWatermarkBits(std::vector<int>& watermarkBits, int index);
		virtual void addWatermarkBits(std::vector<int>& watermarkBits, OUString word);
		virtual bool verifyIndex(com::haathi::document::SmartCursor& smartCursor,
				Reference<com::sun::star::style::XStyle>& xStyle, int index);
		virtual void reset();
		virtual bool isWatermarkable(com::haathi::document::SmartCursor& smartCursor);
		virtual com::haathi::text::Range getSubrange(com::haathi::document::SmartCursor& smartCursor, com::haathi::text::Range range);
		virtual int getMinLength() {
			return 0;
		}

		virtual OUString watermarkWord(OUString word, bool* watermarkable);
		virtual int getBitCount(com::haathi::document::SmartCursor& smartCursor);
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
