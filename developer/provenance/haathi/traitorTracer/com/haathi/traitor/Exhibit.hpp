/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autotype2/haathi/traitorTracer/com/haathi/traitor/Exhibit.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__TRAITOR__EXHIBIT_HPP
#	define COM__HAATHI__TRAITOR__EXHIBIT_HPP
/******************************************************************************
Include
******************************************************************************/
#include <string>
#include <vector>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace traitor {
/******************************************************************************
Class
******************************************************************************/
struct Exhibit {
	public:
		int watermarkId;
		std::string watermarkAbbrev;
		std::string watermark;
		const char* watermarkBits;
		int watermarkLength;
	public:
		Exhibit(int watermarkId, std::string watermarkAbbrev, std::string watermark):
				watermarkId(watermarkId), watermarkAbbrev(watermarkAbbrev),
				watermark(watermark) {
			watermarkBits = this->watermark.c_str();
			watermarkLength = this->watermark.size();
		};

		~Exhibit() {
		};
};

typedef std::vector<Exhibit*> Exhibits;
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
