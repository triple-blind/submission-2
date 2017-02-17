/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/AccuracyMeter/src/com/haathi/performance/accuracy/AccuracyMeter.java#14 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.performance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.File;
import java.io.IOException;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;

import com.haathi.process.ExternalProcess;
import com.haathi.process.GnuPlotProcess;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class SharedMeter extends SharedPerformance {
	public static final String OPEN_OFFICE_DIR = "D:\\ProgramFilesx86\\OpenOffice4\\program";
	
	protected static final String[] orderedKeys = new String[] { "FG", "BG", "BL", "CW", "WS", "Syn", "Total" };

	protected static int indexOf(String key, String[] values) {
		int index = -1;
		
		for (int i = 0; i < values.length; i++)
			if (values[i].equals(key))
				index = i;
		return index;
	}
	
	protected String args[];
	protected String curDir;
	protected String resDir;

	public SharedMeter(String[] args) {
		this.args = args;
		curDir = Paths.get(".").toAbsolutePath().normalize().toString();
		resDir = curDir + File.separator + "res" + File.separator;		
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

	protected boolean copyResFile(String fromFilename, String toFilename) throws IOException {
		File fromFile = new File(resDir + File.separator + fromFilename);
		boolean result = fromFile.exists();
		
		Path fromPath = FileSystems.getDefault().getPath(resDir + File.separator + fromFilename);
		Path toPath = FileSystems.getDefault().getPath(resDir + File.separator + toFilename);
		
		Files.copy(fromPath, toPath, java.nio.file.StandardCopyOption.REPLACE_EXISTING);
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

	protected boolean copyResFiles(String prefix, String[] filenames) throws IOException {
		String[] prefixedFilenames = new String[filenames.length];
		boolean result = false;
		
		for (int i = 0; i < filenames.length; i++)
			prefixedFilenames[i] = prefix + filenames[i];

		// First try to remove the prefixed files in case they are there
		eraseResFiles(prefixedFilenames);
		
		// Then rename the filenames to prefixed files
		for (int i = 0; i < filenames.length; i++)
			result |= copyResFile(filenames[i], prefixedFilenames[i]);
		return result;
	}
	
	protected String archive(String filename, int s) {
		return "archive" + File.separator + filename + "." + s + ".";
	}
	
	protected String archive(String filename) {
		String result = "archive" + File.separator;
		
		if (!filename.isEmpty())
			result += filename + ".";
		return result;
	}

	protected String upToDot(String text) {
		int index = text.indexOf('.');
		
		return index < 0 ? text : text.substring(0,  index);
	}

	protected void  plotResults(String plotPlt, String inPlt, String outPlt) throws Exception {	
		plotPlt = resDir + plotPlt;
		inPlt = resDir + inPlt;
		outPlt = resDir + outPlt;
		
		GnuPlotProcess gnuPlotProcess = new GnuPlotProcess(plotPlt, "pngcairo", inPlt, outPlt);
		gnuPlotProcess.execute();
	}
	
	protected void runAutoPdf(int s, int v, String docTxt, String docPdf, String docOdt, String inWm, String inTxt) throws Exception {
		String autoPdfCmd = Paths.get("..\\..\\haathi\\autoPdf\\Debug-StatLink\\autoPdf.exe").toAbsolutePath().normalize().toString();
		
		ArrayList<String> command = new ArrayList<String>();
		command.add(autoPdfCmd);
		command.add("S" + s);
		command.add("V" + v);
		command.add(resDir + docTxt);
		command.add(resDir + docPdf);
		command.add(resDir + docOdt);
		command.add(resDir + inWm);
		command.add(resDir + inTxt);

		ExternalProcess process = new ExternalProcess(command);
		process.directory(OPEN_OFFICE_DIR);
		process.execute();	
	}
}
/* **************************************************************************/
