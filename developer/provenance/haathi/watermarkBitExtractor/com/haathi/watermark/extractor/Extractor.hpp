/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/extractor/Extractor.hpp#9 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__WATERMARK__EXTRACTOR__EXTRACTOR_HPP
#	define COM__HAATHI__WATERMARK__EXTRACTOR__EXTRACTOR_HPP
/******************************************************************************
Include
******************************************************************************/
#include "com/haathi/watermark/Tesseract.hpp"

#include "tesseractAll.hpp"
#include "leptonicaAll.hpp"

#include <sstream>
#include <string>
#include <vector>
/******************************************************************************
Define
******************************************************************************/
#define DEBUG_WATERMARK_OFF 0
#define DEBUG_WATERMARK_ON  1

#define DEBUG_WATERMARK DEBUG_WATERMARK_OFF
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
typedef struct Pixel {
	l_uint32 x;
	l_uint32 y;

	bool operator<(const Pixel& other) const;
} Pixel;
/******************************************************************************
BoundingBox
******************************************************************************/
template<typename T>
struct BoundingBox {
	T top;
	T left;
	T bottom;
	T right;

	BoundingBox(): top(0), left(0), bottom(0), right(0) {
	};

	BoundingBox(T top, T left, T bottom, T right): top(top), left(left),
		bottom(bottom), right(right) {
	}
};
/******************************************************************************
DiagonalBox
******************************************************************************/
template<typename T>
struct DiagonalBox {
	T x1, y1;
	T x2, y2;

	DiagonalBox(): x1(0), y1(0), x2(0), y2(0) {
	}

	DiagonalBox(T x1, T y1, T x2, T y2): x1(x1), y1(y1),
		x2(x2), y2(y2) {
	}
};
/******************************************************************************
Color
******************************************************************************/
typedef struct Color {
	l_int32 red;
	l_int32 green;
	l_int32 blue;

	bool operator<(const Color& other) const;
} Color;
/******************************************************************************
Extractor
******************************************************************************/
class Extractor {
	protected:
		static const short DEFAULT_BLOCK_SIZE = 64;

		int id;
		com::haathi::watermark::Tesseract& tesseract;
		std::string _imagePath;
		std::vector<unsigned char> sequence;

		int AverageGap(int maxGap, int minGap);
		unsigned char ShiftLeft(void *object, size_t size);
		bool isNthBitSet (unsigned char c, int n);

		char *getSymbol(tesseract::ResultIterator* itr);
		PIX *getPix(tesseract::ResultIterator* itr);
		BoundingBox<int> getBoundingBox(tesseract::ResultIterator* itr, PIX* pix = NULL);
		PIX* copyPix(PIX* oldPix, BoundingBox<int> boundingBox, int* count);
		PIX* makeMaskPix(PIX* pix);

	public:
		Extractor(int id, com::haathi::watermark::Tesseract& tesseract);
		virtual ~Extractor() { };

		void alert(std::string message);
		virtual int getId() { return id; }
		virtual void extractBits(std::stringstream& text, std::stringstream& bits, int vis, bool lastPage = false) = 0;
		virtual bool verifyCounts(int charCount, int bitCount) {
			return true;
		}
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
