/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/AccuracyMeter/src/com/haathi/performance/accuracy/AccuracyMeter.java#17 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.performance.accuracy;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import com.haathi.process.ExternalProcess;

import com.haathi.watermark.analyzer.AccuracyAnalyzer;
import com.haathi.watermark.analyzer.util.ConfusionMatrix;

import java.io.File;
import java.io.PrintStream;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;

import com.haathi.performance.SharedMeter;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class AccuracyMeter extends SharedMeter {
	public static final String GHOST_SCRIPT_COMMAND = "D:\\ProgramFiles\\gs\\gs9.18\\bin\\gswin64.exe";
	public static final String TESSERACT_DIR = "..\\..\\libcpp\\tesseract";
	public static final String DICTIONARY = "dictionary.txt";
	public static final String TOTAL = "Total";

	protected boolean validSyntax;

	protected ArrayList<String> keys;
	protected ArrayList<Double> counts;
	protected ArrayList<Double> accuracies;
	
	public AccuracyMeter(String[] args) {
		super(args);
		validSyntax = validSyntax();

		keys = new ArrayList<String>();
		counts = new ArrayList<Double>();
		accuracies = new ArrayList<Double>();

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
		accuracies.clear();
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

	protected HashMap<String, ConfusionMatrix.Accuracy> runWatermarkBitAnalyzer(String inWm, String inTxt, String outWm, String outTxt) throws Exception {
		AccuracyAnalyzer watermarkBitAnalyzer = new AccuracyAnalyzer(resDir + inWm, resDir + inTxt, resDir + outWm, resDir + outTxt);
		HashMap<String, ConfusionMatrix.Accuracy> accuracies = watermarkBitAnalyzer.run();
		
		return accuracies;
	}
	
	protected void printResults(PrintStream printStream, String filename) {
		printStream.println();
		printStream.println("#Scheme\tCount\tAccuracy");		
		printStream.print("Scheme");
		for (int i = 0; i < 2; i++)
			printStream.print("\t" + upToDot(filename));
		printStream.println();

		for (int i = 0; i < orderedKeys.length; i++) {
			int index = keys.indexOf(orderedKeys[i]);

			if (index >= 0) {
				if (TOTAL.equals(keys.get(index)))
					printStream.println(); // Blank line before totals or last record			
				printStream.print(keys.get(index));
				printStream.print("\t");
				printStream.print(counts.get(index));
				printStream.print("\t");
				printStream.print(accuracies.get(index));
				printStream.println();
			}
		}
		printStream.println();
	}
	/* ***************************************************************************
	 One
	 ****************************************************************************/	
	protected void addOne(HashMap<String, ConfusionMatrix.Accuracy> allAccuracies) {
		try {
			String key = new ArrayList<String>(allAccuracies.keySet()).get(0);
			
			keys.add(key);
			
			ConfusionMatrix.Accuracy accuracy = allAccuracies.get(key);
			counts.add((double) accuracy.goodCount / 64); // Measure in watermarks
			accuracies.add(100.0 * (double) accuracy.goodCount / accuracy.allCount);
		}
		catch (Exception e) {
			e.printStackTrace();
			System.exit(-1);			
		}
	}

	public void printOneAtATime(PrintStream printStream) {
		String[] docTxts = new String[] { "DofI.txt", "Twain.txt", "SofU.txt", "Turing.txt" };
		String docPdf = "doc.pdf";
		String docOdt = "doc.odt";
		String docJpg = "doc%d.jpg"; // Will have multiple to erase
		String inWm = "in.wm";
		String inTxt = "in.txt";
		String outWm = "out.wm";
		String outTxt = "out.txt";
		
		try {
			int r = 400;
			int q = 90;
			int v = 2;
			
			for (int i = 0; i < docTxts.length; i++) {
				String docTxt = docTxts[i];
		
				clear();
				for (int s = 1; s < 64; s <<= 1) {
					eraseResFiles(new String[] { docPdf, docOdt, inWm, inTxt, outWm, outTxt });
					eraseMultiResFiles(new String[] { docJpg });
					runAutoPdf(s, v, docTxt, docPdf, docOdt, inWm, inTxt);
					runPdfToJpg(0, r, q, docPdf, docJpg);
					runWatermarkBitExtractor(docJpg, inWm, outWm, outTxt);
					HashMap<String, ConfusionMatrix.Accuracy> allAccuracies = runWatermarkBitAnalyzer(inWm, inTxt, outWm, outTxt);
	
					addOne(allAccuracies);
					eraseResFiles(archive(docTxt, s), new String[] { docPdf, docOdt, inWm, inTxt, outWm, outTxt });
					eraseMultiResFiles(archive(docTxt, s), new String[] { docJpg });
				}
				printResults(printStream, docTxt);
			}
		}
		catch (Exception exception) {
			exception.printStackTrace();
			System.exit(-1);			
		}
	}
	
	public void plotOneAtATime() {
		String accInPlt = "acc-one.dat";
		
		String accuracyPlt = "accuracy-one.plt";
		String accuracyOutPlt = "accuracy-one.png";

		String countPlt = "count-one.plt";
		String countOutPlt = "count-one.png";
		
		try {
			eraseResFiles(new String[] { accInPlt, accuracyOutPlt, countOutPlt });

			PrintStream printStream = new PrintStream(new File(resDir + accInPlt));
			printOneAtATime(printStream);
			printStream.close();
			
			plotResults(accuracyPlt, accInPlt, accuracyOutPlt);
			plotResults(countPlt, accInPlt, countOutPlt);
			
			eraseResFiles(archive(""), new String[] { accInPlt, accuracyOutPlt, countOutPlt });		
			copyResFiles(archive(""), new String[] { accuracyPlt, countPlt });			
		}
		catch (Exception exception) {
			exception.printStackTrace();
			System.exit(-1);			
		}		
	}	
	/* ***************************************************************************
	 All
	 ****************************************************************************/		
	protected void addAll(HashMap<String, ConfusionMatrix.Accuracy> allAccuracies) {
		int totalGoodCount = 0;
		int totalAllCount = 0;
		
		for (String key: allAccuracies.keySet()) {
			keys.add(key);
		
			ConfusionMatrix.Accuracy accuracy = allAccuracies.get(key);
			int goodCount = accuracy.goodCount;
			int allCount = accuracy.allCount;
			counts.add((double) goodCount / 64); // Measure in watermarks
			accuracies.add(100.0 * (double) goodCount / allCount);
			
			totalGoodCount += goodCount;
			totalAllCount += allCount;
		}
		keys.add(TOTAL);
		counts.add((double) totalGoodCount / 64);
		accuracies.add(100.0 * (double) totalGoodCount / totalAllCount);
	}
	
	public void printAllAtATime(PrintStream printStream) {
		String[] docTxts = new String[] { "DofI.txt" , "Twain.txt", "SofU.txt", "Turing.txt" };
		String docPdf = "doc.pdf";
		String docOdt = "doc.odt";
		String docJpg = "doc%d.jpg"; // Will have multiple to erase
		String inWm = "in.wm";
		String inTxt = "in.txt";
		String outWm = "out.wm";
		String outTxt = "out.txt";
		
		try {
			int s = 63; // Do them all
			int r = 400;
			int q = 90;
			int v = 2;
			
			for (int i = 0; i < docTxts.length; i++) {
				String docTxt = docTxts[i];
		
				clear();
				eraseResFiles(new String[] { docPdf, docOdt, inWm, inTxt, outWm, outTxt });
				eraseMultiResFiles(new String[] { docJpg });
				runAutoPdf(s, v, docTxt, docPdf, docOdt, inWm, inTxt);
				runPdfToJpg(0, r, q, docPdf, docJpg);
				runWatermarkBitExtractor(docJpg, inWm, outWm, outTxt);
				HashMap<String, ConfusionMatrix.Accuracy> allAccuracies = runWatermarkBitAnalyzer(inWm, inTxt, outWm, outTxt);

				addAll(allAccuracies);
				eraseResFiles(archive(docTxt, s), new String[] { docPdf, docOdt, inWm, inTxt, outWm, outTxt });
				eraseMultiResFiles(archive(docTxt, s), new String[] { docJpg });
				printResults(printStream, docTxt);
			}
		}
		catch (Exception exception) {
			exception.printStackTrace();
			System.exit(-1);			
		}
	}	

	public void plotAllAtATime() {
		String accInPlt = "acc-all.dat";
		
		String accuracyPlt = "accuracy-all.plt";
		String accuracyOutPlt = "accuracy-all.png";
		
		String countPlt = "count-all.plt";
		String countOutPlt = "count-all.png";
		
		try {
			eraseResFiles(new String[] { accInPlt, accuracyOutPlt, countOutPlt });

			PrintStream printStream = new PrintStream(new File(resDir + accInPlt));
			printAllAtATime(printStream);
			printStream.close();
			
			plotResults(accuracyPlt, accInPlt, accuracyOutPlt);
			plotResults(countPlt, accInPlt, countOutPlt);
			
			eraseResFiles(archive(""), new String[] { accInPlt, accuracyOutPlt, countOutPlt });
			copyResFiles(archive(""), new String[] { accuracyPlt, countPlt });
		}
		catch (Exception exception) {
			exception.printStackTrace();
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
		new AccuracyMeter(args).run();
	}	
}
/* **************************************************************************/
