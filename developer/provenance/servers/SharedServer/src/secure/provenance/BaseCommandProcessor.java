/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/BaseCommandProcessor.java#1 $
*****************************************************************************
Package
****************************************************************************/
package secure.provenance;
/* ***************************************************************************
Imports
****************************************************************************/
import java.io.IOException;

import org.json.JSONObject;
/* ***************************************************************************
Class
****************************************************************************/
public abstract class BaseCommandProcessor
{
	public static final String PING_RESPONSE = BaseDataAccessor.resultString(true, "");
	
	public abstract void process(String clientRequest);

	protected boolean isRequestProcessed = false;
	protected boolean isValidRequest = false;
	protected String processedResult = null;
	protected JSONObject payloadObject = null;	

	public boolean getIsValidRequest()
	{
		return isValidRequest;
	}
	
	public boolean getIsRequestProcessed()
	{
		return isRequestProcessed;
	}
	
	public String getResult() throws IOException
	{
		if (!isValidRequest || !isRequestProcessed)
			throw new IOException("Request was invalid or not processed.");
		return processedResult;
	}
	
	protected boolean payloadHasAll(String[] names)
	{
		if (names == null)
			return true;
		for (String name : names)
			if (!payloadObject.has(name))
				return false;
		return true;
	}

	protected boolean invalidateRequest(String message, Logger logger)
	{
		logger.error("Invalid request: " + message);
		return false;
	}
	
	protected String resultString(boolean result, String message, Logger logger)
	{
		return BaseDataAccessor.resultString(result, message, logger);
	}
	
	protected String resultString(boolean result, String[] payloadNames, Logger logger)
	{
		String message = "Missing one or more of expected argument(s) ";
		
		for (int i = 0;i < payloadNames.length; i++)
		{
			if (i > 0)
				message += ", ";
			message += payloadNames[i];
		}
		message += ".";
		return resultString(result, message, logger);
	}
}
/* **************************************************************************/
