/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/WordNet/src/only/file/DatFile.java#1 $
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
public class DatFile extends WordNetFile {
	static String[] syntactic_markers = new String[] { "(p)", "(a)", "(ip)" };
	
	public class Record	{
		public int synset_offset;
		public int lex_filenum;
		public char ss_type;
		public int w_cnt;
		public ArrayList<String> words;
		public ArrayList<Integer> lex_ids;
		// We don't need to rest now, so I'm skipping those
		public int upperCount;
		public int lowerCount;
		
		public Record(String line, boolean isAdjective) throws Exception {
			words = new ArrayList<String>();
			lex_ids = new ArrayList<Integer>();
			upperCount = 0;
			lowerCount = 0;
			
			String[] fields = line.split(" ");
			int index = 0;
			
			if (fields.length < 6)
				throw new Exception("Too few fields!");
			
			if (fields[index].length() != 8)
				throw new Exception("synset_offset has the wrong length!");			
			synset_offset = Integer.parseInt(fields[index++]);
			
			if (fields[index].length() != 2)
				throw new Exception("lex_filenum has the wrong length!");			
			lex_filenum = Integer.parseInt(fields[index++]);
			
			if (fields[index].length() != 1)
				throw new Exception("ss_type has the wrong length!");			
			ss_type = fields[index++].charAt(0);
			
			if (fields[index].length() != 2)
				throw new Exception("w_cnt has the wrong length!");			
			w_cnt = Integer.parseInt(fields[index++], 16);
			if (w_cnt < 1)
				throw new Exception("w_cnt should be >= 1!");
			
			if (fields.length < 6 + (w_cnt - 1) * 2)
				throw new Exception("Too few fields!");
		
			for (int i = 0; i < w_cnt; i++) {
				if (fields[index].length() == 0)
					throw new Exception("word is empty");
				String word = fields[index++];			

				if (isAdjective)			
					for (String syntactic_marker: syntactic_markers)
						if (word.endsWith(syntactic_marker)) {
							word = word.substring(0, word.length() - syntactic_marker.length());
							break;
						}
				
				for (int j = 0; j < word.length(); j++) {
					char c = word.charAt(j);

					if (Character.isLetter(c)) {
						if (Character.isUpperCase(c))
							upperCount++;
						else
							lowerCount++;
						break;
					}
				}				
				words.add(word);
				
				if (fields[index].length() != 1)
					throw new Exception("lex_id is wrong length!");
				int lex_id = Integer.parseInt(fields[index++], 16);
				lex_ids.add(lex_id);
			}
		}
	}
	
	protected String filename;
	
	public DatFile(String filename) {
		this.filename = filename;
	}
	
	protected Record newRecord(String line, boolean isAdjective) throws Exception {
		if (line == null)
			return null;
		if (line.charAt(0) == ' ')
			return null;
		return new Record(line, isAdjective);
	}
	
	public ArrayList<Record> read(boolean isAdjective) throws Exception	{
		ArrayList<Record> records = new ArrayList<Record>();
		BufferedReader bufferedReader = null;
		
		try {
			bufferedReader = new BufferedReader(new FileReader(new File(filename)));

		    String line;
			
			while ((line = bufferedReader.readLine()) != null) {
				Record record = newRecord(line, isAdjective);
				
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
