/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/watermark/scheme/WatermarkScheme.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/scheme/WatermarkScheme.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::document;
using namespace com::haathi::utils;
using namespace com::haathi::watermark::generator;

using namespace com::sun::star::style;
using namespace com::sun::star::text;
using namespace com::sun::star::uno;
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
WatermarkScheme::WatermarkScheme(int id, WatermarkGenerator* watermarkGenerator):
		id(id), watermarkGenerator(watermarkGenerator) {
}

WatermarkScheme::~WatermarkScheme() {
#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
	logIn.close();
	logOut.close();
#endif
}

#if DEBUG_WATERMARK == DEBUG_WATERMARK_ON
void WatermarkScheme::openLog(const char* name) {
	logIn.open((std::string("C:\\Users\\Keith Alcock\\Documents\\My Data\\") +
			std::string(name) + std::string(".in")).c_str(), std::ios::out);
	if (!logIn.is_open())
		throw HaathiException("Could not open logIn file");

	logOut.open((std::string("C:\\Users\\Keith Alcock\\Documents\\My Data\\") +
			std::string(name) + std::string(".out")).c_str(), std::ios::out);
	if (!logOut.is_open())
		throw HaathiException("Could not open logOut file");
}

void WatermarkScheme::writeLog(std::ofstream& log, sal_Unicode c, std::vector<int> bits) {
	switch (c) {
		case ' ':
			log << "\\s";
			break;
		case '\r':
			log << "\\r";
			break;
		case '\n':
			log << "\\n";
			break;
		case '\\':
			log << "\\\\";
			break;
		default:
			log << (char) c;
	}

	for (std::vector<int>::iterator it = bits.begin(); it != bits.end(); ++it)
		log << "\t" << *it;
	log << std::endl;
}

void WatermarkScheme::writeLogIn(SmartCursor& smartCursor, std::vector<int> bits) {
	writeLog(logIn, smartCursor.getCurrentCharacter(), bits);
}

void WatermarkScheme::writeLogOut(SmartCursor& smartCursor, std::vector<int> bits) {
	writeLog(logOut, smartCursor.getCurrentCharacter(), bits);
}

#endif

bool WatermarkScheme::isSpace(sal_Unicode c) {
	return c == 0x0020  || // plain space
			c == 0x00a0; // hard space
}

bool WatermarkScheme::isHyphen(sal_Unicode c) {
	return c == 0x002d || // plain hyphen
			c == 0x00ad || // soft hyphen
			c ==  0x2011; // hard hyphen
}

bool WatermarkScheme::isEol(sal_Unicode c) {
	return c == 0x000d || // CR
			c == 0x000a; // LF
}

bool WatermarkScheme::isWatermarkable(sal_Unicode c) {
	return !isEol(c) && !isHyphen(c);
}
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
