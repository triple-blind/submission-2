/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/CertificateGeneratorTest/src/test/CertificateGeneratorTest.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package test;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import static org.junit.Assert.*;

import org.junit.Test;

import secure.provenance.X509CertificateGenerator;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class CertificateGeneratorTest
{
	protected static final String GENERATOR_SUBDIR = "./keystore/";
	protected static final String GENERATOR_FILE = GENERATOR_SUBDIR + "generator.jks";
	protected static final String GENERATOR_PASSWORD = "password";
	protected static final String GENERATOR_ALIAS = "generator";
	
	@Test
	public void testCertificateGenerator() throws InterruptedException
	{
		try
		{
			X509CertificateGenerator certificateGenerator = new X509CertificateGenerator(GENERATOR_FILE, GENERATOR_PASSWORD, GENERATOR_ALIAS);
			assertTrue(certificateGenerator.createCertificate("test", 30));
			
			byte[] pemCertificate = certificateGenerator.getPEMCertificate();
			byte[] pemPrivateKey = certificateGenerator.getPEMPrivateKey();
			byte[] privateKey = certificateGenerator.getPrivateKey();
			byte[] publicKey = certificateGenerator.getPublicKey();
			
			assertNotNull(pemCertificate);
			assertNotNull(pemPrivateKey);
			assertNotNull(privateKey);
			assertNotNull(publicKey);
		}
		catch (Exception ex)
		{
			fail("There are no exceptions!");
			
		}
	}
}
/* **************************************************************************/
