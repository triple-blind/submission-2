/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/WatermarkSchemes.hpp#4 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__SCHEME__WATERMARK_SCHEMES_HPP
#	define COM__HAATHI__WATERMARK__SCHEME__WATERMARK_SCHEMES_HPP
/******************************************************************************
Define
******************************************************************************/
#define RUN_WATERMARK          0x00
#define FG_LUMINANCE_WATERMARK 0x01
#define BG_LUMINANCE_WATERMARK 0x02
#define BASELINE_WATERMARK     0x04
#define CHAR_WIDTH_WATERMARK   0x08
#define WORD_SPACING_WATERMARK 0x10
#define SYNONYM_WATERMARK      0x20
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/scheme/WatermarkScheme.hpp"
#include "com/haathi/watermark/scheme/WatermarkStyle.hpp"

#include <string>
#include <vector>
/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Forward references
******************************************************************************/
namespace com {
	namespace haathi {
		namespace document {
			class SmartCursor;
		};
		namespace watermark {
			namespace generator {
				class WatermarkGenerator;
			};
			namespace scheme {
				class RunScheme;
				class WatermarkScheme;
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
class WatermarkSchemes {
	protected:
		static std::vector<WatermarkStyle> join(std::vector<WatermarkStyle>& lefts, std::vector<WatermarkStyle>& rights);

		std::vector<WatermarkScheme*> watermarkSchemes;
		RunScheme* runScheme;

		std::vector<int> placeValues;
		std::vector<com::haathi::watermark::scheme::WatermarkStyle> watermarkStyles;

		// These get the index out of an existing watermark
		int getWatermarkIndexChannel(int watermark, int channel);

	public:
		static const int RUN_CHANNEL = 0;

		WatermarkSchemes(com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator,
				std::string dictonaryFilename, int schemes, int vis);
		virtual ~WatermarkSchemes();

		bool requiresTextViewCursor();
		void reset();
		void resetRun(int width, int leftRun, int rightRun);
		std::vector<com::haathi::watermark::scheme::WatermarkStyle>& getWatermarkStyles() {
			return watermarkStyles;
		}

		// These two calculate the watermark to be placed at the cursor position
		int getWatermark(com::haathi::document::SmartCursor& smartCursor);
		int getWatermarkIndexRun(com::haathi::document::SmartCursor& smartCursor);

		// These get the index out of an existing watermark
		int getWatermarkIndexRun(int watermark) {
			return getWatermarkIndexChannel(watermark, 0);
		}

		int getWatermarkIndexId(int watermark, int id);
		virtual std::string getWatermarkAbbrev(int watermarkId);

		// This combines an existing watermark with a new watermarkIndex in a certain channel
		int combineChannel(int oldWatermark, int newWatermarkIndex, int channel);
		int combineRun(int oldWatermark, int newWatermarkIndex) {
			return combineChannel(oldWatermark, newWatermarkIndex, 0);
		}

		int count() {
			return watermarkSchemes.size();
		}

		WatermarkScheme* at(int index) {
			if (0 <= index && index < count())
				return watermarkSchemes[index];
			return NULL;
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
