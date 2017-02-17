/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedLocalSQLiteDataAccessorTest/src/test/TrustedLocalSQListDataAccessorTest.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package test;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import static org.junit.Assert.*;

import java.security.NoSuchAlgorithmException;
import java.security.spec.InvalidKeySpecException;
import java.util.ArrayList;

import only.DatabaseGenerator;

import org.json.JSONObject;
import org.junit.Before;
import org.junit.Test;

import com.almworks.sqlite4java.SQLiteException;

import secure.provenance.AuthenticationCredentials;
import secure.provenance.TrustedLocalSQLiteDataAccessor;
import secure.provenance.UserTypeId;
import secure.provenance.config.Config;
import secure.provenance.config.Configure;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class TrustedLocalSQListDataAccessorTest extends DatabaseGenerator
{
	protected static final Config CONFIG = Configure.oldConfig();
	protected static final String DEVELOPER_EMAIL_ADDRESS = CONFIG.getDeveloperConfig().getEmailAddress();
	protected static final String DEVELOPER_PASSWORD = CONFIG.getDeveloperConfig().getPassword();
	
	protected static final String GOOD_USER = DEVELOPER_EMAIL_ADDRESS;
	protected static final String GOOD_PASSWORD = DEVELOPER_PASSWORD;
	protected static final AuthenticationCredentials GOOD_CREDENTIALS = new AuthenticationCredentials(GOOD_USER, GOOD_PASSWORD);

	protected static final String TEST_USER = "test@provenance.com";
	protected static final String TEST_PASSWORD = "test";
	protected static final AuthenticationCredentials TEST_CREDENTIALS = new AuthenticationCredentials(TEST_USER, TEST_PASSWORD);
	
	@Before
	public void generateDatabase() throws SQLiteException, NoSuchAlgorithmException, InvalidKeySpecException
	{
		DatabaseGenerator databaseGenerator = new DatabaseGenerator();
		
		databaseGenerator.generateTrustedDatabase(TRUSTED_DATABASE);
		databaseGenerator.populateTrustedDatabase(TRUSTED_DATABASE);
	}
	
	protected TrustedLocalSQLiteDataAccessor newTrustedLocalSQLiteDataAccessor()
	{
		return new TrustedLocalSQLiteDataAccessor(TRUSTED_DATABASE, TRUSTED_DATABASE);
	}
	
	@Test
	public void testAuthenticateUser()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		AuthenticationCredentials badUser = new AuthenticationCredentials("bad@provenance.com", GOOD_PASSWORD);
		AuthenticationCredentials badPassword = new AuthenticationCredentials(GOOD_USER, "bad");
		AuthenticationCredentials badBoth = new AuthenticationCredentials("bad@provenance.com", "bad");
		String result;

		result = dataAccessor.authenticate(badUser, "", 0);
		assertFalse("Bad user can't log on", asBoolean(result));
		
		result = dataAccessor.authenticate(badPassword, "", 0);
		assertFalse("Bad password can't log on", asBoolean(result));

		result = dataAccessor.authenticate(badBoth, "", 0);
		assertFalse("Both user and password can't log on", asBoolean(result));

		result = dataAccessor.authenticate(GOOD_CREDENTIALS, "", 0);
		assertTrue("Good user and password can log on", asBoolean(result));
	}

	@Test
	public void testCreateUser()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		String result;
	
		result = dataAccessor.createUser(GOOD_CREDENTIALS, TEST_USER, "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		assertTrue("Can create user", asBoolean(result));

		result = dataAccessor.createUser(GOOD_CREDENTIALS, TEST_USER, "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		assertFalse("Can't re-create same user", asBoolean(result));
		
		result = dataAccessor.authenticate(TEST_CREDENTIALS, "", 0);
		assertTrue("New user can log on", asBoolean(result));		
	}	
	
	@Test
	public void testCreateDocument()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		String result;
		JSONObject object;
		result = dataAccessor.createDocument(TEST_CREDENTIALS, "title1");

		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title1");
		assertTrue("Good user can create a first new document", asBoolean(result));
		object = new JSONObject(result);
		int documentId1 = object.getJSONObject("payload").getInt("documentId");
		String versionId1 = object.getJSONObject("payload").getString("versionId");
		
		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title2");
		assertTrue("Good user can create a second new document", asBoolean(result));
		object = new JSONObject(result);
		int documentId2 = object.getJSONObject("payload").getInt("documentId");
		String versionId2 = object.getJSONObject("payload").getString("versionId");

		assertTrue("Second document ID is an increment of first", documentId2 == documentId1 + 1);
		assertFalse("Version IDs are different", versionId1.equals(versionId2));
		
		result = dataAccessor.createDocument(GOOD_CREDENTIALS, null);
		assertFalse("Title cannot be null", asBoolean(result));
		
		result = dataAccessor.createDocument(TEST_CREDENTIALS, "title1");
		assertFalse("Unauthorized user can't create document", asBoolean(result));		
	}

	@Test
	public void testUpdateDocument()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		int documentId = 0;
		String result;
		
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 0, documentId, "versionId", null);
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 0, documentId, "newVersionId", "prevVersionId");
		assertFalse("Can't update non-existing document", asBoolean(result));
				
		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title1");
		documentId = new JSONObject(result).getJSONObject("payload").getInt("documentId");
		
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 0, documentId, "versionId1", null);
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 1, documentId, "newVersionId1", "prevVersionId");
		assertTrue("Can update non-existing document", asBoolean(result));
		
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 0, documentId, "versionId2", null);
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 1, documentId, "newVersionId2", "prevVersionId");		
		assertTrue("Can update existing document", asBoolean(result));		
	}

	@Test
	public void testListDocuments()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		int documentId;
		String result;
		int count;
		
		result = dataAccessor.listDocuments(GOOD_CREDENTIALS, UserTypeId.EDITOR);
		assertTrue("Still true if no documents", asBoolean(result));
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("documents").length();
		assertEquals("Provenance user has zero documents", 0, count);
		
		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title1");
		documentId = new JSONObject(result).getJSONObject("payload").getInt("documentId");
		
		result = dataAccessor.listDocuments(GOOD_CREDENTIALS, UserTypeId.EDITOR);
		assertTrue("User has some documents", asBoolean(result));
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("documents").length();
		assertEquals("Provenance user has one document until updated", 0, count);
		
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 0, documentId, "versionId1", null);
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 1, documentId, "newVersionId1", "oldVersionId1");
		assertTrue("Document gets updated", asBoolean(result));		

		// Can audit them as well by default
		result = dataAccessor.listDocuments(GOOD_CREDENTIALS, UserTypeId.AUDITOR);
		assertTrue("User has some documents", asBoolean(result));
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("documents").length();
		assertEquals("Provenance auditor has one documents", 1, count);

		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title2");
		documentId = new JSONObject(result).getJSONObject("payload").getInt("documentId");
		
		result = dataAccessor.listDocuments(GOOD_CREDENTIALS, UserTypeId.EDITOR);
		assertTrue("User has some documents", asBoolean(result));		
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("documents").length();
		assertEquals("Provenance user still has one document", 1, count);

		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 0, documentId, "versionId2", null);
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 1, documentId, "newVersionId2", "oldVersionId2");
		assertTrue("Document gets updated", asBoolean(result));		

		result = dataAccessor.listDocuments(GOOD_CREDENTIALS, UserTypeId.EDITOR);
		assertTrue("User has some documents", asBoolean(result));		
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("documents").length();
		assertEquals("Provenance user still has one document", 2, count);
		
		result = dataAccessor.listDocuments(GOOD_CREDENTIALS, UserTypeId.ADMINISTRATOR);
		assertTrue("User has some documents", asBoolean(result));		
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("documents").length();
		assertEquals("Provenance administrator has no documents", 0, count);	
	}

	@Test
	public void testGetAllDocuments()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		String result;
		JSONObject object;
		int documentId;
		int count;

		result = dataAccessor.getAllDocuments(GOOD_CREDENTIALS);
		assertTrue("Still true if no documents", asBoolean(result));
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("documents").length();
		assertTrue("There aren't any documents yet", count == 0);	

		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title1");
		documentId = new JSONObject(result).getJSONObject("payload").getInt("documentId");		
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 0, documentId, "versionId1", null);
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 1, documentId, "newVersionId1", "oldVersionId1");

		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title2");
		documentId = new JSONObject(result).getJSONObject("payload").getInt("documentId");		
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 0, documentId, "versionId2", null);
		result = dataAccessor.updateDocument(GOOD_CREDENTIALS, 1, documentId, "newVersionId2", "oldVersionId2");		

		result = dataAccessor.getAllDocuments(GOOD_CREDENTIALS);
		assertTrue("There are some documents", asBoolean(result));
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("documents").length();
		assertTrue("Multiple documents are returned", count > 1);
		
		result = dataAccessor.createUser(GOOD_CREDENTIALS, TEST_USER, "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		result = dataAccessor.getAllDocuments(TEST_CREDENTIALS);
		assertFalse("Normal users can't get all the documents", asBoolean(result));
	}

	@Test
	public void testGetDocumentEditors()
	{	
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		ArrayList<Integer> users = new ArrayList<Integer>();
		JSONObject object;
		int documentId = 9;
		int count;
		String result;

		result = dataAccessor.getDocumentEditors(GOOD_CREDENTIALS, documentId);
		assertTrue("No editors is OK", asBoolean(result));
				
		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title1");
		documentId = new JSONObject(result).getJSONObject("payload").getInt("documentId");		

		result = dataAccessor.getDocumentEditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("editors").length();
		assertTrue("One editor is fine", count == 1);
		
		result = dataAccessor.createUser(GOOD_CREDENTIALS, TEST_USER, "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		
		users.clear();
		users.add(2);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		result = dataAccessor.getDocumentEditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("editors").length();
		assertTrue("Two editors are fine", count == 2);
		
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		result = dataAccessor.getDocumentEditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("editors").length();
		assertTrue("Can re-add editors", count == 2);
		
		result = dataAccessor.createUser(GOOD_CREDENTIALS, "test2@provenance.com", "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		
		users.add(3);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		result = dataAccessor.getDocumentEditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("editors").length();
		assertTrue("Can have three editors", count == 3);
		
		users.clear();
		users.add(9);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		// The foreign key constraint is not working here
		result = dataAccessor.getDocumentEditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("editors").length();
		// Unknown editor is not found on join with user details
		assertTrue("Can have three editors", count == 3);		
	}
	
	@Test
	public void testGetDocumentAuditors()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		ArrayList<Integer> users = new ArrayList<Integer>();
		JSONObject object;
		int documentId = 9;
		int count;
		String result;

		result = dataAccessor.getDocumentAuditors(GOOD_CREDENTIALS, documentId);
		assertTrue("No editors is OK", asBoolean(result));
				
		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title1");
		documentId = new JSONObject(result).getJSONObject("payload").getInt("documentId");		

		result = dataAccessor.getDocumentAuditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("auditors").length();
		assertTrue("One editor is fine", count == 1);
		
		result = dataAccessor.createUser(GOOD_CREDENTIALS, TEST_USER, "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		
		users.clear();
		users.add(2);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		result = dataAccessor.getDocumentAuditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("auditors").length();
		assertTrue("Two editors are fine", count == 2);
		
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		result = dataAccessor.getDocumentAuditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("auditors").length();
		assertTrue("Can re-add editors", count == 2);
		
		result = dataAccessor.createUser(GOOD_CREDENTIALS, "test2@provenance.com", "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		
		users.add(3);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		result = dataAccessor.getDocumentAuditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("auditors").length();
		assertTrue("Can have three editors", count == 3);
		
		users.clear();
		users.add(9);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		// The foreign key constraint is not working here
		result = dataAccessor.getDocumentAuditors(GOOD_CREDENTIALS, documentId);
		object = new JSONObject(result);
		count = object.getJSONObject("payload").getJSONArray("auditors").length();
		// Unknown editor is not found on join with user details
		assertTrue("Can have three editors", count == 3);	
	}

//	@Test
	public void testAddAuditors()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		ArrayList<Integer> auditors = new ArrayList<Integer>();
		int documentId;
		String result;

		documentId = 1;
		auditors.add(2);
		result = dataAccessor.addAuditors(GOOD_CREDENTIALS, documentId, auditors);
		assertTrue("Existing auditors are accounted for", asBoolean(result));
		auditors.clear();
		
		documentId = 2;
		auditors.add(2);
		result = dataAccessor.addAuditors(GOOD_CREDENTIALS, documentId, auditors);
		assertTrue("Existing user is made an auditors", asBoolean(result));
		auditors.clear();
		
		documentId = 2;
		auditors.add(1);
		result = dataAccessor.addAuditors(GOOD_CREDENTIALS, documentId, auditors);
		assertTrue("New document user is added as auditor", asBoolean(result));
		auditors.clear();		
	}

//	@Test
	public void testSubAuditors()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		ArrayList<Integer> auditors = new ArrayList<Integer>();
		int documentId;
		String result;

		documentId = 1;
		auditors.add(2);
		result = dataAccessor.subAuditors(GOOD_CREDENTIALS, documentId, auditors);
		assertTrue("Delete entire record if not user", asBoolean(result));
		auditors.clear();
		
		documentId = 2;
		auditors.add(2);
		result = dataAccessor.subAuditors(GOOD_CREDENTIALS, documentId, auditors);
		assertTrue("Just remove auditor status if is user", asBoolean(result));
		auditors.clear();
		
		documentId = 2;
		auditors.add(1);
		result = dataAccessor.subAuditors(GOOD_CREDENTIALS, documentId, auditors);
		assertTrue("New document user is added as auditor", asBoolean(result));
		auditors.clear();		
	}

	@Test
	public void testAddEditors()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		ArrayList<Integer> users = new ArrayList<Integer>();
		int documentId = 9;
		String result;

		users.add(0);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		assertFalse("Document doesn't exist", asBoolean(result));

		result = dataAccessor.createDocument(GOOD_CREDENTIALS, "title1");
		documentId = new JSONObject(result).getJSONObject("payload").getInt("documentId");		

		result = dataAccessor.createUser(GOOD_CREDENTIALS, TEST_USER, "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		
		users.clear();
		users.add(1);
		result = dataAccessor.addEditors(TEST_CREDENTIALS, documentId, users);
		assertFalse("Can only add if you are already a user of the document", asBoolean(result));

		users.clear();
		users.add(2);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		assertTrue("Can add new user", asBoolean(result));
		
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		assertTrue("Can re-add new user", asBoolean(result));
		
		result = dataAccessor.createUser(GOOD_CREDENTIALS, "test2@provenance.com", "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		
		users.add(3);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		assertTrue("Can add multiple users", asBoolean(result));
		
		users.clear();
		users.add(9);
		result = dataAccessor.addEditors(GOOD_CREDENTIALS, documentId, users);
		// The foreign key constraint is not working here
		assertTrue("User doesn't exist", asBoolean(result));		
	}
	
//	@Test
	public void testSubEditors()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		ArrayList<Integer> auditors = new ArrayList<Integer>();
		int documentId;
		String result;

		documentId = 1;
		auditors.add(2);
		result = dataAccessor.subEditors(GOOD_CREDENTIALS, documentId, auditors);
		assertTrue("Delete entire record if not user", asBoolean(result));
		auditors.clear();
		
		documentId = 2;
		auditors.add(2);
		result = dataAccessor.subEditors(GOOD_CREDENTIALS, documentId, auditors);
		assertTrue("Just remove auditor status if is user", asBoolean(result));
		auditors.clear();
		
		documentId = 2;
		auditors.add(1);
		result = dataAccessor.subEditors(GOOD_CREDENTIALS, documentId, auditors);
		assertTrue("New document user is added as auditor", asBoolean(result));
		auditors.clear();		
	}

	@Test
	public void testListUsers()
	{
		TrustedLocalSQLiteDataAccessor dataAccessor = newTrustedLocalSQLiteDataAccessor();
		String result;
		int count;
		
		result = dataAccessor.listUsers(GOOD_CREDENTIALS, UserTypeId.EDITOR);
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("users").length();
		assertEquals("One user is preconfigured", 0, count);

		result = dataAccessor.createUser(GOOD_CREDENTIALS, TEST_USER, "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		result = dataAccessor.listUsers(GOOD_CREDENTIALS, UserTypeId.EDITOR);
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("users").length();
		assertEquals("There are one editors", 1, count);
		
		result = dataAccessor.listUsers(GOOD_CREDENTIALS, UserTypeId.AUDITOR);
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("users").length();
		assertEquals("There is also one auditor", 1, count);

		result = dataAccessor.createUser(GOOD_CREDENTIALS, "test2@provenance.com", "Super", "Test", UserTypeId.EDITOR, TEST_PASSWORD);		
		result = dataAccessor.listUsers(GOOD_CREDENTIALS, UserTypeId.EDITOR);
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("users").length();
		assertEquals("There are two editors", 2, count);
		
		result = dataAccessor.listUsers(GOOD_CREDENTIALS, UserTypeId.EDITOR);
		count = new JSONObject(result).getJSONObject("payload").getJSONArray("users").length();
		assertEquals("There are two auditors", 2, count);	
	}
	
	protected boolean asBoolean(String result)
	{
		JSONObject object = new JSONObject(result);
		boolean value = object.getBoolean("result");
		
		return value;
	}
}
/* **************************************************************************/
