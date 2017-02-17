/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionAnalyzer/src/com/haathi/watermark/analyzer/ExactPrecisionAnalyzer.java#6 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;

import com.haathi.watermark.analyzer.precision.BaselineAnalyzer;
import com.haathi.watermark.analyzer.precision.BgLuminanceAnalyzer;
import com.haathi.watermark.analyzer.precision.CharacterWidthAnalyzer;
import com.haathi.watermark.analyzer.precision.FgLuminanceAnalyzer;
import com.haathi.watermark.analyzer.precision.SchemeAnalyzer;
import com.haathi.watermark.analyzer.precision.SynonymAnalyzer;
import com.haathi.watermark.analyzer.precision.WordSpacingAnalyzer;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class ExactPrecisionAnalyzer extends PrecisionAnalyzer {
	static final String TOTAL = "Total";
	static final int WM_LENGTH = 64;
	
	protected String[] args;
	protected boolean validSyntax;
	protected boolean validFiles;
	
	protected String pathToInWm;
	protected String pathToOutWm;
	protected String pathToRndWm;
	
	public ExactPrecisionAnalyzer(String[] args) {
		this.args = args;
		validSyntax = validateSyntax();
		if (validSyntax)
			validFiles = validateFiles();
		if (!isValid())
			printUsage();
	}
	
	public ExactPrecisionAnalyzer(String inWm, String outWm, String rndWm) {
		pathToInWm = inWm;
		pathToOutWm = outWm;
		pathToRndWm = rndWm;
		
		validSyntax = true;
		if (validSyntax)
			validFiles = validateFiles();
		if (!isValid())
			printUsage();
	}
	
	protected boolean validateSyntax() {
		if (args.length != 3)
			return false;
		pathToInWm   = args[0];
		pathToOutWm  = args[1];
		pathToRndWm  = args[2];
		return true;
	}
	
	protected boolean fileExists(String path) {
		File file = new File(path);
		
		return file.exists() && !file.isDirectory(); 
	}
	
	protected boolean validateFiles() {
		boolean valid = true;
		
		valid &= fileExists(pathToInWm);
		valid &= fileExists(pathToOutWm);
		valid &= fileExists(pathToRndWm);
		return valid;
	}
	
	protected boolean isValid() {
		return validSyntax && validFiles;
	}
	
	protected void printUsage() {
		System.err.println(
				"Usage: " + this.getClass().getName() +  " <in.wm> <out.wm> <rnd.wm>\n" +
				"Where:  in.wm is the file produced by autoPdf containing the inserted watermark bits,\n" +
				"       out.wm is the file produced by watermarkBitExtractor with the extracted watermark bits, and\n" +
				"       rnd.wm is the file produced by autoRnd that contains a randomly generated watermark sequence, or\n" +
				"              alternatively, from the traitorTracer."
		);
	}
	
	protected SchemeAnalyzer newSchemeAnalyzer(String scheme) {
		SchemeAnalyzer schemeAnalyzer = null;
		
		if (FgLuminanceAnalyzer.ABBREV.equals(scheme))
			schemeAnalyzer = new FgLuminanceAnalyzer();
		else if (BgLuminanceAnalyzer.ABBREV.equals(scheme))
			schemeAnalyzer = new BgLuminanceAnalyzer();
		else if (SynonymAnalyzer.ABBREV.equals(scheme))
			schemeAnalyzer = new SynonymAnalyzer();
		else if (BaselineAnalyzer.ABBREV.equals(scheme))
			schemeAnalyzer = new BaselineAnalyzer();	
		else if (WordSpacingAnalyzer.ABBREV.equals(scheme))
			schemeAnalyzer = new WordSpacingAnalyzer();
		else if (CharacterWidthAnalyzer.ABBREV.equals(scheme))
			schemeAnalyzer = new CharacterWidthAnalyzer();
		return schemeAnalyzer;
	}
	
	public String readAllBytes(String path) throws IOException {
		byte[] encoded = Files.readAllBytes(Paths.get(path));
		
		return new String(encoded, StandardCharsets.UTF_8);
	}
	
	protected long parseLong(String value) {
		long result;

		// It doesn't matter as long as this is consistent, but the watermark schemes
		// insert the low-value bits into the document first and we'll keep it this way.
		value = new StringBuilder(value).reverse().toString();
		if (value.charAt(0) == '0')
			result = Long.parseLong(value, 2);
		else {
			result = -Long.parseLong(value.substring(1), 2);
			// Two's complement
			result--;
		}
		return result;
	}
	
	protected void addGood(String bits, HashSet<Long> watermarks) {
		int count = bits.length() / WM_LENGTH;
		
		for (int i = 0; i < count; i++) {
			String bitString = bits.substring(i * WM_LENGTH, (i + 1) * WM_LENGTH);
			Long watermark = parseLong(bitString);

			watermarks.add(watermark);
		}
	}
	
	protected void addAll(String bits, HashSet<Long> watermarks) {
		for (int i = 0; i < bits.length() - WM_LENGTH; i++) {
			String bitString = bits.substring(i, i + WM_LENGTH);
			Long watermark = parseLong(bitString);

			watermarks.add(watermark);
		}
	}
	
	protected void addBad(String bits, HashSet<Long> goodWatermarks, HashSet<Long> badWatermarks) {
		for (int i = 0; i < bits.length() - WM_LENGTH; i++) {
			String bitString = bits.substring(i, i + WM_LENGTH);
			Long watermark = parseLong(bitString);

			if (!goodWatermarks.contains(watermark))
				badWatermarks.add(watermark);
			else
				if (i % WM_LENGTH != 0)
					System.out.println("Interesting internal watermark overlap");
		}
	}
	
	protected void analyzePrecision(HashMap<String, HashSet<Long>> goodIns, HashMap<String, HashSet<Long>> badIns,
			HashMap<String, HashSet<Long>> allOuts, HashMap<String, HashSet<Long>> allRnds) {
		
		for (String key: goodIns.keySet()) {
			int truePositiveCount = 0;
			int falsePositiveCount = 0;

			HashSet<Long> goodIn = goodIns.get(key);
			HashSet<Long> allOut = allOuts.get(key);
			HashSet<Long> rnd = allRnds.get(key);
	
			for (Long out: allOut) {
				boolean positive = rnd.contains(out);
				
				if (positive) {
					boolean truePositive = goodIn.contains(out);
					
					if (truePositive)
						truePositiveCount++;
					else
						falsePositiveCount++;
				}
			}
			precisions.put(key, (double) truePositiveCount / (truePositiveCount + falsePositiveCount));
			recalls.put(key, (double) truePositiveCount / goodIn.size());
			counts.put(key, goodIn.size());
		}
	}
	
	public double getPrecision() {
		return precisions.get(TOTAL);
	}
	
	public double getRecall() {
		return recalls.get(TOTAL);
	}
	
	public int getCount() {
		return counts.get(TOTAL);
	}
	
	public HashMap<String, Double> run(boolean fullPages) throws IOException {		
		if (!isValid())
			return precisions;

		HashMap<String, HashSet<Long>> goodIns = new HashMap<String, HashSet<Long>>();
		HashMap<String, HashSet<Long>> badIns = new HashMap<String, HashSet<Long>>();
		HashMap<String, HashSet<Long>> allOuts = new HashMap<String, HashSet<Long>>();
		HashMap<String, HashSet<Long>> allRnds = new HashMap<String, HashSet<Long>>();
						
		HashSet<Long> totalGoodIn = new HashSet<Long>();
		HashSet<Long> totalBadIn = new HashSet<Long>();
		HashSet<Long> totalAllOut = new HashSet<Long>();
		HashSet<Long> totalRnd = new HashSet<Long>();
		
		goodIns.put(TOTAL, totalGoodIn);
		badIns.put(TOTAL, totalBadIn);
		allOuts.put(TOTAL, totalAllOut);
		allRnds.put(TOTAL, totalRnd);
		
		BufferedReader inWmBufferedReader = null;
		BufferedReader outWmBufferedReader = null;
		BufferedReader rndWmBufferedReader = null;
		
		try {
			inWmBufferedReader = new BufferedReader(new FileReader(pathToInWm));
			outWmBufferedReader = new BufferedReader(new FileReader(pathToOutWm));
			rndWmBufferedReader = new BufferedReader(new FileReader(pathToRndWm));

			inWmBufferedReader.readLine();
			outWmBufferedReader.readLine();
			rndWmBufferedReader.readLine();
			
			while (true) {
				String inLine = inWmBufferedReader.readLine();
				String outLine = outWmBufferedReader.readLine();
				String rndLine = rndWmBufferedReader.readLine();
				
				if (inLine == null || outLine == null || rndLine == null)
					break;
				String[] inSubstrings = inLine.split("\t");
				String[] outSubstrings = outLine.split("\t");
				String[] rndSubstrings = rndLine.split("\t");
				
				if (inSubstrings.length != 3 || outSubstrings.length != 3 || rndSubstrings.length != 3)
					break;
				if (!inSubstrings[0].equals(outSubstrings[0]))
					break;
				if (!inSubstrings[1].equals(outSubstrings[1]))
					break;

				if (!rndSubstrings[0].equals(outSubstrings[0]))
					break;
				if (!rndSubstrings[1].equals(outSubstrings[1]))
					break;
				
				HashSet<Long> goodIn = new HashSet<Long>();
				goodIns.put(inSubstrings[1], goodIn);
		
				HashSet<Long> allOut = new HashSet<Long>();
				allOuts.put(inSubstrings[1], allOut);

				HashSet<Long> allRnd = new HashSet<Long>();
				allRnds.put(rndSubstrings[1], allRnd);
				
				SchemeAnalyzer schemeAnalyzer = newSchemeAnalyzer(inSubstrings[1]);

				String wmIn = schemeAnalyzer.prepareWmIn(inSubstrings[2]);
				String wmOut = schemeAnalyzer.prepareWmOut(outSubstrings[2]);
				String rnd = rndSubstrings[2].replaceAll(",", "");
				
				addGood(wmIn, goodIn);
				addAll(wmOut, allOut);
				addGood(rnd, allRnd);
				
				addGood(wmIn, totalGoodIn);
				addAll(wmOut, totalAllOut);
				addGood(rnd, totalRnd);
			}
			
			inWmBufferedReader.close();
			inWmBufferedReader = new BufferedReader(new FileReader(pathToInWm));
			inWmBufferedReader.readLine();
			
			while (true) {
				String inLine = inWmBufferedReader.readLine();
				
				if (inLine == null)
					break;
				String[] inSubstrings = inLine.split("\t");
				
				if (inSubstrings.length != 3)
					break;
				
				HashSet<Long> badIn = new HashSet<Long>();
				badIns.put(inSubstrings[1], badIn);

				SchemeAnalyzer schemeAnalyzer = newSchemeAnalyzer(inSubstrings[1]);

				String wmIn = schemeAnalyzer.prepareWmIn(inSubstrings[2]);

				addBad(wmIn, goodIns.get(inSubstrings[1]), badIn);

				addBad(wmIn, totalGoodIn, totalBadIn);
			}			
		}
		finally {
			if (inWmBufferedReader != null)
				inWmBufferedReader.close();
			if (outWmBufferedReader != null)
				outWmBufferedReader.close();
			if (rndWmBufferedReader != null)
				rndWmBufferedReader.close();
		}
		
		analyzePrecision(goodIns, badIns, allOuts, allRnds);
		return precisions;
	}
	
	public void printOn(PrintStream printStream) {
		printStream.println("Scheme\tCount\tPrecision\tRecall");

		ArrayList<String> keys = new ArrayList<String>(precisions.keySet());

		keys.remove(TOTAL);
		Collections.sort(keys);
		keys.add(TOTAL);
		
		for (String key: keys) {
			printStream.print(key);
			printStream.print('\t');
			printStream.print(counts.get(key));
			printStream.print('\t');
			printStream.print(precisions.get(key));
			printStream.print('\t');
			printStream.println(recalls.get(key));	
		}
	}
	
	public static void main(String[] args) {
		ExactPrecisionAnalyzer precisionAnalyzer = new ExactPrecisionAnalyzer(args);
		
		try {
			precisionAnalyzer.run();
			
			precisionAnalyzer.printOn(System.out);
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}
}
/* **************************************************************************/
