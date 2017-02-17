/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionAnalyzer/src/com/haathi/watermark/analyzer/precision/LuminanceAnalyzer.java#1 $
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
public abstract class LuminanceAnalyzer extends SchemeAnalyzer {

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
