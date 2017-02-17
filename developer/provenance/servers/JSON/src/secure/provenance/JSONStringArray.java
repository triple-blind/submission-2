/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/JSON/src/secure/provenance/JSONStringArray.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
Imports
****************************************************************************/
import org.json.JSONArray;
import org.json.JSONObject;
/* ***************************************************************************
Class
****************************************************************************/
// This kind of array holds strings, but the strings themselves are
// JSON strings that can be made into objects themselves.
public class JSONStringArray extends JSONArray
{
	public JSONStringArray()
	{
		super();
	}
	
	public JSONStringArray(String source)
	{
		super(source);
	}
	
	public JSONStringArray append(String more) {
		super.put(more);
		return this;
	}

	public JSONStringArray append(JSONStringArray more)
	{
		for (int i = 0; i < more.length(); i++)
			put(more.getString(i));
		return this;
	}
	
	public JSONStringArray appendTo(JSONArray array)
	{
		for (int i = 0; i < length(); i++)
			array.put(new JSONObject(getString(i)));
		return this;
	}
	
	public String getLast()
	{
		return getString(length()-1);
	}
	
	public String getOnly()
	{
		assert(length() == 1);
		return getLast();
	}
}
/* **************************************************************************/
