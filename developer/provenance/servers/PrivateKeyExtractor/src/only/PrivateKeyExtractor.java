/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/PrivateKeyExtractor/src/only/PrivateKeyExtractor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.FileInputStream;
import java.security.Key;
import java.security.KeyStore;
import javax.xml.bind.DatatypeConverter;;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class PrivateKeyExtractor {
	protected static final String syntax=
			"expected args: keystore filename, keystore password, "+
			"alias, [key password: defaults to keystore password]";
	
    static public void main(String[] args) throws Exception {
        if(args.length < 3)
          throw new IllegalArgumentException(syntax);
        final String keystoreName = args[0];     
        final String keystorePassword = args[1];
        final String alias = args[2];
        final String keyPassword = getKeyPassword(args,keystorePassword);
        KeyStore ks = KeyStore.getInstance("jks");
        ks.load(new FileInputStream(keystoreName), keystorePassword.toCharArray());
        Key key = ks.getKey(alias, keyPassword.toCharArray());
        String b64 = DatatypeConverter.printBase64Binary(key.getEncoded());
        System.out.println("-----BEGIN PRIVATE KEY-----");
        System.out.println(b64);
        System.out.println("-----END PRIVATE KEY-----");
    }
    
    private static String getKeyPassword(final String[] args, final String keystorePassword) {
        String keyPassword = keystorePassword; // default case
        if(args.length == 4)
            keyPassword = args[3];
        return keyPassword;
    }
}
/* **************************************************************************/
