/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/DatabaseGenerator/src/only/DatabaseGenerator.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.File;
import java.security.NoSuchAlgorithmException;
import java.security.spec.InvalidKeySpecException;

import secure.provenance.PasswordHash;
import secure.provenance.config.Config;
import secure.provenance.config.Configure;

import com.almworks.sqlite4java.SQLiteConnection;
import com.almworks.sqlite4java.SQLiteException;
import com.almworks.sqlite4java.SQLiteStatement;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class DatabaseGenerator
{
	protected static final Config CONFIG = Configure.oldConfig();
	protected static final String DEVELOPER_FIRST_NAME = CONFIG.getDeveloperConfig().getFirstName();
	protected static final String DEVELOPER_LAST_NAME = CONFIG.getDeveloperConfig().getLastName();
	protected static final String DEVELOPER_EMAIL_ADDRESS = CONFIG.getDeveloperConfig().getEmailAddress();
	protected static final String DEVELOPER_PASSWORD = CONFIG.getDeveloperConfig().getPassword();
	
	// Notice in the code that File(path) is used so that the database will be
	// created relative to the current directory.  For Eclipse this will be
	// parallel to bin and thus outside of the jar file.
	protected static final String PATH = "../db/";
	protected static final String TRUSTED_DATABASE = PATH + "trusted.db";
	protected static final String PROVENANCE_DATABASE = PATH + "provenance.db";
	protected static final String SHARED_DATABASE = PATH + "shared.db";

	protected void generateTrustedUserTable(String database) throws SQLiteException
	{
		String command = "" +
			"CREATE TABLE [T_User]" +
			"(" +
				"[UserID] INTEGER PRIMARY KEY, " +
				"[FirstName] TEXT NOT NULL, " +
				"[LastName] TEXT NOT NULL, " +
				"[EmailAddress] TEXT NOT NULL UNIQUE, " +
				"[Password] TEXT NOT NULL, " +
				"[SslKey] BLOB NOT NULL, " +
				"[SslCertificate] BLOB NOT NULL, " +
				"[PrivateKey] BLOB NOT NULL, " +
				"[PublicKey] BLOB NOT NULL, " +
				"[IsActive] BOOL NOT NULL DEFAULT 1, " +
				"[UserTypeID] SMALLINT DEFAULT 1" +
			");";
		execute(database, command);
	}
	
	protected void populateTrustedUserTable(String database) throws SQLiteException, NoSuchAlgorithmException, InvalidKeySpecException
	{
		String hash = PasswordHash.createHash(DEVELOPER_PASSWORD);
		
		// The UserID is automatically generated and the last two have default values
		String command = "" +
			"INSERT INTO [T_User]" +
			"(" +
				"[FirstName], " +
				"[LastName], " +
				"[EmailAddress], " +
				"[Password], " +
				"[SslKey], " +
				"[SslCertificate], " +
				"[PrivateKey], " +
				"[PublicKey], " +
				"[UserTypeID]" +
			") " +
			"VALUES" +
			"(" +
				"'" + DEVELOPER_FIRST_NAME + "', " +
				"'" + DEVELOPER_LAST_NAME + "', " +
				"'" + DEVELOPER_EMAIL_ADDRESS + "', " +
				"'" + hash + "', " +
				"'', " +
				"'', " +
				"'', " +
				"'', " +
				"3" +
			");";
		execute(database, command);
	}
	
	protected void generateTrustedDocumentTable(String database) throws SQLiteException
	{
		String command = "" +
			"CREATE TABLE [T_Document]" +
			"(" +
				"[DocumentID] INTEGER PRIMARY KEY, " + 
				"[Title] TEXT NOT NULL, " +
				"[LatestVersionID] TEXT NOT NULL, " + // Must be in T_DocumentVersionTable
				"[VersionNo] INT64 NOT NULL DEFAULT 0, " + // and match this
				"[UpdatedBy] INTEGER NOT NULL, " +
				"[UpdatedOn] TEXT NOT NULL, " +
				"[CreatedBy] INTEGER NOT NULL, " +				
				"[CreatedOn] TEXT NOT NULL, " + 
				"[UniqueKey] TEXT NOT NULL, " +
				"FOREIGN KEY(UpdatedBy) REFERENCES T_User(UserID), " +
				"FOREIGN KEY(CreatedBy) REFERENCES T_User(UserID)" +
			");";
		execute(database, command);
	}

	protected void generateTrustedUserDocumentMappingTable(String database) throws SQLiteException
	{
		String command = "" +
			"CREATE TABLE [T_UserDocumentMapping]" +
			"(" +
				"[UserID] INTEGER NOT NULL, " + 
				"[DocumentID] INTEGER NOT NULL, " + 
				"[IsEditor] BOOL NOT NULL DEFAULT 0, " + 
				"[IsAuditor] BOOL NOT NULL DEFAULT 0, " +
				"[IsAdministrator] BOOL NOT NULL DEFAULT 0, " +
				"UNIQUE([UserID], [DocumentID]), " +
				"FOREIGN KEY(UserID) REFERENCES T_User(UserID), " +
				"FOREIGN KEY(DocumentID) REFERENCES T_Document(DocumentID)" +
			");";
		execute(database, command);
	}
	
	public void populateTrustedDatabase(String database) throws SQLiteException, NoSuchAlgorithmException, InvalidKeySpecException
	{
		populateTrustedUserTable(database);
	}
	
	public void generateTrustedDatabase(String database) throws SQLiteException
	{	
		deleteDatabase(database);
		generateTrustedUserTable(database);
		generateTrustedDocumentVersionTable(database);
		generateTrustedDocumentTable(database);
		generateTrustedUserDocumentMappingTable(database);
		generateTrustedAccessTable(database);
	}
	
	protected void generateTrustedDocumentVersionTable(String database) throws SQLiteException
	{
		String command = "" +
			"CREATE TABLE [T_DocumentVersion]" +
			"(" +
				"[VersionID] TEXT PRIMARY KEY, " +
				"[PrevVersionID] TEXT, " +
				"[DocumentID] INTEGER NOT NULL, " +
				"[UpdatedBy] INTEGER NOT NULL, " +
				"[UpdatedOn] TEXT NOT NULL, " +
				"[VersionNo] INT64 NOT NULL, " +
				"FOREIGN KEY(PrevVersionID) REFERENCES T_DocumentVersion(VersionID), " +
				"FOREIGN KEY(DocumentID) REFERENCES T_Document(DocumentID), " +
				"FOREIGN KEY(UpdatedBy) REFERENCES T_User(UserID)" +
			");";
		execute(database, command);		
	}

	protected void generateTrustedAccessTable(String database) throws SQLiteException
	{
		String command = "" +
			"CREATE TABLE [T_Access]" +
			"(" +
				"[UserID] INTEGER NOT NULL, " +
				"[DocumentID] INTEGER NOT NULL, " +
				"[VersionID] TEXT NOT NULL, " +
				"[VersionNo] INT64 NOT NULL, " +
				"[AccessedOn] TEXT NOT NULL, " +
				"[AccessTypeID] SMALLINT NOT NULL, " +
				"[Round] SMALLINT NOT NULL DEFAULT 0, " +
				"FOREIGN KEY(UserID) REFERENCES T_User(UserID), " +
				"FOREIGN KEY(DocumentID) REFERENCES T_Document(DocumentID), " +
				"FOREIGN KEY(VersionID) REFERENCES T_DocumentVersion(VersionID)" +
			");";
		execute(database, command);
	}
	
	protected void generateProvenanceDocumentBucket(String database) throws SQLiteException
	{
		String command = "" +
			"CREATE TABLE [P_DocumentBucket]" +
			"(" +
				"[VersionID] TEXT PRIMARY KEY, " +
				"[Document] TEXT NOT NULL" +
			");";
		execute(database, command);		
	}
	
	protected void generateProvenanceProvenanceBucket(String database) throws SQLiteException
	{
		String command = "" +
			"CREATE TABLE [P_ProvenanceBucket]" +
			"(" +
				"[VersionID] TEXT PRIMARY KEY, " +
				"[PrevVersionID] TEXT, " +
				"[Provenance] TEXT NOT NULL, " +
				"FOREIGN KEY(PrevVersionID) REFERENCES P_ProvenanceBucket(VersionID)" +
			");";			
		execute(database, command);

		command = "" +
				"PRAGMA recursive_triggers = TRUE;";
		execute(database, command);
		
		command = "" + 
				"CREATE TABLE [P_TriggerTable] (" +
					"[VersionID] TEXT, " + 
					"[PrevVersionID] TEXT, " +
					"[Provenance] TEXT NOT NULL, " + 
					"[Index] INTEGER);";
		execute(database, command);

		command = "" +
				"CREATE TRIGGER [Trigger] " +
					"AFTER INSERT ON [P_TriggerTable] " +
					"FOR EACH ROW " +
					"WHEN [NEW].[VersionID] IS NOT NULL " +
					"BEGIN " +
						"INSERT INTO [P_TriggerTable] ([VersionID], [PrevVersionID], [Provenance], [Index]) " +
						"SELECT [P_ProvenanceBucket].[VersionID], [P_ProvenanceBucket].[PrevVersionId], [P_ProvenanceBucket].[Provenance], [NEW].[Index] + 1 " +
						"FROM [P_ProvenanceBucket] " +
						"WHERE [P_ProvenanceBucket].[VersionID] == [NEW].[PrevVersionID]; " +
					"END;";
		execute(database, command);
	}

	public void generateProvenanceDatabase(String database) throws SQLiteException
	{
		deleteDatabase(database);
		generateProvenanceDocumentBucket(database);
		generateProvenanceProvenanceBucket(database);
	}

	public void generateSharedDatabase(String database) throws SQLiteException
	{
		deleteDatabase(database);
		generateTrustedUserTable(database);
		generateTrustedDocumentVersionTable(database);
		generateTrustedDocumentTable(database);
		generateTrustedUserDocumentMappingTable(database);
		generateTrustedAccessTable(database);
		
		generateProvenanceDocumentBucket(database);
		generateProvenanceProvenanceBucket(database);		
	}
	
	public void populateSharedDatabase(String database) throws SQLiteException, NoSuchAlgorithmException, InvalidKeySpecException
	{
		populateTrustedUserTable(database);
	}
	
	protected void deleteDatabase(String database)
	{
		File file = new File(database);
		if (file.exists())
			file.delete();
	}
	
	private SQLiteConnection newSQLiteConnection(String database) throws SQLiteException
	{
		SQLiteConnection db = new SQLiteConnection(new File(database));
		
		db.open(true);
		return db;
	}
	
	private void execute(SQLiteStatement st) throws SQLiteException
	{
		if (st.step())
			st.stepThrough();
	}
	
	private void dispose(SQLiteConnection db, SQLiteStatement st)
	{
		if (st != null)
			st.dispose();
		if (db != null)
			db.dispose();		
	}
	
	public void execute(String database, String command) throws SQLiteException
	{
		SQLiteConnection db = null;
		SQLiteStatement st = null;
		
		try
		{
			db = newSQLiteConnection(database);
			st = db.prepare(command);
			execute(st);
		}
		catch (Exception ex) {
			ex.printStackTrace();
		}
		finally
		{
			dispose(db, st);			
		}		
	}
	
	public static void main(String[] args)
	{
		try
		{
			DatabaseGenerator databaseGenerator = new DatabaseGenerator();
			
			databaseGenerator.generateTrustedDatabase(TRUSTED_DATABASE);
			databaseGenerator.populateTrustedDatabase(TRUSTED_DATABASE);
			
			databaseGenerator.generateProvenanceDatabase(PROVENANCE_DATABASE);
			
			databaseGenerator.generateSharedDatabase(SHARED_DATABASE);
			databaseGenerator.populateSharedDatabase(SHARED_DATABASE);
		}
		catch (Exception ex)
		{
			System.out.println(ex.getMessage());
			ex.printStackTrace();
		}
		finally
		{
			System.out.println("Completed.");
		}
	}
}
/* **************************************************************************/

