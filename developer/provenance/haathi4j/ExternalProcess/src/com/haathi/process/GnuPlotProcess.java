/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/ExternalProcess/src/com/haathi/process/GnuPlotProcess.java#2 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.process;
/* ***************************************************************************
 Imports
 ****************************************************************************/

/* ***************************************************************************
 Class
 ****************************************************************************/
public class GnuPlotProcess extends ExternalProcess {
	public static final String GNU_PLOT = "D:\\ProgramFilesx86\\gnuplot\\bin\\gnuplot.exe";

	public GnuPlotProcess(String plotFile, String format, String inputFile, String outputFile) {
		super();
		command.add(GNU_PLOT);
		command.add("-e");
		
		inputFile = inputFile.replace("\\", "\\\\");
		outputFile = outputFile.replace("\\", "\\\\");
		
		StringBuffer stringBuffer = new StringBuffer();
		stringBuffer.append("\"");
		// Need to quote strings for GnuPlot, then escape the quote for Java,
		// and then escape the quote for GnuPlot, and then escape the escape for Java.
		stringBuffer.append("terminal='" + format + "'; ");
		stringBuffer.append("input='" + inputFile + "'; ");
		stringBuffer.append("output='" + outputFile + "'; ");
		stringBuffer.append("\"");
		
		command.add(stringBuffer.toString());
		command.add(plotFile);
	}
	
	public static void main(String[] args) {
		try {
			GnuPlotProcess gnuPlotProcess = new GnuPlotProcess("res\\test.plt", "png", "res\\test.dat", "res\\test.png");
		
			gnuPlotProcess.execute();
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
		}
	}
}
/* **************************************************************************/
