/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/WordNet/src/only/file/ExcFile.java#1 $
 *****************************************************************************
 Notes
 
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
public class ExcFile extends WordNetFile {
	
	public class Record	{
		public String inflected;
		public ArrayList<String> bases;
		
		public Record(String line) throws Exception {
			bases = new ArrayList<String>();
			
			String[] fields = line.split(" ");
			int index = 0;
			
			if (fields.length < 1)
				throw new Exception("Too few fields!");
			
			if (fields[index].length() == 0)
				throw new Exception("inflected is empty!");			
			inflected = fields[index++];
					
			for (int i = index; i < fields.length; i++) {
				if (fields[index].length() == 0)
					throw new Exception("base is empty!");				
				bases.add(fields[index++]);
			}
		}
		
		public boolean mapsToSelf() {
			return bases.size() == 1 && inflected.equals(bases.get(0));
		}
	}
	
	protected String filename;
	
	public ExcFile(String filename) {
		this.filename = filename;
	}
	
	protected Record newRecord(String line) throws Exception {
		if (line == null)
			return null;
		
		Record record = new Record(line);
		
		return record;
	}
	
	public ArrayList<Record> read() throws Exception {
		ArrayList<Record> records = new ArrayList<Record>();
		BufferedReader bufferedReader = null;
	    String line;

	    try {
	    	bufferedReader = new BufferedReader(new FileReader(new File(filename)));

			while ((line = bufferedReader.readLine()) != null) {
				Record record = newRecord(line);
				
				if (record != null)
					records.add(record);
			}
	    }
	    finally {
	    	bufferedReader.close();
	    }
		return records;
	}
}
/* **************************************************************************/
