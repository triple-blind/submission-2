/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockServer/src/only/MockServer.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.net.Socket;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class MockServer extends GenericServer
{
	private static final String SUBDIR = "keystore/";
	
	public static final ServerParams TRUSTED_SERVER_PARAMS = new ServerParams(9987, false, SUBDIR + "trusted.jks", "password", SUBDIR + "trusted-trusteds.jks", "password");	
	public static final ServerParams PROVENANCE_SERVER_PARAMS = new ServerParams(9988, true, SUBDIR + "provenance.jks", "password", SUBDIR + "provenance-trusteds.jks", "password");
	
	protected static final ServerParams SERVER_PARAMS = TRUSTED_SERVER_PARAMS;
	//protected static final ServerParams SERVER_PARAMS = PROVENANCE_SERVER_PARAMS;
	
	public static final String PING_RESPONSE = "" +
		"{\n" +
			"\t\"result\": true,\n" +
			"\t\"payload\": \"\"\n" +
		"}";
	
	public static MockServer newMockTrustedServer()
	{
		return newMockServer(false, TRUSTED_SERVER_PARAMS);
	}
	
	public static MockServer newMockProvenanceServer()
	{
		return newMockServer(false, PROVENANCE_SERVER_PARAMS);
	}

	public static MockServer newMockServer(boolean serial, ServerParams serverParams)
	{
		return new MockServer(new ServerParams(serial, serverParams));
	}
	
	class MyResponder extends GenericResponder
	{
		public MyResponder(Socket socket)
		{
			super(socket);
		}
		
		public String respond(String request)
		{
			return PING_RESPONSE;
		}
	}
	
	public MockServer()
	{
		this(SERVER_PARAMS);
	}
	
	public MockServer(int port)
	{
		this(new ServerParams(port, SERVER_PARAMS));
	}

	public MockServer(ServerParams serverParams)
	{
		super(serverParams);
	}
	
	public GenericResponder newResponder(Socket socket)
	{
		return new MyResponder(socket);
	}

	public static void main (String args[])
	{
		int port = args.length > 1 ? Integer.parseInt(args[1]) : SERVER_PARAMS.getPort();		

		new Thread(new MockServer(port)).start();
	}
}
/* **************************************************************************/
