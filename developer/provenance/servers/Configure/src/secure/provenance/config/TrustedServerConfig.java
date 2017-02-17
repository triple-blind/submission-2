/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/Configure/src/secure/provenance/config/TrustedServerConfig.java#1 $
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
public class TrustedServerConfig implements Serializable
{
	private static final long serialVersionUID = 1L;
	
	private String address;
	private int port;
	
	public String getAddress()
	{
		return address;
	}
	
	public void setAddress(String address)
	{
		this.address = address;
	}
	
	public int getPort()
	{
		return port;
	}
	
	public void setPort(int port)
	{
		this.port = port;
	}
}
/* **************************************************************************/
