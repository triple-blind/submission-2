/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/TrustedCommandProcessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONObject;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class TrustedCommandProcessor extends BaseCommandProcessor
{
	protected static class TrustedCommandSpec
	{
		public TrustedRequestType trustedRequestType;
		public String[] payloadNames;
		
		public TrustedCommandSpec(TrustedRequestType trustedRequestType, String[] payloadNames)
		{
			this.trustedRequestType = trustedRequestType;
			this.payloadNames = payloadNames;
		}
	}
	
	protected static final Logger LOGGER = Logger.getLogger(TrustedCommandProcessor.class);
	protected static final Map<String, TrustedCommandSpec> COMMAND_MAP = new HashMap<String, TrustedCommandSpec>()
	{
		private static final long serialVersionUID = 1L;

		{
			put("PING",                    new TrustedCommandSpec(TrustedRequestType.PING,                    null));

			put("AUTHENTICATE",            new TrustedCommandSpec(TrustedRequestType.AUTHENTICATE,            null));
			put("REFRESH",                 new TrustedCommandSpec(TrustedRequestType.REFRESH,                 null));

			put("CREATE_USER",             new TrustedCommandSpec(TrustedRequestType.CREATE_USER,             new String[]{"emailAddress", "firstName", "lastName", "userTypeId", "password"}));
			put("LIST_USERS",              new TrustedCommandSpec(TrustedRequestType.LIST_USERS,              new String[]{"userTypeId"})); // must be admin

			put("GET_PUBLIC_KEYS",         new TrustedCommandSpec(TrustedRequestType.GET_PUBLIC_KEYS,         new String[]{"userIds"})); // must be admin
			put("GET_PUBLIC_KEY_BY_EMAIL", new TrustedCommandSpec(TrustedRequestType.GET_PUBLIC_KEY_BY_EMAIL, new String[]{"emailAddress"})); // must be admin

			put("CREATE_DOCUMENT",         new TrustedCommandSpec(TrustedRequestType.CREATE_DOCUMENT,         new String[]{"title"}));
			put("UPDATE_DOCUMENT",         new TrustedCommandSpec(TrustedRequestType.UPDATE_DOCUMENT,         new String[]{"round", "documentId", "versionId" }));
			put("ACCESS_DOCUMENT",         new TrustedCommandSpec(TrustedRequestType.ACCESS_DOCUMENT,         new String[]{"round", "documentId", "versionId", "accessTypeId" }));			
			put("LIST_DOCUMENTS",          new TrustedCommandSpec(TrustedRequestType.LIST_DOCUMENTS,          new String[]{"userTypeId"}));

			put("ADD_EDITORS",             new TrustedCommandSpec(TrustedRequestType.ADD_EDITORS,             new String[]{"userIds", "documentId"}));
			put("SUB_EDITORS",             new TrustedCommandSpec(TrustedRequestType.SUB_EDITORS,             new String[]{"userIds", "documentId"}));

			put("ADD_AUDITORS",            new TrustedCommandSpec(TrustedRequestType.ADD_AUDITORS,            new String[]{"userIds", "documentId"}));
			put("SUB_AUDITORS",            new TrustedCommandSpec(TrustedRequestType.SUB_AUDITORS,            new String[]{"userIds", "documentId"}));

			put("GET_DOCUMENT_EDITORS",    new TrustedCommandSpec(TrustedRequestType.GET_DOCUMENT_EDITORS,    new String[]{"documentId"}));
			put("GET_DOCUMENT_AUDITORS",   new TrustedCommandSpec(TrustedRequestType.GET_DOCUMENT_AUDITORS,   new String[]{"documentId"}));
			put("GET_DOCUMENT_HISTORY",    new TrustedCommandSpec(TrustedRequestType.GET_DOCUMENT_HISTORY,    new String[]{"documentId"}));

			put("GET_ALL_USERS",           new TrustedCommandSpec(TrustedRequestType.GET_ALL_USERS,           null));
			put("GET_ALL_DOCUMENTS",       new TrustedCommandSpec(TrustedRequestType.GET_ALL_DOCUMENTS,       null));
		}
	};

	protected ITrustedDataAccessor provenanceAccessor = null;
	protected TrustedRequestType requestType = TrustedRequestType.UNKNOWN;
	protected AuthenticationCredentials credentials = null;
	protected String[] payloadNames = null;
	
	protected String provenanceAddress;
	protected int provenancePort;
	
	public TrustedCommandProcessor(String provenanceAddress, int provenancePort) {
		this.provenanceAddress = provenanceAddress;
		this.provenancePort = provenancePort;
	}
	
	protected ITrustedDataAccessor getDataAccessor() throws IOException
	{
		// Turn this into lazy initialization so don't unnecessarily pull in AMS classes unless necessary
		if (provenanceAccessor == null)
			provenanceAccessor = new TrustedLocalSQLiteDataAccessor();
			// provenanceAccessor = new EC2DynamoDataAccessor();
		return provenanceAccessor;
	}
	
	protected List<Integer> idsFromJSONArray(JSONArray jsonArray)
	{
		List<Integer> ids = new ArrayList<Integer>();

		for (int i = 0; i < jsonArray.length(); i++)
			ids.add(jsonArray.getInt(i));
		return ids;
	}
	
	public void process(String clientRequest)
	{
		validateRequest(clientRequest);
		if (!isValidRequest)
		{
			LOGGER.info("Invalid command or payload: " + clientRequest);
			processedResult = resultString(false, "Invalid command or payload", LOGGER);
			return;
		}
		
		try
		{
			LOGGER.info("Valid request for \"" + requestType.toString() +  "\" from " + (credentials != null ? ("\"" + credentials.getEmailAddress() + "\"") : "?"));
			switch (requestType)
			{
				case PING:
					processedResult = PING_RESPONSE;
					break;

				case AUTHENTICATE:
					processedResult = getDataAccessor().authenticate(credentials, provenanceAddress, provenancePort);					
					break;
				case REFRESH:
					processedResult = getDataAccessor().refresh(credentials);
					break;

				case CREATE_USER:
					{
						String emailAddress = payloadObject.getString("emailAddress");
						String firstName = payloadObject.getString("firstName");
						String lastName = payloadObject.getString("lastName");
						UserTypeId userTypeId = UserTypeId.fromInt(payloadObject.getInt("userTypeId"));						
						String password = payloadObject.getString("password");
	
						processedResult = getDataAccessor().createUser(credentials, emailAddress, firstName, lastName, userTypeId, password);
					}
					break;
				case LIST_USERS:
					{
						UserTypeId userTypeId = UserTypeId.fromInt(payloadObject.getInt("userTypeId"));
	
						if (userTypeId == UserTypeId.UNKNOWN)
							processedResult = resultString(false, "Unknown userTypeId", LOGGER);
						else
							processedResult = getDataAccessor().listUsers(credentials,userTypeId);
					}
					break;

				case GET_PUBLIC_KEYS:
					{ 
						boolean auditorsOnly = payloadObject.has("auditorsOnly") ? payloadObject.getBoolean("auditorsOnly") : false;
						List<Integer> userIds = idsFromJSONArray(payloadObject.getJSONArray("userIds"));
					
						if (userIds.size() > 0)				
							processedResult = getDataAccessor().getPublicKeys(credentials, userIds, auditorsOnly);
						else
							processedResult = resultString(false, "Missing userIds", LOGGER);
					}
					break;
				case GET_PUBLIC_KEY_BY_EMAIL:
					processedResult = getDataAccessor().getPublicKeyByEmail(credentials, payloadObject.getString("emailAddress"));
					break;
				
				
				case CREATE_DOCUMENT:
					processedResult = getDataAccessor().createDocument(credentials, payloadObject.getString("title"));
					break;
				case UPDATE_DOCUMENT:
					{
						Integer round = payloadObject.getInt("round");
						Integer documentId = payloadObject.getInt("documentId");
						String versionId = payloadObject.getString("versionId");
						String prevVersionId = round != 0 ? payloadObject.getString("prevVersionId") : null;
						
						processedResult = getDataAccessor().updateDocument(credentials, round, documentId, versionId, prevVersionId);
					}
					break;
				case ACCESS_DOCUMENT:
					{
						Integer round = payloadObject.getInt("round");
						Integer documentId = payloadObject.getInt("documentId");
						String versionId = payloadObject.getString("versionId");
						AccessTypeId accessTypeId = AccessTypeId.fromInt(payloadObject.getInt("accessTypeId"));
						
						processedResult = getDataAccessor().accessDocument(credentials, round, documentId, versionId, accessTypeId);
					}
					break;
				case LIST_DOCUMENTS:
					{
						UserTypeId userTypeId = UserTypeId.fromInt(payloadObject.getInt("userTypeId"));

						if (userTypeId == UserTypeId.UNKNOWN)
							processedResult = resultString(false, "Unknown userTypeId", LOGGER);
						else
							processedResult = getDataAccessor().listDocuments(credentials, userTypeId);
					}
					break;

				case ADD_EDITORS:
					{ 
						Integer documentId = payloadObject.getInt("documentId");
						List<Integer> userIds = idsFromJSONArray(payloadObject.getJSONArray("userIds"));						

						if (userIds.size() > 0)
							processedResult = getDataAccessor().addEditors(credentials, documentId, userIds);
						else
							processedResult = resultString(false, "Missing userIds", LOGGER);						
					}
					break;
				case SUB_EDITORS:
					{ 
						Integer documentId = payloadObject.getInt("documentId");
						List<Integer> userIds = idsFromJSONArray(payloadObject.getJSONArray("userIds"));						

						if (userIds.size() > 0)
							processedResult = getDataAccessor().subEditors(credentials, documentId, userIds);
						else
							processedResult = resultString(false, "Missing userIds", LOGGER);												
					}
					break;

				case ADD_AUDITORS:
					{ 
						Integer documentId = payloadObject.getInt("documentId");
						List<Integer> userIds = idsFromJSONArray(payloadObject.getJSONArray("userIds"));
						
						if (userIds.size() > 0)				
							processedResult = getDataAccessor().addAuditors(credentials, documentId, userIds);
						else
							processedResult = resultString(false, "Missing userIds", LOGGER);
					}
					break;
				case SUB_AUDITORS:
					{ 
						Integer documentId = payloadObject.getInt("documentId");
						List<Integer> userIds = idsFromJSONArray(payloadObject.getJSONArray("userIds"));
						
						if (userIds.size() > 0)				
							processedResult = getDataAccessor().subAuditors(credentials, documentId, userIds);
						else
							processedResult = resultString(false, "Missing userIds", LOGGER);						
					}
					break;
	
				case GET_DOCUMENT_AUDITORS:
					processedResult = getDataAccessor().getDocumentAuditors(credentials, payloadObject.getInt("documentId"));
					break;
				case GET_DOCUMENT_EDITORS:
					processedResult = getDataAccessor().getDocumentEditors(credentials, payloadObject.getInt("documentId"));
					break;
				case GET_DOCUMENT_HISTORY:
					processedResult = getDataAccessor().getDocumentHistory(credentials, payloadObject.getInt("documentId"));
					break;				
					
				case GET_ALL_USERS:
					processedResult  = getDataAccessor().getAllUsers(credentials);
					break;
				case GET_ALL_DOCUMENTS:
					processedResult = getDataAccessor().getAllDocuments(credentials);
					break;
				
				case UNKNOWN:
					processedResult = resultString(false, "Unknown request", LOGGER);
					break;
			}			
			isRequestProcessed = true;
		}
		catch (Exception ex)
		{
			LOGGER.catchException(ex);
		}
	}
	
	private void validateRequest(String clientRequest)
	{
		JSONObject object =  new JSONObject(clientRequest);
		String command = null;
		
		isValidRequest = true;
		if (isValidRequest && object.has("command"))
			command = object.getString("command");
		else
			isValidRequest = invalidateRequest("Missing command", LOGGER);

		if (isValidRequest && COMMAND_MAP.containsKey(command.toUpperCase()))
			requestType = COMMAND_MAP.get(command.toUpperCase()).trustedRequestType;
		else
			isValidRequest = invalidateRequest("Unknown command", LOGGER);
				
		if (isValidRequest && object.has("payload"))
			payloadObject = object.getJSONObject("payload");
		else
			isValidRequest = invalidateRequest("Missing payload", LOGGER);

		if (isValidRequest && object.has("credentials"))
			credentials = new AuthenticationCredentials(object.getJSONObject("credentials"));
		else
			credentials = null; // Not all commands require credentials, so don't set isValidRequest

		if (isValidRequest && payloadHasAll(COMMAND_MAP.get(command.toUpperCase()).payloadNames))
			payloadNames = COMMAND_MAP.get(command.toUpperCase()).payloadNames;
		else
			isValidRequest = invalidateRequest("Faulty payload", LOGGER);		
    }
}
/* **************************************************************************/
