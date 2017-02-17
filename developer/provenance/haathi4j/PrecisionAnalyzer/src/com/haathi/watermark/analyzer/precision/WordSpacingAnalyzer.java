/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionAnalyzer/src/com/haathi/watermark/analyzer/precision/WordSpacingAnalyzer.java#2 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer.precision;
/* ***************************************************************************
 Imports
 ****************************************************************************/

/* ***************************************************************************
 Class
 ****************************************************************************/
public class WordSpacingAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "WS";

	public String prepareWmIn(String wmIn) {
		// Whitespace is recorded as -
		return wmIn.replace("-",  "");
	}
	
	public String prepareWmOut(String wmOut) {
		// Whitespace is recorded as - except as ? at ends of lines
		// At ends of lines just guess some random bit to preserve alignment.
		// That random number is now 1.
		return wmOut.replace("-",  "").replace("?",  "1");
	}
}
/* **************************************************************************/
