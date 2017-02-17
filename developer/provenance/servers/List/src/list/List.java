/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/List.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list;
/* ***************************************************************************
Imports
****************************************************************************/
import java.io.UnsupportedEncodingException;

import only.Request;
import only.SimpleMockClient;

import org.apache.commons.codec.binary.Base64;
import org.json.JSONArray;
import org.json.JSONObject;

import list.config.Config;
import list.config.Configure;
import list.entry.CryptoInfo;
import list.entry.Key;
import list.entry.ProvenanceEntry;
import list.event.ProblemProvenanceEvent;
import list.event.ProvenanceEvent;
/* ***************************************************************************
Class
****************************************************************************/
public class List
{
	protected static final Config CONFIG = Configure.oldConfig();
	protected static final String ADDRESS = CONFIG.getTrustedServerConfig().getAddress();
	protected static final int PORT = CONFIG.getTrustedServerConfig().getPort();
	
	private static final int TIMEOUT = 10000;
	
	protected String email;
	protected String pwd;
	
	protected static String convertFromUtf8(String in) throws UnsupportedEncodingException {
		return new String(in.getBytes("ISO_8859-1"), "UTF-8");
	}

	protected static String convertToUtf8(String in) throws UnsupportedEncodingException {
		return new String(in.getBytes("UTF-8"), "ISO_8859-1");
	}
	
	public List(String email, String pwd)
	{
		this.email = email;
		this.pwd = pwd;
	}
	
	protected Request newRequest(String command, JSONObject payload)
	{		
		return new Request(new JSONObject()
			.put("command", command)
			.put("payload", payload)
			.toString());
	}
	
	protected Request newRequest(String command, String emailAddress, String password, JSONObject payload) {
		return new Request(new JSONObject()
			.put("command", command)
			.put("credentials",  new JSONObject()
				.put("emailAddress", email)
				.put("password", pwd))
			.put("payload", payload)
			.toString());
	}
	
	protected void checkPayload(JSONObject response)
	{
		if (!response.getBoolean("result"))
		{
			System.err.println("Error: " + response.getString("payload"));
			System.exit(-1);
		}		
	}
	
	protected JSONObject getPayloadObject(String response)
	{
		JSONObject jsonResponse = new JSONObject(response);
		
		checkPayload(jsonResponse);
		return jsonResponse.getJSONObject("payload");
	}
	
	protected JSONArray getPayloadArray(String response)
	{
		JSONObject jsonResponse = new JSONObject(response);
		
		checkPayload(jsonResponse);
		return jsonResponse.getJSONArray("payload");		
	}
	
	protected String process(SimpleMockClient client, Request request) throws Exception
	{
		return client.process(request, TIMEOUT);
	}
		
	protected String login(SimpleMockClient trustedClient) throws Exception
	{
		JSONObject payload = new JSONObject();
		Request request = newRequest("AUTHENTICATE", email, pwd, payload);
		String response = process(trustedClient, request);
		
		return response;
	}

	protected String listUsers(SimpleMockClient trustedClient, int userId) throws Exception
	{
		JSONObject payload = new JSONObject()
			.put("userTypeId", 1);
		Request request = newRequest("LIST_USERS", email, pwd, payload);
		String response = process(trustedClient, request);

		return response;
	}	
	
	protected String listFiles(SimpleMockClient trustedClient, int userId) throws Exception
	{
		JSONObject payload = new JSONObject()
			.put("userTypeId", 1);
		Request request = newRequest("LIST_DOCUMENTS", email, pwd, payload);
		String response = process(trustedClient, request);

		return response;
	}
	
	protected void println() throws UnsupportedEncodingException
	{
		println("");
	}
	
	protected void println(String text) throws UnsupportedEncodingException
	{
		print(text + "\n");
	}
	
	protected void print(String text) throws UnsupportedEncodingException {
		text = convertToUtf8(text);
//		if (System.console() != null && System.console().writer() != null)
//			System.console().writer().print(text);
//		else
			System.out.print(text);	
	}
	
	protected void listUsers() throws Exception
	{
		SimpleMockClient trustedClient = SimpleMockClient.newTrustedClient(ADDRESS, PORT);
		JSONObject payloadObject = getPayloadObject(login(trustedClient));
		int userId = payloadObject.getInt("userId");
		
		JSONArray payloadArray = getPayloadObject(listUsers(trustedClient, userId)).getJSONArray("users");
		println("email");

		for (int i = 0; i < payloadArray.length(); i++)
		{
			JSONObject user = payloadArray.getJSONObject(i);
			String email = escape(user.getString("emailAddress"));
			
			println(email);			
		}
	}
	
	
	protected void listDocuments() throws Exception
	{
		SimpleMockClient trustedClient = SimpleMockClient.newTrustedClient(ADDRESS, PORT);
		JSONObject payloadObject = getPayloadObject(login(trustedClient));
		int userId = payloadObject.getInt("userId");
		
		JSONArray payloadArray = getPayloadObject(listFiles(trustedClient, userId)).getJSONArray("documents");
		println("docID" + "\t" + "title");

		for (int i = 0; i < payloadArray.length(); i++)
		{
			JSONObject doc = payloadArray.getJSONObject(i);
			int docId = doc.getInt("documentId");
			String name = escape(convertFromUtf8(doc.getString("title")));
			
			println("" + docId + "\t" + name);			
		}
	}
	
	protected String listEvents(SimpleMockClient provenanceClient, String versionId) throws Exception
	{
		JSONObject payload = new JSONObject()
			.put("versionId", versionId);
		Request request = newRequest("GET_AUDIT", payload);
		String response = process(provenanceClient, request);

		return response;
	}

	protected Key newKey(JSONObject jsonObject, String keyName, String keySizeName)
	{
		return new Key(Base64.decodeBase64(jsonObject.getString(keyName)), jsonObject.getInt(keySizeName));
	}
	
	// Avoid tabs, CR, and LF because they are separators in the output format.
	// Avoid single \ because they are now escape characters.
	// However, newer versions of Haathi, most of the data is pre-escaped,
	// so these things should never happen.  This is just in case.
	protected String escape(String text) {
		String result = text
				.replace("\\", "\\\\")
				.replace("\t", "\\t")
				.replace("\r", "\\r")
				.replace("\n", "\\n");
		return result;
	}
	
	protected String getVersionId(SimpleMockClient trustedClient, int userId, int docId) throws Exception
	{
		JSONArray payloadArray = getPayloadObject(listFiles(trustedClient, userId)).getJSONArray("documents");

		for (int i = 0; i < payloadArray.length(); i++)
		{
			JSONObject doc = payloadArray.getJSONObject(i);

			if (docId == doc.getInt("documentId"))
				return doc.getString("versionId");
		}
		return null;
	}

	protected String getUniqueKey(int docId) throws Exception
	{
		SimpleMockClient trustedClient = SimpleMockClient.newTrustedClient(ADDRESS, PORT);
		JSONObject payloadObject = getPayloadObject(login(trustedClient));
		int userId = payloadObject.getInt("userId");
		
		JSONArray payloadArray = getPayloadObject(listFiles(trustedClient, userId)).getJSONArray("documents");

		for (int i = 0; i < payloadArray.length(); i++)
		{
			JSONObject doc = payloadArray.getJSONObject(i);
			
			if (docId != doc.getInt("documentId"))
				continue;
					
			String uniqueKey = doc.getString("uniqueKey");
			return uniqueKey;
		}
		return null;
	}

	protected JSONArray newJSONArray(String sProvenances, byte[] bUniqueKey) throws Exception
	{
		byte[] bEncryptedProvenances = Base64.decodeBase64(sProvenances);
		byte[] bDecryptedProvenances = ProvenanceEntry.decryptAES(bEncryptedProvenances, bUniqueKey);
		// cJSON does not do any conversion to UTF-8, but any strings there should have come from
		// an OUString converted to std::string and this is encoded as UTF-8, so decode as such here.
		String sDecryptedProvenances = new String(bDecryptedProvenances, "UTF-8");
	
		return new JSONArray(sDecryptedProvenances);
	}	
	
	protected void listDocuments(int docId) throws Exception
	{
		String sUniqueKey = getUniqueKey(docId);
		if (sUniqueKey == null)
			return;
			
		byte[] bUniqueKey = Base64.decodeBase64(sUniqueKey);
		SimpleMockClient trustedClient = SimpleMockClient.newTrustedClient(ADDRESS, PORT);
		JSONObject payloadObject;
		
		payloadObject = getPayloadObject(login(trustedClient));
		
		int userId = payloadObject.getInt("userId");
		String address = payloadObject.getString("host");
		int port = payloadObject.getInt("port");
		CryptoInfo cryptoInfo = new CryptoInfo(
				newKey(payloadObject, "privateKey", "privateKeySize"),
				newKey(payloadObject, "publicKey", "publicKeySize"),
				newKey(payloadObject, "sessionKey", "sessionKeySize"));
		String versionId = getVersionId(trustedClient, userId, docId);
		
		SimpleMockClient provenanceClient = SimpleMockClient.newProvenanceClient(address, port);
		payloadObject = getPayloadObject(listEvents(provenanceClient, versionId));
		JSONArray provenanceArrays = payloadObject.getJSONArray("provenances");

		println(
				"docID" + "\t" +
				"time" + "\t" + 
				"email" + "\t" +
				"type" + "\t" +
				"location" + "\t" +
				"data");

		for (int outer = 0; outer < provenanceArrays.length(); outer++) {
			JSONArray provenanceArray = newJSONArray(provenanceArrays.getString(outer), bUniqueKey);		
			
			for (int inner = 0; inner < provenanceArray.length(); inner++)
			{
				ProvenanceEvent provenanceEvent;
				
				try {
					provenanceEvent = new ProvenanceEntry(userId, cryptoInfo, new JSONObject(provenanceArray.getString(inner))).provenanceEvent;
				}
				catch (Exception exception) {
					provenanceEvent = new ProblemProvenanceEvent(null, "", null, null, "[Unknown: " + exception.getMessage() + "]");
				}
	
				print(
						"" + docId + "\t" +
						escape(provenanceEvent.getEventTime()) + "\t" +
						escape(provenanceEvent.getEventUser()) + "\t" +
						escape(provenanceEvent.getEventType()) + "\t" +
						escape(provenanceEvent.getEventLocation()));
				for (String eventData: provenanceEvent.getEventData())
					print("\t" + escape(eventData));
				println();
			}
		}
	}

	protected static void syntax()
	{
		System.err.println("Syntax: java -jar List.jar [-d] email pwd [docID]");
		System.err.println("    or  java -jar List.jar  -u  email pwd\n");
		System.err.println("Use an optional -d for documents.");
		System.err.println("Without docID, a list of docIDs and document names is printed.");		
		System.err.println("Use the docID to print a list of Provenance events for the document.");
		System.err.println("Use -u for a list of users.");
		System.err.println("By the way, this is version 5.");
		System.exit(-1);
	}
	
	protected static int getDocId(String arg)
	{
		int docId = 0;
		
		try
		{
			docId = Integer.parseInt(arg);
		}
		catch (NumberFormatException ex)
		{
			syntax();
		}
		return docId;
	}
	
	public static void main(String[] args)
	{
		try
		{
			switch (args.length) {
				case 2:
					new List(args[0], args[1]).listDocuments();
					break;
				case 3:
					if (args[0].equals("-u"))
						new List(args[1], args[2]).listUsers();
					else
						if (args[0].equals("-d"))
							new List(args[1], args[2]).listDocuments();
						else
							new List(args[0], args[1]).listDocuments(getDocId(args[2]));
					break;
				case 4:
					if (!args[0].equals("-d"))
						syntax();
					new List(args[1], args[2]).listDocuments(getDocId(args[3]));
					break;
				default:
					syntax();
					
			}
		}
		catch (Exception ex)
		{
			ex.printStackTrace();
			System.exit(-1);
		}
	}
}
/* **************************************************************************/
