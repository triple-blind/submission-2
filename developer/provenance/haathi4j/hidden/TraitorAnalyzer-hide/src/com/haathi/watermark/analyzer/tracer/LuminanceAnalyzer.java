/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/TraitorAnalyzer/src/com/haathi/watermark/analyzer/tracer/LuminanceAnalyzer.java#1 $
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
public abstract class LuminanceAnalyzer extends SchemeAnalyzer {

	public String prepareWmcOut(String wmOut) {
		// Whitespace is recorded as - and some are ambiguous
		return wmOut.replace("-",  "").replace("?",  "0");
	}
}
/* **************************************************************************/
