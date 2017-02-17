/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/retired/servers/TrustedServer/src/secure/provenance/DocumentVersion.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBAttribute;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBHashKey;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBTable;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBVersionAttribute;
/* ***************************************************************************
 Class
 ****************************************************************************/
//@DynamoDBTable(tableName="DocumentVersion")
public class DocumentVersion {
    
    private String versionId;
    private Integer documentId;
    private Integer updatedBy;
    private String updatedOn;
    private Long versionNo;

//    @DynamoDBHashKey(attributeName="VersionID")  
    public String getVersionID() { return versionId; }
    public void setVersionID(String Id) { this.versionId = Id; }
    
//    @DynamoDBAttribute(attributeName="DocumentID")  
    public Integer getDocumentID() { return documentId; }    
    public void setDocumentID(Integer id) { this.documentId = id; }
    
//    @DynamoDBAttribute(attributeName="UpdatedBy")  
    public Integer getUpdatedBy() { return updatedBy; }    
    public void setUpdatedBy(Integer updatedBy) { this.updatedBy = updatedBy;}
    
//    @DynamoDBAttribute(attributeName = "UpdatedOn")
    public String getUpdatedOn() { return updatedOn; }    
    public void setUpdatedOn(String updatedOn) { this.updatedOn = updatedOn; }
    
//    @DynamoDBVersionAttribute
    public Long getVersionNo() { return versionNo; }
    public void setVersionNo(Long versionNo) { this.versionNo = versionNo;}
}
/* **************************************************************************/
