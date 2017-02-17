/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/AuthenticationCredentials.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import org.json.JSONObject;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class AuthenticationCredentials {
	private String emailAddress;
	private String password;

	public AuthenticationCredentials(JSONObject jsonObject) {
		this(jsonObject.getString("emailAddress"), jsonObject.getString("password"));
	}
	
	public AuthenticationCredentials(String emailAddress, String password) {
		this.emailAddress = emailAddress;
		this.password = password;
	}

	public String getEmailAddress() {
		return emailAddress;
	}

	public String getPassword() {
		return password;
	}
}
/* **************************************************************************/
