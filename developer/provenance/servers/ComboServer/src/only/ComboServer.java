/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/ComboServer/src/only/ComboServer.java#1 $
*****************************************************************************
Package
****************************************************************************/
package only;
/* ***************************************************************************
Imports
****************************************************************************/
import secure.provenance.TrustedServer;
import secure.provenance.ProvenanceServer;
/* ***************************************************************************
Class
****************************************************************************/
public class ComboServer
{
	public static void main(String[] args)
	{
		TrustedServer.main(args);
		ProvenanceServer.main(args);
	}
}
/* **************************************************************************/
