/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/JSONTest/src/test/JSONTest.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package test;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import static org.junit.Assert.*;

import org.junit.Test;

import org.json.JSONObject;
import secure.provenance.JSONStringArray;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class JSONTest {

	@Test
	public void testJSONStringArray()
	{
		JSONStringArray array = new JSONStringArray("['1', '2', '3']");
		JSONStringArray more = new JSONStringArray("['1', '2.5', 'hello']");
		
		assertTrue(array.length() == 3);
		assertTrue(more.length() == 3);
		
		array.append(more);
		
		assertTrue(array.length() == 6);
		assertEquals(array.getString(3), more.getString(0));
		assertEquals(array.getString(4), more.getString(1));
		assertEquals(array.getString(5), more.getString(2));
	}
	
	@Test
	public void testGetLast()
	{
		JSONStringArray array = new JSONStringArray("['1', '2', '3']");
		JSONStringArray more = new JSONStringArray("['1', '2.5', 'hello']");
		
		assertEquals(array.getLast(), array.getString(2));
		assertEquals(more.getLast(), more.getString(2));
		
		array.append(more);
		
		assertEquals(array.getLast(), more.getString(2));
	}
	
	@Test
	public void test8Bit()
	{
		// This also works with \\u00fc, which seems to mean that JSON is interpreting it.
		JSONObject object = new JSONObject("{ test: '\u00fcber'}");
		String string = object.getString("test");

		assertEquals(string, "\u00fcber");
		
		String output = object.toString();
		object = new JSONObject(output);
		string = object.getString("test");
		
		assertEquals(string, "\u00fcber");		
	}	

}
/* **************************************************************************/
