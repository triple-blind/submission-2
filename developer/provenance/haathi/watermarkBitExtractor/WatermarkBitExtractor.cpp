/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/WatermarkBitExtractor.cpp#10 $
/******************************************************************************
Include self
******************************************************************************/

/******************************************************************************
Include others
******************************************************************************/
#include "AutoLib.hpp"

#include "com/haathi/watermark/extractor/BaselineExtractor.hpp"
#include "com/haathi/watermark/extractor/BgLuminanceExtractor.hpp"
#include "com/haathi/watermark/extractor/CharWidthExtractor.hpp"
#include "com/haathi/watermark/extractor/FgLuminanceExtractor.hpp"
#include "com/haathi/watermark/extractor/SynonymExtractor.hpp"
#include "com/haathi/watermark/extractor/WordSpacingExtractor.hpp"
#include "com/haathi/watermark/Tesseract.hpp"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
/******************************************************************************
Define
******************************************************************************/
// These need to be coordinated with WatermarkSchemes.hpp
#define RUN_WATERMARK          0x00
#define FG_LUMINANCE_WATERMARK 0x01
#define BG_LUMINANCE_WATERMARK 0x02
#define BASELINE_WATERMARK     0x04
#define CHAR_WIDTH_WATERMARK   0x08
#define WORD_SPACING_WATERMARK 0x10
#define SYNONYM_WATERMARK      0x20

// And these with SchemeConfig.hpp
#define SCHEME_VIS_HI 3
#define SCHEME_VIS_MD 2
#define SCHEME_VIS_LO 1
/******************************************************************************
Using
******************************************************************************/
using namespace com::haathi::watermark;
using namespace com::haathi::watermark::extractor;
/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
WatermarkBitExtractor
******************************************************************************/
class WatermarkBitExtractor {
	protected:
		int argc;
		char** argv;

		bool validSyntax;
		bool validFiles;

		std::string pathToDocJpg;
		std::string pathToInWm;
		std::string pathToOutWm;
		std::string pathToOutTxt;
		std::string pathToSynDict;
	
		AutoLib autoLib;

		bool validateSyntax();
		bool validateFiles();
		bool isValid();
		void printUsage();

		int getPageCount(std::ifstream& in);
		void fillColumns(std::ifstream& in, std::vector<int>& schemes, std::vector<std::string>& abbrevs, std::vector<int>& vises);
		void extract(int page, int pageCount, std::vector<int>& schemes, std::vector<int>& vises, std::vector<std::stringstream*>& bits, std::stringstream& text);	

		std::vector<Extractor*> newExtractors(Tesseract& tesseract);
		void oldExtractors(std::vector<Extractor*> extractors);

		void saveOutWm(std::ofstream& out, int watermarkId, std::string watermarkAbbrev, std::string watermarkBits);
		void saveOutTxt(std::string text);

	public:
		WatermarkBitExtractor(int argc, char** argv);
		int run();
};

WatermarkBitExtractor::WatermarkBitExtractor(int argc, char** argv): argc(argc), argv(argv) {
	validSyntax = validateSyntax();
	if (!validSyntax)
		printUsage();
	else
		validFiles = validateFiles();
}

bool WatermarkBitExtractor::isValid() {
	return validSyntax && validFiles;
}

void WatermarkBitExtractor::printUsage() {
	std::cerr
			<< "Usage: " << argv[0] << " <doc.jpg> <in.wm> <out.wm> <out.txt> <dict.txt>" << std::endl
			<< "Where: doc.jpg is an example name of the JPEG version of a watermarked document." << std::endl
			<< "        in.wm  is the file that recorded the watermarks used for doc.pdf." << std::endl
			<< "       out.wm  is a file that records the watermark bits in doc.jpg." << std::endl
			<< "       out.txt is a file that records the characters in doc.jpg." << std::endl
			<< "      dict.txt is the synonym dictionary, normally taken from haathi." << std::endl;
}

bool WatermarkBitExtractor::validateSyntax() {
	int index = 1;

	if (argc != 6)
		return false;

	pathToDocJpg = std::string(argv[index++]);
	pathToInWm = std::string(argv[index++]);
	pathToOutWm = std::string(argv[index++]);
	pathToOutTxt = std::string(argv[index++]);
	pathToSynDict = std::string(argv[index++]);
	return true;
}

bool WatermarkBitExtractor::validateFiles() {
	bool valid = true;

	// With wildcards, need to check afterwards
	if (pathToDocJpg.find('%') == std::string::npos)
		valid &= validateInputFile(pathToDocJpg, "<doc.jpg>");
	valid &= validateInputFile(pathToInWm, "<in.wm>");
	valid &= validateOutputFile(pathToOutWm, "<out.wm>");
	valid &= validateOutputFile(pathToOutTxt, "<out.txt>");
	valid &= validateInputFile(pathToSynDict, "<dict.txt>");
	return valid;
}

std::vector<Extractor*> WatermarkBitExtractor::newExtractors(Tesseract& tesseract) {
	std::vector<Extractor*> extractors;

	extractors.push_back(new FgLuminanceExtractor(FG_LUMINANCE_WATERMARK, tesseract));
	extractors.push_back(new BgLuminanceExtractor(BG_LUMINANCE_WATERMARK, tesseract));
	extractors.push_back(new SynonymExtractor(SYNONYM_WATERMARK, tesseract, pathToSynDict));
	extractors.push_back(new BaselineExtractor(BASELINE_WATERMARK, tesseract));
	extractors.push_back(new WordSpacingExtractor(WORD_SPACING_WATERMARK, tesseract));
	extractors.push_back(new CharWidthExtractor(CHAR_WIDTH_WATERMARK, tesseract));
	return extractors;
}

void WatermarkBitExtractor::oldExtractors(std::vector<Extractor*> extractors) {
	std::vector<Extractor*>::iterator it;

	for (it = extractors.begin(); it != extractors.end(); ++it)
		delete *it;
}

void WatermarkBitExtractor::saveOutTxt(std::string text) {
	std::ofstream out(pathToOutTxt.c_str(), std::ios::out | std::ios::binary);

	if (!out) {
		throw std::exception("There was a problem with the <out.txt> file.");
		return;
	}
	out << text;
}

void WatermarkBitExtractor::extract(int page, int pageCount, std::vector<int>& schemes, std::vector<int>& vises, std::vector<std::stringstream*>& bits,
		std::stringstream& text) {
	// Warning!  This is a dangerous hack.
	int size = pathToDocJpg.length() * 2 + 20;
	char* newPathToDocJpg = new char[size];
	sprintf_s(newPathToDocJpg, size, pathToDocJpg.c_str(), page);
		
	if (!validateInputFile(newPathToDocJpg, "page image"))
		return;

	Tesseract tesseract(newPathToDocJpg);
	std::vector<Extractor*> extractors = newExtractors(tesseract);
	tesseract.recognize();
	std::stringstream pageText;
	tesseract.extractText(pageText, page == pageCount);

	std::string pageTextStr = pageText.str();
	if (page != 1)
		text << "\t";
	text << pageTextStr;
	int pageTextLength = pageTextStr.length();

	for (size_t i = 0; i < schemes.size(); i++) {
		int schemeId = schemes[i];

		std::vector<Extractor*>::iterator itr;
		for (itr = extractors.begin(); itr != extractors.end(); ++itr)
			if ((*itr)->getId() == schemeId) {
				std::stringstream pageBits;
			
				(*itr)->extractBits(pageText, pageBits, vises[i], page == pageCount);
				std::string pageBitsStr = pageBits.str();
				int pageBitsLength = pageBitsStr.length();

				if (!(*itr)->verifyCounts(pageTextLength, pageBitsLength))
					(*itr)->alert("Wrong number of characters or bits extracted!");

				if (page != 1)
					*bits[i] << "\t";
				*bits[i] << pageBitsStr;
				break;
			}
	}
	oldExtractors(extractors);
	delete newPathToDocJpg;
}

int WatermarkBitExtractor::getPageCount(std::ifstream& in) {
	std::string line;

	std::getline(in, line);
	std::getline(in, line);

	std::istringstream iss(line);
	std::string skip;
	int pageCount;

	iss >> skip; // id
	iss >> skip; // abbrev
	iss >> skip; // vis
	iss >> pageCount; // pp

	in.seekg(0, in.beg);
	return pageCount;
}

void WatermarkBitExtractor::fillColumns(std::ifstream& in, std::vector<int>& schemes,
		std::vector<std::string>& abbrevs, std::vector<int>& vises) {
	std::string headers;
	std::string line;

	std::getline(in, headers);

	while (std::getline(in, line)) {
		if (line.empty())
			break;

		std::istringstream iss(line);
		int id;
		std::string abbrev;
		int vis;

		iss >> id;
		iss >> abbrev;
		iss >> vis;
		schemes.push_back(id);
		abbrevs.push_back(abbrev);
		vises.push_back(vis);
	}
	in.seekg(0, in.beg);
}

int WatermarkBitExtractor::run() {
	if (!isValid())
		return -1;

	try {
		std::ifstream in(pathToInWm.c_str(), std::ios::in);
		if (!in) {
			throw std::exception("There was a problem with the <in.wm> file.");
			return -1;
		}

		std::ofstream out(pathToOutWm.c_str(), std::ios::out | std::ios::app);
		if (!out) {
			throw std::exception("There was a problem with the <out.wm> file.");
			return -1;
		}

		int pageCount = getPageCount(in);

		std::vector<int> schemes;
		std::vector<std::string> abbrevs;
		std::vector<int> vises;
		fillColumns(in, schemes, abbrevs, vises);

		std::vector<std::stringstream*> bitStreams;
		std::stringstream textStream;

		for (size_t i = 0; i < schemes.size(); i++)
			bitStreams.push_back(new std::stringstream());

		for (int i = 1; i <= pageCount; i++)
			extract(i, pageCount, schemes, vises, bitStreams, textStream);

		// saveOutWm
		out << "id\tabbrev\tvis\tbits" << std::endl;
		for (size_t i = 0; i < schemes.size(); i++)
			out
				<< schemes[i]
				<< "\t" << abbrevs[i]
				<< "\t" << vises[i]
				<< "\t" << bitStreams[i]->str()
				<< std::endl;
		saveOutTxt(textStream.str());
		for (size_t i = 0; i < schemes.size(); i++)
			delete bitStreams[i];
	}
	catch (std::runtime_error& exception) {
		std::cerr << exception.what() << std::endl;
		return -1;
	}
	catch (...) {
		std::cerr << "Unexpected exception" << std::endl;
		return -1;
	}
	return 0;
}

int main(int argc, char** argv) {
	return WatermarkBitExtractor(argc, argv).run();
}
/******************************************************************************
Namespace
******************************************************************************/

/*****************************************************************************/
