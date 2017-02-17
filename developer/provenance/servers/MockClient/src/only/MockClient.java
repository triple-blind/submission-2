/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockClient/src/only/MockClient.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
import java.io.IOException;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.net.Socket;
import java.security.GeneralSecurityException;

import javax.net.ssl.SSLSocket;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class MockClient extends GenericClient
{
	private static final String ADDRESS = "127.0.0.1"; // localhost
	private static final String SUBDIR = "keystore/";	
	
	public static final ClientParams TRUSTED_CLIENT_PARAMS = new ClientParams(ADDRESS, 9987, SUBDIR + "haathi.jks", "password", SUBDIR + "haathi-trusteds.jks", "password");	
	public static final ClientParams PROVENANCE_CLIENT_PARAMS = new ClientParams(ADDRESS, 9988, SUBDIR + "haathi.jks", "password", SUBDIR + "haathi-trusteds.jks", "password");
	
	protected static final ClientParams CLIENT_PARAMS = TRUSTED_CLIENT_PARAMS;

	public static final Request PING_REQUEST = new Request
	(
		"{\n" +
			"\t\"command\": \"PING\",\n" +
			"\t\"payload\": { }\n" +
		"}"	
	);
	
	public static MockClient newMockTrustedClient(Request request) throws GeneralSecurityException, IOException
	{
		return newMockClient(request, true, TRUSTED_CLIENT_PARAMS);
	}
	
	public static MockClient newMockTrustedClient(Request request, String address) throws GeneralSecurityException, IOException
	{
		if (address == null)
			return newMockTrustedClient(request);
		
		ClientParams clientParams = new ClientParams(address, TRUSTED_CLIENT_PARAMS.getPort(), TRUSTED_CLIENT_PARAMS);
		
		return newMockClient(request, true, clientParams);
	}

	public static MockClient newMockTrustedClient(Request request, String address, int port) throws GeneralSecurityException, IOException
	{
		ClientParams clientParams = new ClientParams(address, port, TRUSTED_CLIENT_PARAMS);
		
		return newMockClient(request, true, clientParams);
	}
		
	public static MockClient newMockTrustedClient(Request request, String address, int port, boolean serial) throws GeneralSecurityException, IOException
	{
		ClientParams clientParams = new ClientParams(address, port, TRUSTED_CLIENT_PARAMS);
		
		return newMockClient(request, serial, clientParams);
	}

	public static MockClient newMockProvenanceClient(Request request) throws GeneralSecurityException, IOException
	{
		return newMockClient(request, true, PROVENANCE_CLIENT_PARAMS);		
	}
	
	public static MockClient newMockProvenanceClient(Request request, String address) throws GeneralSecurityException, IOException
	{
		if (address == null)
			return newMockProvenanceClient(request);
		
		ClientParams clientParams = new ClientParams(address, PROVENANCE_CLIENT_PARAMS.getPort(), PROVENANCE_CLIENT_PARAMS);

		return newMockClient(request, true, clientParams);		
	}

	public static MockClient newMockProvenanceClient(Request request, String address, int port) throws GeneralSecurityException, IOException
	{
		ClientParams clientParams = new ClientParams(address, port, PROVENANCE_CLIENT_PARAMS);

		return newMockClient(request, true, clientParams);		
	}

	public static MockClient newMockProvenanceClient(Request request, String address, int port, boolean serial) throws GeneralSecurityException, IOException
	{
		ClientParams clientParams = new ClientParams(address, port, PROVENANCE_CLIENT_PARAMS);

		return newMockClient(request, serial, clientParams);		
	}
	
	public static MockClient newMockClient(Request request, boolean serial, ClientParams clientParams) throws GeneralSecurityException, IOException
	{
		return new MockClient(request, new ClientParams(serial, clientParams));
	}
	
	protected Request request;
	
	class MyRequester extends GenericRequester
	{
		protected Request request;
		
		public MyRequester(Socket socket, Request request)
		{
			super(socket);
			this.request = request;
		}
		
		public String getRequest()
		{
			return request.getRequest();		
		}
		
		public void setMostRecentResponse(String response)
		{
			super.setMostRecentResponse(response);
			request.setResponse(response);
		}
		
		public void setMostRecentException(Exception exception)
		{
			super.setMostRecentException(exception);
			request.setException(exception);
		}
	}
	
	public MockClient(Request request) throws GeneralSecurityException, IOException
	{
		this(request, CLIENT_PARAMS);
	}
	
	public MockClient(String address, int port) throws GeneralSecurityException, IOException
	{
		this(null, address, port);
	}

	public MockClient(Request request, String address, int port) throws GeneralSecurityException, IOException
	{
		this(request, new ClientParams(address, port, CLIENT_PARAMS));
	}
	
	public MockClient(Request request, ClientParams clientParams) throws GeneralSecurityException, IOException
	{
		super(clientParams);
		this.request = request;
	}
	
	public void setRequest(Request request)
	{
		this.request = request;
	}
	
	public GenericRequester newRequester(Socket socket)
	{
		return new MyRequester(socket, request);
	}
	
	// Exceptions are recorded by the requestor
	public void run()
	{
		MyRequester requester = null;
		
		try
		{
			SSLSocket socket = createClientSocket();
			
			requester = (MyRequester) newRequester(socket);
			if (clientParams.getSerial())
				requester.run();
			else
				new Thread(requester).start();
		}
		catch (Exception e)
		{
			if (requester != null)
				requester.setMostRecentException(e);
			else
				e.printStackTrace();
		}
	}
		
	public static void main(String[] args) throws GeneralSecurityException, IOException
	{
		String address = args.length > 1 ? args[0] : CLIENT_PARAMS.getAddress();
		int port = args.length > 2 ? Integer.parseInt(args[2]) : CLIENT_PARAMS.getPort();		
		
		new Thread(new MockClient(PING_REQUEST, address, port)).start();
	}
}
/* **************************************************************************/
