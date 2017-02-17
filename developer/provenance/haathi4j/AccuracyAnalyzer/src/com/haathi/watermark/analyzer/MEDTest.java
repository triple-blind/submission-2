/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/MEDTest.java#2 $
*****************************************************************************
Package
****************************************************************************/
package com.haathi.watermark.analyzer;
/* ***************************************************************************
Imports
****************************************************************************/
import static org.junit.Assert.*;

import com.haathi.watermark.analyzer.util.MED;

import org.junit.Test;
/* ***************************************************************************
Class
****************************************************************************/
public class MEDTest { 
	@Test
	public void testMeasure() {
		assertEquals(23, new MED("cat", "cars").measure());
		assertEquals(36, new MED("kitten", "sitting").measure());
		assertEquals(33, new MED("Sunday", "Saturday").measure());
	}
}
/* **************************************************************************/
