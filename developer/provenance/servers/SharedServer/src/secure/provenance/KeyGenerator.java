/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/KeyGenerator.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
Imports
****************************************************************************/
import javax.crypto.SecretKey;

import java.security.NoSuchAlgorithmException;
/* ***************************************************************************
Class
****************************************************************************/
public class KeyGenerator
{
	public static final int KEYSIZE = 128; // 192 and 256 bits may not be available
	
	public static byte[] generateKey() throws NoSuchAlgorithmException
	{
		javax.crypto.KeyGenerator kgen = javax.crypto.KeyGenerator.getInstance("AES");
		kgen.init(KEYSIZE);
		SecretKey skey = kgen.generateKey();
		byte[] raw = skey.getEncoded();		
		return raw;	
	}
}
/* ***************************************************************************
Class
****************************************************************************/



/* **************************************************************************/