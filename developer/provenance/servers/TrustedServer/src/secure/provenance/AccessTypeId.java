/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/AccessTypeId.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/

/* ***************************************************************************
 Class
 ****************************************************************************/
public enum AccessTypeId
{
	NEW(1), OPEN(2), SAVE(3), AUDIT(4), UNKNOWN(-1);

	public static AccessTypeId fromInt(int accessType)
	{
		switch (accessType)
		{
			case 1:
				return AccessTypeId.NEW;
			case 2:
				return AccessTypeId.OPEN;
			case 3:
				return AccessTypeId.SAVE;
			case 4:
				return AccessTypeId.AUDIT;
			default:
				return AccessTypeId.UNKNOWN;
		}		
	}
	
	private final int accessType;

	AccessTypeId(int accessType) {
		this.accessType = accessType;
	}

	public int getAccessType() {
		return accessType;
	}
}
/* **************************************************************************/
