/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/TrustedServer.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.net.Socket;

import secure.provenance.config.Config;
import secure.provenance.config.Configure;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class TrustedServer extends Server
{
	protected static final Config CONFIG = Configure.oldConfig();
	private static final String PROVENANCE_ADDRESS = CONFIG.getProvenanceServerConfig().getAddress();
	private static final int PROVENANCE_PORT = CONFIG.getProvenanceServerConfig().getPort();
	
	private static final int PORT = CONFIG.getTrustedServerConfig().getPort();
	private static final boolean VALIDATE_CLIENT = false;
	private static final String SUBDIR = "keystore/";

	protected static final ServerParams SERVER_PARAMS = new ServerParams(PORT, VALIDATE_CLIENT, SUBDIR + "trusted.jks", "password", SUBDIR + "trusted-trusteds.jks", "password");	

	class TrustedClientService extends ClientService
	{
		public TrustedClientService(Socket clientSocket)
		{
			super(clientSocket);
		}
		
		protected TrustedCommandProcessor newCommandProcessor()
		{
			return new TrustedCommandProcessor(PROVENANCE_ADDRESS, PROVENANCE_PORT);
		}
	}
	
	public TrustedServer()
	{
		super(SERVER_PARAMS);
	}
	
	public TrustedServer(int port)
	{
		super(new ServerParams(port, SERVER_PARAMS));
	}

	public ClientService newClientService(Socket socket)
	{
		return new TrustedClientService(socket);
	}

	public static void main(String args[])
	{
		int port = args.length > 1 ? Integer.parseInt(args[1]) : PORT;		

		new Thread(new TrustedServer(port)).start();
	}
}
/* **************************************************************************/
