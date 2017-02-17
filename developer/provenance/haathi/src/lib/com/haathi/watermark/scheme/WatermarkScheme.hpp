/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/WatermarkScheme.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__SCHEME__WATERMARK_SCHEME_HPP
#	define COM__HAATHI__WATERMARK__SCHEME__WATERMARK_SCHEME_HPP
/******************************************************************************
Define
******************************************************************************/
#define DEBUG_WATERMARK_OFF 0
#define DEBUG_WATERMARK_ON  1

#define DEBUG_WATERMARK DEBUG_WATERMARK_OFF
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/SmartCursor.hpp"
#include "com/haathi/text/Range.hpp"
#include "com/haathi/utils/NamesValuesHelper.hpp"
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/watermark/scheme/WatermarkStyle.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/text/XTextViewCursor.hpp>
#include "sdkEnd.hpp"

#include <bitset>
#include <vector>
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
#	include <fstream>
#endif
/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Forward references
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace generator {
				class WatermarkGenerator;
			};
		};
	};
};
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
class WatermarkScheme {
	protected:
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
		std::ofstream logIn;
		std::ofstream logOut;
		void openLog(const char* name);

		void writeLog(std::ofstream& log, sal_Unicode c, std::vector<int> bits);

		void writeLogIn(com::haathi::document::SmartCursor& smartCursor, std::vector<int> bits);
		void writeLogOut(com::haathi::document::SmartCursor& smartCursor, std::vector<int> bits);
#endif
		int id;
		com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator;

	public:
		static bool isSpace(sal_Unicode c);
		static bool isHyphen(sal_Unicode c);
		static bool isEol(sal_Unicode c);
		static bool isWatermarkable(sal_Unicode c);

		WatermarkScheme(int id, com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator);
		virtual ~WatermarkScheme();

		virtual int getId() {
			return id;
		}

		virtual std::string getAbbrev() = 0;
		virtual std::vector<com::haathi::watermark::scheme::WatermarkStyle> getWatermarkStyles() = 0;

		virtual bool requiresTextViewCursor() {
			return false;
		}

		virtual int getWatermarkStyleIndex(com::haathi::document::SmartCursor& smartCursor) = 0;
		
		virtual void addWatermarkBits(std::vector<int>& watermarkBits, int index) = 0;

		virtual bool verifyIndex(com::haathi::document::SmartCursor& smartCursor,
				Reference<com::sun::star::style::XStyle>& xStyle, int index) = 0;

		virtual void reset() = 0;

		// Override these default implementations
		virtual bool isWatermarkable(com::haathi::document::SmartCursor& smartCursor) { return true; }
		virtual com::haathi::text::Range getSubrange(com::haathi::document::SmartCursor& smartCursor, com::haathi::text::Range range) {
			return com::haathi::text::Range();
		}
		virtual int getMinLength() {
			return com::haathi::watermark::utils::Watermark::SIZE;
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
