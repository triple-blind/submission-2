/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockClient/src/only/GenericRequester.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
/* ***************************************************************************
 Class
 ****************************************************************************/
public abstract class GenericRequester implements Runnable
{
	public abstract String getRequest();
	
	protected static final int BUFFER_SIZE = 4096;

	private Socket socket;
	private String mostRecentResponse;
	private Exception mostRecentException;
	
	public GenericRequester(Socket socket)
	{
		this.socket = socket;
		mostRecentResponse = null;
	}
	
	public String getMostRecentResponse()
	{
		return mostRecentResponse;
	}
	
	public void setMostRecentResponse(String mostRecentResponse)
	{
		this.mostRecentResponse = mostRecentResponse;
	}

	public Exception getMostRecentException()
	{
		return mostRecentException;
	}
	
	public void setMostRecentException(Exception mostRecentException)
	{
		this.mostRecentException = mostRecentException;
	}
	
	public void run()
	{
		StringBuilder builder = new StringBuilder();
		char[] cbuf = new char[BUFFER_SIZE];
		int line;

		setMostRecentResponse(null);
		setMostRecentException(null);

		try
		{
			InputStream is = socket.getInputStream();
			OutputStream os = socket.getOutputStream();		
			BufferedReader reader = new BufferedReader(new InputStreamReader(is));
	    	
			String request = getRequest();
//			System.out.println("Client request:\n" + request);
			os.write(request.getBytes());
			
			while ((line = reader.read(cbuf)) != -1 && line != 0)
		        if (line < cbuf.length)
		        {
		        	builder.append(cbuf, 0, line);
		        	break;
		        }
		        else
		        	builder.append(cbuf);
			
			String response = builder.toString();
			setMostRecentResponse(response);
//			System.out.println("Client response:\n" + response);	
		}
		catch (IOException e)
		{
			setMostRecentException(e);
			e.printStackTrace();
		}			
	}
}
/* **************************************************************************/
