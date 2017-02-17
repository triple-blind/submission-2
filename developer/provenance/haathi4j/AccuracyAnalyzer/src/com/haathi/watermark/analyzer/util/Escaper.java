/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/util/Escaper.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer.util;
/* ***************************************************************************
 Imports
 ****************************************************************************/

/* ***************************************************************************
 Class
 ****************************************************************************/
public class Escaper {

	public static String escape(char c) {
		if (c == '\r')
			return "\\r";
		if (c == '\n')
			return "\\n";
		if (c == '\t')
			return "\\t";			
		if (c == ' ')
			return "\\s";			
		return Character.toString(c);
	}
}	
/* **************************************************************************/
