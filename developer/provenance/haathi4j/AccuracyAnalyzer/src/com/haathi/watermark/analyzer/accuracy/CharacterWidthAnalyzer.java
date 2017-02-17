/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/accuracy/CharacterWidthAnalyzer.java#5 $
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
public class CharacterWidthAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "CW";
		
	public ConfusionMatrix.Accuracy analyzeAccuracy(String inTxt, String outTxt, String scheme, String inBits, String outBits) {
		MED med = new MED(inTxt, outTxt);
		med.measure();

		ArrayList<Edit> edits = analyzeOCR(med);
		med.printEditsOn(System.out, true);
		med.printSummaryOn(System.out);
		
		ConfusionMatrix<Character> confusionMatrix = newConfusionMatrix(edits, inBits, outBits);
		confusionMatrix.printOn(System.out, ABBREV);
		ConfusionMatrix.Accuracy accuracy = confusionMatrix.getAccuracy(new Character[] { '0', '1' });
		// Because the "bits" come in pairs, the actual counts are only half as much
		accuracy.goodCount = accuracy.goodCount / 2 + accuracy.goodCount % 2;
		accuracy.allCount = accuracy.allCount / 2 + accuracy.allCount % 2;
		return accuracy;
	}
}
/* **************************************************************************/
