/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockServer/src/only/KeyStoreManager.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.IOException;
import java.security.GeneralSecurityException;
import java.security.KeyStore;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class KeyStoreManager
{
	// This enforces the policy that keystores can be stored as resources in the jar file.
	public static KeyStore newKeyStore(String format, String filename, String password) throws GeneralSecurityException, IOException
	{
		KeyStore keyStore = KeyStore.getInstance(format);
		keyStore.load(KeyStoreManager.class.getClassLoader().getResourceAsStream(filename), password.toCharArray());
		return keyStore;
	}	
}
/* **************************************************************************/
