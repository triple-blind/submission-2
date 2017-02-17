/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/X509CertificateGenerator.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.io.DataOutputStream;
import java.math.BigInteger;
import java.security.GeneralSecurityException;
import java.security.InvalidKeyException;
import java.security.Key;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.Signature;
import java.security.SignatureException;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import java.security.interfaces.RSAPrivateCrtKey;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.RSAPrivateCrtKeySpec;
import java.util.Calendar;
import java.util.Date;

import org.apache.commons.codec.binary.Hex;
import org.bouncycastle.asn1.ASN1EncodableVector;
import org.bouncycastle.asn1.ASN1InputStream;
import org.bouncycastle.asn1.ASN1Integer;
import org.bouncycastle.asn1.ASN1ObjectIdentifier;
import org.bouncycastle.asn1.ASN1Sequence;
import org.bouncycastle.asn1.DERBitString;
import org.bouncycastle.asn1.DERNull;
import org.bouncycastle.asn1.DERObjectIdentifier;
import org.bouncycastle.asn1.DEROutputStream;
import org.bouncycastle.asn1.DERSequence;
import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.asn1.x509.AlgorithmIdentifier;
import org.bouncycastle.asn1.x509.Certificate;
import org.bouncycastle.asn1.x509.SubjectPublicKeyInfo;
import org.bouncycastle.asn1.x509.TBSCertificate;
import org.bouncycastle.asn1.x509.Time;
import org.bouncycastle.x509.X509Util;
import org.bouncycastle.asn1.x509.V3TBSCertificateGenerator;
import org.bouncycastle.crypto.CryptoException;
import org.bouncycastle.crypto.DataLengthException;
import org.bouncycastle.crypto.params.RSAPrivateCrtKeyParameters;
import org.bouncycastle.jce.provider.X509CertificateObject;
import org.bouncycastle.openssl.PEMWriter;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class X509CertificateGenerator
{
	private static Logger logger = Logger.getLogger(X509CertificateGenerator.class);
	
	/** This holds the certificate of the CA used to sign the new certificate. The object is created in the constructor. */
	private X509Certificate caCert;
	/** This holds the private key of the CA used to sign the new certificate. The object is created in the constructor. */
	private RSAPrivateCrtKeyParameters caPrivateKey;


	private byte[] privateKeyData;
	private byte[] publicKeyData;
	private byte[] pemCertificateData;
	private byte[] pemPrivateKeyData;
	
	public X509CertificateGenerator(String caFile, String caPassword, String caAlias) 
			throws FileNotFoundException, IOException, GeneralSecurityException 
	{
		
		logger.info("Loading CA certificate and private key from file '" + caFile + "', using alias '" + caAlias + "' with JCE API");
		// Can't this be JKS?
		KeyStore caKeyStore = KeyStoreManager.newKeyStore("JKS" /*"PKCS12"*/, caFile, caPassword); 
		
		// load the key entry from the keystore
		Key key = caKeyStore.getKey(caAlias, caPassword.toCharArray());

		if (key == null) 
		{
			throw new RuntimeException("Got null key from keystore. Check the alias!"); 
		}

		RSAPrivateCrtKey privateKey = (RSAPrivateCrtKey) key;
		caPrivateKey = new RSAPrivateCrtKeyParameters(privateKey.getModulus(), privateKey.getPublicExponent(), privateKey.getPrivateExponent(),
				privateKey.getPrimeP(), privateKey.getPrimeQ(), privateKey.getPrimeExponentP(), privateKey.getPrimeExponentQ(), privateKey.getCrtCoefficient());
		// and get the certificate
		caCert = (X509Certificate) caKeyStore.getCertificate(caAlias);
		if (caCert == null) 
		{
			throw new RuntimeException("Got null cert from keystore. Certificate name must match alias!"); 
		}
		logger.debug("Successfully loaded CA key and certificate. CA DN is '" + caCert.getSubjectDN().getName() + "'");
		caCert.verify(caCert.getPublicKey());
		logger.debug("Successfully verified CA certificate with its own public key.");
	}


	public boolean createCertificate(String dn, int validityDays) throws 
			IOException, InvalidKeyException, SecurityException, SignatureException, NoSuchAlgorithmException, DataLengthException, CryptoException,
			KeyStoreException, NoSuchProviderException, CertificateException, InvalidKeySpecException
	{
		logger.info("Generating certificate for distinguished subject name '" + 
				dn + "', valid for " + validityDays + " days");
		SecureRandom sr = new SecureRandom();
		
		PublicKey publicKey;
		PrivateKey privateKey;
		
		logger.debug("Creating RSA keypair");
		// generate the keypair for the new certificate
		
		KeyPairGenerator keyGen = KeyPairGenerator.getInstance("RSA");
		keyGen.initialize(1024, sr);
		KeyPair keypair = keyGen.generateKeyPair();
		privateKey = keypair.getPrivate();
		publicKey = keypair.getPublic();
		
	    	Calendar expiry = Calendar.getInstance();
		expiry.add(Calendar.DAY_OF_YEAR, validityDays);
 
		//X509Name x509Name = new X509Name("CN=" + dn);
		X500Name x500Name = new X500Name("CN=" + dn);
		
		V3TBSCertificateGenerator certGen = new V3TBSCertificateGenerator();
    	certGen.setSerialNumber(new ASN1Integer(BigInteger.valueOf(System.currentTimeMillis())));

    	//certGen.setIssuer(PrincipalUtil.getSubjectX509Principal(caCert));
    	certGen.setIssuer(new X500Name(caCert.getSubjectX500Principal().getName()));
		//certGen.setSubject(x509Name);
		certGen.setSubject(x500Name);
		DERObjectIdentifier derSigOID = X509Util.getAlgorithmOID("SHA1WithRSAEncryption");
		// AlgorithmIdentifier sigAlgId = new AlgorithmIdentifier(sigOID, new DERNull());
		ASN1ObjectIdentifier asn1SigOID = new ASN1ObjectIdentifier(derSigOID.getId());
		AlgorithmIdentifier sigAlgId = new AlgorithmIdentifier(asn1SigOID, DERNull.INSTANCE);
		// kwa: need assert that this results in same thing
		certGen.setSignature(sigAlgId);
		
		ASN1InputStream asn1InputStream = new ASN1InputStream(new ByteArrayInputStream(publicKey.getEncoded()));
		certGen.setSubjectPublicKeyInfo(new SubjectPublicKeyInfo((ASN1Sequence) asn1InputStream.readObject()));
		asn1InputStream.close();
		
		certGen.setStartDate(new Time(new Date(System.currentTimeMillis())));
		certGen.setEndDate(new Time(expiry.getTime()));
		
		logger.debug("Certificate structure generated, creating SHA1 digest");
	
//		TBSCertificateStructure tbsCert = certGen.generateTBSCertificate();
		TBSCertificate tbsCert = certGen.generateTBSCertificate();

		ByteArrayOutputStream   bOut = new ByteArrayOutputStream();
		DEROutputStream         dOut = new DEROutputStream(bOut);
		dOut.writeObject(tbsCert);
		dOut.close();

		// and now sign
		byte[] signature;

	        PrivateKey caPrivKey = KeyFactory.getInstance("RSA").generatePrivate(
	        		new RSAPrivateCrtKeySpec(caPrivateKey.getModulus(), caPrivateKey.getPublicExponent(),
	        				caPrivateKey.getExponent(), caPrivateKey.getP(), caPrivateKey.getQ(), 
	        				caPrivateKey.getDP(), caPrivateKey.getDQ(), caPrivateKey.getQInv()));
			
	        Signature sig = Signature.getInstance(asn1SigOID.getId());
	        sig.initSign(caPrivKey, sr);
	        sig.update(bOut.toByteArray());
	        signature = sig.sign();
		
		logger.debug("SHA1/RSA signature of digest is '" + new String(Hex.encodeHex(signature)) + "'");

		// and finally construct the certificate structure
        ASN1EncodableVector  v = new ASN1EncodableVector();

        v.add(tbsCert);
        v.add(sigAlgId);
        v.add(new DERBitString(signature));

        // X509CertificateObject clientCert = new X509CertificateObject(new X509CertificateStructure(new DERSequence(v)));
        X509CertificateObject clientCert = new X509CertificateObject(new Certificate(new DERSequence(v)));
        
        logger.debug("Verifying certificate for correct signature with CA public key");
        clientCert.verify(caCert.getPublicKey());

	ByteArrayOutputStream publicKeyStream = new ByteArrayOutputStream();
	ByteArrayOutputStream rawPrivateKeyStream = new ByteArrayOutputStream();

 	DataOutputStream dosPrivate = new DataOutputStream(rawPrivateKeyStream);
 	DataOutputStream dosPublic = new DataOutputStream(publicKeyStream);

  	dosPrivate.write(privateKey.getEncoded());
	dosPublic.write(publicKey.getEncoded());
	
	dosPrivate.close();
   	dosPublic.close();

	
	publicKeyData = publicKeyStream.toByteArray();
	privateKeyData = rawPrivateKeyStream.toByteArray();

       	ByteArrayOutputStream op = new ByteArrayOutputStream();
        Writer writer = new OutputStreamWriter(op);

	PEMWriter pem = new PEMWriter(writer);
        pem.writeObject(clientCert);
        
	pem.close();

	pemCertificateData = op.toByteArray();

 
 
        //FileOutputStream pkeyOs = new FileOutputStream("test_key_new.pem");
       	ByteArrayOutputStream pkeyOs = new ByteArrayOutputStream();
        Writer pkeyW = new OutputStreamWriter(pkeyOs);
        PEMWriter keyWriter = new PEMWriter(pkeyW);
        
        keyWriter.writeObject(privateKey);
        keyWriter.close();
	pemPrivateKeyData = pkeyOs.toByteArray();
        return true;

	}

	public byte[] getPrivateKey()
	{
		return privateKeyData;
	}

	public byte[] getPublicKey()
	{
		return publicKeyData;
	}

	public byte[] getPEMCertificate()
	{
		return pemCertificateData;
	}

	public byte[] getPEMPrivateKey()
	{
		return pemPrivateKeyData;
	}	
}
/* **************************************************************************/
