/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/Server.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
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
public abstract class Server implements Runnable
{
	protected abstract ClientService newClientService(Socket socket);

	private static final String ALGORITHM = "SunX509"; // Name of encryption algorithm
	private static final String PROTOCOL = "TLS"; // Name of socket protocol to use
	private static final String FORMAT = "JKS";
	
   	private static Logger logger = Logger.getLogger(Server.class);

   	private KeyStore keyKeyStore;
	private KeyStore trustKeyStore;
	
	private SecureRandom secureRandom;
	private SSLContext sslContext;
	
	protected ServerParams serverParams;
	
	public Server(ServerParams serverParams)
	{
		this.serverParams = serverParams;
		secureRandom = nextSecureRandom();
	}
	
	private SecureRandom nextSecureRandom()
	{
		SecureRandom secureRandom = new SecureRandom();
		secureRandom.nextInt();		
		return secureRandom;
	}
	
	private KeyStore setupKeyStore(String filename, String password) throws GeneralSecurityException, IOException
	{
		return KeyStoreManager.newKeyStore(FORMAT, filename, password);
	}
	
	private void setupKeyStores() throws GeneralSecurityException, IOException
	{
		keyKeyStore = setupKeyStore(serverParams.getKeyFile(), serverParams.getKeyPassword());
		trustKeyStore = setupKeyStore(serverParams.getTrustFile(), serverParams.getTrustPassword());
		logger.trace();
	}
	
	private KeyManagerFactory getKeyManagerFactory(KeyStore keyStore, String password) throws GeneralSecurityException
	{
		KeyManagerFactory kmf = KeyManagerFactory.getInstance(ALGORITHM);
		kmf.init(keyStore, password.toCharArray());
		logger.trace();
		return kmf;
	}
	
	private TrustManagerFactory getTrustManagerFactory(KeyStore keyStore) throws GeneralSecurityException
	{
		TrustManagerFactory tmf = TrustManagerFactory.getInstance(ALGORITHM);
		tmf.init(keyStore); 
		logger.trace();
		return tmf;
	}
	
	private SSLContext getSSLContext(KeyManagerFactory kmf, TrustManagerFactory tmf) throws GeneralSecurityException
	{
		SSLContext sslContext = SSLContext.getInstance(PROTOCOL);
		sslContext.init(kmf.getKeyManagers(), tmf.getTrustManagers(), secureRandom);
		logger.trace();
		return sslContext;
	}
	
	private void setupSSLContext()
	{
		try
		{
			KeyManagerFactory kmf = getKeyManagerFactory(keyKeyStore, serverParams.getKeyPassword());
			TrustManagerFactory tmf = getTrustManagerFactory(trustKeyStore); 
			sslContext = getSSLContext(kmf, tmf);
		}
		catch (Exception e)
		{
			logger.catchException(e);
		}
		logger.trace();
	}

	private SSLServerSocket createServerSocket(int port) throws IOException
	{
		SSLServerSocketFactory sf = sslContext.getServerSocketFactory();
		SSLServerSocket ss = (SSLServerSocket) sf.createServerSocket(port);		
		// "In most modes, SSL provides peer authentication.  Servers are usually authenticated, and clients
		// may be authenticated as requested by servers."
		// This server does not require client authentication.
		ss.setNeedClientAuth(serverParams.getNeedClientAuth());
		logger.info("Listening on port " + port + "....");
		return ss;
	}
	
	/**
	 * Background thread: accept new connections
	 */
	public void run()
	{
		try
		{
			setupKeyStores();
			setupSSLContext();
			SSLServerSocket ss = createServerSocket(serverParams.getPort());

			while (true)
			{
				Socket socket = ss.accept();
				logger.info("Got connection from " + socket.getLocalAddress());
				new Thread(newClientService(socket)).start();
			}
		}
		catch (Exception e)
		{
			logger.catchException(e);
		}
	}
}
/* **************************************************************************/
