/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionAnalyzer/src/com/haathi/watermark/analyzer/precision/SchemeAnalyzer.java#2 $
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
public abstract class SchemeAnalyzer {
	public String prepareWmIn(String[] wmIns, int first, int last) {
		StringBuffer stringBuffer = new StringBuffer();
		
		for (int i = first; i < last; i++)
			stringBuffer.append(prepareWmIn(wmIns[i]));
		return stringBuffer.toString();
	}
	
	public abstract String prepareWmIn(String wmIn);
	public abstract String prepareWmOut(String wmOut);
}
/* **************************************************************************/
