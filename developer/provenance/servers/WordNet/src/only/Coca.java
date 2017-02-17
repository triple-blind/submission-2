/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/WordNet/src/only/Coca.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Random;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class Coca extends WordNet {
	
	protected boolean printStats;
	protected int wordCount;
	protected int letterCount;
	
	public Coca() {
		printDocument = true;
		printSynsets = false;
		printWatermark = false;
		printStats = false;
	}
	
	protected boolean isPunct(char c) {
		return !(Character.isAlphabetic(c) || Character.isDigit(c));
	}
	
	protected boolean isAllPunct(String word) {
		for (int i = 0; i < word.length(); i++)
			if (!isPunct(word.charAt(i)))
				return false;
		return true;
	}
	
	protected boolean isMarker(String word) {
		return word.length() > 2 && word.charAt(0) == '#' && word.charAt(1) == '#';
	}
	
	protected boolean isXmlTag(String word) {
		return word.length() >= 2 &&
				word.charAt(0) == '<' && word.charAt(word.length() - 1) == '>';
	}
	
	protected void watermarkLine(String line, HashMap<String, ArrayList<String>> codeBook, Random random) throws Exception {
		String[] words = line.split(" ");
		
		for (int i = 0; i < words.length; i++) {
			String word = words[i];
			
			if (!isAllPunct(word) && !isXmlTag(word) && !isMarker(word)) {
				letterCount += word.length();
				word = watermarkWord(word, codeBook, random);
				wordCount++;
			
				if (printDocument){ 
					if (i != 0)
						System.out.print(" ");
					System.out.print(word);
				}
			}
		}
		if (printDocument)
			System.out.println();
	}	
	
	public void run() throws Exception {
		HashMap<String, ArrayList<String>> codeBook = getCodeBook();
		
		substitutions = 0;
		information = 0;
		wordCount = 0;
		letterCount = 0;
		
		//ArrayList<String> lines = getDocument(getFilename("w_spok_2012.txt"));
		//ArrayList<String> lines = getDocument(getFilename("DeclarationOfIndependence.txt"));
		ArrayList<String> lines = getDocument(getFilename("ReasoningAboutofStrengthofObfuscation.txt"));
		//ArrayList<String> lines = getDocument(getFilename("ProjectSummary.txt"));
		//ArrayList<String> lines = getDocument(getFilename("contributions.txt"));
		
		for (String line: lines)
			watermarkLine(line, codeBook, new Random(0));

		if (printStats) {
			System.out.println(
					"wordCount\t" +
					"letterCount\t" +
					"substitutions\t" +
					"information\t" +
					"bits/word\t" +
					"bits/letter\t" +
					"words/bit\t" +
					"letters/bit");
			System.out.println("" +
					wordCount + "\t" +
					letterCount + "\t" +
					substitutions + "\t" +
					information + "\t" +
					information / wordCount + "\t" +
					information / letterCount + "\t" +
					(float) wordCount / information + "\t" +
					(float) letterCount / information);
		}
	}
	
	public static void main(String args[]) throws Exception {
			new Coca().run();
	}
}
/* **************************************************************************/
