/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/entry/ProvenanceEntry.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list.entry;
/* ***************************************************************************
Imports
****************************************************************************/
import java.io.IOException;
import java.security.InvalidKeyException;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.interfaces.RSAPrivateKey;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.PKCS8EncodedKeySpec;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.SecretKeySpec;

import list.event.ProvenanceEvent;

import org.apache.commons.codec.binary.Base64;
import org.json.JSONArray;
import org.json.JSONObject;
/* ***************************************************************************
Class
****************************************************************************/
public class ProvenanceEntry
{
	public ProvenanceEvent provenanceEvent;

	protected Cipher newCipher(RSAPrivateKey privateKey) throws NoSuchAlgorithmException, NoSuchPaddingException, InvalidKeyException
	{
        Cipher cipher = Cipher.getInstance("RSA/ECB/OAEPWithSHA-1AndMGF1Padding");

        cipher.init(Cipher.DECRYPT_MODE, privateKey);
        return cipher;
	}
	
	protected byte[] decryptRSA(byte[] encryptedData, byte[] keyBytes) throws IOException, InvalidKeySpecException, NoSuchAlgorithmException, NoSuchPaddingException, InvalidKeyException, IllegalBlockSizeException, BadPaddingException
	{			
		PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(keyBytes);		
        RSAPrivateKey key = (RSAPrivateKey) KeyFactory.getInstance("RSA").generatePrivate(keySpec);
        Cipher cipher = newCipher(key);           
        byte[] decryptedData = cipher.doFinal(encryptedData);
        
        return decryptedData;            
	};

	static protected Cipher newCipher(SecretKeySpec secretKeySpec)
			throws NoSuchAlgorithmException,NoSuchPaddingException, InvalidKeyException
	{
        Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");

        cipher.init(Cipher.DECRYPT_MODE, secretKeySpec);
		return cipher;
	}
	
	static public byte[] decryptAES(byte[] encryptedData, byte[] keyBytes)
		throws IOException, InvalidKeySpecException, NoSuchAlgorithmException, NoSuchPaddingException,
		InvalidKeyException, IllegalBlockSizeException, BadPaddingException, NoSuchProviderException
	{
		SecretKeySpec secretKeySpec = new SecretKeySpec(keyBytes, "AES");
        Cipher cipher = newCipher(secretKeySpec);          
        byte[] decryptedData = cipher.doFinal(encryptedData);

        return decryptedData;            
	};

	protected String getUser(JSONObject jsonObject) {
		String user = null;

		try
		{
			user = jsonObject.getString("pi");
		}
		catch (Exception exception)
		{
			// Just leave the user as null;
		}
		return user;
	}
	
	protected KeyMaterial getKeyMaterial(int userId, JSONObject jsonObject) throws Exception
	{
		KeyMaterials keyMaterials = new KeyMaterials(new JSONArray(jsonObject.getString("km")));
		
		KeyMaterial keyMaterial = keyMaterials.find(userId);

		if (keyMaterial == null)
			throw new Exception("Event cannot be decrypted.");
		return keyMaterial;
	}
	
	public ProvenanceEntry(int userId, CryptoInfo cryptoInfo, JSONObject jsonObject) throws Exception
	{
		String user = getUser(jsonObject);
		KeyMaterial keyMaterial = getKeyMaterial(userId, jsonObject);
			
		byte[] encryptedEncodedSessionKey = keyMaterial.key;
		byte[] encodedSessionKey = decryptRSA(encryptedEncodedSessionKey, cryptoInfo.rsaPrivateKey.key);
		byte[] sessionKey = Base64.decodeBase64(encodedSessionKey);
		
		String encodedEncryptedProvenanceEntry = jsonObject.getString("e");
		byte[] encryptedProvenanceEntry = Base64.decodeBase64(encodedEncryptedProvenanceEntry);
		byte[] xmlBytes = decryptAES(encryptedProvenanceEntry, sessionKey);

		provenanceEvent = ProvenanceEvent.newProvenanceEvent(user, xmlBytes);
	}
}
/* **************************************************************************/
