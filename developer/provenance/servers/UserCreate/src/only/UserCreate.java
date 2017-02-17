/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/UserCreate/src/only/UserCreate.java#1 $
*****************************************************************************
Package
****************************************************************************/
package only;
/* ***************************************************************************
Imports
****************************************************************************/
import java.io.IOException;
import java.security.GeneralSecurityException;

import org.json.JSONObject;

import secure.provenance.TrustedServer;
import secure.provenance.config.Config;
import secure.provenance.config.Configure;
/* ***************************************************************************
Class
****************************************************************************/
public class UserCreate
{
	protected static final Config CONFIG = Configure.oldConfig();
	protected static final String DEVELOPER_EMAIL_ADDRESS = CONFIG.getDeveloperConfig().getEmailAddress();
	protected static final String DEVELOPER_PASSWORD = CONFIG.getDeveloperConfig().getPassword();
	
	private static final String PASSWORD = "provenance";
	private static final int TIMEOUT = 10000;
	
	protected static void createUser(String emailAddress, String firstName, String lastName, int userTypeId, String password) throws GeneralSecurityException, IOException {
		JSONObject credentials = new JSONObject()
			.put("emailAddress", DEVELOPER_EMAIL_ADDRESS)
			.put("password", DEVELOPER_PASSWORD);
		JSONObject payload = new JSONObject()
			.put("emailAddress", emailAddress)
			.put("firstName",  firstName)
			.put("lastName", lastName)
			.put("userTypeId", userTypeId)
			.put("password", password);
		Request request = new Request(new JSONObject()
			.put("command", "CREATE_USER")
			.put("credentials", credentials)
			.put("payload", payload)
			.toString());
		
		MockClient trustedClient = MockClient.newMockTrustedClient(request);
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
			System.exit(-1);
		}
		
		String response = request.getResponse();
		System.out.println(response);
	}

	protected static void createUser(String emailAddress, String firstName, String lastName, int userTypeID) throws GeneralSecurityException, IOException {
		createUser(emailAddress, firstName, lastName, userTypeID, PASSWORD);
	}
	
	@SuppressWarnings("deprecation")	
	protected static void createUsers(boolean startServer) throws GeneralSecurityException, IOException
	{
		TrustedServer trustedServer;
		Thread serverThread = null;
		
		if (startServer)
		{
			trustedServer = new TrustedServer();
			serverThread = new Thread(trustedServer);
			serverThread.start();
		}
		
		createUser("administrator@provenance.com", "Super", "Administrator", 3); // UserType.Administrator

		createUser("auditor@provenance.com", "Super", "Auditor", 2); // UserType.Auditor
		
		createUser("user@provenance.com",    "Super",     "User",  1); // UserType.User
		createUser("user1@provenance.com",   "Super",     "User1",  1); // UserType.User
		createUser("user2@provenance.com",   "Super",     "User2",  1); // UserType.User
		
		createUser("collberg@gmail.com",     "Christian", "Collberg",  1); // UserType.User
		createUser("debray@cs.arizona.edu",  "Saumya",    "Debray",  1); // UserType.User
		createUser("sram@email.arizona.edu", "Sudha",     "Ram",  1); // UserType.User

		createUser("davidchristy520@gmail.com",   "David",    "Christy",  1); // UserType.User
		createUser("nrtopping@email.arizona.edu", "Nicholas", "Topping",  1); // UserType.User

		createUser("karthikeyan90@email.arizona.edu", "Karthikeyan", "Dhakshinamoorthy",  1); // UserType.User
		createUser("gruzdevad@gmail.com", "Alexander", "Gruzdev",  1); // UserType.User

		if (startServer)
			serverThread.stop();		
	}
	
	public static void main(String[] args) throws GeneralSecurityException, IOException
	{
		//createUsers(true); // to start the server
		createUsers(false); // if server has already been started
	}
}
/* **************************************************************************/
