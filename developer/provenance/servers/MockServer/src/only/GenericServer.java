/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockServer/src/only/GenericServer.java#1 $
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
import javax.net.ssl.SSLServerSocket;
import javax.net.ssl.SSLServerSocketFactory;
import javax.net.ssl.TrustManagerFactory;
/* ***************************************************************************
 Class
 ****************************************************************************/
public abstract class GenericServer implements Runnable
{
	protected abstract GenericResponder newResponder(Socket socket);

	protected static final String ALGORITHM = "SunX509";
	protected static final String PROTOCOL = "TLS";
	protected static final String KEYSTORE = "JKS";
	
	protected KeyStore keyKeyStore;
	protected KeyStore trustKeyStore;

	protected SecureRandom secureRandom;
	protected SSLContext sslContext;
	
	protected ServerParams serverParams;

	public GenericServer(ServerParams serverParams)
	{
		this.serverParams = serverParams;
		secureRandom = nextSecureRandom();
	}
	
	private SecureRandom nextSecureRandom()
	{
		secureRandom = new SecureRandom();
		secureRandom.nextInt();		
		return secureRandom;
	}
	
	private void setupKeyStores() throws GeneralSecurityException, IOException
	{
		keyKeyStore = KeyStoreManager.newKeyStore(KEYSTORE, serverParams.getKeyFile(), serverParams.getKeyPassword());
		trustKeyStore = KeyStoreManager.newKeyStore(KEYSTORE, serverParams.getTrustFile(), serverParams.getTrustPassword());
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
		KeyManagerFactory kmf = getKeyManagerFactory(keyKeyStore, serverParams.getKeyPassword());
		sslContext = getSSLContext(kmf, tmf);
	}

	private SSLServerSocket createServerSocket() throws IOException
	{
		SSLServerSocketFactory sf = sslContext.getServerSocketFactory();
		SSLServerSocket ss = (SSLServerSocket) sf.createServerSocket(serverParams.getPort());		
		ss.setNeedClientAuth(serverParams.getNeedClientAuth());
		return ss;
	}

	public void run()
	{
		try
		{
			setupKeyStores();
			setupSSLContext();
			SSLServerSocket ss = createServerSocket();

			while (true)
			{
				Socket socket = ss.accept();
				if (serverParams.getSerial())
					newResponder(socket).run();
				else
					new Thread(newResponder(socket)).start();
				if (serverParams.getSingleShot())
					break;
			}
			ss.close();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
/* **************************************************************************/
