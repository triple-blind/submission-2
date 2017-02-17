/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/WordNet/src/only/WordNet.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Random;

import only.file.DatFile;
import only.file.ExcFile;
import only.file.IdxFile;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class WordNet {
	
	protected static final String CLASS_SUBDIR = "./res/";
	protected static final String JAR_SUBDIR = "./res/";

	protected boolean printSynsets = true;
	protected boolean printDocument = false;
	protected boolean printWatermark = false;
	protected boolean saveSynsets = true;
	
	protected int substitutions;
	protected float information;
	
	protected boolean isJar() {
		String className = "/" + getClass().getName().replace('.', '/') + ".class";
		String resourceName = getClass().getResource(className).toString();
		
		return !resourceName.startsWith("file:");
	}
	
	protected String getFilename(String filename) {
		return isJar() ? JAR_SUBDIR + filename : CLASS_SUBDIR + filename;
	}
	
	protected ArrayList<ArrayList<IdxFile.Record>> newIdxRecords() throws Exception {
		ArrayList<ArrayList<IdxFile.Record>> idxRecords = new ArrayList<ArrayList<IdxFile.Record>>();

		idxRecords.add(new IdxFile(getFilename("index.noun")).read());
		idxRecords.add(new IdxFile(getFilename("index.verb")).read());
		idxRecords.add(new IdxFile(getFilename("index.adj")).read());
		idxRecords.add(new IdxFile(getFilename("index.adv")).read());

		return idxRecords;
	}

	protected ArrayList<ArrayList<ExcFile.Record>> newExcRecords() throws Exception {
		ArrayList<ArrayList<ExcFile.Record>> excRecords = new ArrayList<ArrayList<ExcFile.Record>>();

		excRecords.add(new ExcFile(getFilename("noun.exc")).read());
		excRecords.add(new ExcFile(getFilename("verb.exc")).read());
		excRecords.add(new ExcFile(getFilename("adj.exc")).read());
		excRecords.add(new ExcFile(getFilename("adv.exc")).read());

		return excRecords;
	}

	protected ArrayList<ArrayList<DatFile.Record>> newDatRecords() throws Exception {
		ArrayList<ArrayList<DatFile.Record>> datRecords = new ArrayList<ArrayList<DatFile.Record>>();

		datRecords.add(new DatFile(getFilename("data.noun")).read(false));
		datRecords.add(new DatFile(getFilename("data.verb")).read(false));
		datRecords.add(new DatFile(getFilename("data.adj")).read(true));
		datRecords.add(new DatFile(getFilename("data.adv")).read(false));

		return datRecords;
	}
	
	protected ArrayList<ArrayList<IdxFile.Record>> removeIdxDuplicates(ArrayList<ArrayList<IdxFile.Record>> oldIdxRecords) {
		ArrayList<ArrayList<IdxFile.Record>> newIdxRecords = new ArrayList<ArrayList<IdxFile.Record>>();
		HashMap<String, Integer> counts = new HashMap<String, Integer>();
		
		for (ArrayList<IdxFile.Record> posIdxRecords: oldIdxRecords) {
			for (IdxFile.Record idxRecord: posIdxRecords) {
				String lemma = idxRecord.lemma;
				int count = counts.containsKey(lemma) ? counts.get(lemma) : 0;
					
				counts.put(lemma, count + 1);
			}
		}
		
		for (ArrayList<IdxFile.Record> posIdxRecords: oldIdxRecords) {
			ArrayList<IdxFile.Record> nonduplicateRecords = new ArrayList<IdxFile.Record>();

			newIdxRecords.add(nonduplicateRecords);
			for (IdxFile.Record idxRecord: posIdxRecords) {
				String lemma = idxRecord.lemma;
				int count = counts.get(lemma);
				
				if (count == 1)
					nonduplicateRecords.add(idxRecord);
			}
		}
		return newIdxRecords;
	}

	protected ArrayList<ArrayList<IdxFile.Record>> removeExcDuplicates(ArrayList<ArrayList<IdxFile.Record>> oldIdxRecords,
				ArrayList<ArrayList<ExcFile.Record>> excRecords) {
		ArrayList<ArrayList<IdxFile.Record>> newIdxRecords = new ArrayList<ArrayList<IdxFile.Record>>();
		ArrayList<HashMap<String, Integer>> allCounts = new ArrayList<HashMap<String, Integer>>();
		ArrayList<HashMap<String, Integer>> selfCounts = new ArrayList<HashMap<String, Integer>>();
	
		for (ArrayList<ExcFile.Record> posExcRecords: excRecords) {
			HashMap<String, Integer> posAllCounts = new HashMap<String, Integer>();
			HashMap<String, Integer> posSelfCounts = new HashMap<String, Integer>();

			allCounts.add(posAllCounts);
			selfCounts.add(posSelfCounts);
			for (ExcFile.Record excRecord: posExcRecords) {
				String inflected = excRecord.inflected;
				
				{
					int count = posAllCounts.containsKey(inflected) ? posAllCounts.get(inflected) : 0;
					
					posAllCounts.put(inflected, count + 1);
				}
				{
					int count = excRecord.bases.size();
					
					if (!excRecord.mapsToSelf())
						posSelfCounts.put(inflected, count);
				}
			}
		}
		
		for (int i = 0; i < oldIdxRecords.size(); i++) {
			ArrayList<IdxFile.Record> posIdxRecords = oldIdxRecords.get(i); 
			ArrayList<IdxFile.Record> nonduplicateRecords = new ArrayList<IdxFile.Record>();

			newIdxRecords.add(nonduplicateRecords);
			for (IdxFile.Record idxRecord: posIdxRecords) {
				String lemma = idxRecord.lemma;
				boolean ruledOut = false;
				
				for (int j = 0; j < allCounts.size(); j++)
					if (i != j && allCounts.get(j).containsKey(lemma)) {
						ruledOut = true;
						break;
					}
				if (ruledOut)
					continue;
				// If they are there at all, they aren't mapped to self, since that was ruled out above.
				if (selfCounts.get(i).containsKey(lemma))
					ruledOut = true;
				if (ruledOut)
					continue;
				nonduplicateRecords.add(idxRecord);
			}
		}
		return newIdxRecords;
	}

	protected boolean isLower(String lemma) {
		for (int i = 0; i < lemma.length(); i++) {
			char c = lemma.charAt(i);

			if (Character.isAlphabetic(c))
				return Character.isLowerCase(c);
		}
		return false;
	}
	
	protected boolean isUpper(String lemma) {
		for (int i = 0; i < lemma.length(); i++) {
			char c = lemma.charAt(i);

			if (Character.isAlphabetic(c))
				return Character.isUpperCase(c);
		}
		return false;
	}
	
	protected ArrayList<ArrayList<IdxFile.Record>> removeDatMixedCase(ArrayList<ArrayList<IdxFile.Record>> oldIdxRecords,
			ArrayList<HashMap<Integer, DatFile.Record>> datMaps) {
		ArrayList<ArrayList<IdxFile.Record>> newIdxRecords = new ArrayList<ArrayList<IdxFile.Record>>();
	
		for (int i = 0; i < oldIdxRecords.size(); i++) {
			ArrayList<IdxFile.Record> posIdxRecords = oldIdxRecords.get(i); 
			ArrayList<IdxFile.Record> sameCaseRecords = new ArrayList<IdxFile.Record>();
	
			newIdxRecords.add(sameCaseRecords);
			for (IdxFile.Record idxRecord: posIdxRecords) {
				String lemma = idxRecord.lemma;
				int synset_offset = idxRecord.getSynsetOffset();
				DatFile.Record datRecord = datMaps.get(i).get(synset_offset);

				int matches = 0;
				String match = lemma;
				for (String word: datRecord.words) {
					if (lemma.compareToIgnoreCase(word) == 0) {
						match = word;
						matches++;
					}
				}
				// Mulitiple matches, so have to rule it out for ambiguous capitalization.
				if (matches != 1) {
					//System.out.println();
					continue;
				}
				// Use capitalization from dat file
				idxRecord.lemma = match;
				
				// But only if that is in the majority or lowercase on tie.
				if (isLower(match)) {
					if (datRecord.lowerCount >= datRecord.upperCount)
						sameCaseRecords.add(idxRecord);
					//else
						//System.out.println();
				}
				else if (isUpper(match)) {
					if (datRecord.lowerCount < datRecord.upperCount)
						sameCaseRecords.add(idxRecord);
					//else
						//System.out.println();
				}
				else
					sameCaseRecords.add(idxRecord);					
			}
		}
		return newIdxRecords;
	}
	
	protected ArrayList<ArrayList<IdxFile.Record>> removeIdxAmbiguous(ArrayList<ArrayList<IdxFile.Record>> oldIdxRecords) {
		ArrayList<ArrayList<IdxFile.Record>> newIdxRecords = new ArrayList<ArrayList<IdxFile.Record>>();
		
		for (ArrayList<IdxFile.Record> posIdxRecords: oldIdxRecords) {
			ArrayList<IdxFile.Record> unambiguousRecords = new ArrayList<IdxFile.Record>();

			newIdxRecords.add(unambiguousRecords);
			for (IdxFile.Record idxRecord: posIdxRecords)
				if (idxRecord.synset_cnt == 1)
					unambiguousRecords.add(idxRecord);
		}		
		return newIdxRecords;
	}

	protected ArrayList<HashMap<Integer, ArrayList<IdxFile.Record>>> newSynsets(ArrayList<ArrayList<IdxFile.Record>> idxRecords) throws Exception {
		ArrayList<HashMap<Integer, ArrayList<IdxFile.Record>>> synsets = new ArrayList<HashMap<Integer, ArrayList<IdxFile.Record>>>();
		
		for (ArrayList<IdxFile.Record> posIdxRecords: idxRecords) { 
			HashMap<Integer, ArrayList<IdxFile.Record>> synset = new HashMap<Integer, ArrayList<IdxFile.Record>>();

			synsets.add(synset);
			for (IdxFile.Record idxRecord: posIdxRecords) {
				int synset_offset = idxRecord.synset_offsets.get(0);
				
				if (!synset.containsKey(synset_offset))
					synset.put(synset_offset,  new ArrayList<IdxFile.Record>());		
				synset.get(synset_offset).add(idxRecord);	
			}
		}
		return synsets;
	}

	protected void printSynset(HashMap<Integer, ArrayList<IdxFile.Record>> synset) {
		for (Integer synset_offset: synset.keySet()) { 
			for (IdxFile.Record idxRecord: synset.get(synset_offset))
				System.out.print(idxRecord.lemma + "\t");
			System.out.println();
		}
		System.out.println();
	}
	
	protected void printSynsets(ArrayList<HashMap<Integer, ArrayList<IdxFile.Record>>> synsets) {
		for (HashMap<Integer, ArrayList<IdxFile.Record>> synset: synsets)
			printSynset(synset);
	}
	
	protected void saveSynset(HashMap<Integer, ArrayList<IdxFile.Record>> synset, FileWriter fileWriter) throws IOException {		
		for (Integer synset_offset: synset.keySet()) { 
			boolean first = true;
			
			for (IdxFile.Record idxRecord: synset.get(synset_offset)) {
				if (!first)
					fileWriter.write(' ');
				first = false;
				fileWriter.write(idxRecord.lemma);
			}
			fileWriter.write('\n');
		}
	}
	
	protected void saveSynsets(ArrayList<HashMap<Integer, ArrayList<IdxFile.Record>>> synsets) throws IOException {
		FileWriter fileWriter = new FileWriter(new File(getFilename("dictionary.txt")));
		
		try {
			for (HashMap<Integer, ArrayList<IdxFile.Record>> synset: synsets)
				saveSynset(synset, fileWriter);
		}
		finally {
			fileWriter.close();
		}
	}
	
	protected ArrayList<ArrayList<IdxFile.Record>> removeIdxChoiceless(ArrayList<ArrayList<IdxFile.Record>> oldIdxRecords,
			ArrayList<HashMap<Integer, ArrayList<IdxFile.Record>>> synsets) { 

		for (HashMap<Integer, ArrayList<IdxFile.Record>> synset: synsets) {
			HashSet<Integer> keySet = new HashSet<Integer>(synset.keySet());
			
			for (Integer synset_offset: keySet) {
				if (synset.get(synset_offset).size() <= 1)
					synset.remove(synset_offset);
			}
		}
		
		ArrayList<ArrayList<IdxFile.Record>> newIdxRecords = new ArrayList<ArrayList<IdxFile.Record>>(); 
		for (int i = 0; i < oldIdxRecords.size(); i++) {
			ArrayList<IdxFile.Record> foundRecords = new ArrayList<IdxFile.Record>();
			HashMap<Integer, ArrayList<IdxFile.Record>> synset = synsets.get(i);
			
			newIdxRecords.add(foundRecords);
			for (IdxFile.Record idxRecord: oldIdxRecords.get(i))
				if (synset.containsKey(idxRecord.getSynsetOffset()))
					foundRecords.add(idxRecord);
		}
		return newIdxRecords;
	}
	
	
	protected ArrayList<HashMap<Integer, DatFile.Record>> newDatMaps(ArrayList<ArrayList<DatFile.Record>> datRecords) {
		ArrayList<HashMap<Integer, DatFile.Record>> datMaps = new ArrayList<HashMap<Integer, DatFile.Record>>(); 
		
		for (ArrayList<DatFile.Record> posDatRecords: datRecords) {
			HashMap<Integer, DatFile.Record> datMap = new HashMap<Integer, DatFile.Record>();
			
			datMaps.add(datMap);
			for (DatFile.Record datRecord: posDatRecords) {
				int synset_offset = datRecord.synset_offset;
				
				datMap.put(synset_offset, datRecord);
			}
		}
		return datMaps;
	}
	
	protected HashMap<String, ArrayList<String>> newCodeBook(ArrayList<ArrayList<IdxFile.Record>> idxRecords,
				ArrayList<HashMap<Integer, ArrayList<IdxFile.Record>>> synsets) {
		HashMap<String, ArrayList<String>> codeBook = new HashMap<String, ArrayList<String>>();
		
		for (int i = 0; i < idxRecords.size(); i++) {
			ArrayList<IdxFile.Record> posIdxRecords = idxRecords.get(i);
			HashMap<Integer, ArrayList<IdxFile.Record>> synset = synsets.get(i);
			
			for (IdxFile.Record posIdxRecord: posIdxRecords) {
				ArrayList<IdxFile.Record> records = synset.get(posIdxRecord.getSynsetOffset());
				ArrayList<String> lemmas = new ArrayList<String>();
				
				for (IdxFile.Record record: records)
					lemmas.add(record.lemma);			
				Collections.sort(lemmas);
				codeBook.put(posIdxRecord.lemma, lemmas);
			}
		}
		return codeBook;
	}
	
	protected HashMap<String, ArrayList<String>> getCodeBook() throws Exception {
		ArrayList<ArrayList<IdxFile.Record>> idxRecords = newIdxRecords();
		ArrayList<ArrayList<ExcFile.Record>> excRecords = newExcRecords();
		ArrayList<ArrayList<DatFile.Record>> datRecords = newDatRecords(); 
		ArrayList<HashMap<Integer, DatFile.Record>> datMaps = newDatMaps(datRecords); 

		idxRecords = removeIdxDuplicates(idxRecords); // Those in different parts of speech
		idxRecords = removeExcDuplicates(idxRecords, excRecords); // Likewise, but account for same POS
		idxRecords = removeIdxAmbiguous(idxRecords); //  Those in multiple synsets
		idxRecords = removeDatMixedCase(idxRecords, datMaps); // Keep all lower or all upper 
		
		ArrayList<HashMap<Integer, ArrayList<IdxFile.Record>>> synsets = newSynsets(idxRecords);
		
		idxRecords = removeIdxChoiceless(idxRecords, synsets); //  Those without synset siblings		
		if (printSynsets)
			printSynsets(synsets);
		if (saveSynsets)
			saveSynsets(synsets);
		return newCodeBook(idxRecords, synsets);
	}

	protected ArrayList<String> getDocument(String filename) throws IOException {
		BufferedReader bufferedReader = new BufferedReader(new FileReader(new File(filename)));
		ArrayList<String> document = new ArrayList<String>();
		
	    String line;
		
		while ((line = bufferedReader.readLine()) != null)
			document.add(line);
		bufferedReader.close();
		return document;
	}
	
	protected int findLemma(String lemma, ArrayList<IdxFile.Record> records) {
		for (int i = 0; i < records.size(); i++)
			if (lemma.equals(records.get(i).lemma))
				return i;
		return -1;
	}
	
	protected String watermarkWord(String word, HashMap<String, ArrayList<String>> codeBook, Random random) throws Exception {
		if (codeBook.containsKey(word)) {
			ArrayList<String> synonyms = codeBook.get(word);
			int oldPos = synonyms.indexOf(word);
			int newPos = random.nextInt(synonyms.size() - 1);
			
			if (newPos >= oldPos)
				newPos++;

			String newWord = synonyms.get(newPos);
			
			if (word.equals(newWord))
				throw new Exception("Bad watermark!");
			if (printWatermark)
				System.out.println(word + " -> " + newWord);
			word = newWord;
			substitutions++;
			information += Math.log(synonyms.size()) / Math.log(2);
		}
		return word;
	}
	
	protected void watermarkLine(String line, HashMap<String, ArrayList<String>> codeBook, Random random) throws Exception {
		String[] words = line.split(" ");
		
		for (int i = 0; i < words.length; i++) {
			String word = watermarkWord(words[i], codeBook, random);
			
			if (printDocument){ 
				if (i != 0)
					System.out.print(" ");
				System.out.print(word);
			}
		}
		if (printDocument)
			System.out.println();
	}
	
	public void run() throws Exception {
		HashMap<String, ArrayList<String>> codeBook = getCodeBook();

		substitutions = 0;
		information = 0;
		
		// Produces 31 substitutions of 37 bits
		ArrayList<String> lines = getDocument(getFilename("DeclarationOfIndependence.txt"));
		for (String line: lines)
			watermarkLine(line, codeBook, new Random(0));
	}
	
	public static void main(String args[]) throws Exception {
			new WordNet().run();
	}
}
/* **************************************************************************/
