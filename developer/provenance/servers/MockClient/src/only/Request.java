/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockClient/src/only/Request.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/

/* ***************************************************************************
 Class
 ****************************************************************************/
public class Request
{
	protected String request;
	protected String response;
	protected Exception exception;
	
	public Request(String request)
	{
		this.request = request;
		response = null;
	}
	
	public String getRequest()
	{
		return request;
	}
	
	public String getResponse()
	{
		return response;
	}
	
	public void setResponse(String response)
	{
		this.response = response;
	}
	
	public Exception getException()
	{
		return exception;
	}
	
	public void setException(Exception exception)
	{
		this.exception = exception;
	}
}
/* **************************************************************************/
