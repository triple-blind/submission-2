/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/AccuracyMeter/src/com/haathi/performance/accuracy/AccuracyMeter.java#14 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.performance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class SharedPerformance {
	
	protected String readAllBytes(String path) throws IOException {
		byte[] encoded = Files.readAllBytes(Paths.get(path));
		
		return new String(encoded, StandardCharsets.US_ASCII);
	}
	
	protected ArrayList<String> getHeaders(BufferedReader bufferedReader) throws IOException {
		String line = bufferedReader.readLine();
		String[] substrings = line.split("\t");
		
		return toArrayList(substrings);
	}
	
	protected void putHeaders(ArrayList<String> headers, BufferedWriter bufferedWriter) throws IOException {
		for (int i = 0; i < headers.size(); i++) {
			if (i > 0)
				bufferedWriter.write("\t");
			bufferedWriter.write(headers.get(i));
		}
		bufferedWriter.write("\n");
	}
	
	protected ArrayList<String> toArrayList(String[] array) {
		ArrayList<String> arrayList = new ArrayList<String>(array.length);
		
		for (int i = 0; i < array.length; i++)
			arrayList.add(array[i]);
		return arrayList;
	}	
}
/* **************************************************************************/
