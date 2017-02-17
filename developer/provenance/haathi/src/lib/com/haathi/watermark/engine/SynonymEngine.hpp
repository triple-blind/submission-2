/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/SynonymEngine.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__ENGINE__SYNONYM_ENGINE_HPP
#	define COM__HAATHI__WATERMARK__ENGINE__SYNONYM_ENGINE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/document/DocumentHistory.hpp"
#include "com/haathi/document/ProvenanceDocument.hpp"
#include "com/haathi/text/TextHelper.hpp"
#include "com/haathi/watermark/engine/OffScreenEngine.hpp"
#include "com/haathi/watermark/scheme/SynonymScheme.hpp"

#include "sdkBeg.hpp"
#include "sdkEnd.hpp"

#include "boostBeg.hpp"
#include <boost/shared_ptr.hpp>
#include "boostEnd.hpp"
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
class SynonymEngine: public OffScreenEngine {
	protected:
		boost::shared_ptr<com::haathi::watermark::scheme::SynonymScheme> synonymScheme;
		com::haathi::text::TextHelper textHelper;
		int round;
		com::haathi::document::DocumentHistory& documentHistory;
		// IProvenanceSink
		com::haathi::document::ProvenanceDocument& provenanceDocument;

		void addWatermarkBits(std::vector<int>& watermarkBits, OUString word);

	public:
		SynonymEngine(Reference<com::sun::star::frame::XController> xController,
				Reference<com::sun::star::text::XText> xText,
				Reference<com::sun::star::lang::XComponent> xComponent,
				com::haathi::watermark::scheme::WatermarkSchemes& watermarkSchemes,
				com::haathi::watermark::generator::WatermarkGenerator* watermarkGenerator,
				com::haathi::watermark::utils::Watermarks& watermarks,
				com::haathi::watermark::embedder::WatermarkEmbedder& watermarkEmbedder,
				std::string versionId, std::string dictionaryFilename,
				com::haathi::document::DocumentHistory& documentHistory,
				com::haathi::document::ProvenanceDocument& provenanceDocument);
		virtual ~SynonymEngine();	

		virtual void rewatermark(sal_Int32 length);
		virtual void watermarkSome(rtl::OUString documentState);

		virtual std::string getWatermarkAbbrev(int watermarkId);
		virtual std::vector<int> getWatermarkBits(int watermarkId);
		virtual std::vector<int> getWatermarkBits(int watermarkId, Reference<com::sun::star::text::XTextRange> xTextRange);
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
