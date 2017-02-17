/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/TraitorAnalyzer/src/com/haathi/watermark/analyzer/tracer/SchemeAnalyzer.java#1 $
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
public abstract class SchemeAnalyzer {
	
	public String prepareWmcOut(String[] wmOuts, int first, int last) {
		StringBuffer stringBuffer = new StringBuffer();
		
		for (int i = first; i < last; i++)
			stringBuffer.append(prepareWmcOut(wmOuts[i]));
		return stringBuffer.toString();		
	}
	
	public abstract String prepareWmcOut(String wmOut);
}
/* **************************************************************************/
