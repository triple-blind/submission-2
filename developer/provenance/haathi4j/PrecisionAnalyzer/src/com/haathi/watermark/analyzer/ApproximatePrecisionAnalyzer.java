/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionAnalyzer/src/com/haathi/watermark/analyzer/ApproximatePrecisionAnalyzer.java#8 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintStream;
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
public class ApproximatePrecisionAnalyzer extends PrecisionAnalyzer {
	static final String TOTAL = "Total";
	static final int WM_LENGTH = 64;
	
	protected String[] args;
	protected boolean validSyntax;
	protected boolean validFiles;
	
	protected String pathToInWm;
	protected String pathToOutEv;
	
	public ApproximatePrecisionAnalyzer(String[] args) {
		this.args = args;
		validSyntax = validateSyntax();
		if (validSyntax)
			validFiles = validateFiles();
		if (!isValid())
			printUsage();
	}
	
	public ApproximatePrecisionAnalyzer(String inWm, String outEv) {
		pathToInWm = inWm;
		pathToOutEv = outEv;
		
		validSyntax = true;
		if (validSyntax)
			validFiles = validateFiles();
		if (!isValid())
			printUsage();
	}
	
	protected boolean validateSyntax() {
		if (args.length != 2)
			return false;
		pathToInWm   = args[0];
		pathToOutEv = args[1];
		return true;
	}

	protected boolean validateFiles() {
		boolean valid = true;
		
		valid &= fileExists(pathToInWm);
		valid &= fileExists(pathToOutEv);
		return valid;
	}
	
	protected boolean isValid() {
		return validSyntax && validFiles;
	}
	
	protected void printUsage() {
		System.err.println(
				"Usage: " + this.getClass().getName() +  " <in.wm> <out.ev>\n" +
				"Where:  in.wm is the file produced by autoPdf containing the inserted watermark bits and\n" +
				"       out.ev is the file produced by the traitorTracer containing suspected watermarks."
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
			if (watermark == 8304571097496941580L)
				System.out.println("Found you!");

			
			watermarks.add(watermark);
			
			if (watermark == 8304571097496941580L)
				if (!watermarks.contains(8304571097496941580L))
					System.out.println("Where did it go?");
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
			
			if (watermark == 8304571097496941580L)
				System.out.println("Found you!");

			if (i % WM_LENGTH == 0)
				assert(goodWatermarks.contains(watermark));
			else
				if (goodWatermarks.contains(watermark))
					System.out.println("Interesting internal watermark overlap");
				else
					badWatermarks.add(watermark);
		}
	}
	
	protected void analyzePrecision(HashMap<String, HashSet<Long>> goodIns, HashMap<String, HashSet<Long>> badIns,
			HashMap<String, HashSet<Long>> allEvs) {
		
		for (String key: goodIns.keySet()) {
			int truePositiveCount = 0;
			int falsePositiveCount = 0;

			HashSet<Long> goodIn = goodIns.get(key);
			HashSet<Long> badIn= badIns.get(key);
			HashSet<Long> allEv = allEvs.get(key);
	
			for (Long ev: allEv) {
				if (ev == 8304571097496941580L)
					System.out.println("Found you!");

				if (goodIn.contains(ev))
					truePositiveCount++;
				else
					falsePositiveCount++;
				if (badIn.contains(ev))
					System.err.println("Incorrectly bit-shifted watermark detected.");
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

	public HashMap<String, Double> run(boolean fullPages) throws Exception {		
		if (!isValid())
			return precisions;

		HashMap<String, HashSet<Long>> goodIns = new HashMap<String, HashSet<Long>>();
		HashMap<String, HashSet<Long>> badIns = new HashMap<String, HashSet<Long>>();
		HashMap<String, HashSet<Long>> allEvs = new HashMap<String, HashSet<Long>>();
						
		HashSet<Long> totalGoodIn = new HashSet<Long>();
		HashSet<Long> totalBadIn = new HashSet<Long>();
		HashSet<Long> totalEv = new HashSet<Long>();
		
		goodIns.put(TOTAL, totalGoodIn);
		badIns.put(TOTAL, totalBadIn);
		allEvs.put(TOTAL, totalEv);
		
		BufferedReader inWmBufferedReader = null;
		BufferedReader outEvBufferedReader = null;
		BufferedWriter inWmcBufferedWriter = null;
		
		try {
			inWmBufferedReader = new BufferedReader(new FileReader(pathToInWm));
			outEvBufferedReader = new BufferedReader(new FileReader(pathToOutEv));
			inWmcBufferedWriter = new BufferedWriter(new FileWriter(pathToInWm + "c"));

			ArrayList<String> wmHeaders = getHeaders(inWmBufferedReader);
			ArrayList<String> evHeaders = getHeaders(outEvBufferedReader);
			
			while (true) {
				String wmLine = inWmBufferedReader.readLine();
				String evLine = outEvBufferedReader.readLine();
				
				if (wmLine == null || evLine == null)
					break;
				String[] wmSubstrings = wmLine.split("\t");
				String[] evSubstrings = evLine.split("\t");
				
				// id, abbrev, vis, pp, ppBits, allBits
				// UserId, DocumentId, VersionNo, SchemeId, SchemeAbbrev, count, [idx, rnd, out]...
				
				if (wmSubstrings.length < 6 || evSubstrings.length < 6) // last could be empty
					throw new Exception("Fields are missing in the watermark or evidence files.");
				
				int wmIdIndex = wmHeaders.indexOf("id");
				int evIdIndex = evHeaders.indexOf("SchemeId");
				if (!wmSubstrings[wmIdIndex].equals(evSubstrings[evIdIndex]))
					throw new Exception("The scheme IDs don't match up.");
				
				int wmAbbrevIndex = wmHeaders.indexOf("abbrev");
				int evAbbrevIndex = evHeaders.indexOf("SchemeAbbrev");
				String abbrev = wmSubstrings[wmAbbrevIndex];
				if (!wmSubstrings[wmAbbrevIndex].equals(evSubstrings[evAbbrevIndex]))
					throw new Exception("The scheme abbreviations don't match up.");
				
				HashSet<Long> goodIn = new HashSet<Long>();
				if (goodIns.containsKey(abbrev))
					System.out.println("Double dip!");
				goodIns.put(abbrev, goodIn);
		
				HashSet<Long> allEv = new HashSet<Long>();
				allEvs.put(abbrev, allEv);
				
				SchemeAnalyzer schemeAnalyzer = newSchemeAnalyzer(abbrev);

				int wmBitsIndex = wmHeaders.indexOf("allBits");
				
				int end = wmSubstrings.length;
				if (fullPages)
					end--;

				String wmIn = schemeAnalyzer.prepareWmIn(wmSubstrings, wmBitsIndex, end);
				
				// Write this out to file
				inWmcBufferedWriter.write(abbrev + " after");
				inWmcBufferedWriter.write("\n");
				for (int z = 0; z < wmIn.length() - 64; z += 64) {
					inWmcBufferedWriter.write(wmIn.substring(z, z + 64));
					inWmcBufferedWriter.write("\n");
				}
				inWmcBufferedWriter.write("\n");
				
				int evBitsIndex = evHeaders.indexOf("[idx, rnd, out]...");
				StringBuffer stringBuffer = new StringBuffer();
				for (int i = evBitsIndex; i < evSubstrings.length; i++) {
					String[] fields = evSubstrings[i].split(",");
					
					// Skip the ones that run off the end for now because they lead
					// to false positives.  They really are positive, but the partial
					// positive watermark is not included in the list.
					if (fields[2].endsWith("-"))
						continue;
					if (fields[1].length() != 64)
						continue;
					stringBuffer.append(fields[1]);
				}
				String ev = stringBuffer.toString();
				
				// Add strings starting every 64th to goodIn
				addGood(wmIn, goodIn);
				if (abbrev.equals("CW"))
					if (!goodIns.get("CW").contains(8304571097496941580L))
						System.out.println("Where did it go?");
					
					
				// Add each of the events starting every 64th as well.
				// They have been concatenated together
				addGood(ev, allEv);
				
				// Also add them to the totals
				addGood(wmIn, totalGoodIn);
				addGood(ev, totalEv);
			}
			
			if (!goodIns.get("CW").contains(8304571097496941580L))
				System.out.println("Where did it go?");
			
			inWmBufferedReader.close();
			inWmBufferedReader = new BufferedReader(new FileReader(pathToInWm));
			inWmBufferedReader.readLine();
			
			while (true) {
				String inLine = inWmBufferedReader.readLine();
				
				if (inLine == null)
					break;
				String[] inSubstrings = inLine.split("\t");
				
				if (inSubstrings.length < 6)
					throw new Exception("Fields are missing in the watermark file.");
				
				int wmAbbrevIndex = wmHeaders.indexOf("abbrev");
				String abbrev = inSubstrings[wmAbbrevIndex];
				
				HashSet<Long> badIn = new HashSet<Long>();
				badIns.put(abbrev, badIn);

				SchemeAnalyzer schemeAnalyzer = newSchemeAnalyzer(abbrev);

				int wmBitsIndex = wmHeaders.indexOf("allBits");
				
				int end = inSubstrings.length;
				if (fullPages)
					end--;
				
				String wmIn = schemeAnalyzer.prepareWmIn(inSubstrings, wmBitsIndex, end);

				// Add all the ones _not_ beginning on multiple of the 64th bit
				addBad(wmIn, goodIns.get(abbrev), badIn);
				// Do it to both collections
				addBad(wmIn, totalGoodIn, totalBadIn);
			}			
		}
		finally {
			if (inWmBufferedReader != null)
				inWmBufferedReader.close();
			if (outEvBufferedReader != null)
				outEvBufferedReader.close();
			if (inWmcBufferedWriter != null)
				inWmcBufferedWriter.close();
		}
		
		if (!goodIns.get("CW").contains(8304571097496941580L))
				System.out.println("Where did it go?");
		
		analyzePrecision(goodIns, badIns, allEvs);
		return precisions;
	}
	
	public static void main(String[] args) {
		ApproximatePrecisionAnalyzer precisionAnalyzer = new ApproximatePrecisionAnalyzer(args);
		
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
