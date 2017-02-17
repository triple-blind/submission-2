/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionAnalyzer/src/com/haathi/watermark/analyzer/precision/CharacterWidthAnalyzer.java#2 $
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
public class CharacterWidthAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "CW";

	protected String even(String text) {
		StringBuffer stringBuffer = new StringBuffer();
		
		for (int i = 0; i < text.length(); i += 2) {
			stringBuffer.append(text.charAt(i));
			if (i + 1 < text.length() && text.charAt(i) == text.charAt(i + 1))
				System.err.println("Parity error in wmOut.");
		}
		return stringBuffer.toString();
	}
	
	public String prepareWmIn(String wmIn) {
		// Whitespace and unmarked characters are recorded as -
		return even(wmIn.replace("-",  ""));
	}
	
	public String prepareWmOut(String wmOut) {
		// Whitespace and unmarked characters are recorded as -
		return even(wmOut.replace("-",  "").replace("?",  ""));
	}	
}
/* **************************************************************************/
