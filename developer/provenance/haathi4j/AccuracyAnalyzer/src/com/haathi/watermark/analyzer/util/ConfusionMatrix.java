/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/WatermarkBitAnalyzer/src/com/haathi/watermark/analyzer/util/ConfusionMatrix.java#4 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.watermark.analyzer.util;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.PrintStream;
import java.util.HashMap;
import java.util.TreeSet;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class ConfusionMatrix<T> {

	static public class Accuracy {
		public int goodCount;
		public int allCount;
		
		public Accuracy() {
			this(0, 0);
		}
		
		public Accuracy(int goodCount, int allCount) {
			this.goodCount = goodCount;
			this.allCount = allCount;
		}
		
		public void add(Accuracy other) {
			goodCount += other.goodCount;
			allCount += other.allCount;
		}
	}
	
	protected HashMap<T, HashMap<T, Integer>> confusions;
	protected TreeSet<T> keys;
	
	public ConfusionMatrix() {
		confusions = new HashMap<T, HashMap<T, Integer>>();
		keys = new TreeSet<T>();
	}
	
	public HashMap<T, Integer> get(T in) {
		if (confusions.containsKey(in))
			return confusions.get(in);
		return null;
	}
	
	public Integer get(T in, T out) {
		if (confusions.containsKey(in)) {
			HashMap<T, Integer> confusion = confusions.get(in);
			
			if (confusion.containsKey(out))
				return confusion.get(out);
		}
		return 0;
	}
	
	public void add(T in, T out) {
		HashMap<T, Integer> confusion;
		int count;
		
		keys.add(in);
		keys.add(out);
		if (confusions.containsKey(in))
			confusion = confusions.get(in);
		else {
			confusion = new HashMap<T, Integer>();
			confusions.put(in,  confusion);
		}
		
		if (confusion.containsKey(out))
			count = confusion.get(out);
		else
			count = 0;
		count++;
		confusion.put(out, count);
	}
	
	public void printOn(PrintStream printStream, String name) {
		printStream.print(name);
		for (T out: keys) {
			printStream.print("\t");
			printStream.print(out.toString());
		}
		printStream.println();
		for (T in: keys) {
			printStream.print(in.toString());
			for (T out: keys) {
				printStream.print("\t");
				printStream.print(get(in, out));
			}
			printStream.println();
		}
	}
	
	public Accuracy getAccuracy(T[] inKeys) {
		int goodCount = 0;
		int allCount = 0;
		
		for (T inKey: inKeys)
			if (confusions.containsKey(inKey)) {
				HashMap<T, Integer> confusion = confusions.get(inKey);
				
				for (T outKey: keys) {
					if (confusion.containsKey(outKey)) {
						Integer count = confusion.get(outKey);
						
						allCount += count;
						if (inKey.equals(outKey))
							goodCount += count;
					}
				}
			}
		return new Accuracy(goodCount, allCount);
	}
}
/* **************************************************************************/
