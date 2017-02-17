/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionAnalyzer/src/com/haathi/watermark/analyzer/precision/BaselineAnalyzer.java#1 $
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
public class BaselineAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "BL";
	
	public String prepareWmIn(String wmIn) {
		// Whitespace is recorded as -
		return wmIn.replace("-",  "");
	}
	
	public String prepareWmOut(String wmOut) {
		// Whitespace is recorded as - and some are ambiguous
		return wmOut.replace("-",  "").replace("?",  "");
	}
}
/* **************************************************************************/
