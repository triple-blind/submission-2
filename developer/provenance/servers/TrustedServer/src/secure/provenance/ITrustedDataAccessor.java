/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/ITrustedDataAccessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.util.List;
/* ***************************************************************************
 Class
 ****************************************************************************/
public interface ITrustedDataAccessor
{
	String authenticate(AuthenticationCredentials credentials, String provenanceAddress, Integer provenancePort);
	String refresh(AuthenticationCredentials credentials);
	
	String createUser(AuthenticationCredentials credentials, String emailAddress, String firstName,
			String lastName, UserTypeId userTypeId, String password);
	String listUsers(AuthenticationCredentials credentials, UserTypeId userTypeId);

	String getPublicKeys(AuthenticationCredentials credentials, List<Integer> userIds, boolean onlyAuditors);
	String getPublicKeyByEmail(AuthenticationCredentials credentials, String emailAddress);
	
	String createDocument(AuthenticationCredentials credentials, String title);
	String updateDocument(AuthenticationCredentials credentials, Integer round, Integer documentId, String versionId, String prevVersionId);
	String accessDocument(AuthenticationCredentials credentials, Integer round, Integer documentId, String versionId, AccessTypeId accessTypeId);
	String listDocuments(AuthenticationCredentials credentials, UserTypeId userTypeId);

	String addEditors(AuthenticationCredentials credentials, Integer documentId, List<Integer> userIds);
	String subEditors(AuthenticationCredentials credentials, Integer documentId, List<Integer> userIds);

	String addAuditors(AuthenticationCredentials credentials, Integer documentId, List<Integer> userIds);
	String subAuditors(AuthenticationCredentials credentials, Integer documentId, List<Integer> userIds);

	String getDocumentEditors(AuthenticationCredentials credentials, Integer documentId);
	String getDocumentAuditors(AuthenticationCredentials credentials, Integer documentId);
	String getDocumentHistory(AuthenticationCredentials credentials, Integer documentId);

	String getAllUsers(AuthenticationCredentials credentials);
	String getAllDocuments(AuthenticationCredentials credentials);
}
/* **************************************************************************/
