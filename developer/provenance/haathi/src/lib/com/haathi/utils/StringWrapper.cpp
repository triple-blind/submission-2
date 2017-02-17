/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/utils/StringWrapper.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/utils/StringWrapper.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "Config.hpp"
/******************************************************************************
Using
******************************************************************************/

/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace utils {
/******************************************************************************
Content
******************************************************************************/
#if OSYS == OSYS_WIN

OUString withoutCrLf(OUString text) {
	const sal_Unicode* str = text.getStr();
	rtl::OUStringBuffer buffer;

	for (int i = 0; i < text.getLength(); i++) {
		buffer.append(str[i]);
		if (str[i] == '\r' && i + 1 < text.getLength() && str[i + 1] == '\n')
			i++; // skip the Lf
	}
	return buffer.makeStringAndClear();
}

#else

OUString withoutCrLf(OUString text) {
	return text;
}

#endif

OUString escape(OUString text) {
	const sal_Unicode* str = text.getStr();
	rtl::OUStringBuffer buffer;

	for (int i = 0; i < text.getLength(); i++)
		switch (str[i]) {
			case '\t':
				buffer.appendAscii("\\t");
				break;
			case '\n':
				buffer.appendAscii("\\n");
				break;
			case '\r':
				buffer.appendAscii("\\r");
				break;
			case '\\':
				buffer.appendAscii("\\\\");
				break;
			default:
				buffer.append(str[i]);
		}
	return buffer.makeStringAndClear();
}

OUString unescape(OUString text) {
	const sal_Unicode* str = text.getStr();
	rtl::OUStringBuffer buffer;

	for (int i = 0; i < text.getLength(); i++)
		if (str[i] == '\\' && i + 1 < text.getLength())
			switch (str[++i]) {
				case 't':
					buffer.appendAscii("\t");
					break;
				case 'n':
					buffer.appendAscii("\n");
					break;
				case 'r':
					buffer.appendAscii("\r");
					break;
				case '\\':
					buffer.appendAscii("\\");
					break;
				default:
					buffer.append(str[i - 1]);
					buffer.append(str[i]);
			}
		else
			buffer.append(str[i]);
	return buffer.makeStringAndClear();
}
/******************************************************************************
Namespace
******************************************************************************/
		};	
	};
};
/*****************************************************************************/
