/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/WatermarkStyle.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/WatermarkStyle.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"
/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::NamesValuesHelper;
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
WatermarkStyle::WatermarkStyle(): name(OUSTRING("")), style(NamesValuesHelper()) {
}

WatermarkStyle::WatermarkStyle(OUString name, NamesValuesHelper style):
		name(name), style(style) {
}

WatermarkStyle WatermarkStyle::merge(WatermarkStyle& other) {
	NamesValuesHelper namesValuesHelper(style.getSize() + other.style.getSize());

	namesValuesHelper.add(style);
	namesValuesHelper.add(other.style);

	WatermarkStyle watermarkStyle((name.getLength() > 0 ? name + OUSTRING("/") : OUSTRING("")) + other.name, namesValuesHelper);

	return watermarkStyle;
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
