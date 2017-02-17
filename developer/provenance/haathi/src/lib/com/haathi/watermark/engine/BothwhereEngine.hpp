/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/engine/BothwhereEngine.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__ENGINE__BOTHWHERE_ENGINE_HPP
#	define COM__HAATHI__WATERMARK__ENGINE__BOTHWHERE_ENGINE_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/engine/EverywhereEngine.hpp"
#include "com/haathi/watermark/engine/IWatermarkEngine.hpp"
#include "com/haathi/watermark/engine/SomewhereEngine.hpp"

#include "sdkBeg.hpp"
#include <com/sun/star/lang/XComponent.hpp>
#include "sdkEnd.hpp"

#include <vector>
/******************************************************************************
Define
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
class BothwhereEngine: public IWatermarkEngine {
	protected:
		EverywhereEngine& everywhereEngine;
		SomewhereEngine& somewhereEngine;

	public:
		BothwhereEngine(EverywhereEngine& everywhereEngine, SomewhereEngine& somewhereEngine);
		virtual ~BothwhereEngine();	

		void reset(std::string versionId);
		void watermark(com::haathi::text::Range range, rtl::OUString documentState);
		void rewatermark(sal_Int32 length);
		void unwatermark(sal_Int32 length);
		void verifyWatermark(sal_Int32 length);
		virtual std::string getWatermarkAbbrev(int watermarkId);
		virtual std::vector<int> getWatermarkBits(int watermarkId);
		virtual std::vector<int> getWatermarkBits(int watermarkId, Reference<com::sun::star::text::XTextRange> xTextRange);
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
