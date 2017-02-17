/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/PasswordHashTest/src/test/PasswordHashTest.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package test;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.security.NoSuchAlgorithmException;
import java.security.spec.InvalidKeySpecException;

import secure.provenance.PasswordHash;

import org.junit.Assert;
import org.junit.Test;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class PasswordHashTest {

	@Test
	public void testPasswordHash() {
	    String password = "provenance";
        String wrongPassword = "something";

        try {
			String hash = PasswordHash.createHash(password);
	        String secondHash = PasswordHash.createHash(password);

	        Assert.assertNotEquals("Two hashes should not be equal",  hash,  secondHash);

	        Assert.assertFalse("Wrong password should not be accepted", PasswordHash.validatePassword(wrongPassword, hash));
	        Assert.assertTrue("Rightpassword should be accepted", PasswordHash.validatePassword(password, hash));			
		}
		catch (NoSuchAlgorithmException | InvalidKeySpecException e) {
			Assert.fail("Exception caused");
		}
	}
}
/* **************************************************************************/
