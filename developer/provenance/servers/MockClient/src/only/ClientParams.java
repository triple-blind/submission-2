/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockClient/src/only/ClientParams.java#1 $
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
public class ClientParams
{
	protected boolean serial;
	protected String address;
	protected int port;
	protected String keyFile;
	protected String keyPassword;
	protected String trustFile;
	protected String trustPassword;

	public ClientParams(String address, int port, String keyFile, String keyPassword, String trustFile, String trustPassword)
	{
		this(false, address, port, keyFile, keyPassword, trustFile, trustPassword);
	}

	public ClientParams(boolean serial, String address, int port, String keyFile, String keyPassword, String trustFile, String trustPassword)
	{
		this.serial = serial;
		this.address = address;
		this.port = port;
		this.keyFile = keyFile;
		this.keyPassword = keyPassword;
		this.trustFile = trustFile;
		this.trustPassword = trustPassword;
	}
	
	public ClientParams(ClientParams other)
	{
		this.serial = other.serial;
		this.address = other.address;
		this.port = other.port;
		
		this.keyFile = other.keyFile;
		this.keyPassword = other.keyPassword;
		
		this.trustFile = other.trustFile;
		this.trustPassword = other.trustPassword;
	}
	
	public ClientParams(String address, ClientParams other)
	{
		this(address, other.getPort(), other);
	}
	
	public ClientParams(String address, int port, ClientParams other)
	{
		this(other);
		this.address = address;
		this.port = port;
	}
	
	public ClientParams(boolean serial, ClientParams other)
	{
		this(other);
		this.serial = serial;
	}
	
	public boolean getSerial()
	{
		return serial;
	}
	
	public String getAddress()
	{
		return address;
	}
	
	public int getPort()
	{
		return port;
	}
	
	public String getKeyFile()
	{
		return keyFile;
	}
	
	public String getKeyPassword()
	{
		return keyPassword;
	}
	
	public String getTrustFile()
	{
		return trustFile;
	}
	
	public String getTrustPassword()
	{
		return trustPassword;
	}
}
/* **************************************************************************/
