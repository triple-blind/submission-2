/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/WordNet/src/only/file/IdxFile.java#1 $
 *****************************************************************************
 Notes
 
 Nouns
 	117953 for all
 	57616 after phrases removed
 	50775 after duplicates removed
 	50462 after exceptions removed
 	40033 after choiceless removed
 	
 Verbs
 	11540 for all
 	8702 after phrases removed
 	4488 after duplicates removed
 	4487 after exceptions removed
 	2437 after choiceless removed
 	
 Adjectives
 	21499 for all
 	20994 after phrases removed
 	17478 after duplicates removed
 	17120 after exceptions removed
 	13478 after choiceless removed
 
 Adverbs
 	4475 for all
 	3780 after phrases removed
 	3255 after duplicates removed
 	3254 after exceptions removed
 	2725 after choiceless removed
 *****************************************************************************
 Package
 ****************************************************************************/
package only.file;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

import java.util.ArrayList;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class IdxFile extends WordNetFile {
	
	public class Record	{
		public String lemma;
		public char pos;
		public int synset_cnt;
		public int p_cnt;
		public ArrayList<String> ptr_symbols;
		public int sense_cnt;
		public int tagsense_cnt;
		public ArrayList<Integer> synset_offsets;
		
		public Record(String line) throws Exception {
			ptr_symbols = new ArrayList<String>();
			synset_offsets = new ArrayList<Integer>();
			
			String[] fields = line.split(" ");
			int index = 0;
			
			if (fields.length < 7)
				throw new Exception("Too few fields!");
			
			if (fields[index].length() == 0)
				throw new Exception("lemma is empty!");			
			lemma = fields[index++];
			
			if (fields[index].length() != 1)
				throw new Exception("pos length is incorrect!");
			pos = fields[index++].charAt(0);
			
			if (fields[index].length() == 0)
				throw new Exception("synset_cnt is empty!");
			synset_cnt = Integer.parseInt(fields[index++]);
			if (synset_cnt < 1)
				throw new Exception("lemmas must belong to at least one synset!");

			if (fields[index].length() == 0)
				throw new Exception("p_cnt is empty!");
			p_cnt = Integer.parseInt(fields[index++]);
			
			if (fields.length < 7 + p_cnt)
				throw new Exception("Too few fields given p_cnt of " + p_cnt);
			
			for (int i = 0; i < p_cnt; i++) {
				if (fields[index].length() == 0)
					throw new Exception("ptr_symbol is empty!");
				ptr_symbols.add(fields[index++]);
			}
			
			if (fields[index].length() == 0)
				throw new Exception("sense_cnt is empty!");
			sense_cnt = Integer.parseInt(fields[index++]);
			if (sense_cnt != synset_cnt)
				throw new Exception("sense_cnt and synset_cnt must be equal!");

			if (fields[index].length() == 0)
				throw new Exception("tagsense_cnt is empty!");
			tagsense_cnt = Integer.parseInt(fields[index++]);
			
			if (fields.length != 7 + p_cnt + synset_cnt - 1)
				throw new Exception("Too few fields given p_cnt of " + p_cnt + " synset_cnt of " + synset_cnt);
			
			for (int i = 0; i < synset_cnt; i++) {
				if (fields[index].length() != 8)
					throw new Exception("synset_offset format is incorrect!");
				synset_offsets.add(Integer.parseInt(fields[index++]));
			}
		}
		
		public Integer getSynsetOffset() {
			return synset_offsets.get(0);
		}
	}
	
	protected String filename;
	
	public IdxFile(String filename) {
		this.filename = filename;
	}
	
	protected Record newRecord(String line) throws Exception {
		if (line == null)
			return null;
		if (line.charAt(0) == ' ')
			return null;
		
		Record record = new Record(line);
		
		// Rule out phrases
		if (record.lemma.contains("_"))
			return null;
		return record;
	}
	
	public ArrayList<Record> read() throws Exception {
		ArrayList<Record> records = new ArrayList<Record>();
		BufferedReader bufferedReader = null;
		
		try {
			bufferedReader = new BufferedReader(new FileReader(new File(filename)));

		    String line;
			
			while ((line = bufferedReader.readLine()) != null) {
				Record record = newRecord(line);
				
				if (record != null)
					records.add(record);
			}
		}
		finally {
			if (bufferedReader != null)
				bufferedReader.close();
		}
		return records;
	}
}
/* **************************************************************************/
