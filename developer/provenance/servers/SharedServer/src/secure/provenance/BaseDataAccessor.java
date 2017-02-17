/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/BaseDataAccessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import org.apache.commons.codec.binary.Base64;

import org.json.JSONArray;
import org.json.JSONObject;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class BaseDataAccessor
{
	protected static final String TRUE_RESULT = newResultString(true);
	protected static final String FALSE_RESULT = newResultString(false);
	
	protected static DateFormat DATE_FORMAT = new DateFormat();
	
	protected static String newResultString(boolean result)
	{
		return new JSONObject()
			.put("result", result)
			.put("payload", (result ? "success" : "failure"))
			.toString();
	}
	
	public static String resultString(boolean result)
	{
		return result ? TRUE_RESULT : FALSE_RESULT; 
	}

	public static String resultString(boolean result, String message)
	{
		return new JSONObject()
				.put("result",  result)
				.put("payload",  message)
				.toString();
	}
	
	public static String resultString(boolean result, String message, Logger logger)
	{
		if (!result)
			logger.warn(message);
		return resultString(result, message);
	}
	
	public static String resultString(JSONArray resultSet)
	{
		return new JSONObject()
				// kwa: This is for backwards compatibility but should soon be removed
				//.put("result", resultSet.length() > 0 ? true : false)
				//.put("payload", resultSet.length() > 0 ? resultSet : JSONObject.NULL)
				.put("result", true)
				.put("payload", resultSet)
				.toString();
	}
	
	protected static String catchException(Exception exception, Logger logger, boolean stackTrace)
	{
		String message;
		
		if (logger != null) {
			logger.catchException(exception, stackTrace);
			message = logger.getMethodName(exception);
		}
		else
			message = "unlogged function";
		return resultString(false, "Exception encountered in " + message);
	}
	
	public static String resultString(boolean result, JSONObject payload)
	{
		return new JSONObject()
				.put("result", result)
				.put("payload", payload)
				.toString();					
	}

	public static String today()
	{
		return DATE_FORMAT.today();
	}
	
	protected static String asBase64(byte buf[])
	{
		if (buf == null)
			return "";
		return new String(Base64.encodeBase64(buf));
	}	
}
/* **************************************************************************/
