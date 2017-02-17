/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/accuracy/SchemeAnalyzer.java#2 $
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
public abstract class SchemeAnalyzer {
	
	public abstract ConfusionMatrix.Accuracy analyzeAccuracy(String inTxt, String outTxt, String scheme, String inBits, String outBits);
	
	protected ArrayList<Edit> analyzeOCR(MED med) {
		ArrayList<Edit> edits = med.getEdits();
		
		// Want to remove trailing deletions
		for (int i = edits.size() - 1; i >= 0; i--)
			if (edits.get(i).getType() == MED.DELETION)
				edits.remove(i);
			else
				break;
		return edits;
	}
	
	protected ConfusionMatrix<Character> newConfusionMatrix(ArrayList<Edit> edits,
			String inBits, String outBits) {
		ConfusionMatrix<Character> confusionMatrix = new ConfusionMatrix<Character>();
		
		for (Edit edit: edits) {
			char inBit;
			char outBit;
				
			if (edit.getType() == MED.INSERTION) {
				inBit = '_';
				outBit = outBits.charAt(edit.getPrevTargetIndex());
			}	
			else if (edit.getType() == MED.DELETION) {
				inBit = inBits.charAt(edit.getPrevSourceIndex());			
				outBit = '_';
			}
			else {				
				inBit = inBits.charAt(edit.getPrevSourceIndex());
				outBit = outBits.charAt(edit.getPrevTargetIndex());
			}
			confusionMatrix.add(inBit, outBit);
		}
		return confusionMatrix;
	}	
}
/* **************************************************************************/
