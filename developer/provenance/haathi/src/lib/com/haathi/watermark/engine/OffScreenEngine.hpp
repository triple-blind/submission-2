/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/OffScreenEngine.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__ENGINE__OFF_SCREEN_ENGINE_HPP
#	define COM__HAATHI__WATERMARK__ENGINE__OFF_SCREEN_ENGINE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/text/TextHelper.hpp"
#include "com/haathi/watermark/engine/SomewhereEngine.hpp"

#include "sdkBeg.hpp"
#include "sdkEnd.hpp"
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Forward declarations
******************************************************************************/

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
class OffScreenEngine: public SomewhereEngine {
	protected:
		com::haathi::text::TextHelper textHelper;
	public:
		OffScreenEngine(Reference<com::sun::star::frame::XController> xController,
				Reference<com::sun::star::text::XText> xText,
				Reference<com::sun::star::lang::XComponent> xComponent,
				com::haathi::watermark::scheme::WatermarkSchemes& watermarkSchemes,
				com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator,
				com::haathi::watermark::utils::Watermarks& watermarks,
				com::haathi::watermark::embedder::WatermarkEmbedder& watermarkEmbedder,
				std::string versionId);
		virtual ~OffScreenEngine();	

		virtual void watermarkSome(rtl::OUString documentState);
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
