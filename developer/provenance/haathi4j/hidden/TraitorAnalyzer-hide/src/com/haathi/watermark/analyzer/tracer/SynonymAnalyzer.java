/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/TraitorAnalyzer/src/com/haathi/watermark/analyzer/tracer/SynonymAnalyzer.java#1 $
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
public class SynonymAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "Syn";
	
	public String prepareWmcOut(String wmOut) {
		// Watermarks are separated by spaces and - indicates unwatermarked words
		return wmOut.replace(" ", "").replace("-",  "");
	}
}
/* **************************************************************************/
