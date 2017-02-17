/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/AccuracyMeter/src/com/haathi/performance/accuracy/AccuracyMeter.java#14 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.performance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.File;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class SharedAnalyzer extends SharedPerformance {
	
	protected boolean fileExists(String path) {
		File file = new File(path);
		
		return file.exists() && !file.isDirectory(); 
	}

}
/* **************************************************************************/
