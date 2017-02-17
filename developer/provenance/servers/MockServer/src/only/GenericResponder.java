/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockServer/src/only/GenericResponder.java#1 $
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
public abstract class GenericResponder implements Runnable
{
	public abstract String respond(String request);
	
	protected static final int BUFFER_SIZE = 4096;

	private Socket socket;

	public GenericResponder(Socket socket)
	{
		this.socket = socket;
	}
	
	public void run()
	{
	    StringBuilder builder = new StringBuilder();
		char[] cbuf = new char[BUFFER_SIZE];
	    int line;

		try
		{
			InputStream is = socket.getInputStream();
			OutputStream os = socket.getOutputStream();		
			BufferedReader reader = new BufferedReader(new InputStreamReader(is));
			
	    	while ((line = reader.read(cbuf)) != -1 && line != 0)
                if (line < cbuf.length)
                {
                	builder.append(cbuf, 0, line);
                	break;
                }
                else
                	builder.append(cbuf);
		    
	    	String request = builder.toString();
	    	System.out.println("Server request:\n" + request);
	    	String response = respond(request);
	    	System.out.println("Server response:\n" + response);
			os.write(response.getBytes());
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}			
	}
}
/* **************************************************************************/
