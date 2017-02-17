/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockServerTest/src/test/MockServerTest.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package test;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import static org.junit.Assert.*;

import java.io.IOException;
import java.security.GeneralSecurityException;

import org.junit.Test;

import only.ClientParams;
import only.MockClient;
import only.MockServer;
import only.Request;
import only.ServerParams;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class MockServerTest
{
	@Test
	public void testParallelTrustedServer() throws InterruptedException, GeneralSecurityException, IOException
	{
		ServerParams serverParams = new ServerParams(false, true, MockServer.TRUSTED_SERVER_PARAMS);
		MockServer trustedServer = new MockServer(serverParams);
		Thread serverThread = new Thread(trustedServer);
		serverThread.start();
		
		Request request = MockClient.PING_REQUEST;		
		ClientParams clientParams = new ClientParams(true, MockClient.TRUSTED_CLIENT_PARAMS);
		MockClient trustedClient = new MockClient(request, clientParams);
		Thread clientThread = new Thread(trustedClient);
		clientThread.start();

		try
		{
			serverThread.join();
			clientThread.join();
		}
		catch (InterruptedException e)
		{
			fail("Thread didn't end.");
		}
		
		String response = request.getResponse();
		assertEquals("Ping should return expected response", MockServer.PING_RESPONSE, response);
	}

	@Test
	public void testParallelProvenanceServer() throws InterruptedException, GeneralSecurityException, IOException
	{
		ServerParams serverParams = new ServerParams(false, true, MockServer.PROVENANCE_SERVER_PARAMS);
		MockServer provenanceServer = new MockServer(serverParams);
		Thread serverThread = new Thread(provenanceServer);
		serverThread.start();
		
		Request request = MockClient.PING_REQUEST;
		ClientParams clientParams = new ClientParams(true, MockClient.PROVENANCE_CLIENT_PARAMS);
		MockClient provenanceClient = new MockClient(request, clientParams);
		Thread clientThread = new Thread(provenanceClient);
		clientThread.start();
		
		try
		{
			serverThread.join();
			clientThread.join();
		}
		catch (InterruptedException e)
		{
			fail("Thread didn't end.");
		}
		
		String response = request.getResponse();
		assertEquals("Ping should return expected response", MockServer.PING_RESPONSE, response);
	}

	@Test
	public void testSerialTrustedServer() throws InterruptedException, GeneralSecurityException, IOException
	{
		ServerParams serverParams = new ServerParams(true, true, MockServer.TRUSTED_SERVER_PARAMS);
		MockServer trustedServer = new MockServer(serverParams);
		Thread serverThread = new Thread(trustedServer);
		serverThread.start();
		
		Request request = MockClient.PING_REQUEST;		
		ClientParams clientParams = new ClientParams(true, MockClient.TRUSTED_CLIENT_PARAMS);
		MockClient trustedClient = new MockClient(request, clientParams);
		Thread clientThread = new Thread(trustedClient);
		clientThread.start();
		
		try
		{
			serverThread.join();
			clientThread.join();
		}
		catch (InterruptedException e)
		{
			fail("Thread didn't end.");
		}

		String response = request.getResponse();
		assertEquals("Ping should return expected response", MockServer.PING_RESPONSE, response);
	}

	@Test
	public void testSerialProvenanceServer() throws InterruptedException, GeneralSecurityException, IOException
	{
		ServerParams serverParams = new ServerParams(true, true, MockServer.PROVENANCE_SERVER_PARAMS);
		MockServer provenanceServer = new MockServer(serverParams);
		Thread serverThread = new Thread(provenanceServer);
		serverThread.start();
		
		Request request = MockClient.PING_REQUEST;
		ClientParams clientParams = new ClientParams(true, MockClient.PROVENANCE_CLIENT_PARAMS);
		MockClient provenanceClient = new MockClient(request, clientParams);
		Thread clientThread = new Thread(provenanceClient);
		clientThread.start();
		
		try
		{
			serverThread.join();
			clientThread.join();
		}
		catch (InterruptedException e)
		{
			fail("Thread didn't end.");
		}
		String response = request.getResponse();
		assertEquals("Ping should return expected response", MockServer.PING_RESPONSE, response);
	}
}
/* **************************************************************************/
