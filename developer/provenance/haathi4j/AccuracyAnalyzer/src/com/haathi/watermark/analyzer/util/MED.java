/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/util/MED.java#5 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer.util;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class MED {
	public class Confirmation extends Edit {
		// The source character and target character match
		public Confirmation(int nextSourceIndex, int nextTargetIndex) {
			super(MED.CONFIRMATION, nextSourceIndex - 1, nextTargetIndex - 1, nextSourceIndex, nextTargetIndex);
		}
		
		public void print(PrintStream printStream, String sourceString, String targetString) {
			printStream.println("Confirmation" + "\t" +
					getPrevSourceIndex() + "\t" + Escaper.escape(getSourceChar(sourceString)) + "\t" +
					getPrevTargetIndex() + "\t" + Escaper.escape(getTargetChar(targetString))
			);
		}
	}
	
	public class Insertion extends Edit {
		public Insertion(int nextSourceIndex, int nextTargetIndex) {
			super(MED.INSERTION, nextSourceIndex, nextTargetIndex - 1, nextSourceIndex, nextTargetIndex);
		}
		
		public void print(PrintStream printStream, String sourceString, String targetString) {
			printStream.println("Insertion" + "\t" +
					"-" + "\t" + "_" + "\t" +
					getPrevTargetIndex() + "\t" + Escaper.escape(getTargetChar(targetString))
			);
		}
	}
	
	public class Substitution extends Edit {
		// The source character has been misinterpreted as the target character
		public Substitution(int nextSourceIndex, int nextTargetIndex) {
			super(MED.SUBSTITUTION, nextSourceIndex - 1, nextTargetIndex - 1, nextSourceIndex, nextTargetIndex);
		}
		
		public void print(PrintStream printStream, String sourceString, String targetString) {
			printStream.println("Substitution" + "\t" + 
					getPrevSourceIndex() + "\t" + Escaper.escape(getSourceChar(sourceString)) + "\t" + 
					getPrevTargetIndex() + "\t" + Escaper.escape(getTargetChar(targetString))
			);
		}
	}

	public class Deletion extends Edit {
		public Deletion(int nextSourceIndex, int nextTargetIndex) {
			super(MED.DELETION, nextSourceIndex - 1, nextTargetIndex, nextSourceIndex, nextTargetIndex);
		}
		
		public void print(PrintStream printStream, String sourceString, String targetString) {
			printStream.println("Deletion" + "\t" + 
					getPrevSourceIndex() + "\t" + Escaper.escape(getSourceChar(sourceString)) + "\t" +
					"-" + "\t" + "_"
			);
		}
	}

	// These are recorded now in the order of preference for tie breaking where we want
	// deletions to win when the target text is shorter than the source.
	public static final int DELETION = 0;
	public static final int CONFIRMATION = 1;
	public static final int INSERTION = 2;
	public static final int SUBSTITUTION = 3;
	
	protected String sourceString;
	protected String targetString;
	protected int[][] distances;
	protected int[][] minIndexes;
	protected ArrayList<Edit> edits;
	
	public MED(String sourceString, String targetString) {
		this.sourceString = sourceString;
		this.targetString = targetString;
		distances = new int[targetString.length() + 1][sourceString.length() + 1];
		minIndexes = new int[targetString.length() + 1][sourceString.length() + 1];
	}
	
	protected int getConfirmationCost(char sourceChar, char targetChar) {
		return sourceChar == targetChar ? 0 : Integer.MAX_VALUE;
	}
	
	protected int getInsertionCost(char c) {
		return 10;
	}
	
	protected int getDeletionCost(char c) {
		return 10;
	}
	
	protected int getSubstitutionCost(char sourceChar, char targetChar) {
		return sourceChar != targetChar ? 13 : Integer.MAX_VALUE;
	}
	
	protected int calcConfirmationCost(int sourceIndex, int targetIndex) {
		if (targetIndex == 0 && sourceIndex == 0)
			return 0;
		if (targetIndex == 0 || sourceIndex == 0)
			return Integer.MAX_VALUE;
		
		int cost = getConfirmationCost(sourceString.charAt(sourceIndex - 1), targetString.charAt(targetIndex - 1));
		
		if (cost == Integer.MAX_VALUE)
			return cost;
		return distances[targetIndex - 1][sourceIndex - 1] + cost;
	}

	protected int calcInsertionCost(int sourceIndex, int targetIndex) {
		if (targetIndex == 0)
			return Integer.MAX_VALUE;
		
		int cost = getInsertionCost(targetString.charAt(targetIndex - 1));
		
		if (cost == Integer.MAX_VALUE)
			return cost;
		return distances[targetIndex - 1][sourceIndex] + cost;
	}
	
	protected int calcSubstitutionCost(int sourceIndex, int targetIndex) {
		if (targetIndex == 0 && sourceIndex == 0)
			return 0;
		if (targetIndex == 0 || sourceIndex == 0)
			return Integer.MAX_VALUE;
		
		int cost = getSubstitutionCost(sourceString.charAt(sourceIndex - 1), targetString.charAt(targetIndex - 1));
		
		if (cost == Integer.MAX_VALUE)
			return cost;
		return distances[targetIndex - 1][sourceIndex - 1] + cost;
	}
	
	protected int calcDeletionCost(int sourceIndex, int targetIndex) {
		if (sourceIndex == 0)
			return Integer.MAX_VALUE;
		
		int cost = getDeletionCost(sourceString.charAt(sourceIndex - 1));
		
		if (cost == Integer.MAX_VALUE)
			return cost;
		return distances[targetIndex][sourceIndex - 1] + cost;
	}
	
	protected int indexOfMin(int[] array) {
		// Can assume not empty because it isn't.
		int index = 0;
		int min = array[0];
		
		for (int i = 1; i < array.length; i++)
			if (array[i] < min) {
				min = array[i];
				index = i;
			}
		return index;
	}
	
	public int measure() {
		int[] costs = new int[4];
		
		for (int targetIndex = 0; targetIndex < targetString.length() + 1; targetIndex++)
			for (int sourceIndex = 0; sourceIndex < sourceString.length() + 1; sourceIndex++) {
				costs[CONFIRMATION] = calcConfirmationCost(sourceIndex, targetIndex);
				costs[INSERTION] = calcInsertionCost(sourceIndex, targetIndex);
				costs[SUBSTITUTION] = calcSubstitutionCost(sourceIndex, targetIndex);
				costs[DELETION] = calcDeletionCost(sourceIndex, targetIndex);
				
				int minIndex = indexOfMin(costs);
				int minCost = costs[minIndex];
				
				distances[targetIndex][sourceIndex] = minCost;
				minIndexes[targetIndex][sourceIndex] = minIndex;
			}
		edits = newEdits();
		return distances[targetString.length()][sourceString.length()];
	}
	
	public void printDistancesOn(PrintStream printStream) {
		printStream.print("\t");
		for (int sourceIndex = 0; sourceIndex < sourceString.length() + 1; sourceIndex++) {
			if (sourceIndex > 0)
				printStream.print(sourceString.charAt(sourceIndex - 1));
			printStream.print("\t");
		}
		printStream.println();

		for (int targetIndex = 0; targetIndex < targetString.length() + 1; targetIndex++) {
			for (int sourceIndex = 0; sourceIndex < sourceString.length() + 1; sourceIndex++) {
				if (sourceIndex == 0) {
					if (targetIndex > 0)
						printStream.print(targetString.charAt(targetIndex - 1));
					printStream.print("\t");
				}
				printStream.print(distances[targetIndex][sourceIndex]);
				printStream.print("\t");
			}
			printStream.println();
		}
	}
	
	protected Edit getEdit(int sourceIndex, int targetIndex) {
		switch (minIndexes[targetIndex][sourceIndex]) {
			case CONFIRMATION:
				return new Confirmation(sourceIndex, targetIndex);
			case INSERTION:
				return new Insertion(sourceIndex, targetIndex);
			case SUBSTITUTION:
				return new Substitution(sourceIndex, targetIndex);
			case DELETION:
				return new Deletion(sourceIndex, targetIndex);
		}
		return null;
	}
	
	protected ArrayList<Edit> newEdits() {
		ArrayList<Edit> edits = new ArrayList<Edit>();
		int sourceIndex = sourceString.length(); 
		int targetIndex = targetString.length();
		
		while (!(sourceIndex == 0 && targetIndex == 0)) {
			Edit edit = getEdit(sourceIndex, targetIndex);

			sourceIndex = edit.getPrevSourceIndex();
			targetIndex = edit.getPrevTargetIndex();
			edits.add(edit);
		}
		Collections.reverse(edits);		
		return edits;
	}
	
	public void printEditsOn(PrintStream printStream, boolean onlyErrors) {
		Edit.printHeader(printStream);
		for (Edit edit: edits)
			if (!(onlyErrors && edit.getType() == CONFIRMATION))
				edit.print(printStream, sourceString, targetString);
	}

	public void printSummaryOn(PrintStream printStream) {
		HashMap<String, Integer> counts = new HashMap<String, Integer>();
		
		for (Edit edit: edits) {
			String className = edit.getClass().getName();
						
			if (counts.containsKey(className))
				counts.put(className, counts.get(className) + 1);
			else
				counts.put(className, 1);
		}
			
		for (String className: counts.keySet()) {
			String[] parts = className.split("\\$");
			String lastPart = parts[parts.length - 1];

			printStream.print(lastPart + "\t");
		}
		printStream.println();
		for (String className: counts.keySet())
			printStream.print(counts.get(className) + "\t");
		printStream.println();
	}

	public ArrayList<Edit> getEdits() {
		return edits;
	}
	
	public char getSourceChar(Edit edit) {
		return edit.getSourceChar(sourceString);
	}
	
	public char getTargetChar(Edit edit) {
		return edit.getTargetChar(targetString);
	}
		
	public static void main(String[] args) {
//		MED med = new MED("cat", "cars");
//		MED med = new MED("kitten", "sitting");
		MED med = new MED("Sunday", "Saturday");
		
		System.out.println(med.measure());
		med.printDistancesOn(System.out);
		med.printEditsOn(System.out, true);
		System.out.println();
	}
}
/* **************************************************************************/
