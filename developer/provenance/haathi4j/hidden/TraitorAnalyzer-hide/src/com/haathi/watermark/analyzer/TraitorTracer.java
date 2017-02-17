/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/TraitorAnalyzer/src/com/haathi/watermark/analyzer/TraitorTracer.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

import com.haathi.performance.SharedAnalyzer;
import com.haathi.watermark.analyzer.tracer.BaselineAnalyzer;
import com.haathi.watermark.analyzer.tracer.BgLuminanceAnalyzer;
import com.haathi.watermark.analyzer.tracer.CharacterWidthAnalyzer;
import com.haathi.watermark.analyzer.tracer.FgLuminanceAnalyzer;
import com.haathi.watermark.analyzer.tracer.SchemeAnalyzer;
import com.haathi.watermark.analyzer.tracer.SynonymAnalyzer;
import com.haathi.watermark.analyzer.tracer.WordSpacingAnalyzer;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class TraitorTracer extends SharedAnalyzer {
	public static class Configurator {
		protected ProcessBuilder processBuilder;
		
		public Configurator(List<String> command) {
			processBuilder = new ProcessBuilder(command);
		}
		
		ProcessBuilder getProcessBuilder() {
			return processBuilder;
		}
		
		void execute() throws IOException, InterruptedException {
			Process process = processBuilder.start();
			
			process.waitFor();
		}
		
		List<String> command() {
			return processBuilder.command();
		}
		
		void directory(String name) {
			processBuilder.directory(new java.io.File(name));
		}
	}

	public class CppConfigurator extends Configurator {
		public CppConfigurator(List<String> command) {
			super(command);
		}
	}
	
	protected String[] args;
	protected boolean validSyntax;
	protected boolean validFiles;
	
	protected int count;
	protected String pathToOutWm;
	protected String pathToOutWmc;
	protected String pathToInTh;
	protected String pathToOutEv;
	
	public TraitorTracer(String[] args) {
		this.args = args;
		validSyntax = validateSyntax();
		if (validSyntax)
			validFiles = validateFiles();
		if (!isValid())
			printUsage();
	}
	
	public TraitorTracer(int count, String outWm, String outWmc, String inTh, String outEv) {
		this.count = count;
		pathToOutWm = outWm;
		pathToOutWmc = outWmc;
		pathToInTh = inTh;
		pathToOutEv = outEv;
		
		validSyntax = true;
		if (validSyntax)
			validFiles = validateFiles();
		if (!isValid())
			printUsage();
	}
	
	protected boolean validateSyntax() {
		if (args.length != 5)
			return false;
		count = Integer.parseInt(args[0]);
		pathToOutWm = args[1];
		pathToOutWmc = args[2];
		pathToInTh = args[3];
		pathToOutEv = args[4];
		
		// If called this way, it's from a launch and these are relative paths.
		// Otherwise it was from a constructor and the paths are already complete.
		String currentDir = Paths.get(".").toAbsolutePath().normalize().toString();
		pathToOutWmc = currentDir + File.separator + pathToOutWmc;
		pathToInTh = currentDir + File.separator + pathToInTh;
		pathToOutEv = currentDir + File.separator + pathToOutEv;	
		return true;
	}
	
	protected boolean validateFiles() {
		boolean valid = true;
		
		valid &= fileExists(pathToOutWm);
		valid &= !fileExists(pathToOutWmc);
		valid &= fileExists(pathToInTh);
		valid &= !fileExists(pathToOutEv);
		return valid;
	}
	
	protected boolean isValid() {
		return validSyntax && validFiles;
	}
	
	protected void printUsage() {
		System.err.println(
				"Usage: " + this.getClass().getName() +  " count <out.wm> <out.wmc> <in.th> <out.ev>\n" +
				"Where: count determines how long the traitor tracer searches.\n\n" +

				" out.wm  is the file produced by the watermarkBitExtractor,\n" +
				" out.wmc is the file to contain the completed watermarks generated here,\n" +
				"  in.th  specifies the theories as to which people and documents might be involved,\n" +
				" out.ev  will contain the evidence supporting the theories."
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
	
	protected void putHeaders(ArrayList<String> headers, BufferedWriter bufferedWriter) throws IOException {
		for (int i = 0; i < headers.size(); i++) {
			if (i > 0)
				bufferedWriter.write("\t");
			bufferedWriter.write(headers.get(i));
		}
		bufferedWriter.write("\n");
	}
	
	protected void prepareWmcOut() throws Exception {
		BufferedReader outWmBufferedReader = null;
		BufferedWriter outWmcBufferedWriter = null;
		
		try {
			outWmBufferedReader = new BufferedReader(new FileReader(pathToOutWm));
			outWmcBufferedWriter = new BufferedWriter(new FileWriter(pathToOutWmc));
		
			ArrayList<String> headers = getHeaders(outWmBufferedReader);
			putHeaders(headers, outWmcBufferedWriter);
			
			while (true) {
				String outLine = outWmBufferedReader.readLine();
				
				if (outLine == null)
					break;
				String[] outSubstrings = outLine.split("\t");
				
				// id, abbrev, vis, bits
				if (outSubstrings.length < 4)
					throw new Exception("There weren't enough watermark columns.");
				
				int idIndex = headers.indexOf("id");
				int abbrevIndex = headers.indexOf("abbrev");
				int visIndex = headers.indexOf("vis");
				int bitsIndex = headers.indexOf("bits");
				
				String schemeAbbrev = outSubstrings[abbrevIndex];
				String outcLine = newSchemeAnalyzer(schemeAbbrev).prepareWmcOut(outSubstrings, bitsIndex, outSubstrings.length);
				
				outWmcBufferedWriter.write(outSubstrings[idIndex]);
				outWmcBufferedWriter.write("\t");
				outWmcBufferedWriter.write(schemeAbbrev);
				outWmcBufferedWriter.write("\t");
				outWmcBufferedWriter.write(outSubstrings[visIndex]);
				outWmcBufferedWriter.write("\t");				
				outWmcBufferedWriter.write(outcLine);
				outWmcBufferedWriter.newLine();
			}
		}
		finally {
			if (outWmBufferedReader != null)
				outWmBufferedReader.close();
			if (outWmcBufferedWriter != null)
				outWmcBufferedWriter.close();
		}		
	}

	protected void runTraitorTracer() throws IOException, InterruptedException {
		String traitorTracerCmd = "..\\..\\haathi\\traitorTracer\\Debug-StatLink\\traitorTracer.exe";
		
		traitorTracerCmd = Paths.get(traitorTracerCmd).toAbsolutePath().normalize().toString();
		ArrayList<String> command = new ArrayList<String>();
		command.add(traitorTracerCmd);
		command.add(Integer.toString(count));
		command.add(pathToOutWmc);
		command.add(pathToInTh);
		command.add(pathToOutEv);

		Configurator configurator = new CppConfigurator(command);
		configurator.execute();		
	}
	
	public void run() throws Exception {
		if (!isValid())
			return;
		
		prepareWmcOut();
		runTraitorTracer();
//		new File(pathToOutWmc).delete();
	}
	
	public static void main(String[] args) {
		TraitorTracer traitorTracer = new TraitorTracer(args);
		
		try {
			traitorTracer.run();
		}
		catch (Exception e) {
			e.printStackTrace();
			System.exit(-1);
		}
	}
}
/* **************************************************************************/
