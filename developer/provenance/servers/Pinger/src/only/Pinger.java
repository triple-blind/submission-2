/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/Pinger/src/only/Pinger.java#1 $
*****************************************************************************
Package
****************************************************************************/
package only;

import java.io.IOException;
import java.security.GeneralSecurityException;
/* ***************************************************************************
Imports
****************************************************************************/

/* ***************************************************************************
Class
****************************************************************************/
public class Pinger
{
	private static final int TIMEOUT = 5000;

	public static void ping(MockClient client, Request request, String message)
	{
		request.setResponse("");
		
		System.out.println("Pinging " + client.getAddressPort());
		Thread clientThread = new Thread(client);
		
		clientThread.start();
		try
		{
			clientThread.join(TIMEOUT);
			if (clientThread.isAlive())
			{
				clientThread.interrupt();
				throw new Exception("Timeout");
			}
			System.out.println("Command for " + message + " did not cause an exception (at this level).");
		}
		catch (Exception ex)
		{
			System.out.println("Command for " + message + " caused an exception.");
			ex.printStackTrace();
			return;
		}
		
		String response = request.getResponse();
		
		System.out.println(response);
	}
	
	public static void main(String[] args) throws GeneralSecurityException, IOException
	{
		Request request = MockClient.PING_REQUEST;
		MockClient client;
		
		client = MockClient.newMockTrustedClient(request, args.length > 0 ? args[0] : null);
		ping(client, request, "trusted server");

		client = MockClient.newMockProvenanceClient(request, args.length > 0 ? args[0] : null);
		ping(client, request, "provenance server");
	}
}
/* **************************************************************************/
