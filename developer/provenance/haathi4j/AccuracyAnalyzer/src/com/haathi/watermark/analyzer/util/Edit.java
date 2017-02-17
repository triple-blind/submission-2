/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/util/Edit.java#2 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer.util;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.PrintStream;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class Edit {
	protected int type;
	protected int prevSourceIndex;
	protected int prevTargetIndex;
	protected int nextSourceIndex;
	protected int nextTargetIndex;
	
	public static void printHeader(PrintStream printStream) {
		printStream.println("Type" + "\t" +
				"Source Index" + "\t" + "Source Value" + "\t" +
				"Target Index" + "\t" + "Target Value"
		);
	}
	
	public Edit(int type, int prevSourceIndex, int prevTargetIndex, int nextSourceIndex, int nextTargetIndex) {
		this.type = type;
		this.prevSourceIndex = prevSourceIndex;
		this.prevTargetIndex = prevTargetIndex;
		this.nextSourceIndex = nextSourceIndex;
		this.nextTargetIndex = nextTargetIndex;
	}
	
	public int getType() {
		return type;
	}
	
	public int getPrevSourceIndex() {
		return prevSourceIndex;
	}
	
	public int getPrevTargetIndex() {
		return prevTargetIndex;
	}

	public int getNextSourceIndex() {
		return nextSourceIndex;
	}
	
	public int getNextTargetIndex() {
		return nextTargetIndex;
	}
	
	public void print(PrintStream printStream, String sourceString, String targetString) {
		printStream.println();
	}
	
	public char getSourceChar(String sourceString) {
		return sourceString.charAt(prevSourceIndex);
	}
	
	public char getTargetChar(String targetString) {
		return targetString.charAt(prevTargetIndex);
	}
}	
/* **************************************************************************/
