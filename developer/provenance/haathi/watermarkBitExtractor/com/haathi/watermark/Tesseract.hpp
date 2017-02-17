/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/Tesseract.hpp#3 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__TESSERACT_HPP
#	define COM__HAATHI__WATERMARK__TESSERACT_HPP
/******************************************************************************
Include
******************************************************************************/
#include "tesseractAll.hpp"
#include "leptonicaAll.hpp"

#include <sstream>
#include <string>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
/******************************************************************************
Class
******************************************************************************/
class Tesseract {
	protected:
		Pix *pix; // part of Leptonica
		tesseract::TessBaseAPI api;

	public:
		Tesseract(std::string inputPath);
		~Tesseract();

		void recognize();

		tesseract::ResultIterator* getIterator() { return api.GetIterator(); }
		Pix *getPix() { return pix; }
		virtual void extractText(std::stringstream& text, bool lastPage = false);
		std::string nextLetter(tesseract::ResultIterator* itr);
		char *nextSymbol(tesseract::ResultIterator* itr);
};
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
