/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/accuracy/BgLuminanceAnalyzer.java#3 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer.accuracy;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.util.ArrayList;

import com.haathi.watermark.analyzer.util.ConfusionMatrix;
import com.haathi.watermark.analyzer.util.Edit;
import com.haathi.watermark.analyzer.util.MED;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class BgLuminanceAnalyzer extends LuminanceAnalyzer {
	public static final String ABBREV = "BG";

	public ConfusionMatrix.Accuracy analyzeAccuracy(String inTxt, String outTxt, String scheme, String inBits, String outBits) {
		MED med = new MED(inTxt, outTxt);
		med.measure();

		ArrayList<Edit> edits = analyzeOCR(med);
		med.printEditsOn(System.out, true);
		med.printSummaryOn(System.out);
		
		ConfusionMatrix<Character> confusionMatrix = newConfusionMatrix(edits, inBits, outBits);
		confusionMatrix.printOn(System.out, ABBREV);
		return confusionMatrix.getAccuracy(new Character[] { '0', '1' });
	}
}
/* **************************************************************************/
