/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/MockServer/src/only/ServerParams.java#1 $
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
public class ServerParams
{
	protected boolean serial;
	protected boolean singleShot;
	
	protected int port;
	protected boolean needClientAuth;

	protected String keyFile;
	protected String keyPassword;
	
	protected String trustFile;
	protected String trustPassword;

	public ServerParams(int port, boolean needClientAuth, String keyFile, String keyPassword, String trustFile, String trustPassword)
	{
		this(false, false, port, needClientAuth, keyFile, keyPassword, trustFile, trustPassword);
	}
	
	public ServerParams(boolean serial, boolean singleShot, int port, boolean needClientAuth, String keyFile, String keyPassword, String trustFile, String trustPassword)
	{
		this.serial = serial;
		this.singleShot = singleShot;
		this.port = port;
		this.needClientAuth = needClientAuth;
		this.keyFile = keyFile;
		this.keyPassword = keyPassword;
		this.trustFile = trustFile;
		this.trustPassword = trustPassword;
	}
	
	public ServerParams(ServerParams other)
	{
		this.serial = other.serial;
		this.singleShot = other.singleShot;
		this.port = other.port;
		this.needClientAuth = other.needClientAuth;
		this.keyFile = other.keyFile;
		this.keyPassword = other.keyPassword;
		this.trustFile = other.trustFile;
		this.trustPassword = other.trustPassword;
		
	}
	
	public ServerParams(int port, ServerParams other)
	{
		this(other);
		this.port = port;
	}
	
	public ServerParams(boolean serial, ServerParams other)
	{
		this(other);
		this.serial = serial;
	}

	public ServerParams(boolean serial, boolean singleShot, ServerParams other)
	{
		this(other);
		this.serial = serial;
		this.singleShot = singleShot;
	}
	
	public boolean getSerial()
	{
		return serial;
	}
	
	public boolean getSingleShot()
	{
		return singleShot;
	}
	
	public int getPort()
	{
		return port;
	}
	
	public boolean getNeedClientAuth()
	{
		return needClientAuth;
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
