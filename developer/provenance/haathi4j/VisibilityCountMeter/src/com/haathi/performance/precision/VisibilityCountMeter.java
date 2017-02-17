/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/PrecisionMeter/src/com/haathi/performance/precision/PrecisionMeter.java#18 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.performance.precision;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import com.haathi.process.ExternalProcess;
import com.haathi.watermark.analyzer.ApproximatePrecisionAnalyzer;
import com.haathi.watermark.analyzer.TraitorAnalyzer;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.PrintStream;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;

import com.haathi.performance.SharedMeter;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class VisibilityCountMeter extends SharedMeter {
	protected static class FullPagePrecisionAnalyzer extends ApproximatePrecisionAnalyzer {
	
		public FullPagePrecisionAnalyzer(String inWm, String outEv) {
			super(inWm, outEv);
		}
		
		public HashMap<String, Double> run() throws Exception {
			return super.run(true);
		}
	}

	public static final String GHOST_SCRIPT_COMMAND = "D:\\ProgramFiles\\gs\\gs9.18\\bin\\gswin64.exe";
	public static final String TESSERACT_DIR = "..\\..\\libcpp\\tesseract";
	public static final String DICTIONARY = "dictionary.txt";
	public static final int RANDOM_COUNT = 50000;
	
	protected boolean validSyntax;
	
	protected ArrayList<String> keys;
	protected ArrayList<Integer> counts;
	protected ArrayList<Double> precisions;
	protected ArrayList<Double> recalls;	
	
	public VisibilityCountMeter(String[] args) {
		super(args);
		this.args = args;
		validSyntax = validSyntax();

		keys = new ArrayList<String>();
		counts = new ArrayList<Integer>();
		precisions = new ArrayList<Double>();
		recalls = new ArrayList<Double>();
		
		if (!validSyntax)
			printUsage();
	}
	
	protected boolean validSyntax() {
		return args[0].equals("one") || args[0].equals("all");
	}
	
	protected boolean isValid() {
		return validSyntax;
	}

	protected void printUsage() {
		System.out.println("Usage: " + this.getClass().getName() + " one|all");		
	}
	
	protected void clear() {
		keys.clear();
		counts.clear();
		precisions.clear();
		recalls.clear();
	}
	
	public int countPages(String inWm) throws Exception {
		BufferedReader inWmBufferedReader = null;
		
		try {
			inWmBufferedReader = new BufferedReader(new FileReader(inWm));
			ArrayList<String> headers = getHeaders(inWmBufferedReader);
			int ppIndex = headers.indexOf("pp");
			if (ppIndex < 0)
				throw new Exception("There was no header for page count.");
						
			String inLine = inWmBufferedReader.readLine();	
			if (inLine == null)
				throw new Exception("There was no line for page count.");
			
			String[] inSubstrings = inLine.split("\t");
			if (inSubstrings.length < ppIndex)
				throw new Exception("There was no data for page count.");

			int ppCount = Integer.parseInt(inSubstrings[ppIndex]);
			return ppCount;
		}
		finally {
			if (inWmBufferedReader != null)
				inWmBufferedReader.close();
		}		
	}		
	
	public void runPdfToJpg(int page, int r, int q, String docPdf, String docJpg) throws Exception {
		ArrayList<String> command = new ArrayList<String>();
		command.add(GHOST_SCRIPT_COMMAND);
		command.add("-dSAFER");
		command.add("-dBATCH");
		command.add("-dNOPAUSE");
		command.add("-dTextAlphaBits=4");
		command.add("-dGraphicsAlphaBits=4");
		if (page != 0) {
			command.add("-dFirstPage=" + page);
			command.add("-dLastPage=" + page);
		}
		command.add("-r" + Integer.toString(r));
		command.add("-dJPEGQ=" + Integer.toString(q));
		command.add("-sDEVICE=jpeg");
		command.add("-sOutputFile=" + resDir + docJpg);
		command.add(resDir + docPdf);

		ExternalProcess process = new ExternalProcess(command);
		process.execute();	
	}
		
	protected void runWatermarkBitExtractor(String docJpg, String inWm, String outWm, String outTxt) throws Exception {
		String watermarkBitExtractorCmd = "..\\..\\haathi\\watermarkBitExtractor\\Debug-DynLink\\watermarkBitExtractor.exe";
		String tesseractDir = Paths.get(curDir + File.separator + TESSERACT_DIR).toAbsolutePath().normalize().toString();
		
		watermarkBitExtractorCmd = Paths.get(watermarkBitExtractorCmd).toAbsolutePath().normalize().toString();
		ArrayList<String> command = new ArrayList<String>();
		command.add(watermarkBitExtractorCmd);
		command.add(resDir + docJpg);
		command.add(resDir + inWm);
		command.add(resDir + outWm);
		command.add(resDir + outTxt);
		command.add(resDir + DICTIONARY);

		ExternalProcess process = new ExternalProcess(command);
		process.directory(tesseractDir);		
		process.execute();	
	}

	protected void runAutoRnd(int s, String rndWm) throws Exception {
		String autoRndCmd = Paths.get("..\\..\\haathi\\autoRnd\\Debug-StatLink\\autoRnd.exe").toAbsolutePath().normalize().toString();
		
		ArrayList<String> command = new ArrayList<String>();
		command.add(autoRndCmd);
		command.add("S" + s);
		command.add(Integer.toString(RANDOM_COUNT));
		command.add(resDir + rndWm);

		ExternalProcess process = new ExternalProcess(command);
		process.directory(OPEN_OFFICE_DIR);
		process.execute();	
	}

	protected FullPagePrecisionAnalyzer runApproximatePrecisionAnalyzer(String inWm, String outWm, String outWmc, String inTh, String outEv) throws Exception {
		TraitorAnalyzer traitorTracer = new TraitorAnalyzer(RANDOM_COUNT, resDir + outWm, resDir + outWmc, resDir + inTh, resDir + outEv, false);
		traitorTracer.run();
		
		FullPagePrecisionAnalyzer precisionAnalyzer = new FullPagePrecisionAnalyzer(resDir + inWm, resDir + outEv);
		precisionAnalyzer.run();
		
		return precisionAnalyzer;
	}
	
	protected String visToString(int vis) {
		String[] names = new String[] { null, "Low", "Medium", "High" };
		
		return names[vis];
	}
	
	protected void printResults(PrintStream printStream, int vis, int pageCount) {
		printStream.println();		
		printStream.println("#Scheme\tPages\tEmbedded\tExtracted");
		printStream.print("Scheme");
		for (int i = 0; i < 3; i++)
			printStream.print("\t" + visToString(vis));
		printStream.println();
		
		for (int i = 0; i < orderedKeys.length; i++) {
			int index = keys.indexOf(orderedKeys[i]);

			if (index >= 0) {
				long extracted = Math.round(recalls.get(index) * counts.get(index));

				if ("Total".equals(keys.get(index)))
					printStream.println(); // Blank line before totals or last record
				printStream.print(keys.get(index));
				printStream.print("\t");
				printStream.print(pageCount - 1); // Only note full pages
				printStream.print("\t");
				printStream.print(counts.get(index));
				printStream.print("\t");
				printStream.print(extracted);
				printStream.println();			
			}
		}
		printStream.println();
	}
	/* ***************************************************************************
	 One
	 ****************************************************************************/		
	protected <T> void addOne(ArrayList<T> values, HashMap<String, T> allValues) {
		values.add(allValues.get("Total"));

		for (String key: allValues.keySet())
			if (!"Total".equals(key)) {
				if (!keys.contains(key))
					keys.add(key);
				return;
			}
		System.err.println("Key not found!");
	}
	
	protected void printOneAtATime(PrintStream printStream) {
		String inTh = "in.th";
				
		String docTxt = "DofI.txt";
		String docPdf = "doc.pdf";
		String docOdt = "doc.odt";
		String docJpg = "doc%d.jpg";
		String inWm = "in.wm";
		String inTxt = "in.txt";
		String outWm = "out.wm";
		String outTxt = "out.txt";
		String rndWm = "rnd.wm";
		
		String outWmc = "out.wmc";
		String outEv = "out.ev";
		
		try {
			int r = 400;
			int q = 90;
			
			for (int v = 3; v >= 1; v--) {			
				clear();
				for (int s = 1; s < 64; s <<= 1) {
					eraseResFiles(new String[] { docPdf, docOdt, inWm, inTxt, outWm, outWmc, outTxt, rndWm, outEv });
					eraseMultiResFiles(new String[] { docJpg });
					runAutoPdf(s, v, docTxt, docPdf, docOdt, inWm, inTxt);
					int pageCount = countPages(resDir + inWm);
					runPdfToJpg(0, r, q, docPdf, docJpg);
					runWatermarkBitExtractor(docJpg, inWm, outWm, outTxt);
	
					FullPagePrecisionAnalyzer precisionAnalyzer = runApproximatePrecisionAnalyzer(inWm, outWm, outWmc, inTh, outEv);
					
					addOne(counts, precisionAnalyzer.counts);				
					addOne(precisions, precisionAnalyzer.precisions);
					addOne(recalls, precisionAnalyzer.recalls);
					eraseResFiles(archive(docTxt + "." + v, s), new String[] { docPdf, docOdt, inWm, inTxt, outWm, outWmc, outTxt, rndWm, outEv });
					eraseMultiResFiles(archive(docTxt + "." + v, s), new String[] { docJpg });					
					printResults(printStream, v, pageCount);
				}
			}
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
			System.exit(-1);
		}
	}
	
	protected void plotOneAtATime() {
		String prcInPlt = "prc-one.dat"; // Precision, recall, and possibly count

		String recallPlt = "recall-one.plt";
		String recallOutPlt = "recall-one.png";
		
		try {
			eraseResFiles(new String[] { prcInPlt, recallOutPlt });

			PrintStream printStream = new PrintStream(new File(resDir + prcInPlt));
			printOneAtATime(printStream);
			printStream.close();
			
			plotResults(recallPlt, prcInPlt, recallOutPlt);

			eraseResFiles(archive(""), new String[] { prcInPlt, recallOutPlt });
			copyResFiles(archive(""), new String[] { recallPlt });			
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
			System.exit(-1);
		}		
	}
	/* ***************************************************************************
	 All
	 ****************************************************************************/
	protected <T> void addAll(ArrayList<T> values, HashMap<String, T> allValues) {
		for (String key: allValues.keySet())
			/*if (!"Total".equals(key))*/ {
				if (!keys.contains(key))
					keys.add(key);
				values.add(allValues.get(key));
			}
	}

	public void printAllAtATime(PrintStream printStream) {
		String inTh = "in.th";
				
		String docTxt = "DofI.txt";
		String docPdf = "doc.pdf";
		String docOdt = "doc.odt";
		String docJpg = "doc%d.jpg";
		String inWm = "in.wm";
		String inTxt = "in.txt";
		String outWm = "out.wm";
		String outTxt = "out.txt";
		String rndWm = "rnd.wm";
		
		String outWmc = "out.wmc";
		String outEv = "out.ev";
		
		try {
			int s = 63; // Do them all
			int r = 400;
			int q = 90;
			
			for (int v = 3; v >= 1; v--) {			
				clear();
				eraseResFiles(new String[] { docPdf, docOdt, inWm, inTxt, outWm, outWmc, outTxt, rndWm, outEv });
				eraseMultiResFiles(new String[] { docJpg });
				runAutoPdf(s, v, docTxt, docPdf, docOdt, inWm, inTxt);
				int pageCount = countPages(resDir + inWm);

				runPdfToJpg(0, r, q, docPdf, docJpg);
				runWatermarkBitExtractor(docJpg, inWm, outWm, outTxt);

				FullPagePrecisionAnalyzer precisionAnalyzer = runApproximatePrecisionAnalyzer(inWm, outWm, outWmc, inTh, outEv);
				
				addAll(counts, precisionAnalyzer.counts);				
				addAll(precisions, precisionAnalyzer.precisions);
				addAll(recalls, precisionAnalyzer.recalls);
				eraseResFiles(archive(docTxt, v), new String[] { docPdf, docOdt, inWm, inTxt, outWm, outWmc, outTxt, rndWm, outEv });
				eraseMultiResFiles(archive(docTxt, v), new String[] { docJpg });
				printResults(printStream, v, pageCount);
			}
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
			System.exit(-1);
		}
	}	
	
	protected void plotAllAtATime() {
		String prcInPlt = "prc-all.dat"; // Precision, recall, and possibly count

		String recallPlt = "recall-all.plt";
		String recallOutPlt = "recall-all.png";
		
		try {
			eraseResFiles(new String[] { prcInPlt, recallOutPlt });

			PrintStream printStream = new PrintStream(new File(resDir + prcInPlt));
			printAllAtATime(printStream);
			printStream.close();
			
			plotResults(recallPlt, prcInPlt, recallOutPlt);
			
			eraseResFiles(archive(""), new String[] { prcInPlt, recallOutPlt });
			copyResFiles(archive(""), new String[] { recallPlt });
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
			System.exit(-1);			
		}		
	}
	/* ***************************************************************************
	 Main
	 ****************************************************************************/		
	public void run() {
		if (!isValid())
			return;
		
		if (args[0].equals("one"))
			//printOneAtATime(System.out);
			plotOneAtATime();
		else if (args[0].equals("all"))
			//printAllAtATime(System.out);
			plotAllAtATime();
	}
	
	public static void main(String[] args) {
		new VisibilityCountMeter(args).run();
	}
}
/* **************************************************************************/
