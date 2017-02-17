/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/ClientService.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
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
public abstract class ClientService implements Runnable
{
	private static Logger logger = Logger.getLogger(ClientService.class);
	protected static final int BUFFER_SIZE = 4096;
	
	private Socket clientSocket;

	public ClientService(Socket clientSocket)
	{
		this.clientSocket = clientSocket;
	}

	protected abstract BaseCommandProcessor newCommandProcessor();
	
	public void run()
	{
	    StringBuilder builder = new StringBuilder();
		char[] cbuf = new char[BUFFER_SIZE];
	    int line;

		logger.info("Accepted Client from Address - " + clientSocket.getInetAddress() );

		try
		{
			InputStream is = clientSocket.getInputStream();
			BufferedReader reader = new BufferedReader(new InputStreamReader(is));
			
	    	while ((line = reader.read(cbuf)) != -1 && line != 0)
	    	{
                if (line < cbuf.length)
                {
                	builder.append(cbuf, 0, line);
                	break;
                }
                else
                	builder.append(cbuf);
            }
		    
	    	BaseCommandProcessor commandProcessor = newCommandProcessor();
	    	commandProcessor.process(builder.toString());
	    	
			OutputStream os = clientSocket.getOutputStream();		

			if (commandProcessor.getIsValidRequest() && commandProcessor.getIsRequestProcessed())
			{
				// Send the response back to the requester
				os.write(commandProcessor.getResult().getBytes());
			}
		}
		catch (IOException e)
		{
			logger.catchException(e);
		}			
	}
}
/* **************************************************************************/
