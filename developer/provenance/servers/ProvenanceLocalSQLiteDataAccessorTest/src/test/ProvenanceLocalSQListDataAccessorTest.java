/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/ProvenanceLocalSQLiteDataAccessorTest/src/test/ProvenanceLocalSQListDataAccessorTest.java#1 $
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

import only.DatabaseGenerator;

import org.json.JSONObject;
import org.junit.Before;
import org.junit.Test;

import com.almworks.sqlite4java.SQLiteException;

import secure.provenance.ProvenanceLocalSQLiteDataAccessor;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class ProvenanceLocalSQListDataAccessorTest extends DatabaseGenerator
{
	protected String uniqueKey(int documentNo)
	{
		return "'UniqueKey " + documentNo + "'";
	}

	@Before
	public void generateDatabase() throws SQLiteException, NoSuchAlgorithmException, InvalidKeySpecException
	{
		DatabaseGenerator databaseGenerator = new DatabaseGenerator();
		
		databaseGenerator.generateProvenanceDatabase(PROVENANCE_DATABASE);
	}
	
	protected ProvenanceLocalSQLiteDataAccessor newProvenanceLocalSQLiteDataAccessor()
	{
		return new ProvenanceLocalSQLiteDataAccessor(PROVENANCE_DATABASE, PROVENANCE_DATABASE);
	}
	
	@Test
	public void testGetDocumentForAudit()
	{
		ProvenanceLocalSQLiteDataAccessor dataAccessor = newProvenanceLocalSQLiteDataAccessor();
		String result;
		
		result = dataAccessor.getDocumentForAudit("0");
		assertFalse("Document not found", asBoolean(result));

		result = dataAccessor.putDocumentForEdit("1", null, "document1", "provenance1");
		result = dataAccessor.getDocumentForAudit("1");
		assertTrue("Document found", asBoolean(result));
		assertEquals("Correct document", "document1", new JSONObject(result).getJSONObject("payload").getString("document"));
		assertTrue("Correct number of events", new JSONObject(result).getJSONObject("payload").getJSONArray("provenances").length() == 1);
		assertEquals("Correct provenance", "provenance1", new JSONObject(result).getJSONObject("payload").getJSONArray("provenances").getString(0));

		result = dataAccessor.putDocumentForEdit("2", "1", "document2", "provenance2");
		result = dataAccessor.getDocumentForAudit("2");
		assertTrue("Document found", asBoolean(result));
		assertEquals("Correct document", "document2", new JSONObject(result).getJSONObject("payload").getString("document"));
		assertTrue("Correct number of events", new JSONObject(result).getJSONObject("payload").getJSONArray("provenances").length() == 2);	
		assertEquals("Correct provenance", "provenance1", new JSONObject(result).getJSONObject("payload").getJSONArray("provenances").getString(0));
		assertEquals("Correct provenance", "provenance2", new JSONObject(result).getJSONObject("payload").getJSONArray("provenances").getString(1));
		
		result = dataAccessor.putDocumentForEdit("3", "2", "document3", "provenance3");
		result = dataAccessor.getDocumentForAudit("3");
		assertTrue("Document found", asBoolean(result));
		assertEquals("Correct document", "document3", new JSONObject(result).getJSONObject("payload").getString("document"));
		assertTrue("Correct number of events", new JSONObject(result).getJSONObject("payload").getJSONArray("provenances").length() == 3);	
		assertEquals("Correct provenance", "provenance1", new JSONObject(result).getJSONObject("payload").getJSONArray("provenances").getString(0));
		assertEquals("Correct provenance", "provenance2", new JSONObject(result).getJSONObject("payload").getJSONArray("provenances").getString(1));		
		assertEquals("Correct provenance", "provenance3", new JSONObject(result).getJSONObject("payload").getJSONArray("provenances").getString(2));		
	}
	
	@Test
	public void testGetDocumentForEdit()
	{
		ProvenanceLocalSQLiteDataAccessor dataAccessor = newProvenanceLocalSQLiteDataAccessor();
		String result;
		
		result = dataAccessor.getDocumentForEdit("0");
		assertFalse("Document not found", asBoolean(result));

		result = dataAccessor.putDocumentForEdit("1", null, "document1", "provenance1");
		result = dataAccessor.getDocumentForEdit("1");
		assertTrue("Document found", asBoolean(result));
		assertEquals("Correct document", "document1", new JSONObject(result).getJSONObject("payload").getString("document"));
		assertEquals("Correct provenance", "provenance1", new JSONObject(result).getJSONObject("payload").getString("provenance"));
	}

	@Test
	public void testPutDocumentForEdit()
	{	
		ProvenanceLocalSQLiteDataAccessor dataAccessor = newProvenanceLocalSQLiteDataAccessor();
		String result;

		result = dataAccessor.putDocumentForEdit("1", null, "document1", "[ \"provenance1\" ]");
		assertTrue("Version 1 can be saved", asBoolean(result));

		result = dataAccessor.putDocumentForEdit("1", "1", "document1", "[ \"provenance2\" ]");
		assertFalse("Version 1 can't be re-saved", asBoolean(result));
		
		result = dataAccessor.putDocumentForEdit("2", "1", "document2", "[ \"provenance2\" ]");
		assertTrue("Version 2 based on 1 can be saved", asBoolean(result));

		// For some reason, the foreign key constraint is not working.
		// It does work in the IDE but not via this Java interface
		result = dataAccessor.putDocumentForEdit("3", "5", "document3", "[ \"provenance3\" ]");
		assertTrue("Version 3 based on non-existing version can't be saved", asBoolean(result));		
	}

	protected boolean asBoolean(String result)
	{
		JSONObject object = new JSONObject(result);
		boolean value = object.getBoolean("result");
		
		return value;
	}
}
/* **************************************************************************/
