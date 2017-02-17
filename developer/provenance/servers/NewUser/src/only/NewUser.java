/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/NewUser/src/only/NewUser.java#1 $
*****************************************************************************
Package
****************************************************************************/
package only;
/* ***************************************************************************
Imports
****************************************************************************/
import java.io.IOException;
import java.security.GeneralSecurityException;

import only.MockClient;
import only.Request;

import org.json.JSONObject;

import secure.provenance.config.Config;
import secure.provenance.config.Configure;
/* ***************************************************************************
Class
****************************************************************************/
public class NewUser
{
	protected static final Config CONFIG = Configure.oldConfig();
	protected static final String DEVELOPER_EMAIL_ADDRESS = CONFIG.getDeveloperConfig().getEmailAddress();
	protected static final String DEVELOPER_PASSWORD = CONFIG.getDeveloperConfig().getPassword();
	
	private static final int TIMEOUT = 5000;
	private static final String ADDRESS = "127.0.0.1"; // use localhost only
	
	protected static void createUser(String emailAddress, String firstName, String lastName, int userTypeID, String tmpPassword) throws GeneralSecurityException, IOException {
		JSONObject payload = new JSONObject()
			.put("username", DEVELOPER_EMAIL_ADDRESS)
			.put("password", DEVELOPER_PASSWORD)
			.put("EmailAddress", emailAddress)
			.put("FirstName",  firstName)
			.put("LastName", lastName)
			.put("UserTypeID", userTypeID)
			.put("Password", tmpPassword);
		
		Request request = new Request(new JSONObject()
			.put("command", "CREATE_USER")
			.put("payload", payload)
			.toString());
		
		MockClient trustedClient = MockClient.newMockTrustedClient(request, ADDRESS);
		Thread clientThread = new Thread(trustedClient);

		clientThread.start();
		
		try
		{
			clientThread.join(TIMEOUT);
			if (clientThread.isAlive())
			{
				clientThread.interrupt();
				throw new Exception("Timeout");
			}
			
		}
		catch (Exception ex)
		{
			ex.printStackTrace();
			return;
		}
		
		String response = request.getResponse();
		System.out.println(response);
	}
	
	protected static void syntax()
	{
		System.err.println("Syntax: java -jar NewUser.jar first last email password editor|auditor");		
	}
	
	public static void main(String[] args) throws GeneralSecurityException, IOException
	{
		if (args.length != 5)
		{
			syntax();
			return;
		}
		String first = args[0];
		String last = args[1];
		String email = args[2];
		String password = args[3];
		String type = args[4];
		
		if (!type.equals("editor") && !type.equals("auditor"))
		{
			syntax();
			return;
		}
		
		createUser(email, first, last, type.equals("editor") ? 1 : 2, password);
	}
}
/* **************************************************************************/
