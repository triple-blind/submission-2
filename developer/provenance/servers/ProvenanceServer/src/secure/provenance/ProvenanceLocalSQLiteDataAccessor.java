/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/ProvenanceServer/src/secure/provenance/ProvenanceLocalSQLiteDataAccessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import org.json.JSONArray;
import org.json.JSONObject;

import com.almworks.sqlite4java.SQLiteConnection;
import com.almworks.sqlite4java.SQLiteStatement;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class ProvenanceLocalSQLiteDataAccessor extends BaseLocalSQLiteDataAccessor implements IProvenanceDataAccessor
{	
	protected static final String DB_FILE_SUBDIR = "../db/";
	protected static final String DB_JAR_SUBDIR = "./db/";
//	protected static final String DB_FILE = "shared.db";
	protected static final String DB_FILE = "provenance.db";
	
	private static Logger logger = Logger.getLogger(ProvenanceLocalSQLiteDataAccessor.class);
		
	public ProvenanceLocalSQLiteDataAccessor(String fileDatabase, String jarDatabase)
	{
		super(fileDatabase, jarDatabase, logger);
	}
	
	public ProvenanceLocalSQLiteDataAccessor()
	{
		this(DB_FILE_SUBDIR + DB_FILE, DB_JAR_SUBDIR + DB_FILE);
	}
			
	@Override
	public String getDocumentForEdit(final String versionId)
	{
		return processUnauthenticatedCommand(new UnauthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db) throws Exception
			{
				String result = resultString(false);
				SQLiteStatement st = null;
				
				try
				{
					String document = null;
					String provenance = null;

					// Only need to last provenance for this, not all
					// Write a better statement
					st = db.prepare("SELECT [Document], [Provenance] FROM " +
							"(SELECT [Document] FROM [P_DocumentBucket] WHERE [VersionID] = ? LIMIT 1)," +
							"(SELECT [Provenance] FROM [P_ProvenanceBucket] WHERE [VersionID] = ? LIMIT 1);")
							.bind(1, versionId)
							.bind(2, versionId);
					if (st.step())
					{
						document = st.columnString(0);
						provenance = st.columnString(1);
						st.stepThrough();
					}
					else
						return result;
						
//					JSONStringArray currentChain = new JSONStringArray(provenance);
					JSONObject payload = new JSONObject()
					// TODO: return the entirety and let haathi extract the last one
							//.put("provenance", currentChain.getLast())
							.put("provenance",  provenance)
							.put("document", document);
					result = resultString(true, payload);
				}
				finally
				{
					dispose(st);
				}
				return result;
			}
		}, false);
	}
		
	@Override
	public String putDocumentForEdit(final String versionId, final String prevVersionId, final String document, final String provenance)
	{
		return processUnauthenticatedCommand(new UnauthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db) throws Exception
			{
				String result = resultString(false);
				SQLiteStatement st = null;
				int col = 1;
				
				try
				{
					st = db.prepare("INSERT INTO [P_DocumentBucket] ([VersionID], [Document]) VALUES (?, ?);")
							.bind(col++, versionId)
							.bind(col++, document);
					execute(st);
					if (db.getChanges() == 0)
						return result;
					dispose(st);
					
					boolean isNew = true;
					// If this is a new file, the PrevVersionID is not present and should be recorded as NULL
					st = db.prepare("SELECT [VersionID] FROM [P_ProvenanceBucket] WHERE [VersionID] = ? LIMIT 1;")
							.bind(1, prevVersionId);
					if (st.step())
					{
						isNew = false;
						st.stepThrough();
					}
							
					col = 1;
					st = db.prepare("INSERT INTO [P_ProvenanceBucket] ([VersionID], [PrevVersionID], [Provenance]) VALUES (?, ?, ?);")
							.bind(col++, versionId)
							.bind(col++, isNew ? null : prevVersionId)
							.bind(col++, provenance);
					execute(st);
					if (db.getChanges() == 0)
						return result;
					result = resultString(true);
				}
				finally
				{
					dispose(st);
				}
				return result;
			}
		});
	}
	
	@Override
	public String getDocumentForAudit(final String versionId)
	{
		return processUnauthenticatedCommand(new UnauthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db) throws Exception
			{
				String result = resultString(false);
				SQLiteStatement st = null;
				
				try
				{
					String document = null;
					
					st = db.prepare("SELECT [Document] FROM [P_DocumentBucket] WHERE [VersionID] = ? LIMIT 1;")
							.bind(1, versionId);
					if (st.step())
					{
						document = st.columnString(0);
						st.stepThrough();
					}
					else
						return result;
					dispose(st);
					
					// This seems not to stick when the database is created.  Perhaps it is per connection.
					st = db.prepare("DELETE FROM [P_TriggerTable];");
					execute(st);
					dispose(st);
					
					st = db.prepare("PRAGMA recursive_triggers = TRUE;");
					execute(st);
					dispose(st);
										
					st = db.prepare("INSERT INTO [P_TriggerTable] ([VersionID], [PrevVersionId], [Provenance], [Index]) " +
							"SELECT [VersionID], [PrevVersionID], [Provenance], 0 " +
							"FROM [P_ProvenanceBucket] " +
							"WHERE [VersionID] = ?;")
							.bind(1, versionId);
					execute(st);
					dispose(st);
	
					JSONArray completeChain = new JSONArray();

					st = db.prepare("SELECT [Provenance] FROM [P_TriggerTable] ORDER BY [Index] DESC;");
//					st = db.prepare("SELECT [Provenance] FROM [P_ProvenanceBucket] WHERE [VersionID] = ?;")
//							.bind(1, versionId);
					while (st.step())
					{
						String provenance = st.columnString(0);
						completeChain.put(provenance);
					}
					
					JSONObject payload = new JSONObject()
							.put("provenances", completeChain)
							.put("document", document);					
					result = resultString(true, payload);
				}
				finally
				{
					dispose(st);
				}
				return result;
			}
		});
	}	
}
/* **************************************************************************/
