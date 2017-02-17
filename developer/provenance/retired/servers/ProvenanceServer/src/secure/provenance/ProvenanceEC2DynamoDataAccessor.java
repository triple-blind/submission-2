/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/retired/servers/ProvenanceServer/src/secure/provenance/ProvenanceEC2DynamoDataAccessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringWriter;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDBClient;
import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBMapper;
import com.amazonaws.services.dynamodbv2.model.AttributeValue;
import com.amazonaws.services.dynamodbv2.model.ComparisonOperator;
import com.amazonaws.services.dynamodbv2.model.Condition;
import com.amazonaws.services.dynamodbv2.model.ScanRequest;
import com.amazonaws.services.dynamodbv2.model.ScanResult;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.model.ObjectMetadata;
import com.amazonaws.services.s3.model.PutObjectRequest;
import com.amazonaws.services.s3.model.GetObjectRequest;
import com.amazonaws.services.s3.model.S3Object;

import org.json.JSONArray;
import org.json.JSONObject;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class ProvenanceEC2DynamoDataAccessor extends BaseEC2DynamoDataAccessor implements IProvenanceDataAccessor
{
	private static final String PROVENANCE_BUCKET_NAME = "provenanceBucket";
	private static final String DOCUMENT_BUCKET_NAME = "documentBucket";
	private static final Logger logger = Logger.getLogger(ProvenanceEC2DynamoDataAccessor.class);

	private AmazonS3 s3 = null;
	private AmazonDynamoDBClient dynamoDB = null;
	private DynamoDBMapper mapper = null;

	private static String displayTextInputStream(InputStream input) throws IOException
	{
		BufferedReader reader = new BufferedReader(new InputStreamReader(input));
		StringWriter writer = new StringWriter();

		while (true)
		{
			String line = reader.readLine();

			if (line == null)
				break;
			writer.write(line);
		}
		System.out.println();
		return writer.toString();
	}
		
	public ProvenanceEC2DynamoDataAccessor() throws IOException
	{
		BasicAWSCredentials basicAWSCredentials = newBasicAWSCredentials();

		s3 = new AmazonS3Client (basicAWSCredentials);
		dynamoDB = new AmazonDynamoDBClient(basicAWSCredentials);
		mapper = new DynamoDBMapper(dynamoDB);
	}

	@Override
	public String getDocumentToAudit(Integer documentID)
	{
		String result = resultString(false);

		try
		{
			// Find the document and whether it is readonly
			Document itemRetrieved = mapper.load(Document.class, documentID);
			
			if (itemRetrieved == null)
				return result;

			boolean isReadOnlyMode = itemRetrieved.getStatusId() == 1;
			
			HashMap<String, Condition> scanFilter = new HashMap<String, Condition>();
			HashMap<Date, String> versionIDMap = new HashMap<Date, String>();
	    	Condition condition = new Condition()
	    			.withComparisonOperator(ComparisonOperator.EQ.toString())
	    			.withAttributeValueList(new AttributeValue().withN(documentID.toString()));
	    	scanFilter.put("DocumentID", condition);
			
			// Get all document versions for this document and add including UpdateOn and Version ID
			ScanRequest scanRequest = new ScanRequest("DocumentVersion").withScanFilter(scanFilter);
		    ScanResult scanResult = dynamoDB.scan(scanRequest);	
		    
			SimpleDateFormat df = new SimpleDateFormat("yyyy-MMM-dd HH:mm:ss");
			List<Date> dates = new ArrayList<Date>();
			
			for (Map<String, AttributeValue> item : scanResult.getItems()) 
			{
				versionIDMap.put(df.parse(item.get("UpdatedOn").getS()), item.get("VersionID").getS()); 	
				dates.add(df.parse(item.get("UpdatedOn").getS()));			
			}
			logger.fatal("Total versions : " + dates.size());
			Collections.sort(dates);
			// The database could really sort these and then join with provenance data
			
			S3Object provenanceObject = null;
			S3Object documentObject = null;
			List<JSONObject> completeChain = new ArrayList<JSONObject>();
			// Get all the provenance info orderd by date and looked up by uniqueKey
			for (Date date : dates)
			{
				String uniqueKey = versionIDMap.get(date);
				provenanceObject = s3.getObject(new GetObjectRequest(PROVENANCE_BUCKET_NAME, uniqueKey));

				if (provenanceObject == null)
					return result;
				else
				{
					String provenanceDocument = displayTextInputStream(provenanceObject.getObjectContent());
					logger.info("Version on "+date+" contains : "+provenanceDocument);
					JSONArray localChain = new JSONArray(provenanceDocument);
					logger.fatal("Version on " + date + "has " + localChain.length() + " events");
					for (int index = 0; index < localChain.length(); index++)
					{
						String event = localChain.getString(index);
						completeChain.add(new JSONObject(event));
					}
				}
			}      

			String uniqueKey = versionIDMap.get(dates.get(dates.size()-1));
			documentObject = s3.getObject(new GetObjectRequest(DOCUMENT_BUCKET_NAME, uniqueKey));
			JSONArray finalArray = new JSONArray(completeChain);
			String finalProvenanceChain = finalArray.toString();
			logger.info("Provenance chain is : " + finalProvenanceChain);
			logger.info("Total events : " + completeChain.size());
			String plainTextDocument = displayTextInputStream(documentObject.getObjectContent());
			
			JSONObject payload = new JSONObject()
					.put("provenance", finalProvenanceChain)
					.put("document", plainTextDocument)
					.put("readonly", isReadOnlyMode);
			result = resultString(true, payload);
		}
		catch(Exception ex)
		{
			ex.printStackTrace();
		}
		return result;
	}

	
	@Override
	public String getHistory(Integer documentID) {
		String result = resultString(false, "Not implemented");
		
		return result;
	}
	
	@Override
	public String getProvenanceDocument(Integer documentID)
	{
		String result = resultString(false);

		try
		{
			Document itemRetrieved = mapper.load(Document.class, documentID);

			if (itemRetrieved == null)
				return result;

			boolean isReadOnlyMode = itemRetrieved.getStatusId() == 1;
			String uniqueKey = itemRetrieved.getLatestVersionId();
			S3Object provenanceObject = s3.getObject(new GetObjectRequest(PROVENANCE_BUCKET_NAME, uniqueKey));
			S3Object documentObject = s3.getObject(new GetObjectRequest(DOCUMENT_BUCKET_NAME, uniqueKey));

			if (provenanceObject == null || documentObject == null)
				return result;
			else
			{
				String provenanceDocument = displayTextInputStream(provenanceObject.getObjectContent());
				String plainTextDocument = displayTextInputStream(documentObject.getObjectContent());
				JSONArray currentChain = new JSONArray(provenanceDocument);
				
				JSONObject payload = new JSONObject()
						.put("provenance", currentChain.get(currentChain.length()-1))
						.put("document", plainTextDocument)
						.put("readonly", isReadOnlyMode);
				result = resultString(true, payload);
			}
		}
		catch (Exception ex)
		{
			ex.printStackTrace();
		}
		return result;
	}

	@Override
	public String saveProvenanceDocument(Integer documentID, Integer userID, String plainDocument, String provenaceDocument)
	{
		String uniqueKey = UUID.randomUUID().toString();
		String result = resultString(false);

		try
		{					
			Document itemRetrieved = mapper.load(Document.class, documentID);
			boolean isNewDocument = false;
			
			if (itemRetrieved == null)
			{
				System.out.println("Item not found");
				return result;				
			}
			
/*			// Create/update document
			if (itemRetrieved.getIsPlaceHolder())
			{
				System.out.println("New document being saved");
				isNewDocument = true;
				itemRetrieved.setCreatedBy(userID);
				itemRetrieved.setCreatedOn(today());
				itemRetrieved.setIsPlaceHolder(false);
			}
			else
			{
				System.out.println("Saving new version");
				isNewDocument = false;
			}
*/			logger.info("Creating document..");
			itemRetrieved.setLatestVersionId(uniqueKey);
			itemRetrieved.setStatusId(Document.STATUS_READWRITE);
			mapper.save(itemRetrieved);			
			System.out.println("document created..");
			
			// Create version
			System.out.println("Creating version..");
			DocumentVersion version = new DocumentVersion();
			version.setVersionID(uniqueKey);
			version.setUpdatedBy(userID);
			version.setUpdatedOn(today());
			version.setDocumentID(documentID);
			mapper.save(version);
			System.out.println("version created..");
			
			if (isNewDocument)
			{
				// Add record to user document mapping
				System.out.println("Creating mapping for : "+userID+" , "+documentID);
				UserDocumentMapping userDocumentMapping = new UserDocumentMapping();
				userDocumentMapping.setUserID(userID);
				userDocumentMapping.setDocumentID(documentID);
				userDocumentMapping.setIsUser(true);
				userDocumentMapping.setIsAuditor(false);
				mapper.save(userDocumentMapping);
				System.out.println("mapping created..");
			}

			// put the objects onto S3
			ByteArrayInputStream inputStream = new ByteArrayInputStream((provenaceDocument.getBytes()));
			ByteArrayInputStream plainTextStream = new ByteArrayInputStream((plainDocument.getBytes()));

			s3.putObject(new PutObjectRequest( PROVENANCE_BUCKET_NAME, uniqueKey, inputStream,new ObjectMetadata()));
			s3.putObject(new PutObjectRequest( DOCUMENT_BUCKET_NAME, uniqueKey, plainTextStream,new ObjectMetadata()));

			String newProvenanceDocument = displayTextInputStream(s3.getObject(new GetObjectRequest( PROVENANCE_BUCKET_NAME, uniqueKey)).getObjectContent());

			JSONArray currentChain = new JSONArray(newProvenanceDocument);
			
			JSONObject payload = new JSONObject();
			payload.put("documentid", documentID);
			payload.put("provenance", currentChain.get(currentChain.length()-1));
			result = resultString(true, payload);
		}
		catch (Exception ex)
		{
			ex.printStackTrace();
			// may want to add message to result
		}
		return result;
	}
}
/* **************************************************************************/
