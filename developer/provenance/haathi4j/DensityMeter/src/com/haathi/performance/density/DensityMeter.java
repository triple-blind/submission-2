/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/DensityMeter/src/com/haathi/performance/density/DensityMeter.java#9 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.performance.density;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;

import com.haathi.performance.SharedMeter;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class DensityMeter extends SharedMeter {
	// This is the order being used in most other graphs, so reuse it.
	protected static final String[] orderedKeys = new String[] { "FG", "BG", "BL", "CW", "WS", "Syn" };	
	
	protected ArrayList<String> documents;
	protected ArrayList<HashMap<String, Integer>> allCounts;
	protected ArrayList<Integer> charCounts;
	protected ArrayList<Integer> wordCounts;
	protected ArrayList<Integer> ppCounts;
	protected ArrayList<Integer> ppBitCounts;

	public DensityMeter(String[] args) {
		super(args);
		
		documents = new ArrayList<String>();
		allCounts = new ArrayList<HashMap<String, Integer>>();
		charCounts = new ArrayList<Integer>();
		wordCounts = new ArrayList<Integer>();
		ppCounts = new ArrayList<Integer>();
		ppBitCounts = new ArrayList<Integer>();
	}

	protected int count01(String bits) {
		int count = 0;
				
		for (int i = 0; i < bits.length(); i++)
			if (bits.charAt(i) == '0' || bits.charAt(i) == '1')
				count++;
		return count;
	}
	
	protected void collectWmResults(String inWm, String document) throws IOException {
		documents.add(document);
		
		HashMap<String, Integer> counts = new HashMap<String, Integer>();
		allCounts.add(counts);		
		
		BufferedReader inWmBufferedReader = null;
		
		try {
			inWmBufferedReader = new BufferedReader(new FileReader(resDir + inWm));
			ArrayList<String> headers = getHeaders(inWmBufferedReader);

			while (true) {
				String inLine = inWmBufferedReader.readLine();
				
				// id, abbrev, vis, pp, ppBits, allBits
				
				if (inLine == null)
					break;
				String[] inSubstrings = inLine.split("\t");
				if (inSubstrings.length < 6)
					break;
				
				if (ppCounts.size() < documents.size()) {
					int ppIndex = headers.indexOf("pp");
					// Counts are even across a document, so just do this once
					int ppCount = Integer.parseInt(inSubstrings[ppIndex]);
					ppCounts.add(ppCount);
				}
				
				// This is the number of bits over the full pages
				int ppBitsIndex = headers.indexOf("ppBits");
				int ppBitCount = Integer.parseInt(inSubstrings[ppBitsIndex]);
				counts.put(inSubstrings[1], ppBitCount);
			}
		}
		finally {
			if (inWmBufferedReader != null)
				inWmBufferedReader.close();
		}		
	}

	// This should match the SynonymEngine.  We only care about what _we_ think a word is.
	protected int countWords(String text) {
		int count = 0;
		boolean inWord = false;
		
		for (int i = 0; i < text.length(); i++) {
			char c = text.charAt(i);
			
			if (c == 0x20 || c == 0x0d || c == 0x0a) {
				if (inWord) {
					inWord = false;
					count++;
				}
			}
			else
				if (!inWord)
					inWord = true;
		}
		if (inWord)
			count++;
		return count;
	}
	
	protected void collectTxtResults(String inTxt) throws IOException {
		String text = readAllBytes(resDir + inTxt);
		
		text.replace("\t",  ""); // Support multiple pages
		charCounts.add(text.length());
		wordCounts.add(countWords(text));		
	}
	
	protected void printResults(PrintStream printStream) {
		if (allCounts.isEmpty())
			return;
		
		ArrayList<String> headers = new ArrayList<String>(allCounts.get(0).keySet());
		ArrayList<String> orderedKeyList = new ArrayList<String>(Arrays.asList(orderedKeys));
	
		Collections.sort(headers, new Comparator<String>() {
			public int compare(String left, String right) {
				return orderedKeyList.indexOf(left) - orderedKeyList.indexOf(right);
			}
		});
				
		printStream.print("Document");
		printStream.print("\tChars");
		printStream.print("\tWords");
		printStream.print("\tPages");
		printStream.print("\tTotal");
		for (String header: headers)
			printStream.print("\t" + header);
		printStream.println();
		
		for (int i = 0; i < documents.size(); i++) {
			double total = 0;
			
			printStream.print(upToDot(documents.get(i)));
			printStream.print("\t" + charCounts.get(i));
			printStream.print("\t" + wordCounts.get(i));
			printStream.print("\t" + ppCounts.get(i));
		
			for (String header: headers)
				total += (double) allCounts.get(i).get(header) / (ppCounts.get(i) - 1) / 64;
			printStream.print("\t" + total);
			
			for (String header: headers)
				printStream.print("\t" + (double) allCounts.get(i).get(header) / (ppCounts.get(i) - 1) / 64);
			printStream.println();
		}
		printStream.println();
	}
	
	protected void printAtATime(PrintStream printStream) {
		String[] docTxts = new String[] { "DofI.txt" , "Twain.txt", "SofU.txt", "Turing.txt" };
		String docPdf = "doc.pdf";
		String docOdt = "doc.odt";
		String inWm = "in.wm";
		String inTxt = "in.txt";
		int s = 63;
		int v = 2;
		
		try {
			for (String docTxt: docTxts) {				
				eraseResFiles(new String[] { docPdf, docOdt, inWm, inTxt });
				runAutoPdf(s, v, docTxt, docPdf, docOdt, inWm, inTxt);
				collectWmResults(inWm, docTxt);
				collectTxtResults(inTxt);
				eraseResFiles(archive(docTxt, s), new String[] { docPdf, docOdt, inWm, inTxt });
			}
			printResults(printStream);
			eraseResFiles(archive("density", s), new String[] { "dens.dat", "density.png" });
		}
		catch (Exception exception) {
			exception.printStackTrace();
			System.exit(-1);			
		}		
	}
	
	protected void plotAtATime() {
		String densInPlt = "dens.dat";
		
		String densityPlt = "density.plt";
		String densityOutPlt = "density.png";

		try {
			eraseResFiles(new String[] { densInPlt, densityOutPlt });
			
			PrintStream printStream = new PrintStream(new File(resDir + densInPlt));
			printAtATime(printStream);
			printStream.close();
			
			plotResults(densityPlt, densInPlt, densityOutPlt);

			eraseResFiles(archive("dens"), new String[] { densInPlt, densityOutPlt });
		}
		catch (Exception exception) {
			exception.printStackTrace();
			System.exit(-1);			
		}
	}
	
	public void run() {
		printAtATime(System.out);
		//plotAtATime();
	}
	
	public static void main(String[] args) {
		new DensityMeter(args).run();
	}
}
/* **************************************************************************/
