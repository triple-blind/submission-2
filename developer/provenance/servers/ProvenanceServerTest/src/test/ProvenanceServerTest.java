/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/ProvenanceServerTest/src/test/ProvenanceServerTest.java#1 $
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
import secure.provenance.ProvenanceCommandProcessor;
import secure.provenance.ProvenanceServer;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class ProvenanceServerTest
{
	private static final int TIMEOUT = 10000;

	@Test
	public void testPing() throws Exception
	{
		ProvenanceServer provenanceServer = new ProvenanceServer();
		Thread serverThread = new Thread(provenanceServer);
		
		serverThread.start();
		
		Request request = MockClient.PING_REQUEST;
		MockClient provenanceClient = MockClient.newMockProvenanceClient(request);
		Thread clientThread = new Thread(provenanceClient);
		
		clientThread.start();
		clientThread.join(TIMEOUT);
		if (clientThread.isAlive())
		{
			clientThread.interrupt();
			throw new Exception("Timeout");
		}
		
		String response = request.getResponse();
		
		assertEquals("Ping should return expected response", ProvenanceCommandProcessor.PING_RESPONSE, response);
		serverThread.interrupt();
	}
}
/* **************************************************************************/
