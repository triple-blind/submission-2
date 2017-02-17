/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionAnalyzer/src/com/haathi/watermark/analyzer/PrecisionAnalyzer.java#11 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.PrintStream;
import java.util.HashMap;

import com.haathi.performance.SharedAnalyzer;
/* ***************************************************************************
 Class
 ****************************************************************************/
public abstract class PrecisionAnalyzer extends SharedAnalyzer {
	public HashMap<String, Double> precisions = new HashMap<String, Double>();
	public HashMap<String, Double> recalls = new HashMap<String, Double>();
	public HashMap<String, Integer> counts = new HashMap<String, Integer>();
	
	public HashMap<String, Double> run() throws Exception {
		return run(false);
	}
	
	public abstract HashMap<String, Double> run(boolean fullPages) throws Exception;
	public abstract void printOn(PrintStream printStream);
}
/* **************************************************************************/
