/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/TraitorTracer/src/com/haathi/watermark/analyzer/tracer/CharacterWidthAnalyzer.java#1 $
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
public class CharacterWidthAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "CW";

	protected String even(String text) {
		StringBuffer stringBuffer = new StringBuffer();
		
		for (int i = 0; i < text.length(); i += 2)
			stringBuffer.append(text.charAt(i));
		return stringBuffer.toString();
	}
	
	public String prepareWmcOut(String wmOut) {
		// Whitespace and unmarked characters are recorded as -
		return even(wmOut.replace("-",  "").replace("?",  "0"));
	}	
}
/* **************************************************************************/
