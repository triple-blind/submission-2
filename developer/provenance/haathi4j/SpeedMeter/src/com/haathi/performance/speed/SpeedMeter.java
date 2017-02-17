/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/SpeedMeter/src/com/haathi/performance/speed/SpeedMeter.java#14 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.performance.speed;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.PrintStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.Arrays;

import com.haathi.performance.SharedMeter;
import com.haathi.process.ExternalProcess;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class SpeedMeter extends SharedMeter {
	// We may want to order by speed instead of density for this test!	
	public static final String[] args0 = new String[] { "load", "type", "loadsave", "typesave" };
	public static final String[] args1 = new String[] { "one", "all" };
	
	protected static void printAll(String[] values, String sep) {
		for (int i = 0; i < values.length; i++)
			System.out.print((i == 0 ? "" : sep) + values[i]);	
	}
	
	protected ArrayList<String> keys;
	protected ArrayList<ArrayList<String>> allHeaders;
	protected ArrayList<ArrayList<String>> allValues;
	
	protected boolean validSyntax;
	
	protected boolean loadNotType;
	protected boolean save;
	protected boolean one;
	
	public SpeedMeter(String[] args) {
		super(args);
		validSyntax = validSyntax();
		
		keys = new ArrayList<String>();
		allHeaders = new ArrayList<ArrayList<String>>();
		allValues = new ArrayList<ArrayList<String>>();

		if (!validSyntax)
			printUsage();
	}

	protected boolean isValid() {
		return validSyntax;
	}
	
	protected boolean validSyntax() {
		if (args.length != 2)
			return false;		
		if (indexOf(args[0], args0) < 0)
			return false;
		if (indexOf(args[1], args1) < 0)
			return false;
				
		loadNotType = "load".equals(args[0]) || "loadsave".equals(args[0]);
		save = "loadsave".equals(args[0]) || "typesave".equals(args[0]);		
		one = "one".equals(args[1]);
		return true;
	}
	
	protected void printUsage() {
		System.out.print("Usage: " + this.getClass().getName() + " ");
		printAll(args0, "|");
		System.out.print(" ");
		printAll(args1, "|");
		System.out.println();		
	}

	protected void clear() {
		keys.clear();
		allHeaders.clear();
		allValues.clear();
	}
	
	protected void runAutoType(boolean loadNotType, boolean save, boolean provenance, int s, String docTxt, String docOdt, String logDat, int typeCount, String tmpTxt) throws Exception {
		String autoPdfCmd = Paths.get("..\\..\\haathi\\autotype\\Debug-StatLink\\autotype.exe").toAbsolutePath().normalize().toString();
		
		if (!loadNotType && typeCount > 0) {
			byte[] bytes = Files.readAllBytes(Paths.get(resDir + docTxt));

			// Hope that no encoded characters are split here
			bytes = Arrays.copyOf(bytes, typeCount);
			docTxt = tmpTxt;
			Files.write(Paths.get(resDir + docTxt), bytes, StandardOpenOption.CREATE);
		}

		ArrayList<String> command = new ArrayList<String>();
		command.add(autoPdfCmd);
		String arg = loadNotType ? "L" : "T";
		if (save)
			arg += "S";
		command.add(arg);
		command.add("P" + (provenance ? "1" : "0"));
		command.add("W1");
		command.add("S" + s);
		command.add("V2");
		command.add(resDir + docTxt);
		command.add(resDir + docOdt);
		command.add(resDir + logDat);

		ExternalProcess process = new ExternalProcess(command);
		process.directory(OPEN_OFFICE_DIR);
		process.execute();	
	}

	protected void addAll(ArrayList<String> arrayList, String[] array) {
		for (int i = 0; i < array.length; i++)
			arrayList.add(array[i]);
	}
	
	protected void analyzeResult(String logDat) throws Exception {
		BufferedReader bufferedReader = null;
		
		try {
			bufferedReader = new BufferedReader(new FileReader(resDir + logDat));
	
			while (true) {
				String headerLine = bufferedReader.readLine();
				String valuesLine = bufferedReader.readLine();
				
				if (headerLine == null || valuesLine == null)
					break;
				String[] headerSubstrings = headerLine.split("\t");
				String[] valueSubstrings = valuesLine.split("\t");
				
				if (headerSubstrings.length != valueSubstrings.length)
					throw new Exception("Column count does not match up.");
				
				ArrayList<String> headers = new ArrayList<String>();
				addAll(headers, headerSubstrings);
				allHeaders.add(headers);									

				ArrayList<String> values = new ArrayList<String>();
				addAll(values, valueSubstrings);
				allValues.add(values);									
			}
		}
		finally {
			if (bufferedReader != null)
				bufferedReader.close();			
		}
	}

	protected String fromLastPlus(String text) {
		int index = text.lastIndexOf('+');
		
		return index < 0 ? text : text.substring(index);
	}
	
	int highestBit(int value) {
		for (int mask = 0x40000000; mask > 0; mask >>= 1)
			if ((mask & value) != 0)
				return mask;
		return 0;
	}
	
	protected void printResult(PrintStream printStream, int s) {
		// These should be the same across all values
		int abbrevIndex = allHeaders.get(0).indexOf("Abbrev");
		int charCountIndex = allHeaders.get(0).indexOf("CharCount");
		int loadTimeIndex = allHeaders.get(0).indexOf("LoadTime");
		int typeTimeIndex = allHeaders.get(0).indexOf("TypeTime");
		int saveTimeIndex = allHeaders.get(0).indexOf("SaveTime");
		String scheme = allValues.get(0).get(abbrevIndex);
		scheme = fromLastPlus(scheme);
			
		printStream.println();
		printStream.println("#Document\tLoad\tType\tSave");
		printStream.print("Scheme");
		for (int i = 0; i < 3; i++)
			printStream.print("\t" + scheme);
		printStream.println();
		
		for (int i = 0; i < keys.size(); i++) {
			ArrayList<String> values = allValues.get(i);
				
			printStream.print(upToDot(keys.get(i)));
			printStream.print("\t");
			

			// If typing, values are invalid for CW and Syn
			if (!loadNotType && (highestBit(s) == 8 || highestBit(s) == 32))
				printStream.println("-\t-\t-");
			else {
				printStream.print(values.get(loadTimeIndex));
				printStream.print("\t");
	
				String wpm = "-";
				if (!loadNotType) {
					double typeTime = Double.parseDouble(values.get(typeTimeIndex));
					int charCount = Integer.parseInt(values.get(charCountIndex));
					
					wpm = Double.toString((double) charCount / typeTime * 12);
				}	
				printStream.print(wpm);
				printStream.print("\t");
				
				printStream.print(values.get(saveTimeIndex));
				printStream.println();
			}
		}
		printStream.println();
	}
	/* ***************************************************************************
	 One
	 ****************************************************************************/		
	protected void printOneAtATime(PrintStream printStream) {
		String[] docTxts = new String[] { "DofI.txt", "Twain.txt", "SofU.txt", "Turing.txt" };

		String docOdt = "doc.odt";
		String logDat = "log.dat";
		String tmpTxt = "tmp.txt";	
		
		boolean provenance = true;
		int typeCount = 2000;

		try {
			for (int s = 1; s < 64; s <<= 1) {
				clear();
				for (int i = 0; i < docTxts.length; i++) {
					String docTxt = docTxts[i];
					keys.add(docTxt);
		
					eraseResFiles(new String[] { docOdt, logDat, tmpTxt });
					runAutoType(loadNotType, save, provenance, s, docTxt, docOdt, logDat, typeCount, tmpTxt);
					analyzeResult(logDat);
					eraseResFiles(new String[] { tmpTxt });
					eraseResFiles(archive(docTxt, s), new String[] { docOdt, logDat });
					
				}
				printResult(printStream, s);
			}
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
			System.exit(-1);
		}
	}
	
	protected void plotOneAtATime() {
		String spdInPlt = "spd-one.dat";
		
		String loadPlt = "load-one.plt";
		String loadOutPlt = "load-one.png";
		
		String typePlt = "type-one.plt";
		String typeOutPlt = "type-one.png";
		
		String savePlt = "save-one.plt";
		String saveOutPlt = "save-one.png";
		
		try {
			eraseResFiles(new String[] { spdInPlt, loadOutPlt, typeOutPlt, saveOutPlt });

			PrintStream printStream = new PrintStream(new File(resDir + spdInPlt));
			printOneAtATime(printStream);
			printStream.close();
			
			if (loadNotType)
				plotResults(loadPlt, spdInPlt, loadOutPlt);
			else
				plotResults(typePlt, spdInPlt, typeOutPlt);
			if (save)
				plotResults(savePlt, spdInPlt, saveOutPlt);
			
			eraseResFiles(archive(""), new String[] { spdInPlt, loadOutPlt, typeOutPlt, saveOutPlt });
			copyResFiles(archive(""), new String[] { loadPlt, typePlt, savePlt });
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
			System.exit(-1);
		}
	}	
	/* ***************************************************************************
	 Stacked
	 ****************************************************************************/
	protected void printAllAtATime(PrintStream printStream) {
		String[] docTxts = new String[] { "DofI.txt", "Twain.txt", "SofU.txt", "Turing.txt"  };

		String docOdt = "doc.odt";
		String logDat = "log.dat";
		String tmpTxt = "tmp.txt";	
		
		boolean provenance = true;
		int typeCount = 2000;
		int s = 0;

		try {
			for (int mask = 1; mask < 64; mask <<= 1) {
				s += mask;
				clear();
				for (int i = 0; i < docTxts.length; i++) {
					String docTxt = docTxts[i];
					keys.add(docTxt);
		
					eraseResFiles(new String[] { docOdt, logDat, tmpTxt });
					runAutoType(loadNotType, save, provenance, s, docTxt, docOdt, logDat, typeCount, tmpTxt);
					analyzeResult(logDat);
					eraseResFiles(new String[] { tmpTxt });
					eraseResFiles(archive(docTxt, s), new String[] { docOdt, logDat });
					
				}
				printResult(printStream, s);
			}
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
		}
	}

	protected void plotAllAtATime() {
		String spdInPlt = "spd-all.dat";
		
		String loadPlt = "load-all.plt";
		String loadOutPlt = "load-all.png";
		
		String typePlt = "type-all.plt";
		String typeOutPlt = "type-all.png";
		
		String savePlt = "save-all.plt";
		String saveOutPlt = "save-all.png";

		try {
			eraseResFiles(new String[] { spdInPlt, loadOutPlt, typeOutPlt, saveOutPlt });

			PrintStream printStream = new PrintStream(new File(resDir + spdInPlt));
			printAllAtATime(printStream);
			printStream.close();			
			
			if (loadNotType)
				plotResults(loadPlt, spdInPlt, loadOutPlt);
			else
				plotResults(typePlt, spdInPlt, typeOutPlt);
			if (save)
				plotResults(savePlt, spdInPlt, saveOutPlt);

			eraseResFiles(archive(""), new String[] { spdInPlt, loadOutPlt, typeOutPlt, saveOutPlt });			
			copyResFiles(archive(""), new String[] { loadPlt, typePlt, savePlt });
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
		
		if (one)
			//printOneAtATime(System.out);
			plotOneAtATime();
		else
			//printAllAtATime(System.out);
			plotAllAtATime();
	}
	
	public static void main(String[] args) {
		new SpeedMeter(args).run();
	}	
}
/* **************************************************************************/
