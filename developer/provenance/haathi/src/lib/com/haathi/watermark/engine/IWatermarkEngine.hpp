/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/IWatermarkEngine.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__ENGINE__IWATERMARK_ENGINE_HPP
#	define COM__HAATHI__WATERMARK__ENGINE__IWATERMARK_ENGINE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/text/Range.hpp"
#include "com/haathi/watermark/utils/Watermark.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/text/XTextRange.hpp>
#include "sdkEnd.hpp"

#include <string>
#include <vector>
/******************************************************************************
Forward declarations
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace scheme {
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
			namespace engine {
/******************************************************************************
Class
******************************************************************************/
class IWatermarkEngine {
	public:
		virtual void reset(std::string versionId) = 0;
		virtual void watermark(com::haathi::text::Range range, rtl::OUString documentState) = 0;
		virtual void rewatermark(sal_Int32 length) = 0;
		virtual void verifyWatermark(sal_Int32 length) = 0;
		virtual void unwatermark(sal_Int32 length) = 0;
		virtual std::string getWatermarkAbbrev(int watermarkId) = 0;
		virtual std::vector<int> getWatermarkBits(int watermarkId) = 0;
		virtual std::vector<int> getWatermarkBits(int watermarkId, Reference<com::sun::star::text::XTextRange> xTextRange) = 0;
		virtual com::haathi::watermark::utils::Watermark generate() = 0;
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
