/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/Extractor.cpp#8 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/extractor/Extractor.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include <climits>
#include <exception>
#include <iostream>
/******************************************************************************
Using
******************************************************************************/
using namespace tesseract;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace extractor {
/******************************************************************************
Pixel
******************************************************************************/
bool Pixel::operator<(const Pixel& other) const {
		return x < other.x ?
			true :
			(x > other.x ?
				false :
				y < other.y);
}
/******************************************************************************
Color
******************************************************************************/
bool Color::operator<(const Color& other) const {
	return red < other.red ?
		true :
		(red > other.red ?
			false :
			(green < other.green ?
				true :
				(green > other.green ?
					false :
					blue < other.blue)));
}
/******************************************************************************
Extractor
******************************************************************************/
void Extractor::alert(std::string message) {
#if 0 // This is for interactive operation
	std::cout << message << std::endl;
	std::cout << "Press enter to continue..." << std::endl;
	std::cin.get();
#else // And this is for batch mode
	throw std::runtime_error(message);
#endif
}

int Extractor::AverageGap(int maxGap, int minGap) {
	// Adding one and then truncating the result is the same as
	// (int) ceil((maxGap - minGap)/2)
	// but without ambiguous conversions from/to float or double
	return maxGap - (maxGap + 1 - minGap) / 2;
}

// This is repeated in WatermarkHelper
unsigned char Extractor::ShiftLeft(void *object, size_t size) {
	unsigned char lbit = (static_cast<unsigned char*>(object)[0]) & (1 << (CHAR_BIT - 1)) ? 1 : 0;

	for (unsigned char* data = static_cast<unsigned char*>(object); size--; ++data) {
		unsigned char bit = size ? (data[1] & (1 << (CHAR_BIT - 1)) ? 1 : 0) : 0;
	      
	   	*data <<= 1;
	   	*data |= bit;
	}
	return lbit;
}

bool Extractor::isNthBitSet(unsigned char c, int n) {
	unsigned char mask[] = {1, 2, 4, 8, 16, 32, 64, 128};

	return ((c & mask[n]) != 0);
}

char *Extractor::getSymbol(ResultIterator* itr) {
	return itr->GetUTF8Text(RIL_SYMBOL);
}

PIX *Extractor::getPix(ResultIterator* itr) {
	int scratchLeft;
	int scratchTop;
	PIX* image = itr->GetImage(RIL_SYMBOL, 0, &scratchLeft, &scratchTop);

	return image;
}

BoundingBox<int> Extractor::getBoundingBox(ResultIterator* itr, PIX* pix) {
	BoundingBox<int> boundingBox;

	itr->BoundingBoxInternal(RIL_SYMBOL, &boundingBox.left, &boundingBox.top, &boundingBox.right, &boundingBox.bottom);

	int w = boundingBox.right - boundingBox.left;
	int h = boundingBox.bottom - boundingBox.top;

	if (pix != NULL) {
		if (w != (int) pix->w)
			std::cerr << "Unexpected width!" << std::endl;
		if (h != (int) pix->h)
			std::cerr << "Unexpected height!" << std::endl;
	}
	return boundingBox;
}

PIX* Extractor::copyPix(PIX* oldPix, BoundingBox<int> boundingBox, int* count) {
	l_uint32 width = boundingBox.right - boundingBox.left;
	l_uint32 height = boundingBox.bottom - boundingBox.top;
	if (width < 0)
		std::cerr << "Bad width!" << std::endl;
	PIX* newPix = pixCreate(width, height, oldPix->d);
	l_int32 red, green, blue;

	for (l_uint32 y = 0; y < height; y++)
		for (l_uint32 x = 0; x < width; x++) {
			pixGetRGBPixel(oldPix, x + boundingBox.left, y + boundingBox.top, &red, &green, &blue);
			pixSetRGBPixel(newPix, x, y, red, green, blue);
		}
	*count = width * height;
	return newPix;
}

PIX* Extractor::makeMaskPix(PIX* pix) {
	if (pix == NULL)
		std::cerr << "pix is null!" << std::endl;
	PIX* tmpPix = pixConvertRGBToLuminance(pix);
	if (tmpPix == NULL)
		std::cerr << "tmpPix is null!" << std::endl;

	pixContrastTRC(tmpPix, tmpPix, 127.0);
	pixContrastTRC(tmpPix, tmpPix, 127.0);

	PIX* maskPix = pixUnsharpMaskingGrayFast(tmpPix, 2, 0.7f, L_BOTH_DIRECTIONS);

	pixDestroy(&tmpPix);
	return maskPix;
}

Extractor::Extractor(int id, Tesseract& tesseract): id(id), tesseract(tesseract) {
};
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
