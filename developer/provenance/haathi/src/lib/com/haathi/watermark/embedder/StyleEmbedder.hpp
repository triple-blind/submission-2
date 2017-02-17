/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/embedder/StyleEmbedder.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EMBEDDER__STYLE_EMBEDDER_HPP
#	define COM__HAATHI__WATERMARK__EMBEDDER__STYLE_EMBEDDER_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/SmartCursor.hpp"
#include "com/haathi/utils/NamesValuesHelper.hpp"
#include "com/haathi/watermark/embedder/WatermarkEmbedder.hpp"
#include "com/haathi/watermark/scheme/WatermarkStyle.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include "sdkEnd.hpp"

#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace embedder {
/******************************************************************************
Class
******************************************************************************/
class StyleEmbedder: public WatermarkEmbedder {
	protected:
		Reference<com::sun::star::lang::XComponent> xComponent;
		std::vector<com::haathi::watermark::scheme::WatermarkStyle>& watermarkStyles;

		void addCharacterStyle(rtl::OUString styleName, rtl::OUString stylePrettyName, com::haathi::utils::NamesValuesHelper& namesValuesHelper);
		void addIntCharacterStyle(int index, rtl::OUString name, com::haathi::utils::NamesValuesHelper namesValuesHelper);

		void subCharacterStyle(rtl::OUString styleName, rtl::OUString stylePrettyName, com::haathi::utils::NamesValuesHelper& namesValuesHelper);
		void subIntCharacterStyle(int index, rtl::OUString name, com::haathi::utils::NamesValuesHelper namesValuesHelper);

		com::sun::star::uno::Reference<com::sun::star::container::XNameContainer> getXNameContainer(rtl::OUString name);
		com::sun::star::uno::Reference<com::sun::star::style::XStyle> newXStyle();

	public:
		StyleEmbedder(Reference<com::sun::star::lang::XComponent> xComponent,
				std::vector<com::haathi::watermark::scheme::WatermarkStyle>& watermarkStyles);

		void preWatermark();
		int extract(com::haathi::document::SmartCursor& smartCursor);
		void embed(com::haathi::document::SmartCursor& smartCursor, int watermark);
		void unembed(com::haathi::document::SmartCursor& smartCursor);
		void postWatermark();
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
