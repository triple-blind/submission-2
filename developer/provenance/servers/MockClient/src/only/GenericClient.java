/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockClient/src/only/GenericClient.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.IOException;
import java.net.Socket;
import java.security.GeneralSecurityException;
import java.security.KeyStore;
import java.security.SecureRandom;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManagerFactory;
/* ***************************************************************************
 Class
 ****************************************************************************/
public abstract class GenericClient implements Runnable
{
	protected abstract GenericRequester newRequester(Socket socket);
		
	private static final String ALGORITHM = "SunX509";
	private static final String PROTOCOL = "TLS";
	private static final String KEYSTORE = "JKS";
	
	private KeyStore keyKeyStore;
	private KeyStore trustKeyStore;

	private SecureRandom secureRandom;
	private SSLContext sslContext;
	
	protected ClientParams clientParams;
	
	protected boolean initialized = false;

	public GenericClient(ClientParams clientParams) throws GeneralSecurityException, IOException
	{
		this.clientParams = clientParams;
		secureRandom = nextSecureRandom();
		initialize();
	}
	
	public String getAddressPort() {
		return clientParams.getAddress() + ":" + clientParams.getPort();
	}
	
	private SecureRandom nextSecureRandom()
	{
		secureRandom = new SecureRandom();
		secureRandom.nextInt();
		return secureRandom;
	}

	private void setupKeyStores() throws GeneralSecurityException, IOException
	{
		keyKeyStore = KeyStoreManager.newKeyStore(KEYSTORE, clientParams.getKeyFile(), clientParams.getKeyPassword());
		trustKeyStore = KeyStoreManager.newKeyStore(KEYSTORE, clientParams.getTrustFile(), clientParams.getTrustPassword());
	}
	
	private TrustManagerFactory getTrustManagerFactory(KeyStore trustKeyStore) throws GeneralSecurityException
	{
		TrustManagerFactory tmf = TrustManagerFactory.getInstance(ALGORITHM);
		tmf.init(trustKeyStore); 
		return tmf;
	}
	
	private KeyManagerFactory getKeyManagerFactory(KeyStore keyKeyStore, String password) throws GeneralSecurityException
	{
		KeyManagerFactory kmf = KeyManagerFactory.getInstance(ALGORITHM);
		kmf.init(keyKeyStore, password.toCharArray());
		return kmf;
	}
	
	private SSLContext getSSLContext(KeyManagerFactory kmf, TrustManagerFactory tmf) throws GeneralSecurityException
	{
		SSLContext sslContext = SSLContext.getInstance(PROTOCOL);
		sslContext.init(kmf.getKeyManagers(), tmf.getTrustManagers(), secureRandom);
		return sslContext;
	}
	
	private void setupSSLContext() throws GeneralSecurityException
	{
		TrustManagerFactory tmf = getTrustManagerFactory(trustKeyStore); 
		KeyManagerFactory kmf = getKeyManagerFactory(keyKeyStore, clientParams.getKeyPassword());
		sslContext = getSSLContext(kmf, tmf);
	}

	protected SSLSocket createClientSocket() throws IOException
	{
		SSLSocketFactory sf = sslContext.getSocketFactory();
		SSLSocket ss = (SSLSocket) sf.createSocket(clientParams.getAddress(), clientParams.getPort());		
		return ss;
	}

	protected void initialize() throws GeneralSecurityException, IOException
	{
		if (!initialized)
		{
			setupKeyStores();
			setupSSLContext();
			initialized = true;
		}		
	}
	
	// Since this can be called in a threaded environment itself, it cannot throw an exception.
	public void run()
	{
		try
		{
			SSLSocket socket = createClientSocket();
			
			if (clientParams.getSerial())
				newRequester(socket).run();
			else
				new Thread(newRequester(socket)).start();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
/* **************************************************************************/
