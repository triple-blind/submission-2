/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/TraitorTracer/src/com/haathi/watermark/analyzer/tracer/WordSpacingAnalyzer.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer.tracer;
/* ***************************************************************************
 Imports
 ****************************************************************************/

/* ***************************************************************************
 Class
 ****************************************************************************/
public class WordSpacingAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "WS";
	
	public String prepareWmcOut(String wmOut) {
		// Whitespace is recorded as - except as ? at ends of lines
		// At ends of lines just guess some random bit to preserve alignment.
		// That random number is now 1.
		return wmOut.replace("-",  "").replace("?",  "0");
	}
}
/* **************************************************************************/
