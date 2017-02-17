/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/TrustedRequestType.java#1 $
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
public enum TrustedRequestType
{
	PING, // Login-

	AUTHENTICATE, // Login-
	REFRESH, 
	
	CREATE_USER, // UserCreate-
	LIST_USERS, // Share-

	GET_PUBLIC_KEYS,
	GET_PUBLIC_KEY_BY_EMAIL,

	CREATE_DOCUMENT, // New-
	UPDATE_DOCUMENT, // Save-
	ACCESS_DOCUMENT, // Open, Audit
	LIST_DOCUMENTS, // Audit-, New-, Open-

	ADD_EDITORS, // New-, Share-
	SUB_EDITORS,
	
	ADD_AUDITORS,
	SUB_AUDITORS,

	GET_DOCUMENT_EDITORS, // Audit-, History-, Share-
	GET_DOCUMENT_AUDITORS, // Open-, Share-
	GET_DOCUMENT_HISTORY, // History

	GET_ALL_USERS, // Share
	GET_ALL_DOCUMENTS, // later for List

	UNKNOWN
}
/* **************************************************************************/
