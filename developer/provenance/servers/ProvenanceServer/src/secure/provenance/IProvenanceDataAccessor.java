/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/ProvenanceServer/src/secure/provenance/IProvenanceDataAccessor.java#1 $
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
public interface IProvenanceDataAccessor
{
	String getDocumentForEdit(String versionId);
	String putDocumentForEdit(String versionId, String prevVersionId, String document, String provenance);

	String getDocumentForAudit(String versionId);
}
/* **************************************************************************/
