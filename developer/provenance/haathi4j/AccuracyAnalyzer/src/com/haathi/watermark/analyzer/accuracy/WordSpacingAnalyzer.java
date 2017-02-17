/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/accuracy/WordSpacingAnalyzer.java#3 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer.accuracy;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.PrintStream;
import java.util.ArrayList;

import com.haathi.watermark.analyzer.util.ConfusionMatrix;
import com.haathi.watermark.analyzer.util.Edit;
import com.haathi.watermark.analyzer.util.Escaper;
import com.haathi.watermark.analyzer.util.MED;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class WordSpacingAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "WS";
	
	protected void printEditsOn(PrintStream printStream, ArrayList<Edit> edits, 
			String inBits, String outBits, String inTxt, String outTxt) {

		for (Edit edit: edits) {
			char inBit;
			char outBit;
			char inChar;
			char outChar;
			
			if (edit.getType() == MED.INSERTION) {
				inBit = '_';
				outBit = outBits.charAt(edit.getPrevTargetIndex());
				
				inChar = '_';
				outChar = outTxt.charAt(edit.getPrevTargetIndex());
			}	
			else if (edit.getType() == MED.DELETION) {
				inBit = inBits.charAt(edit.getPrevSourceIndex());
				outBit = '_';
				
				inChar = inTxt.charAt(edit.getPrevSourceIndex());
				outChar = '_';
			}
			else if (edit.getType() == MED.SUBSTITUTION) {				
				inBit = inBits.charAt(edit.getPrevSourceIndex());
				outBit = outBits.charAt(edit.getPrevTargetIndex());
				
				inChar = inTxt.charAt(edit.getPrevSourceIndex());
				outChar = outTxt.charAt(edit.getPrevTargetIndex());				
			}
			else
				continue;
			printStream.println("Compare " + Escaper.escape(inChar) + " to " + Escaper.escape(outChar) + " with bits " + inBit + " and " + outBit);
		}
	}
	
	public ConfusionMatrix.Accuracy analyzeAccuracy(String inTxt, String outTxt, String scheme, String inBits, String outBits) {
		MED med = new MED(inTxt, outTxt);
		med.measure();
		
		ArrayList<Edit> edits = analyzeOCR(med);
		//printEditsOn(System.out, edits, inBits, outBits, inTxt, outTxt);
		med.printSummaryOn(System.out);
		
		ConfusionMatrix<Character> confusionMatrix = newConfusionMatrix(edits, inBits, outBits);
		confusionMatrix.printOn(System.out, ABBREV);
		return confusionMatrix.getAccuracy(new Character[] { '0', '1' });
	}
}
/* **************************************************************************/
