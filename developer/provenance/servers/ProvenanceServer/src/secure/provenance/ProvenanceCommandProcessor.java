/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/ProvenanceServer/src/secure/provenance/ProvenanceCommandProcessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.util.HashMap;
import java.util.Map;

import org.json.JSONObject;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class ProvenanceCommandProcessor extends BaseCommandProcessor
{
	protected static class ProvenanceCommandSpec
	{
		public ProvenanceRequestType provenanceRequestType;
		public String[] payloadNames;
		
		public ProvenanceCommandSpec(ProvenanceRequestType provenanceRequestType, String[] payloadNames)
		{
			this.provenanceRequestType = provenanceRequestType;
			this.payloadNames = payloadNames;
		}
	}
	
	protected static final Logger LOGGER = Logger.getLogger(ProvenanceCommandProcessor.class);
	protected static final Map<String, ProvenanceCommandSpec> COMMAND_MAP = new HashMap<String, ProvenanceCommandSpec>()
	{
		private static final long serialVersionUID = 1L;
	
		{
			put("PING",        new ProvenanceCommandSpec(ProvenanceRequestType.PING,                   null));

			put("GET_EDIT",    new ProvenanceCommandSpec(ProvenanceRequestType.GET_DOCUMENT_FOR_EDIT,     new String[]{"versionId"}));
			put("PUT_EDIT",    new ProvenanceCommandSpec(ProvenanceRequestType.PUT_DOCUMENT_FOR_EDIT,     new String[]{"versionId", "prevVersionId", "document", "provenance"}));

			put("GET_AUDIT",   new ProvenanceCommandSpec(ProvenanceRequestType.GET_DOCUMENT_FOR_AUDIT,    new String[]{"versionId"}));
		}
	};

	private IProvenanceDataAccessor provenanceAccessor = null;
	private ProvenanceRequestType requestType = ProvenanceRequestType.UNKNOWN;
	protected String[] payloadNames = null;	
	
	protected IProvenanceDataAccessor getDataAccessor()
	{
		if (provenanceAccessor == null)
			provenanceAccessor = new ProvenanceLocalSQLiteDataAccessor();
			// provenanceAccessor = new EC2ProvenanceAccessor();
		return provenanceAccessor;
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
			LOGGER.info("Valid request for \"" + requestType.toString());
			switch (requestType)
			{
				case PING:
					processedResult = PING_RESPONSE;
					break;

				case GET_DOCUMENT_FOR_EDIT:
					processedResult = getDataAccessor().getDocumentForEdit(payloadObject.getString("versionId"));
					break;
				case PUT_DOCUMENT_FOR_EDIT:
					{ 
						String versionId = payloadObject.getString("versionId");
						String prevVersionId = payloadObject.getString("prevVersionId");
						String document = payloadObject.getString("document");
						String provenance = payloadObject.getString("provenance");
						
						processedResult = getDataAccessor().putDocumentForEdit(versionId, prevVersionId, document, provenance);
					}				
					break;
				case GET_DOCUMENT_FOR_AUDIT:
					processedResult = getDataAccessor().getDocumentForAudit(payloadObject.getString("versionId"));
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
		System.out.println("Reading json..");
		JSONObject object = new JSONObject(clientRequest);
		String command = null;
		
		isValidRequest = true;
		if (isValidRequest && object.has("command"))
			command = object.getString("command");
		else
			isValidRequest = invalidateRequest("Missing command", LOGGER);
		
		if (isValidRequest && COMMAND_MAP.containsKey(command.toUpperCase()))
			requestType = COMMAND_MAP.get(command.toUpperCase()).provenanceRequestType;
		else
			isValidRequest = invalidateRequest("Unknown command", LOGGER);
		
		if (isValidRequest && object.has("payload"))
			payloadObject = object.getJSONObject("payload");
		else
			isValidRequest = invalidateRequest("Missing payload", LOGGER);

		if (isValidRequest && payloadHasAll(COMMAND_MAP.get(command.toUpperCase()).payloadNames))
			payloadNames = COMMAND_MAP.get(command.toUpperCase()).payloadNames;
		else
			isValidRequest = invalidateRequest("Faulty payload", LOGGER);		
    }
}
/* **************************************************************************/
