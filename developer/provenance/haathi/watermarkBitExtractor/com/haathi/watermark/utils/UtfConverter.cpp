/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/watermarkBitExtractor/com/haathi/watermark/utils/SynonymDictionary.cpp#3 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/watermark/utils/UtfConverter.hpp"
/******************************************************************************
Include others
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace watermark {
			namespace utils {
/******************************************************************************
Class
******************************************************************************/
std::string UtfConverter::toSingle(std::string letter) {
	static std::string multiLeftApostrophe("\xe2\x80\x98");
	static std::string multiRightApostrophe("\xe2\x80\x9c");
	static std::string anotherApostrophe("\xe2\x80\x99");
	static std::string multiDash("\xe2\x80\x94");
	static std::string multiSection("\xc2\xa7");
	static std::string multiIfLigature("\xef\xac\x81");

	static std::string singleApostrophe("'");
	static std::string singleDash("-");
	static std::string singleQuestion("?");
	static std::string singleF("f");

	if (letter.length() == 1)
		return letter;

	if (letter == multiLeftApostrophe || letter == multiRightApostrophe || letter == anotherApostrophe)
		return singleApostrophe;
	if (letter == multiDash)
		return singleDash;
	if (letter == multiSection)
		return singleQuestion;
	if (letter == multiIfLigature)
		return singleF;

	return singleQuestion;
}

/*
std::string tosingle(std::string letters) {
	static std::string multiLeftApostrophe("\xe2\x80\x98");
	static std::string multiRightApostrophe("\xe2\x80\x9c");
	static std::string anotherApostrophe("\xe2\x80\x99");
	static std::string multiDash("\xe2\x80\x94");
	static std::string multiSection("\xc2\xa7");
	static std::string multiIfLigature("\xef\xac\x81");
	static std::string singleApostrophe("'");
	static std::string singleDash("-");
	static std::string singleQuestion("?");
	static std::string singleF("f");

	if (letters.length() == 1)
		return letters;

	int pos;

	while ((pos = letters.find(multiLeftApostrophe)) != std::string::npos)
		letters = letters.replace(pos, 3, singleApostrophe);
	while ((pos = letters.find(multiRightApostrophe)) != std::string::npos)
		letters = letters.replace(pos, 3, singleApostrophe);
	while ((pos = letters.find(anotherApostrophe)) != std::string::npos)
		letters = letters.replace(pos, 3, singleApostrophe);
	while ((pos = letters.find(multiDash)) != std::string::npos)
		letters = letters.replace(pos, 3, singleDash);
	while ((pos = letters.find(multiSection)) != std::string::npos)
		letters = letters.replace(pos, 2, singleQuestion);
	while ((pos = letters.find(multiIfLigature)) != std::string::npos)
		letters = letters.replace(pos, 3, singleF);
	return letters;
}
*/
/******************************************************************************
Namespace
******************************************************************************/
			};
		};
	};
};
/*****************************************************************************/
