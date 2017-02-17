/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/entry/KeyMaterials.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list.entry;
/* ***************************************************************************
Imports
****************************************************************************/
import java.util.HashMap;

import org.json.JSONArray;
/* ***************************************************************************
Class
****************************************************************************/
public class KeyMaterials
{
	protected HashMap<Integer, KeyMaterial> keyMaterials;
	
	public KeyMaterials(JSONArray jsonArray)
	{
		keyMaterials = new HashMap<Integer, KeyMaterial>(jsonArray.length());
		
		for (int i = 0; i < jsonArray.length(); i++)
		{
			KeyMaterial keyMaterial = new KeyMaterial(jsonArray.getJSONObject(i));
			keyMaterials.put(keyMaterial.userId, keyMaterial);
		}
	}

	public KeyMaterial find(int userId)
	{
		return keyMaterials.get(userId);
	}
}
/* **************************************************************************/
