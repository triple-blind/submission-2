/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/TrustedLocalSQLiteDataAccessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.ByteArrayInputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import org.json.JSONArray;
import org.json.JSONObject;

import com.almworks.sqlite4java.SQLiteConnection;
import com.almworks.sqlite4java.SQLiteException;
import com.almworks.sqlite4java.SQLiteStatement;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class TrustedLocalSQLiteDataAccessor extends BaseLocalSQLiteDataAccessor implements ITrustedDataAccessor
{
	public class AuthenticatedCommandProcessor
	{
		public String process(SQLiteConnection db, Integer userId, UserTypeId userTypeId, String authenticationResult)  throws Exception
		{
			return resultString(false);
		}
		
		public String process(SQLiteConnection db, String authenticationResult)  throws Exception
		{
			JSONObject object = new JSONObject(authenticationResult);
			JSONObject payload = object.getJSONObject("payload");
			Integer userId = payload.getInt("userId");
			UserTypeId userTypeId = UserTypeId.fromInt(payload.getInt("userTypeId"));
			
			return process(db, userId, userTypeId, authenticationResult);
		}		
	}

	protected static final int KEYSIZE = 128; // 192 and 256 bits may not be available

	protected static final String DB_FILE_SUBDIR = "../db/";
	protected static final String DB_JAR_SUBDIR = "./db/";
//	protected static final String DB_FILE = "shared.db";
	protected static final String DB_FILE = "trusted.db";
	
	protected static final String CA_SUBDIR = "keystore/";
	protected static final String CA_FILE = CA_SUBDIR + "generator.jks"; // "ca_new.p12"; // previously ca.p12
	protected static final String CA_PASSWORD = "password"; // "ca_haathi"; // previously nitin
	protected static final String CA_ALIAS = "generator"; // ca_haathi
	
	private static DateFormat DATE_FORMAT = new DateFormat();
	
	private static Logger logger = Logger.getLogger(TrustedLocalSQLiteDataAccessor.class);
	
	private boolean isValidUser = false;

	public TrustedLocalSQLiteDataAccessor(String fileDatabase, String jarDatabase)
	{
		super(fileDatabase, jarDatabase, logger);
	}
	
	public TrustedLocalSQLiteDataAccessor()
	{
		this(DB_FILE_SUBDIR + DB_FILE, DB_JAR_SUBDIR + DB_FILE);
	}

	protected String processTransaction(SQLiteConnection db, AuthenticatedCommandProcessor processor, String authenticationResult)
	{
		String result = null;
		SQLiteStatement st = null;
		
		try
		{
			try
			{
				st = db.prepare("BEGIN TRANSACTION;");
				execute(st);
				dispose(st);
			}
			catch (Exception ex1)
			{
				result = catchException(ex1, logger, true);				
				return result;
			}
			
			try
			{
				result = processor.process(db, authenticationResult);
			}
			catch (Exception ex2)
			{
				result = catchException(ex2, logger, true);
				try
				{
					st = db.prepare("ROLLBACK;");
					execute(st);
				}
				catch (Exception ex2a)
				{
				}
				return result;
			}	

			try
			{
				st = db.prepare("COMMIT;");
				execute(st);
			}
			catch (Exception ex3)
			{
				result = catchException(ex3, logger, true);				
				return result;
			}
		}
		finally
		{
			dispose(st);
		}		
		return result;
	}
		
	protected String processAuthenticatedCommand(final AuthenticationCredentials credentials, final AuthenticatedCommandProcessor processor,
			final boolean transact)
	{
		return processUnauthenticatedCommand(new UnauthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db) throws Exception
			{
				String result = authenticateUser(db, credentials);

				if (isValidUser)
					if (transact)
						result = processTransaction(db, processor, result);
					else
						result = processor.process(db, result);				
				return result;			
			}
		}, false);
	}

	protected String processAuthenticatedCommand(AuthenticationCredentials credentials, AuthenticatedCommandProcessor processor)
	{
		return processAuthenticatedCommand(credentials, processor, true);
	}
	
	@Override
	public String authenticate(final AuthenticationCredentials credentials, final String provenanceAddress, final Integer provenancePort)
	{
		return processUnauthenticatedCommand(new UnauthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db)
			{
				return authenticateUser(db, credentials, provenanceAddress, provenancePort);
			}
		});
	}
	
	@Override
	public String refresh(AuthenticationCredentials credentials)
	{
		return resultString(false, "The requested functionality has been disabled");
/*	
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult) throws Exception
			{
				JSONObject credentialsObject = new JSONObject(authenticationResult);
				JSONObject resultObject = new JSONObject()
					.put("sessionKey", credentialsObject.getString("sessionKey"))
					.put("sessionKeySize", credentialsObject.getInt("sessionKeySize"));
				String result = resultObject.toString();
				
				return result;
			}
		}, false);
*/	}	
	
	
	@Override
	public String createUser(AuthenticationCredentials credentials, final String emailAddress, final String firstName,
			final String lastName, final UserTypeId userTypeId, final String password)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult) throws Exception
			{
				UserTypeId executingUserType = UserTypeId.fromInt(getExecutingUserTypeId(authenticationResult));

				if (executingUserType != UserTypeId.ADMINISTRATOR && executingUserType != UserTypeId.PROGRAMMER)
					return resultString(false, "Lack of access privileges");
				
				String result = null;		
				SQLiteStatement st = null;
				int col = 1;
								
				try
				{
					if (!isUserPresent(db, emailAddress))
					{						
						String hash = PasswordHash.createHash(password);
						X509CertificateGenerator certificateGenerator = new X509CertificateGenerator(CA_FILE, CA_PASSWORD, CA_ALIAS);
						if (certificateGenerator.createCertificate(emailAddress,365))
						{
							st = db.prepare("INSERT INTO [T_User] ([FirstName], [LastName], [EmailAddress], [Password], " +
									"[SslKey], [SslCertificate], [PrivateKey], [PublicKey], [UserTypeID]) values (?,?,?,?,?,?,?,?,?);")
									.bind(col++, firstName)
									.bind(col++, lastName)
									.bind(col++, emailAddress)
									.bind(col++, hash)
									.bind(col++, Utils.getBytesFromInputStream(new ByteArrayInputStream(certificateGenerator.getPEMPrivateKey())))
									.bind(col++, Utils.getBytesFromInputStream(new ByteArrayInputStream(certificateGenerator.getPEMCertificate())))
									.bind(col++, Utils.getBytesFromInputStream(new ByteArrayInputStream(certificateGenerator.getPrivateKey())))
									.bind(col++, Utils.getBytesFromInputStream(new ByteArrayInputStream(certificateGenerator.getPublicKey())))
									.bind(col++, userTypeId.getStatus());
							st.step();
							result = resultString(true);
						}
						else
							result = resultString(false, "Error while creating a new user: Error during certificate generation", logger);
					}
					else
						result = resultString(false, "User already exists", logger);
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
	public String listUsers(AuthenticationCredentials credentials, final UserTypeId userTypeId)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult) throws Exception
			{
				String result = null;
				SQLiteStatement st = null;
		
				try
				{
					JSONArray resultSet = new JSONArray();

					// We now hide administrators in this list
					st = db.prepare("SELECT [UserID], [FirstName], [LastName], [EmailAddress] FROM [T_User] WHERE [UserTypeID] <= ? AND [UserTypeID] != " + UserTypeId.ADMINISTRATOR.getStatus() + " AND [IsActive] = 1;" )
							.bind(1, userTypeId.getStatus());
					while (st.step())
					{
						JSONObject userObject = new JSONObject()
								.put("userId", st.columnInt(0))
								.put("firstName", st.columnString(1))
								.put("lastName", st.columnString(2))
								.put("emailAddress", st.columnString(3));
						resultSet.put(userObject);
					}
					
					JSONObject payload = new JSONObject()
						.put("users", resultSet);
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
	public String getPublicKeys(AuthenticationCredentials credentials, final List<Integer> usersToRetrieve, boolean onlyAuditors)
	{
		return getPublicKeys(credentials, usersToRetrieve, onlyAuditors, "publicKeys");
	}
	
	public String getPublicKeys(AuthenticationCredentials credentials, final List<Integer> usersToRetrieve, boolean onlyAuditors, final String name)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult) throws Exception
			{
				String result = null;
				SQLiteStatement st = null;
		
				try
				{
					JSONArray resultSet = new JSONArray();
					byte[] publicKey = null;

					for (Integer userId : usersToRetrieve)
					{
						st = db.prepare("SELECT [UserID], [PublicKey] FROM [T_User] WHERE [UserID] = ? AND [IsActive] = 1;")
								.bind(1, userId);
						while (st.step())
						{
							publicKey = st.columnBlob(1);
							JSONObject userObject = new JSONObject()
									.put("userId", st.columnInt(0))
									.put("publicKey", asBase64(publicKey))
									.put("publicKeySize", publicKey == null ? 0 : publicKey.length);
							resultSet.put(userObject);
						}
					}
					
					JSONObject payload = new JSONObject()
						.put(name, resultSet);
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
	
	@Override
	public String getPublicKeyByEmail(AuthenticationCredentials credentials, final String emailId)
	{
		return resultString(false, "The requested functionality has been disabled");
/*		
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult) throws Exception
			{
				String result = null;
				SQLiteStatement st = null;
		
				try
				{
					JSONObject userObject = new JSONObject();
					byte[] publicKey = null;

					st = db.prepare("SELECT [UserID], [PublicKey] FROM [T_User] WHERE [EmailAddress] = ? AND [IsActive] = 1;" )
							.bind(1, emailId);
					while (st.step())
					{
						publicKey = st.columnBlob(1);
						userObject
								.put("userId", st.columnInt(0))
								.put("publicKey", asBase64(publicKey))
								.put("publicKeySize", publicKey.length);
					}
					result = resultString(true, userObject);
				}
				finally
				{
					dispose(st);
				}
				return result;
			}
		});
*/	}

	@Override
	public String createDocument(AuthenticationCredentials credentials, final String title)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, Integer userId, UserTypeId userTypeId, String authenticationResult)  throws Exception
			{
				String result = null;
				SQLiteStatement st = null;

				try
				{	
					Document document = new Document();
					String timestamp = today();
					int documentId = -1;
					int col = 1;
					String versionId = UUID.randomUUID().toString();
					long versionNo = 0;
					
					document.setTitle(title);
					document.setLatestVersionId(versionId);				
					document.setUpdatedOn(timestamp);
					document.setUpdatedBy(userId);
					document.setCreatedOn(timestamp);
					document.setCreatedBy(userId);
					document.setUniqueKey(asBase64(KeyGenerator.generateKey()));
					document.setVersionNo(versionNo);						

					st = db.prepare("INSERT INTO [T_Document]([Title], [UpdatedOn], [UpdatedBy], [CreatedOn], [CreatedBy], " +
							"[UniqueKey], [VersionNo], [LatestVersionID]) VALUES (?, ?, ?, ?, ?, ?, ?, ?);")
							.bind(col++, document.getTitle())
							.bind(col++, document.getUpdatedOn())
							.bind(col++, document.getUpdatedBy())
							.bind(col++, document.getCreatedOn())
							.bind(col++, document.getCreatedBy())
							.bind(col++, document.getUniqueKey())
							.bind(col++, document.getVersionNo())
							.bind(col++, document.getLatestVersionId());
					execute(st);				
					documentId = (int) db.getLastInsertId();
					document.setDocumentId(documentId);
					
					col = 1;
					st = db.prepare("INSERT INTO [T_UserDocumentMapping] ([UserID], [DocumentID], [IsEditor], [IsAuditor], [IsAdministrator]) " +
							"VALUES (?, ?, 1, 1, 0);") // All users are now automatically auditors
							.bind(col++, userId)
							.bind(col++, documentId);
					execute(st);
					if (db.getChanges() == 0)
						return result;
					dispose(st);

					String prevVersionId = null;
					col = 1;
					st = db.prepare("INSERT INTO [T_DocumentVersion] ([VersionID], [PrevVersionID], [DocumentID], [UpdatedBy], [UpdatedOn], [VersionNo]) " +
							"VALUES (?, ?, ?, ?, ?, ?);")
							.bind(col++, document.getLatestVersionId())
							.bind(col++, prevVersionId)
							.bind(col++, documentId)
							.bind(col++, document.getUpdatedBy())
							.bind(col++, document.getUpdatedOn())
							.bind(col++, document.getVersionNo());
					execute(st);
					if (db.getChanges() == 0)
						return result;
					dispose(st);

					col = 1;
					st = db.prepare("INSERT INTO [T_Access] ([UserID], [DocumentID], [VersionID], [VersionNo], [AccessedOn], [AccessTypeID]) " +
							"VALUES (?, ?, ?, ?, ?, ?);")
							.bind(col++, userId)
							.bind(col++, documentId)
							.bind(col++, versionId)
							.bind(col++, versionNo)
							.bind(col++, timestamp)
							.bind(col++, AccessTypeId.NEW.getAccessType());
					execute(st);
					if (db.getChanges() == 0)
						return result;
					dispose(st);
					
					result = resultString(document);
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
	public String updateDocument(AuthenticationCredentials credentials, final Integer round, final Integer documentId,
			final String versionId, final String prevVersionId)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, Integer userId, UserTypeId userTypeId, String authenticationResult) throws Exception
			{
				if (round != 0 && round != 1)
					return resultString(false, "Invalid round");					

				String result = resultString(false);
				String timestamp = today();
				SQLiteStatement st = null;
				int col = 1;
				int versionNo;
					
				try
				{			
					st = db.prepare("SELECT [VersionNo] + 1 FROM [T_Document] JOIN [T_UserDocumentMapping] ON [T_Document].[DocumentID] = [T_UserDocumentMapping].[DocumentID] " +
							"WHERE [T_Document].[DocumentID] = ? AND [T_UserDocumentMapping].[UserID] = ? AND [T_UserDocumentMapping].[IsEditor] = 1 LIMIT 1;")
						.bind(col++, documentId)
						.bind(col++, userId);
					if (st.step())
					{
						versionNo = st.columnInt(0);						
						st.stepThrough();
					}
					else
						return resultString(false, "Invalid user/document combination");
					dispose(st);
					col = 1;
					
					// In this round a suggested versionId is returned as well as the
					// versionNo that would result if the document is saved before anyone
					// else does it.  All sorts of errors could be flagged.
					if (round == 0)
					{
						// For example, check to see if versionId matches lastestVersionId.
						// For now return the likely versionNo.  The client can complain if
						// it isn't exactly one more than what the client has.
				 		String latestVersionId = UUID.randomUUID().toString();			 		
				 		
						st = db.prepare("INSERT INTO [T_Access] ([UserID], [DocumentID], [VersionID], [VersionNo], [AccessedOn], [AccessTypeID], [Round]) " +
								"VALUES (?, ?, ?, ?, ?, ?, ?);")
								.bind(col++, userId)
								.bind(col++, documentId)
								.bind(col++, latestVersionId)
								.bind(col++, versionNo)
								.bind(col++, timestamp)
								.bind(col++, AccessTypeId.SAVE.getAccessType())
								.bind(col++, 0);
						execute(st);
						if (db.getChanges() == 0)
							return result;

						JSONObject payload = new JSONObject()
							.put("versionId", latestVersionId)
							.put("versionNo", versionNo);
						result = resultString(true, payload);
						return result;							
					}
					
					if (round == 1)
					{		
				 		String latestVersionId = versionId;
		
						st = db.prepare("UPDATE [T_Document] SET [UpdatedBy] = ?, [UpdatedOn] = ?, [LatestVersionID] = ?, [VersionNo] = [VersionNo] + 1  WHERE [DocumentID] = ?;")
								.bind(col++, userId)
								.bind(col++, timestamp)
								.bind(col++, latestVersionId)
								.bind(col++, documentId);
						execute(st);
						if (db.getChanges() == 0)
							return resultString(false, "No document was updated.  Is the documentId correct?", logger);
						dispose(st);
								
						col = 1;
						st = db.prepare("INSERT INTO [T_DocumentVersion] ([VersionID], [PrevVersionID], [DocumentID], [UpdatedBy], [UpdatedOn], [VersionNo]) VALUES (?, ?, ?, ?, ?, ?);")
								.bind(col++, latestVersionId)
								.bind(col++, prevVersionId)
								.bind(col++, documentId)
								.bind(col++, userId)
								.bind(col++, timestamp)
								.bind(col++, versionNo);
						execute(st);
						if (db.getChanges() == 0)
							return result;
						dispose(st);
						
						col = 1;
						st = db.prepare("INSERT INTO [T_Access] ([UserID], [DocumentID], [VersionID], [VersionNo], [AccessedOn], [AccessTypeID], [Round]) " +
								"VALUES (?, ?, ?, ?, ?, ?, ?);")
								.bind(col++, userId)
								.bind(col++, documentId)
								.bind(col++, latestVersionId)
								.bind(col++, versionNo)
								.bind(col++, timestamp)
								.bind(col++, AccessTypeId.SAVE.getAccessType())
								.bind(col++, 1);
						execute(st);
						if (db.getChanges() == 0)
							return result;
						dispose(st);
						
						JSONObject payload = new JSONObject()
								.put("versionId", latestVersionId)
								.put("versionNo", versionNo);
						result = resultString(true, payload);
						return result;
					}
					return resultString(false, "Invalid round");					
				}
				finally
				{
					dispose(st);
				}
			}
		});
	}
	

	@Override
	public String accessDocument(AuthenticationCredentials credentials, final Integer round, final Integer documentId, final String versionId, final AccessTypeId accessTypeId)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, Integer userId, UserTypeId userTypeId, String authenticationResult) throws Exception
			{
				if (round != 0 && round != 1)
					return resultString(false, "Invalid round");					

				String result = resultString(false);
				String timestamp = today();
				SQLiteStatement st = null;
				int col = 1;
				int versionNo;
					
				try
				{			
					st = db.prepare("SELECT [VersionNo] FROM [T_DocumentVersion] " +
							"WHERE [DocumentID] = ? AND [VersionID] = ? LIMIT 1;")
						.bind(col++, documentId)
						.bind(col++, versionId);
					if (st.step())
					{
						versionNo = st.columnInt(0);						
						st.stepThrough();
					}
					else
						versionNo = -1;
					dispose(st);
					col = 1;
					
					st = db.prepare("INSERT INTO [T_Access] ([UserID], [DocumentID], [VersionID], [VersionNo], [AccessedOn], [AccessTypeID], [Round]) " +
							"VALUES (?, ?, ?, ?, ?, ?, ?);")
						.bind(col++, userId)
						.bind(col++, documentId)
						.bind(col++, versionId)
						.bind(col++, versionNo)
						.bind(col++, timestamp)
						.bind(col++, accessTypeId.getAccessType())
						.bind(col++, round);
					execute(st);
					if (db.getChanges() == 0)
						return result;
					return resultString(true);
				}
				finally
				{
					dispose(st);
				}
			}
		});
	}
	
	@Override
	public String listDocuments(AuthenticationCredentials credentials, final UserTypeId userTypeId)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, Integer userId, UserTypeId _userTypeId, String authenticationResult) throws Exception
			{
				String result = null;
				SQLiteStatement st = null;
				
				try
				{
					JSONArray resultSet = new JSONArray();
							    		  
					st = db.prepare("SELECT [T_User].[FirstName], [T_User].[LastName], [T_User].[EmailAddress], [T_Document].[DocumentID], [Title], [UniqueKey], [CreatedOn], [UpdatedOn], [VersionNo], [LatestVersionID] " +
							"FROM [T_UserDocumentMapping], [T_Document], [T_User] " +
							"WHERE [T_UserDocumentMapping].[UserID] = ? AND [T_UserDocumentMapping].[" + columnNameForUserType(userTypeId) + "] = 1 AND " +
									"[T_UserDocumentMapping].[DocumentID] = [T_Document].[DocumentID] AND " +
									"[T_User].[UserID] = [T_Document].[CreatedBy] AND [T_Document].[VersionNo] != 0;") // To show creator, not version updater
							.bind(1, userId);				
					while (st.step())
					{
						Document document = new Document();
						document.setDocumentId(st.columnInt(3));
						document.setTitle(st.columnString(4));
						document.setUniqueKey(st.columnString(5));
						document.setCreatedOn(st.columnString(6));
						document.setUpdatedOn(st.columnString(7));
						document.setVersionNo(st.columnLong(8));
						document.setLatestVersionId(st.columnString(9));
												
						JSONObject userObject = document.asJSONObject(KEYSIZE)
							.put("firstName", st.columnString(0))
							.put("lastName", st.columnString(1))
							.put("emailAddress", st.columnString(2));
						resultSet.put(userObject);						
					}
				
					JSONObject payload = new JSONObject()
						.put("documents", resultSet);
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
	public String addEditors(AuthenticationCredentials credentials, final Integer documentId, final List<Integer> usersToShare)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, Integer userId, UserTypeId userTypeId, String authenticationResult) throws SQLiteException
			{
				SQLiteStatement st = null;
				boolean isUser = false;

				try
				{
					int col = 1;
					
					st = db.prepare("SELECT * " +
							"FROM [T_UserDocumentMapping] " +
							"WHERE [UserID] = ? AND [DocumentID] = ? LIMIT 1;")
							.bind(col++, userId)
							.bind(col++, documentId);
					if (st.step()) {
						isUser = true;
						st.stepThrough();
					}
					if (!isUser)
						return resultString(false, "Only someone with access can pass it on");
				}
				finally
				{
					dispose(st);
				}		
							
				boolean successful = true;
				
				for (Integer user : usersToShare)
					if (!addEditor(db, documentId, user))
					{
						successful = false;
						break;
					}
				return resultString(successful);
			}
		});
	}
	
	@Override
	public String subEditors(AuthenticationCredentials credentials, final Integer documentId, final List<Integer> usersToRevoke)
	{
		return resultString(false, "The requested functionality has been disabled");
		
/*		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult)
			{
				boolean successful = true;
		
				for (Integer user : usersToRevoke)
					if (!subAuditor(db, documentId, user))
					{
						successful = false;
						break;
					}
				return resultString(successful);
			}
		});
*/	}
	

	@Override
	public String addAuditors(AuthenticationCredentials credentials, final Integer documentId, final List<Integer> auditors)
	{
		return resultString(false, "The requested functionality has been disabled");
		
/*		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult)
			{
				boolean successful = true;
				
				for (Integer auditor : auditors)
					if (!addAuditor(db, documentId, auditor))
					{
						successful = false;
						break;
					}
				return resultString(successful);
			}
		});
*/	}

	@Override
	public String subAuditors(AuthenticationCredentials credentials, final Integer documentId, final List<Integer> auditors)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult)
			{
				boolean successful = true;
		
				for (Integer auditor : auditors)
					if (!subAuditor(db, documentId, auditor))
					{
						successful = false;
						break;
					}
				return resultString(successful);
			}
		});
	}


	@Override
	public String getDocumentEditors(AuthenticationCredentials credentials, final Integer documentId)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult) throws Exception
			{
				String result = null;
				SQLiteStatement st = null;

				try
				{
					JSONArray resultSet = new JSONArray();

					st = db.prepare("SELECT [T_UserDocumentMapping].[UserID], [FirstName], [LastName], [EmailAddress] " +
							"FROM [T_UserDocumentMapping] JOIN [T_User] ON [T_UserDocumentMapping].[UserID] = [T_User].[UserID] " +
							"WHERE [DocumentID] = ? AND [IsEditor] = 1;")
							.bind(1, documentId);
					while (st.step()) {
						JSONObject userObject = new JSONObject()
							.put("userId", st.columnInt(0))
							.put("firstName", st.columnString(1))
							.put("lastName", st.columnString(2))
							.put("emailAddress", st.columnString(3));
						resultSet.put(userObject);
					}
					JSONObject payload = new JSONObject()
						.put("editors", resultSet);
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
	public String getDocumentAuditors(final AuthenticationCredentials credentials, final Integer documentId)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult) throws Exception
			{
				String result = null;
				SQLiteStatement st = null;
		
				try
				{
					List<Integer> auditors = new ArrayList<Integer>();

					st = db.prepare("SELECT [UserID] FROM [T_UserDocumentMapping] WHERE [DocumentID] = ? AND [IsAuditor] = 1;")
							.bind(1, documentId);
					while (st.step())
						auditors.add(st.columnInt(0));
					result = getPublicKeys(credentials, auditors, true, "auditors");
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
	public String getDocumentHistory(AuthenticationCredentials credentials, final Integer documentId)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, String authenticationResult) throws Exception
			{
				String result = resultString(false);
				SQLiteStatement st = null;
				// TODO: take verified into account
				
				try
				{
					JSONArray resultSet = new JSONArray();
					
					st = db.prepare("SELECT [VersionNo], [UpdatedBy], [UpdatedOn] " +
							"FROM [T_DocumentVersion] " +
							"WHERE [DocumentID] = ? AND [VersionNo] > 0 " +
							"ORDER BY [VersionNo];")
							.bind(1, documentId);
					while (st.step())
					{
						JSONObject history = new JSONObject()
							.put("versionNo", st.columnInt(0))
							.put("createdBy", st.columnInt(1))
							.put("createdOn", DATE_FORMAT.toLongString(st.columnString(2), 0L));
						resultSet.put(history);
					}
					
					JSONObject payload = new JSONObject()
						.put("history", resultSet);
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
	public String getAllUsers(AuthenticationCredentials credentials)
	{
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, Integer userId, UserTypeId userTypeId, String authenticationResult) throws Exception
			{
				String result = null;
				SQLiteStatement st = null;
		
				if (userTypeId != UserTypeId.ADMINISTRATOR)
					return resultString(false, "Only an administrator can get all the documents", logger);
				
				try
				{
					JSONArray resultSet = new JSONArray();
					byte[] sslKey;
					byte[] sslCertificate;
					byte[] privateKey;
					byte[] publicKey;

					st = db.prepare("SELECT [UserID], [SslKey], [SslCertificate], [PrivateKey], [PublicKey], [UserTypeID] FROM [T_User];");
					while (st.step())
					{
						sslKey = st.columnBlob(1);
						sslCertificate = st.columnBlob(2);
						privateKey = st.columnBlob(3);
						publicKey = st.columnBlob(4);
					
						JSONObject payload = new JSONObject()
								.put("userId", st.columnInt(0))
								.put("sslKey", asBase64(sslKey))
								.put("sslKeySize", sslKey != null ? sslKey.length : 0)
								.put("sslCertificate", asBase64(sslCertificate))
								.put("sslCertificateSize", sslCertificate != null ? sslCertificate.length : 0)
								.put("privateKey", asBase64(privateKey))
								.put("privateKeySize", privateKey != null ? privateKey.length : 0)
								.put("publicKey", asBase64(publicKey))
								.put("publicKeySize", publicKey != null ? publicKey.length : 0)
								.put("userTypeId", st.columnInt(5));	
						resultSet.put(payload);
					}
					result = resultString(resultSet);
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
	public String getAllDocuments(AuthenticationCredentials credentials)
	{		
		return processAuthenticatedCommand(credentials, new AuthenticatedCommandProcessor()
		{
			public String process(SQLiteConnection db, Integer userId, UserTypeId userTypeId, String authenticationResult) throws Exception
			{
				String result = null;
				SQLiteStatement st = null;

				if (userTypeId != UserTypeId.ADMINISTRATOR)
					return resultString(false, "Only an administrator can get all the documents", logger);
				
				try
				{
					JSONArray resultSet = new JSONArray();

					// kwa: Shouldn't this capability be limited to an administrator of some kind
					// Why do we have to avoid documents with id 0?  Are these placeholders?
					// There's a better way to avoid them
					st = db.prepare("SELECT [DocumentID], [Title], [UpdatedBy], [UpdatedOn], [LatestVersionID], " +
							"[CreatedOn], [UniqueKey], [VersionNo] FROM [T_Document] WHERE [VersionNo] != 0;");
					while (st.step())
					{	
						Document document = new Document();
						document.setDocumentId((int) st.columnInt(0));
						document.setTitle(st.columnString(1));
						document.setUpdatedBy(st.columnInt(2));
						document.setUpdatedOn(st.columnString(3));
						document.setLatestVersionId(st.columnString(4));
						document.setCreatedOn(st.columnString(5));
						document.setUniqueKey(st.columnString(6));
						document.setVersionNo(st.columnLong(7));
						resultSet.put(document.asJSONObject(KEYSIZE));
					}
				
					JSONObject payload = new JSONObject()
						.put("documents", resultSet);
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
	
	protected int getExecutingUserTypeId(String authenticationResult)
	{
		return new JSONObject(authenticationResult)
				.getJSONObject("payload")
				.getInt("userTypeId");
	}
	
	// This version is used for internal purposes and the result is never reported to the outside world.
	protected String authenticateUser(SQLiteConnection db, AuthenticationCredentials credentials) {
		return authenticateUser(db, credentials, "", 0);
	}
	
	protected String authenticateUser(SQLiteConnection db, AuthenticationCredentials credentials, String provenanceAddress, int provenancePort)
	{
		String result = null;
		SQLiteStatement st = null;

		try
		{		
			Integer userId = -1;
			byte[] sslKey = null;
			byte[] sslCertificate = null;
			byte[] privateKey = null;
			byte[] publicKey = null;
			int userTypeId = -1;
			String firstName = "";
			String lastName = "";
			
			isValidUser = false;
			st = db.prepare("SELECT [UserID], [SslKey], [SslCertificate], [PrivateKey], [PublicKey], [UserTypeID], [FirstName], [LastName], [Password] FROM [T_User] WHERE [EmailAddress] = ? LIMIT 1;")
					.bind(1, credentials.getEmailAddress());
			if (st.step())
			{
				String password = credentials.getPassword();
				String hash = st.columnString(8);
				
		        isValidUser = PasswordHash.validatePassword(password, hash);			
				userId = st.columnInt(0);
				sslKey = st.columnBlob(1);
				sslCertificate = st.columnBlob(2);
				privateKey = st.columnBlob(3);
				publicKey = st.columnBlob(4);
				userTypeId = st.columnInt(5);
				firstName = st.columnString(6);
				lastName = st.columnString(7);
			}

			if (isValidUser)
			{
				JSONObject payload = new JSONObject()
						.put("userId", userId)
						.put("firstName", firstName)
						.put("lastName", lastName)
						.put("sslKey", asBase64(sslKey))
						.put("sslKeySize", sslKey == null ? 0 : sslKey.length)
						.put("sslCertificate", asBase64(sslCertificate))
						.put("sslCertificateSize", sslCertificate == null ? 0 : sslCertificate.length)
						.put("sessionKey", asBase64(KeyGenerator.generateKey()))
						.put("sessionKeySize", KeyGenerator.KEYSIZE / 8)
						.put("privateKey", asBase64(privateKey))
						.put("privateKeySize", privateKey == null ? 0 : privateKey.length)
						.put("publicKey", asBase64(publicKey))
						.put("publicKeySize", publicKey == null ? 0 : publicKey.length)
						.put("userTypeId", userTypeId)		
						.put("host", provenanceAddress)
						.put("port", provenancePort);
				
				result = resultString(true, payload);
			}
			else
				result = resultString(false, "Error during authentication", logger);
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		finally
		{
			dispose(st);
		}
		return result;
	}		
	
	protected String deleteUserDocumentMapping(SQLiteConnection db, long userId, long documentId)
	{
		String result = resultString(false);
		SQLiteStatement st = null;
		int col = 1;

		try
		{			
			st = db.prepare("DELETE FROM [T_UserDocumentMapping] WHERE [DocumentID] = ? AND [UserID] = ?;")
					.bind(col++, documentId)
					.bind(col++, userId);
			execute(st);
			result = resultString(db.getChanges() > 0);
		}
		catch (Exception ex)
		{
			catchException(ex, logger, false);
		}
		finally
		{
			dispose(st);
		}		
		return result;
	}

	protected String columnNameForUserType(UserTypeId userTypeId)
	{
		switch (userTypeId)
		{
			case EDITOR:
				return "IsEditor";
			case AUDITOR:
				return "IsAuditor";
			case ADMINISTRATOR:
				return "IsAdministrator";
			default:
				return "";
		}
	}
	
	protected String resultString(Document document)
	{
		return resultString(true, document.asJSONObject(KEYSIZE));
	}
		
	private boolean isUserPresent(SQLiteConnection db, String emailAddress)
	{
		boolean result = false;
		SQLiteStatement st = null;

		try
		{
			st = db.prepare("SELECT ROWID FROM [T_User] WHERE [EmailAddress] = ? LIMIT 1;")
					.bind(1, emailAddress);
			if (st.step())
				result = true;
			execute(st);
		}
		catch (Exception exception)
		{
			catchException(exception, logger, true);
			result = false;
		}
		finally
		{
			dispose(st);
		}
		return result;
	}
	
	protected boolean isUserPresent(SQLiteConnection db, Integer userId, int documentId)
	{
		boolean result = false;
		SQLiteStatement st = null;
		int col = 1;

		try
		{
			st = db.prepare("SELECT * FROM [T_UserDocumentMapping] WHERE [UserID] = ? AND [DocumentID] = ? LIMIT 1;")
					.bind(col++, userId)
					.bind(col++, documentId);
			if (st.step())
				result = true;
			execute(st);
		}
		catch (Exception ex)
		{
			 catchException(ex, logger, true);
		}
		finally
		{
			dispose(st);
		}		
		return result;
	}
	
	protected boolean addEditor(SQLiteConnection db, Integer documentId, Integer userId)
	{
		boolean result = false;
		SQLiteStatement st = null;
		int col = 1;

		try
		{
			// This should result in a row change even if the user was already an auditor
			st = db.prepare("UPDATE [T_UserDocumentMapping] SET [IsEditor] = 1, [IsAuditor] = 1 WHERE [DocumentID] = ? AND [UserID] = ?;")
					.bind(col++, documentId)
					.bind(col++, userId);
			execute(st);
			st.dispose();
			if (db.getChanges() > 0)
				result = true;
			else
			{
				col = 1;
				st = db.prepare("INSERT INTO [T_UserDocumentMapping] ([UserID], [DocumentID], [IsEditor], [IsAuditor], [IsAdministrator]) " +
						"VALUES (?, ?, 1, 1, 0);")
						.bind(col++, userId)
						.bind(col++, documentId);
				execute(st);
				st.dispose();
				if (db.getChanges() > 0)
					result = true;
			}
		}
		catch (Exception ex)
		{
			catchException(ex, logger, false);
		}
		finally
		{
			dispose(st);
		}		
		return result;		
	}
	
	protected boolean subEditor(SQLiteConnection db, Integer documentId, Integer userId)
	{
		boolean result = false;
		SQLiteStatement st = null;
		int col = 1;

		try
		{
			st = db.prepare("DELETE FROM [T_UserDocumentMapping] WHERE [DocumentID] = ? AND [UserID] = ? AND [IsEditor] = 1 AND [IsAuditor] = 0;")
					.bind(col++, documentId)
					.bind(col++, userId);
			execute(st);
			st.dispose();
			if (db.getChanges() > 0)
				result = true;
			else
			{
				col = 1;
				st = db.prepare("UPDATE [T_UserDocumentMapping] SET [IsEditor] = 0 WHERE [DocumentID] = ? AND [UserID] = ? AND [ISUser] = 1 AND [IsAuditor] = 1;")
						.bind(col++, documentId)
						.bind(col++, userId);
				execute(st);
				st.dispose();
				if (db.getChanges() > 0)
					result = true;
				else
					result = true;
			}
		}
		catch (Exception ex)
		{
			catchException(ex, logger, false);
		}
		finally
		{
			dispose(st);
		}		
		return result;		
	}
	
	protected boolean addAuditor(SQLiteConnection db, Integer documentId, Integer userId)
	{
		boolean result = false;
		SQLiteStatement st = null;
		int col = 1;

		try
		{
			// This should result in a row change even if the user was already an auditor
			st = db.prepare("UPDATE [T_UserDocumentMapping] SET [IsAuditor] = 1 WHERE [DocumentID] = ? AND [UserID] = ?;")
					.bind(col++, documentId)
					.bind(col++, userId);
			execute(st);
			st.dispose();
			if (db.getChanges() > 0)
				result = true;
			else
			{
				col = 1;
				st = db.prepare("INSERT INTO [T_UserDocumentMapping] ([UserID], [DocumentID], [IsEditor], [IsAuditor], [IsAdministrator]) " +
						"VALUES (?, ?, 0, 1, 0);")
						.bind(col++, userId)
						.bind(col++, documentId);
				execute(st);
				st.dispose();
				if (db.getChanges() > 0)
					result = true;
			}
		}
		catch (Exception ex)
		{
			catchException(ex, logger, false);
		}
		finally
		{
			dispose(st);
		}		
		return result;		
	}	
	
	protected boolean subAuditor(SQLiteConnection db, Integer documentId, Integer userId)
	{
		boolean result = false;
		SQLiteStatement st = null;
		int col = 1;

		try
		{
			st = db.prepare("UPDATE [T_UserDocumentMapping] SET [IsAuditor] = 0 WHERE [DocumentID] = ? AND [UserID] = ? AND [IsAuditor] = 1;")
					.bind(col++, documentId)
					.bind(col++, userId);
			execute(st);
			st.dispose();
			// Assume there is only one record and we're done
			if (db.getChanges() > 0)
				result = true;
			else
			{
				col = 1;
				st = db.prepare("DELETE FROM [T_UserDocumentMapping] WHERE [DocumentID] = ? AND [UserID] = ? AND [IsEditor] = 0;")
						.bind(col++, documentId)
						.bind(col++, userId);
				execute(st);
				st.dispose();
				if (db.getChanges() > 0)
					result = true;
				else
					result = true; // There must have been none to delete
			}
		}
		catch (Exception ex)
		{
			catchException(ex, logger, false);
		}
		finally
		{
			dispose(st);
		}		
		return result;		
	}	
}
/* **************************************************************************/
