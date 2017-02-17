/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/WatermarkEngine.hpp#2 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__ENGINE__WATERMARK_ENGINE_HPP
#	define COM__HAATHI__WATERMARK__ENGINE__WATERMARK_ENGINE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/SmartCursor.hpp"
#include "com/haathi/text/Range.hpp"
#include "com/haathi/utils/BeforeAndAfter.hpp"
#include "com/haathi/utils/NamesValuesHelper.hpp"
#include "com/haathi/watermark/engine/IWatermarkEngine.hpp"
#include "com/haathi/watermark/generator/WatermarkGenerator.hpp"
#include "com/haathi/watermark/scheme/WatermarkSchemes.hpp"
#include "com/haathi/watermark/scheme/WatermarkStyle.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/text/XTextRangeCompare.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/text/XTextViewCursor.hpp>
#include "sdkEnd.hpp"

#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace engine {
/******************************************************************************
HideAndShowCursor
******************************************************************************/
class HideAndShowCursor: public com::haathi::utils::BeforeAndAfter {
	protected:
		com::haathi::document::SmartCursor& smartCursor;

		void before();
		void after();
	public:
		HideAndShowCursor(com::haathi::document::SmartCursor& smartCursor);
		virtual ~HideAndShowCursor();
};
/******************************************************************************
SaveAndRestoreRange
******************************************************************************/
class SaveAndRestoreRange: public com::haathi::utils::BeforeAndAfter {
	protected:
		com::haathi::document::SmartCursor& smartCursor;
		Reference<com::sun::star::text::XTextCursor> xTextCursor;

		void before(Reference<com::sun::star::text::XText> xText);
		void after();
	public:
		SaveAndRestoreRange(Reference<com::sun::star::text::XText> xText, com::haathi::document::SmartCursor& smartCursor);
		virtual ~SaveAndRestoreRange();
};
/******************************************************************************
WatermarkEngine
******************************************************************************/
class WatermarkEngine: public IWatermarkEngine {
	protected:
		Reference<com::sun::star::frame::XController> xController;
		Reference<com::sun::star::text::XText> xText;
		com::haathi::watermark::scheme::WatermarkSchemes& watermarkSchemes;
		com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator;
		bool requiresTextViewCursor;

		Reference<com::sun::star::text::XTextViewCursor> getXTextViewCursor(bool really = true);
		Reference<com::sun::star::text::XTextCursor> getXTextCursor(bool really = true);
		Reference<com::sun::star::text::XTextCursor> getCompleteXTextCursor();
		Reference<com::sun::star::text::XTextRangeCompare> getXTextRangeCompare();

		void reset();

	public:
		WatermarkEngine(Reference<com::sun::star::frame::XController> xController,
				Reference<com::sun::star::text::XText> xText,
				com::haathi::watermark::scheme::WatermarkSchemes& watermarkSchemes,
				com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator);
		virtual ~WatermarkEngine();
		virtual std::string getWatermarkAbbrev(int watermarkId);
		virtual com::haathi::watermark::utils::Watermark generate();
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
