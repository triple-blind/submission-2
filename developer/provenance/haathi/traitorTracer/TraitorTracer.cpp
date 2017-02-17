/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/traitorTracer/TraitorTracer.cpp#20 $
/******************************************************************************
Include self
******************************************************************************/

/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/traitor/LocalThreadedLongPopCntTraitorTracer.hpp"
#include "com/haathi/traitor/LongPopCntTraitorTracer.hpp"
#include "com/haathi/traitor/QuickTraitorTracer.hpp"
#include "com/haathi/traitor/TheoryTraitorTracer.hpp"
#include "com/haathi/traitor/ThreadedLongPopCntTraitorTracer.hpp"

#include "com/haathi/traitor/Exhibit.hpp"
#include "com/haathi/traitor/Theory.hpp"
#include "com/haathi/utils/Timer.hpp"

#include <fstream>
#include <sstream>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::Timer;

using namespace com::haathi::traitor;
/******************************************************************************
TraitorTracer
******************************************************************************/
class MainProgram {
	protected:
		int argc;
		char** argv;

	// http://stackoverflow.com/questions/4316442/stdofstream-check-if-file-exists-before-writing
	bool fileExists(std::string& filename) {
		std::ifstream infile(filename.c_str());

		return infile.good();
	}

	public:
		MainProgram(int argc, char** argv): argc(argc), argv(argv) {
		}

		virtual ~MainProgram() {
		}

		virtual void tryRun() {
			try {
				run();
			}
			catch (std::exception& e){
				std::cerr << e.what() << std::endl;
			}
			catch (...) {
				std::cerr << "Unknown Exception..." << std::endl;
			}
		}
		
		virtual void run() = 0;
};

class TraitorTracer: public MainProgram {
	protected:
		bool validSyntax;
		bool validFiles;

		int count;
		std::string pathToOutWm;
		std::string pathToInTh;
		std::string pathToOutEv;

		bool validateSyntax();
		bool validateFiles();

		GenericTraitorTracer* newTraitorTracer(Exhibits& exhibits, Theories& theories);

		Exhibits makeExhibits();
		Theories makeTheories();

		void deleteExhibits(Exhibits& exhibits);
		void deleteTheories(Theories& theories);
	public:
		TraitorTracer(int argc, char** argv): MainProgram(argc, argv) {
			validSyntax = validateSyntax();
			if (!validSyntax)
				printUsage();
			else
				validFiles = validateFiles();
		}

		bool isValid() {
			return validSyntax && validFiles;
		}

		void printUsage();
		void run();
};

void TraitorTracer::printUsage() {
	std::cerr
			<< "Usage: " << argv[0] << " count <out.wmc> <in.th> <out.ev>" << std::endl
			<< "Where: count specifies how many watermarks to search." << std::endl << std::endl

			<< "out.wmc contains the extracted watermark bits output from watermarkBitExtractor" << std::endl
			<< "        but which have been subsequently cleaned by, for example, the Java TraitorTracer," << std::endl
			<< " in.th  lists theories regarding the traitors, and" << std::endl
			<< "out.ev  will detail the evidence collected for each theory." << std::endl;
}

bool TraitorTracer::validateSyntax() {
	int index = 1;

	if (argc != 5)
		return false;

	count = 0;

	for (int i = 0; argv[index][i] != 0; i++) {
		char c = argv[index][i];

		if (c < '0' || '9' < c)
			return false;
		count = count * 10 + (c - '0');
	}
	index++;

	pathToOutWm = std::string(argv[index++]);
	pathToInTh = std::string(argv[index++]);
	pathToOutEv = std::string(argv[index++]);
	return true;
}

bool TraitorTracer::validateFiles() {
	if (!fileExists(pathToOutWm)) {
		std::cerr << "The <out.wmc> file doesn't exist: \"" << pathToOutWm << "\"";
		return false;
	}
	if (!fileExists(pathToInTh)) {
		std::cerr << "The <in.th> file doesn't exist: \"" << pathToInTh << "\"";
		return false;
	}
	if (fileExists(pathToOutEv)) {
		std::cerr << "The <out.ev> file already exists: \"" << pathToInTh << "\"";
		return false;
	}
	return true;
}

GenericTraitorTracer* TraitorTracer::newTraitorTracer(Exhibits& exhibits, Theories& theories) {
	GenericTraitorTracer* traitorTracer;

//	traitorTracer = new QuickTraitorTracer(exhibits, theories);
//	traitorTracer = new LongPopCntTraitorTracer(exhibits, theories);
	traitorTracer = new ThreadedLongPopCntTraitorTracer(exhibits, theories);
//	traitorTracer = new LocalThreadedLongPopCntTraitorTracer(exhibits, theories);
//	traitorTracer = new TheoryTraitorTracer(exhibits, theories);
	return traitorTracer;
}
			
Exhibits TraitorTracer::makeExhibits() {
	Exhibits exhibits;

	try {
		std::ifstream in(pathToOutWm.c_str(), std::ios::in);

		if (!in) {
			throw std::runtime_error("There was a problem with the <out.wmc> file.");
			return exhibits;
		}

		std::string line;
		std::getline(in, line); // skip header

		while (std::getline(in, line)) {
			if (line.empty())
				break;

			std::istringstream iss(line);
			int watermarkId;
			std::string watermarkAbbrev;
			std::string watermarkVis;
			std::string watermarkBits;

			iss >> watermarkId;
			iss >> watermarkAbbrev;
			iss >> watermarkVis;
			iss >> watermarkBits;

			exhibits.push_back(new Exhibit(watermarkId, watermarkAbbrev, watermarkBits));
		}
	}
	catch (...) {
		std::cerr << "Unexpected exception" << std::endl;
	}
	return exhibits;
}

void TraitorTracer::deleteExhibits(Exhibits& exhibits) {
	Exhibits::iterator it;

	for (it = exhibits.begin(); it != exhibits.end(); ++it)
		delete *it;
}

Theories TraitorTracer::makeTheories() {
	Theories theories;

	try {
		std::ifstream in(pathToInTh.c_str(), std::ios::in);

		if (!in) {
			throw std::runtime_error("There was a problem with the <in.th> file.");
			return theories;
		}

		std::string line;
		std::getline(in, line); // skip header

		while (std::getline(in, line)) {
			if (line.empty())
				break;

			std::istringstream iss(line);
			std::string userId;
			std::string documentId;
			std::string versionNo;
			std::string versionId;
			std::string userPrivateKey;

			iss >> userId;
			iss >> documentId;
			iss >> versionNo;
			iss >> versionId;
			iss >> userPrivateKey;

			theories.push_back(new Theory(userId, userPrivateKey, documentId, versionNo, versionId));
		}
	}
	catch (...) {
		std::cerr << "Unexpected exception" << std::endl;
	}

	return theories;
}

void TraitorTracer::deleteTheories(Theories& theories) {
	Theories::iterator it;

	for (it = theories.begin(); it != theories.end(); ++it)
		delete *it;
}

void TraitorTracer::run() {
	if (!isValid())
		return;

	Exhibits exhibits = makeExhibits();
	Theories theories = makeTheories();

	std::ofstream out(pathToOutEv.c_str(), std::ios::out | std::ios::app);
	if (!out) {
		std::cerr << "There was a problem with the <out.ev> file: \"" << pathToOutEv << "\"";
		return;
	}

	GenericTraitorTracer* traitorTracer = newTraitorTracer(exhibits, theories);

	Timer timer;
	timer.start();
	traitorTracer->trace(out, count);
	timer.stop();
	
	std::cout << "Search time: " << timer.getElapsedTime() << std::endl;

	delete traitorTracer;
	deleteTheories(theories);
	deleteExhibits(exhibits);
}

int main(int argc, char** argv) {
	TraitorTracer(argc, argv).tryRun();
}
/******************************************************************************
Namespace
******************************************************************************/

/*****************************************************************************/
