/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/AccuracyMeter/src/com/haathi/performance/accuracy/AccuracyMeter.java#12 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.performance.accuracy;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import com.haathi.process.ExternalProcess;
import com.haathi.process.GnuPlotProcess;
import com.haathi.watermark.analyzer.AccuracyAnalyzer;
import com.haathi.watermark.analyzer.util.ConfusionMatrix;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class AccuracyMeter {
	public static final String OPEN_OFFICE_DIR = "D:\\ProgramFilesx86\\OpenOffice4\\program";
	public static final String GHOST_SCRIPT_COMMAND = "D:\\ProgramFiles\\gs\\gs9.18\\bin\\gswin64.exe";
	public static final String TESSERACT_DIR = "..\\..\\libcpp\\tesseract";
	public static final String DICTIONARY = "dictionary.txt";

	protected static final String[] orderedKeys = new String[] { "FG", "BG", "BL", "CW", "WS", "Syn" };
	
	protected String args[];
	protected boolean validSyntax;

	protected String curDir;
	protected String resDir;	
		
	protected ArrayList<String> keys;
	protected ArrayList<Double> counts;
	protected ArrayList<Double> accuracies;
	
	public AccuracyMeter(String[] args) {
		this.args = args;
		validSyntax = validSyntax();

		curDir = Paths.get(".").toAbsolutePath().normalize().toString();
		resDir = curDir + File.separator + "res" + File.separator;
				
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
	
	protected void runAutoPdf(int s, String docTxt, String docPdf, String docOdt, String inWm, String inTxt) throws IOException, InterruptedException {
		String autoPdfCmd = Paths.get("..\\..\\haathi\\autoPdf\\Debug-StatLink\\autoPdf.exe").toAbsolutePath().normalize().toString();
		
		ArrayList<String> command = new ArrayList<String>();
		command.add(autoPdfCmd);
		command.add("S" + s);
		command.add(resDir + docTxt);
		command.add(resDir + docPdf);
		command.add(resDir + docOdt);
		command.add(resDir + inWm);
		command.add(resDir + inTxt);

		ExternalProcess process = new ExternalProcess(command);
		process.directory(OPEN_OFFICE_DIR);
		process.execute();	
	}
	
	public void runPdfToJpg(int page, int r, int q, String docPdf, String docJpg) throws IOException, InterruptedException {
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

	protected void runWatermarkBitExtractor(String docJpg, String inWm, String outWm, String outTxt) throws IOException, InterruptedException {
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
	
	protected void eraseMultiResFiles(String[] filenames) {
		for (String filename: filenames) {
			int index = 1;
			
			while (eraseResFile(String.format(filename, index)))
				index++;
		}
	}
	
	protected boolean eraseResFile(String filename) {
		File file = new File(resDir + filename);
		boolean result = file.exists();
		
		file.delete();
		return result;
	}

	protected boolean eraseResFiles(String[] filenames) {
		boolean result = false;
		
		for (String filename: filenames)
			result |= eraseResFile(filename);
		return result;
	}

	protected void eraseMultiResFiles(String prefix, String[] filenames) {
		String[] prefixedFilenames = new String[filenames.length];
		
		for (int i = 0; i < filenames.length; i++)
			prefixedFilenames[i] = prefix + filenames[i];

		eraseMultiResFiles(prefixedFilenames);
		
		for (int i = 0; i < filenames.length; i++) {
			int index = 1;
			
			while (renameResFile(String.format(filenames[i], index), String.format(prefixedFilenames[i], index)))
				index++;			
		}		
	}

	protected boolean renameResFile(String fromFilename, String toFilename) {
		File fromFile = new File(resDir + File.separator + fromFilename);
		File toFile = new File(resDir + File.separator + toFilename);
		boolean result = fromFile.exists();
		
		fromFile.renameTo(toFile);
		return result;
	}
	
	protected boolean eraseResFiles(String prefix, String[] filenames) {
		String[] prefixedFilenames = new String[filenames.length];
		boolean result = false;
		
		for (int i = 0; i < filenames.length; i++)
			prefixedFilenames[i] = prefix + filenames[i];

		// First try to remove the prefixed files in case they are there
		eraseResFiles(prefixedFilenames);
		
		// Then rename the filenames to prefixed files
		for (int i = 0; i < filenames.length; i++)
			result |= renameResFile(filenames[i], prefixedFilenames[i]);
		return result;
	}
	
	protected void printResults(PrintStream printStream, String filename) {
		printStream.println("#Scheme\tCount\tAccuracy");		
		printStream.print("Scheme");
		for (int i = 0; i < 2; i++)
			printStream.print("\t" + filename);
		printStream.println();

		for (int i = 0; i < orderedKeys.length; i++) {
			int index = keys.indexOf(orderedKeys[i]);

			if (index >= 0) {
				printStream.print(keys.get(index));
				printStream.print("\t");
				printStream.print(counts.get(index));
				printStream.print("\t");
				printStream.print(accuracies.get(index));
				printStream.println();
			}
		}
		printStream.println();
		printStream.println();
	}
	
	protected void  plotResults(String plotPlt, String inPlt, String outPlt) throws IOException, InterruptedException {	
		plotPlt = resDir + plotPlt;
		inPlt = resDir + inPlt;
		outPlt = resDir + outPlt;
		
		GnuPlotProcess gnuPlotProcess = new GnuPlotProcess(plotPlt, "pngcairo", inPlt, outPlt);
		gnuPlotProcess.execute();
	}
	
	protected String archive(String docTxt, int s) {
		return "archive" + File.separator + docTxt + "." + s + ".";
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
		}
	}

	public void printOneAtATime(PrintStream printStream) {
		String[] docTxts = new String[] { /*"DofI.txt", "Twain.txt",*/ "Turing.txt" /*, "SofU.txt" */ };
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
			for (int i = 0; i < docTxts.length; i++) {
				String docTxt = docTxts[i];
		
				clear();
				// TODO change back to 64
				for (int s = 1; s < 64; s <<= 1) {
					eraseResFiles(new String[] { docPdf, docOdt, inWm, inTxt, outWm, outTxt });
					eraseMultiResFiles(new String[] { docJpg });
					runAutoPdf(s, docTxt, docPdf, docOdt, inWm, inTxt);
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
			exception.printStackTrace(System.err);
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
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
		}		
	}	
	/* ***************************************************************************
	 All
	 ****************************************************************************/		
	protected void addAll(HashMap<String, ConfusionMatrix.Accuracy> allAccuracies) {
		for (String key: allAccuracies.keySet()) {
			if (key.equals("Total"))
				continue;
			keys.add(key);
		
			ConfusionMatrix.Accuracy accuracy = allAccuracies.get(key);
			counts.add((double) accuracy.goodCount / 64); // Measure in watermarks
			accuracies.add(100.0 * (double) accuracy.goodCount / accuracy.allCount);
		}
	}
	
	public void printAllAtATime(PrintStream printStream) {
		String[] docTxts = new String[] { /*"DofI.txt", "Twain.txt",*/ "Turing.txt"/*, "SofU.txt"*/ };
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
			
			for (int i = 0; i < docTxts.length; i++) {
				String docTxt = docTxts[i];
		
				clear();
				eraseResFiles(new String[] { docPdf, docOdt, inWm, inTxt, outWm, outTxt });
				eraseMultiResFiles(new String[] { docJpg });
				runAutoPdf(s, docTxt, docPdf, docOdt, inWm, inTxt);
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
			exception.printStackTrace(System.err);
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
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
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
