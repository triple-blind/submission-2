/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/AccuracyAnalyzer/src/com/haathi/watermark/analyzer/accuracy/LuminanceAnalyzer.java#2 $
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
public abstract class LuminanceAnalyzer extends SchemeAnalyzer {

	protected ConfusionMatrix<Character> newConfusionMatrix(ArrayList<Edit> edits,
			String inBits, String outBits) {
		ConfusionMatrix<Character> confusionMatrix = new ConfusionMatrix<Character>();

		for (Edit edit: edits) {
			for (int i = 0; i < 2; i++) {
				char inBit = '\0';
				char outBit = '\0';
				
				if (edit.getType() == MED.INSERTION) {
					inBit = '_';
					outBit = outBits.charAt(edit.getPrevTargetIndex() * 2 + i);
				}	
				else if (edit.getType() == MED.DELETION) {
					inBit = inBits.charAt(edit.getPrevSourceIndex() * 2 + i);			
					outBit = '_';
				}
				else {
					try {
					inBit = inBits.charAt(edit.getPrevSourceIndex() * 2 + i);
					outBit = outBits.charAt(edit.getPrevTargetIndex() * 2 + i);
					}
					catch (StringIndexOutOfBoundsException e){
						System.out.println("Crash!");
						
					}				
				}
				confusionMatrix.add(inBit, outBit);
			}
		}
		return confusionMatrix;
	}
}
/* **************************************************************************/
