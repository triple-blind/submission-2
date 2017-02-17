/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/entry/KeyMaterial.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list.entry;
/* ***************************************************************************
Imports
****************************************************************************/
import org.apache.commons.codec.binary.Base64;
import org.json.JSONObject;
/* ***************************************************************************
Class
****************************************************************************/
public class KeyMaterial
{
	public int userId;
	public byte[] key;
	public int keySize;
	
	public KeyMaterial(JSONObject jsonObject)
	{
		userId = jsonObject.getInt("id");
		key = Base64.decodeBase64(jsonObject.getString("sk")); // session key
		keySize = jsonObject.getInt("sz");
	}
}
/* **************************************************************************/
