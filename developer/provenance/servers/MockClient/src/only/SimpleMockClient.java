/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockClient/src/only/SimpleMockClient.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.IOException;
import java.security.GeneralSecurityException;

import only.MockClient;
import only.Request;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class SimpleMockClient
{
	protected MockClient mockClient;
	
	static public SimpleMockClient newProvenanceClient(String address, int port) throws GeneralSecurityException, IOException
	{
		MockClient mockClient = MockClient.newMockProvenanceClient(null, address, port, true);
		
		return new SimpleMockClient(mockClient);
	}
	
	static public SimpleMockClient newTrustedClient(String address, int port) throws GeneralSecurityException, IOException
	{
		MockClient mockClient = MockClient.newMockTrustedClient(null, address, port, true);
		
		return new SimpleMockClient(mockClient);		
	}

	SimpleMockClient(MockClient mockClient)
	{
		this.mockClient = mockClient;
	}
	
	public String process(Request request, int timeout) throws Exception
	{
		mockClient.setRequest(request);

		Thread clientThread = new Thread(mockClient);
		
		clientThread.start();
		
		try
		{
			clientThread.join(timeout);
			if (clientThread.isAlive())
			{
				clientThread.interrupt();
				throw new Exception("Timeout");
			}
		}
		catch (Exception ex)
		{
			ex.printStackTrace();
			System.exit(-1);
		}
		
		Exception exception = request.getException();
		
		if (exception != null)
		{
			exception.printStackTrace();
			System.exit(-1);
		}
			
		String response = request.getResponse();
		
		return response;
	}	
}
/* **************************************************************************/
