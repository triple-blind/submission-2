/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionAnalyzer/src/com/haathi/watermark/analyzer/precision/SynonymAnalyzer.java#1 $
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
public class SynonymAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "Syn";
	
	// Need to remove spaces
	public String prepareWmIn(String wmIn) {
		// Watermarks are separated by spaces and - indicates unwatermarked words
		return wmIn.replace(" ", "").replace("-",  "");
	}
	
	public String prepareWmOut(String wmOut) {
		// Watermarks are separated by spaces and - indicates unwatermarked words
		return wmOut.replace(" ", "").replace("-",  "");
	}
}
/* **************************************************************************/
