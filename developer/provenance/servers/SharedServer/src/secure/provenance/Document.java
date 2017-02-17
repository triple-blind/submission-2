/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/Document.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import org.json.JSONObject;

//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBAttribute;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBHashKey;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBTable;
//import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBVersionAttribute;
/* ***************************************************************************
 Class
 ****************************************************************************/
//@DynamoDBTable(tableName="Document")
public class Document {
	protected static DateFormat DATE_FORMAT = new DateFormat();
	
    private Integer documentId;
    private String title;
    private Integer updatedBy;
    private String updatedOn;
    private String latestVersionId;
    private Integer createdBy;
    private String createdOn;
    private String uniqueKey;
    private Long versionNo;

    //@DynamoDBHashKey(attributeName="documentId")  
    public Integer getDocumentId() { return documentId; }
    public void setDocumentId(Integer Id) { this.documentId = Id; }
    
    //@DynamoDBAttribute(attributeName="title")  
    public String getTitle() { return title; }    
    public void setTitle(String title) { this.title = title; }
    
    //@DynamoDBAttribute(attributeName="updatedBy")  
    public Integer getUpdatedBy() { return updatedBy; }    
    public void setUpdatedBy(Integer updatedBy) { this.updatedBy = updatedBy;}

    //@DynamoDBAttribute(attributeName="updatedOn")  
    public String getUpdatedOn() { return updatedOn; }    
    public void setUpdatedOn(String updatedOn) { this.updatedOn = updatedOn; }
         
    //@DynamoDBAttribute(attributeName = "latestVersionID")
    public String getLatestVersionId() { return latestVersionId; }    
    public void setLatestVersionId(String latestVersionId) { this.latestVersionId = latestVersionId; }
 
    //@DynamoDBAttribute(attributeName="createdBy")  
    public Integer getCreatedBy() { return createdBy; }    
    public void setCreatedBy(Integer createdBy) { this.createdBy = createdBy;}
    
    //@DynamoDBAttribute(attributeName = "createdOn")
    public String getCreatedOn() { return createdOn; }    
    public void setCreatedOn(String createdOn) { this.createdOn = createdOn; }
    //@DynamoDBAttribute(attributeName="uniqueKey")  
    public String getUniqueKey() { return uniqueKey; }    
    public void setUniqueKey(String uniqueKey) { this.uniqueKey = uniqueKey; }

    //@DynamoDBVersionAttribute
    public Long getVersionNo() { return versionNo; }
    public void setVersionNo(Long versionNo) { this.versionNo = versionNo;}
    
	public JSONObject asJSONObject(int keySize)
	{	
		return new JSONObject()
				// kwa: See Packet.hpp for expected names
				.put("title", this.getTitle())
				.put("documentId", this.getDocumentId())
				.put("versionId", this.getLatestVersionId())
				// TODO: Encrypt it using the user's public key
				.put("uniqueKey", this.getUniqueKey())
				.put("uniqueKeySize", keySize / 8)
				.put("updatedOn",  DATE_FORMAT.toLongString(this.getUpdatedOn(), 0L))
				.put("createdOn", DATE_FORMAT.toLongString(this.getCreatedOn(), 0L))
				.put("versionNo",  this.getVersionNo());
	}
}
/* **************************************************************************/
