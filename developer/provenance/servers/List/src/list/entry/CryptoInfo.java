/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/entry/CryptoInfo.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list.entry;


/* ***************************************************************************
Class
****************************************************************************/
public class CryptoInfo
{
	public Key rsaPrivateKey;
	public Key rsaPublicKey;
	public Key sessionKey;
	
	public CryptoInfo(Key rsaPrivateKey, Key rsaPublicKey, Key sessionKey)
	{
		this.rsaPrivateKey = rsaPrivateKey;
		this.rsaPublicKey = rsaPublicKey;
		this.sessionKey = sessionKey;
	}
}
/* **************************************************************************/
