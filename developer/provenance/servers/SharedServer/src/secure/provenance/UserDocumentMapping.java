/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/UserDocumentMapping.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBAttribute;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBHashKey;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBRangeKey;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBTable;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBVersionAttribute;
/* ***************************************************************************
 Class
 ****************************************************************************/
//@DynamoDBTable(tableName="UserDocumentMapping")
public class UserDocumentMapping {
    
    private Integer userID;
    private Integer documentID;
    private Boolean isAuditor;
    private Boolean isEditor;
//    private Long version;

//    @DynamoDBHashKey(attributeName="UserID")  
    public Integer getUserID() { return userID; }
    public void setUserID(Integer Id) { this.userID = Id; }
    
//    @DynamoDBRangeKey(attributeName="DocumentID")
    public Integer getDocumentID() { return documentID; }    
    public void setDocumentID(Integer id) { this.documentID = id; }
    
//    @DynamoDBAttribute(attributeName="IsAuditor")
    public Boolean getIsAuditor() { return isAuditor; }    
    public void setIsAuditor(Boolean isAuditor) { this.isAuditor = isAuditor;}

//    @DynamoDBAttribute(attributeName="IsEditor")
    public Boolean getIsEditor() { return isEditor; }    
    public void setIsEditor(Boolean isEditor) { this.isEditor = isEditor;}

//    @DynamoDBVersionAttribute
//    public Long getVersion() { return version; }
//    public void setVersion(Long version) { this.version = version;}
}
/* **************************************************************************/
