/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/AccuracyAnalyzer/src/com/haathi/watermark/analyzer/AccuracyAnalyzer.java#6 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.HashMap;

import com.haathi.performance.SharedAnalyzer;
import com.haathi.watermark.analyzer.accuracy.BaselineAnalyzer;
import com.haathi.watermark.analyzer.accuracy.BgLuminanceAnalyzer;
import com.haathi.watermark.analyzer.accuracy.CharacterWidthAnalyzer;
import com.haathi.watermark.analyzer.accuracy.FgLuminanceAnalyzer;
import com.haathi.watermark.analyzer.accuracy.SynonymAnalyzer;
import com.haathi.watermark.analyzer.accuracy.SchemeAnalyzer;
import com.haathi.watermark.analyzer.accuracy.WordSpacingAnalyzer;
import com.haathi.watermark.analyzer.util.ConfusionMatrix;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class AccuracyAnalyzer extends SharedAnalyzer {
	protected String[] args;
	protected boolean validSyntax;
	protected boolean validFiles;
	
	protected String pathToInWm;
	protected String pathToInTxt;
	protected String pathToOutWm;
	protected String pathToOutTxt;
	
	public AccuracyAnalyzer(String[] args) {
		this.args = args;
		validSyntax = validateSyntax();
		if (validSyntax)
			validFiles = validateFiles();
		if (!isValid())
			printUsage();
	}
	
	public AccuracyAnalyzer(String inWm, String inTxt, String outWm, String outTxt) {
		pathToInWm = inWm;
		pathToInTxt = inTxt;
		pathToOutWm = outWm;
		pathToOutTxt = outTxt;
		
		validSyntax = true;
		if (validSyntax)
			validFiles = validateFiles();
		if (!isValid())
			printUsage();
	}
	
	protected boolean validateSyntax() {
		if (args.length != 4)
			return false;
		pathToInWm   = args[0];
		pathToInTxt  = args[1];
		pathToOutWm  = args[2];
		pathToOutTxt = args[3];
		return true;
	}

	protected boolean validateFiles() {
		boolean valid = true;
		
		valid &= fileExists(pathToInWm);
		valid &= fileExists(pathToInTxt);
		valid &= fileExists(pathToOutWm);
		valid &= fileExists(pathToOutTxt);
		return valid;
	}
	
	protected boolean isValid() {
		return validSyntax && validFiles;
	}
	
	protected void printUsage() {
		System.err.println(
				"Usage: " + this.getClass().getName() +  " <in.wm> <in.txt> <out.wm> <out.txt>\n" +
				"Where:  in.wm  is the file produced by autoPdf containing the inserted watermark bits,\n" +
				"        in.txt contains the inserted text as recorded by autoPdf,\n" +
				"       out.wm  is the file produced by watermarkBitExtractor with the extracted watermark bits, and\n" +
				"       out.txt contains the extracted text as recorded by watermarkBitExtractor."
		);
	}
	
	protected SchemeAnalyzer newWatermarkAnalyzer(String scheme) throws Exception {
		SchemeAnalyzer watermarkAnalyzer = null;
		
		if (FgLuminanceAnalyzer.ABBREV.equals(scheme))
			watermarkAnalyzer = new FgLuminanceAnalyzer();
		else if (BgLuminanceAnalyzer.ABBREV.equals(scheme))
			watermarkAnalyzer = new BgLuminanceAnalyzer();
		else if (SynonymAnalyzer.ABBREV.equals(scheme))
			watermarkAnalyzer = new SynonymAnalyzer();
		else if (BaselineAnalyzer.ABBREV.equals(scheme))
			watermarkAnalyzer = new BaselineAnalyzer();	
		else if (WordSpacingAnalyzer.ABBREV.equals(scheme))
			watermarkAnalyzer = new WordSpacingAnalyzer();
		else if (CharacterWidthAnalyzer.ABBREV.equals(scheme))
			watermarkAnalyzer = new CharacterWidthAnalyzer();
		else
			throw new Exception("Unknown watermarking scheme.");
		return watermarkAnalyzer;
	}
	
	protected ConfusionMatrix.Accuracy analyzeWatermarkBits(String inTxt, String outTxt, String scheme, String inBits, String outBits) throws Exception {
		SchemeAnalyzer watermarkAnalyzer = newWatermarkAnalyzer(scheme);
		
		if (watermarkAnalyzer == null)
			return null;
		return watermarkAnalyzer.analyzeAccuracy(inTxt, outTxt, scheme, inBits, outBits);
	}

	protected ConfusionMatrix.Accuracy analyzeAccuracy(String[] inTxts, String[] outTxts, String scheme, String[] inBits, String[] outBits) throws Exception {
		ConfusionMatrix.Accuracy accuracies = new ConfusionMatrix.Accuracy();
		
		for (int i = 0; i < inTxts.length; i++) {
			ConfusionMatrix.Accuracy accuracy = analyzeWatermarkBits(inTxts[i], outTxts[i], scheme, inBits[i], outBits[i]);
			
			accuracies.add(accuracy);
		}
		return accuracies;
	}
	
	public HashMap<String, ConfusionMatrix.Accuracy> run() throws Exception {
		HashMap<String, ConfusionMatrix.Accuracy> result = new HashMap<String, ConfusionMatrix.Accuracy>();
		
		if (!isValid())
			return result;
		
		BufferedReader inWmBufferedReader = null;
		BufferedReader outWmBufferedReader = null;
		
		try {
			String inTxt = readAllBytes(pathToInTxt);
			String outTxt = readAllBytes(pathToOutTxt);
			// Allow 10% insertion of characters, but no more.  Otherwise,
			// the characters match too far down the document and the
			// deletions do not cluster all at the end.

//			int maxLength = (int) (outTxt.length() * 1.1);			
//			if (inTxt.length() > maxLength)
//				inTxt = inTxt.substring(0, maxLength);
			
			inWmBufferedReader = new BufferedReader(new FileReader(pathToInWm));
			outWmBufferedReader = new BufferedReader(new FileReader(pathToOutWm));
		
			ArrayList<String> inHeaders = getHeaders(inWmBufferedReader);
			ArrayList<String> outHeaders = getHeaders(outWmBufferedReader);
			
			while (true) {
				String inLine = inWmBufferedReader.readLine();
				String outLine = outWmBufferedReader.readLine();
				
				if (inLine == null || outLine == null)
					break;
				String[] inSubstrings = inLine.split("\t");
				String[] outSubstrings = outLine.split("\t");
				
				// id, abbrev, vis, pp, ppBits, allBits
				// id, abbrev, vis, bits
				
				int idIndex = inHeaders.indexOf("id");
				if (!inSubstrings[idIndex].equals(outSubstrings[idIndex]))
					throw new Exception("The scheme IDs don't match up.");
				
				int abbrevIndex = inHeaders.indexOf("abbrev");
				if (!inSubstrings[abbrevIndex].equals(outSubstrings[abbrevIndex]))
					throw new Exception("The scheme abbreviations don't match up.");
				
				int visIndex = inHeaders.indexOf("vis");
				if (!inSubstrings[visIndex].equals(outSubstrings[visIndex]))
					throw new Exception("Visibilities don't match up.");
				
				String schemeAbbrev = inSubstrings[abbrevIndex];

				int inBitsIndex = inHeaders.indexOf("allBits");
				int outBitsIndex = outHeaders.indexOf("bits");
				int inPages = inSubstrings.length - inBitsIndex;
				int outPages = outSubstrings.length - outBitsIndex;
				
				if (inPages != outPages)
					throw new Exception("The number of input and output pages don't match.");
	
				String[] inTxts = inTxt.split("\t");
				String[] outTxts = outTxt.split("\t");
				if (inTxts.length != inPages || outTxts.length != outPages)
					throw new Exception("The text and watermark files indicate different numbers of pages.");

				String[] inWms = new String[inPages];
				String[] outWms = new String[outPages];
				for (int i = 0; i < inPages; i++)
					inWms[i] = inSubstrings[inBitsIndex + i];
				for (int i = 0; i < outPages; i++)
					outWms[i] = outSubstrings[outBitsIndex + i];
				
				//ConfusionMatrix.Accuracy accuracy = analyzeAccuracy(inTxt, outTxt, schemeAbbrev, inSubstrings[2], outSubstrings[2]);
				ConfusionMatrix.Accuracy accuracy = analyzeAccuracy(inTxts, outTxts, schemeAbbrev, inWms, outWms);
				
				if (accuracy != null)
					result.put(schemeAbbrev, accuracy);
			}
		}
		finally {
			if (inWmBufferedReader != null)
				inWmBufferedReader.close();
			if (outWmBufferedReader != null)
				outWmBufferedReader.close();
		}
		return result;
	}
	
	public static void main(String[] args) {
		AccuracyAnalyzer accuracyAnalyzer = new AccuracyAnalyzer(args);
		
		try {
			accuracyAnalyzer.run();
		}
		catch (Exception e) {
			e.printStackTrace();
			System.exit(-1);
		}
	}
}
/* **************************************************************************/
