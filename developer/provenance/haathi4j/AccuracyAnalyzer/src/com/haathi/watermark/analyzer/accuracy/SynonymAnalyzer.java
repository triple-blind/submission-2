/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/accuracy/SynonymAnalyzer.java#3 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer.accuracy;

import java.util.ArrayList;

import com.haathi.watermark.analyzer.util.ConfusionMatrix;
import com.haathi.watermark.analyzer.util.Edit;
import com.haathi.watermark.analyzer.util.MED;
/* ***************************************************************************
 Imports
 ****************************************************************************/

/* ***************************************************************************
 Class
 ****************************************************************************/
public class SynonymAnalyzer extends SchemeAnalyzer {
	public static final String ABBREV = "Syn";
	
	protected String getBits(String values) {
		StringBuffer stringBuffer = new StringBuffer();
		
		for (int i = 0; i < values.length(); i++) {
			char c = values.charAt(i);

			if (c == '0' || c == '1')
				stringBuffer.append(c);
		}
		return stringBuffer.toString();
	}
	
	public void analyzeAccuracy2(String inTxt, String outTxt, String scheme, String inValues, String outValues) {
		int in0out0 = 0;
		int in0out1 = 0;
		int in1out0 = 0;
		int in1out1 = 0;
		int inXout0 = 0; // No input, but did get extra output 0
		int inXout1 = 0; // No input, but did get extra output 1
		
		int count = 0;
		
		// We don't really know if outValues are shorter because the document is shorter or because the
		// OCR was bad and words couldn't be identified so that bits were not output.  This makes accurate
		// comparison difficult in the general case.  For this test the input words could be stopped at the
		// end of the first page or the OCR could work across the entire document.  As it is, the out document
		// is shorter than the in document.  Perhaps that can be addressed elsewhere.
		String inBits = getBits(inValues);
		String outBits = getBits(outValues);
		int minLength = Math.min(inBits.length(), outBits.length());
		
		if (inBits.length() < outBits.length())
			System.err.println("Fewer input watermarks than output.");
			
		for (int i = 0; i < minLength; i++) {
			char inBit = inBits.charAt(i);
			char outBit = outBits.charAt(i);

			if (inBit == '0' && outBit == '0')
				in0out0++;
			else if (inBit == '0' && outBit == '1') {
				// Would like to know the characters involved
				in0out1++;
			}
			else if (inBit == '1' && outBit == '0') {
				// Would like to know the characters involved
				in1out0++;
			}
			else if (inBit == '1' && outBit == '1')
				in1out1++;
			else {
				System.err.println("Something strange happened with \"" + inBit + "\" and \"" + outBit + "\".");
			}
			count++;
		}
		
		for (int i = minLength; i < outBits.length(); i++) {
			char outBit = outBits.charAt(i);

			if (outBit == '0')
				inXout0++;
			else if (outBit == '1')
				inXout1++;
			count++;
		}

		System.out.println("scheme\tcount\tin0out0\tin0out1\tin1out0\tin1out1\tinXout0\tinXout1\tin0outX\tin1OutX\tinXoutX");	
		System.out.println(scheme + "\t" + count + "\t" +
				in0out0 + "\t" + in0out1 + "\t" + in1out0 + "\t" + in1out1 + "\t" +
				inXout0 + "\t" + inXout1 + "\t" + 0 + "\t" + 0 + "\t" + 0);
	}
	
	public ConfusionMatrix.Accuracy analyzeAccuracy(String inTxt, String outTxt, String scheme, String inValues, String outValues) {
		String inBits = getBits(inValues);
		String outBits = getBits(outValues);

		MED med = new MED(inBits, outBits);
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
