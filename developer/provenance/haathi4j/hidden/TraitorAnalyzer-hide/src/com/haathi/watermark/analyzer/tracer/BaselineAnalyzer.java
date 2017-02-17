/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/TraitorAnalyzer/src/com/haathi/watermark/analyzer/tracer/BaselineAnalyzer.java#1 $
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
public class BaselineAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "BL";
	
	public String prepareWmcOut(String wmOut) {
		// Whitespace is recorded as - and some are ambiguous
		return wmOut.replace("-",  "").replace("?",  "0");
	}
}
/* **************************************************************************/
