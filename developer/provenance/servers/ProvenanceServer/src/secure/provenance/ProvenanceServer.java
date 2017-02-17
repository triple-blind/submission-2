/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/ProvenanceServer/src/secure/provenance/ProvenanceServer.java#1 $
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
public class ProvenanceServer extends Server
{
	protected static final Config CONFIG = Configure.oldConfig();	
	private static int PORT = CONFIG.getProvenanceServerConfig().getPort();
	private static final boolean VALIDATE_CLIENT = false;	
	protected static final String SUBDIR = "keystore/";
	
	protected static ServerParams SERVER_PARAMS = new ServerParams(PORT, VALIDATE_CLIENT, SUBDIR + "provenance.jks", "password", SUBDIR + "provenance-trusteds.jks", "password");	

	class ProvenanceClientService extends ClientService
	{
		public ProvenanceClientService(Socket clientSocket)
		{
			super(clientSocket);
		}
		
		protected ProvenanceCommandProcessor newCommandProcessor()
		{
			return new ProvenanceCommandProcessor();
		}	
	}

	public ProvenanceServer()
	{
		super(SERVER_PARAMS);
	}
	
	public ProvenanceServer(int port)
	{
		super(new ServerParams(port, SERVER_PARAMS));
	}
	
	public ClientService newClientService(Socket socket)
	{
		return new ProvenanceClientService(socket);
	}
	
	static public void main(String args[])
	{
		int port = args.length > 1 ? Integer.parseInt(args[1]) : PORT;
		
		new Thread(new ProvenanceServer(port)).start();
	}
}
/* **************************************************************************/
