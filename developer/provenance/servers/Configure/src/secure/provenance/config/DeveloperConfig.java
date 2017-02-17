/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/Configure/src/secure/provenance/config/DeveloperConfig.java#1 $
*****************************************************************************
Package
****************************************************************************/
package secure.provenance.config;
/* ***************************************************************************
Imports
****************************************************************************/
import java.io.Serializable;
/* ***************************************************************************
Class
****************************************************************************/
public class DeveloperConfig implements Serializable
{
	private static final long serialVersionUID = 1L;

	private String firstName;
	private String lastName;
	protected String emailAddress;
	protected String password;
	
	public String getFirstName()
	{
		return firstName;
	}

	public void setFirstName(String firstName)
	{
		this.firstName = firstName;
	}

	public String getLastName()
	{
		return lastName;
	}

	public void setLastName(String lastName)
	{
		this.lastName = lastName;
	}
	
	public String getEmailAddress()
	{
		return emailAddress;
	}
	
	public void setEmailAddress(String emailAddress)
	{
		this.emailAddress = emailAddress;
	}	
	
	public String getPassword()
	{
		return password;
	}
	
	public void setPassword(String password)
	{
		this.password = password;
	}		
}
/* **************************************************************************/
