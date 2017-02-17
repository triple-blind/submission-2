/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/config/Config.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list.config;
/* ***************************************************************************
Imports
****************************************************************************/
import java.io.Serializable;
/* ***************************************************************************
Class
****************************************************************************/
public class Config implements Serializable
{
	private static final long serialVersionUID = 1L;

	private TrustedServerConfig trustedServerConfig;
	
	public TrustedServerConfig getTrustedServerConfig()
	{
		return trustedServerConfig;
	}
	
	public void setTrustedServerConfig(TrustedServerConfig trustedServerConfig)
	{
		this.trustedServerConfig = trustedServerConfig;
	}	
}
/* **************************************************************************/
