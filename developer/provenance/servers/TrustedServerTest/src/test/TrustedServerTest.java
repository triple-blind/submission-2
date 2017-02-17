/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServerTest/src/test/TrustedServerTest.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package test;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import static org.junit.Assert.*;

import org.junit.Test;

import only.MockClient;
import only.Request;
import secure.provenance.TrustedCommandProcessor;
import secure.provenance.TrustedServer;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class TrustedServerTest
{
	private static final int TIMEOUT = 10000;

	@Test
	public void testPing() throws Exception
	{
		TrustedServer trustedServer = new TrustedServer();
		Thread serverThread = new Thread(trustedServer);
		
		serverThread.start();
		
		Request request = MockClient.PING_REQUEST;
		MockClient trustedClient = MockClient.newMockTrustedClient(request);
		Thread clientThread = new Thread(trustedClient);
		
		clientThread.start();
		clientThread.join(TIMEOUT);
		if (clientThread.isAlive())
		{
			clientThread.interrupt();
			throw new Exception("Timeout");
		}
		
		String response = request.getResponse();
		
		assertEquals("Ping should return expected response", TrustedCommandProcessor.PING_RESPONSE, response);
		serverThread.interrupt();
	}
}
/* **************************************************************************/
