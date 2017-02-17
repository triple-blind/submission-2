/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/UserTypeId.java#1 $
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
public enum UserTypeId
{
	EDITOR(1), AUDITOR(2), ADMINISTRATOR(3), PROGRAMMER(4), UNKNOWN(-1);

	public static UserTypeId fromInt(int userType)
	{
		switch (userType)
		{
			case 1:
				return UserTypeId.EDITOR;
			case 2:
				return UserTypeId.AUDITOR;
			case 3:
				return UserTypeId.ADMINISTRATOR;
			case 4:
				return UserTypeId.PROGRAMMER;
			default:
				return UserTypeId.UNKNOWN;
		}		
	}
	
	private final int status;

	UserTypeId(int status) {
		this.status = status;
	}

	public int getStatus() {
		return status;
	}
}
/* **************************************************************************/
