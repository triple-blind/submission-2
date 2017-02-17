/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/retired/servers/TrustedServer/src/secure/provenance/TrustedEC2DynamoDataAccessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TimeZone;

import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;

import com.amazonaws.services.dynamodbv2.AmazonDynamoDBClient;
import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBMapper;
import com.amazonaws.services.dynamodbv2.datamodeling.DynamoDBScanExpression;
import com.amazonaws.services.dynamodbv2.model.AttributeValue;
import com.amazonaws.services.dynamodbv2.model.ComparisonOperator;
import com.amazonaws.services.dynamodbv2.model.Condition;
import com.amazonaws.services.dynamodbv2.model.DeleteItemRequest;
import com.amazonaws.services.dynamodbv2.model.ScanRequest;
import com.amazonaws.services.dynamodbv2.model.ScanResult;

import org.apache.commons.codec.binary.Base64;
import org.json.JSONArray;
import org.json.JSONObject;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class TrustedEC2DynamoDataAccessor extends BaseEC2DynamoDataAccessor implements ITrustedDataAccessor
{
	private static final int MILLISECONDS_IN_DAY = 1000 * 60 * 60 * 24;
	private static final int KEYSIZE = 128; // 192 and 256 bits may not be available
	private static final Logger logger = Logger.getLogger(TrustedEC2DynamoDataAccessor.class);

	private AmazonDynamoDBClient dynamoDB = null;
	private DynamoDBMapper mapper = null;

	private boolean isValidUser = false;

	protected static String asBase64(byte buf[])
	{
		return new String(Base64.encodeBase64(buf));
	}

	public TrustedEC2DynamoDataAccessor() throws IOException
	{
		dynamoDB = new AmazonDynamoDBClient(newBasicAWSCredentials());
		mapper = new DynamoDBMapper (dynamoDB);
	}

	@Override
	public String createUser(AuthenticationCredentials credentials,String emailAddress, String firstName,String middleName,String lastName,
			int userTypeID,String password)
	{
		throw new RuntimeException("EC2DynamoDataAccessor.createUser(...) is not implemented!");
	}

	@Override
	public String getUsers ( AuthenticationCredentials credentials, UserType userType )
	{
		throw new RuntimeException("EC2DynamoDataAccessor.getUsers(...) is not implemented!");
	}

	@Override
	public String getAllUsers(AuthenticationCredentials credentials)
	{
		throw new RuntimeException("EC2DynamoDataAccessor.getAllUsers(...) is not implemented!");
	}

	protected String authenticateUser(AuthenticationCredentials credentials)
	{
		throw new RuntimeException("EC2DynamoDataAccessor.authenticateUser(...) is not implemented!");		
	}
	
	@Override
	public String authenticateUser(AuthenticationCredentials credentials, String provenanceAddress, int provenancePort)
	{
		throw new RuntimeException("EC2DynamoDataAccessor.authenticateUser(...) is not implemented!");
	}

	@Override
	public String refreshSessionKey ( AuthenticationCredentials credentials )
	{
		throw new RuntimeException("EC2DynamoDataAccessor.refreshSessionKey(...) is not implemented!");
	}

	@Override
	public String getPublicKey(AuthenticationCredentials credentials, List<Integer> usersToRetrieve, boolean onlyAuditors)
	{
		throw new RuntimeException("EC2DynamoDataAccessor.getPublicKey(...) is not implemented!");
	}

	@Override
	public String getPublicKey(AuthenticationCredentials credentials, String emailID)
	{
		throw new RuntimeException("EC2DynamoDataAccessor.getPublicKey(...) is not implemented!");
	}

	@Override
	public String createNewDocument(AuthenticationCredentials credentials, int userID, String title)
	{
		String result = null;

		try
		{
			result = authenticateUser(credentials);

			if (isValidUser)
			{
				boolean isPlaceHolderFound = false;
				DynamoDBScanExpression scanExpression = new DynamoDBScanExpression();
				addIsPlaceHolderFilterCondition(scanExpression);

				List<Document> documents = mapper.scan(Document.class, scanExpression);
				Date currentDate = new Date();
				SimpleDateFormat dateFormatGmt = new SimpleDateFormat("yyyy-MMM-dd HH:mm:ss");
				dateFormatGmt.setTimeZone(TimeZone.getTimeZone("GMT"));
				String allocatedOn = dateFormatGmt.format(currentDate);

				if (documents != null)
				{
					for (Document document : documents)
					{
						if (document.getAllocatedOn() == null || document.getAllocatedOn().equalsIgnoreCase(""))
							isPlaceHolderFound = true;
						else
						{
							DateFormat formatter = new SimpleDateFormat("dd-MMM-yy");
							Date date = (Date) formatter.parse(document.getAllocatedOn());

							int days = (int) (Math.abs(currentDate.getTime() - date.getTime()) / MILLISECONDS_IN_DAY);
							if (days > 2)
							{
								// Entries are recycled after two days.  They have somehow expired.
								isPlaceHolderFound = true;
								// Delete all user document mappings for this document id - In future may be moved to another thread
								HashMap<String, Condition> scanFilter = new HashMap<String, Condition>();
								scanFilter.put("DocumentID", new Condition()
											.withComparisonOperator(ComparisonOperator.EQ.toString())
											.withAttributeValueList(new AttributeValue().withN("0"))); // kwa: why 0?
								ScanRequest scanRequest = new ScanRequest("UserDocumentMapping").withScanFilter(scanFilter);
								ScanResult scanResult = dynamoDB.scan(scanRequest);

								for (Map<String, AttributeValue> item : scanResult.getItems())
								{
									/*
									DeleteItemRequest deleteItemRequest = new DeleteItemRequest()
											.withTableName("UserDocumentMapping")
											.withKey(new Key()
												.withHashKeyElement(new AttributeValue().withN(item.get("UserID").getN()))
												.withRangeKeyElement(new AttributeValue().withN(item.get("DocumentID").getN())));
									*/
									//item.get("UserID")/
									DeleteItemRequest deleteItemRequest = new DeleteItemRequest("UserDocumentMapping", item);
									dynamoDB.deleteItem(deleteItemRequest);
								}
							}
						}

						if (isPlaceHolderFound)
						{
							// update allocated on in DB
							document.setTitle(title);
							//document.setIsPlaceHolder(false);
							document.setAllocatedOn(allocatedOn);
							mapper.save(document);
							result = resultString(document);
							break;
						}
					}
				}

				if (!isPlaceHolderFound)
				{
					// Create a new dummy document record
					Document document = new Document();
					DynamoDBScanExpression countScanExpression = new DynamoDBScanExpression();
					countScanExpression.addFilterCondition("DocumentID", new Condition()
							.withComparisonOperator(ComparisonOperator.NOT_NULL));
					int documentID = mapper.count(Document.class, countScanExpression);
					// kwa: This is probably not a good way to get an ID
					// It is not guaranteed unique, especially if a hole develops.
					documentID++;

					KeyGenerator kgen = KeyGenerator.getInstance("AES");
					kgen.init(KEYSIZE);
					// Generate the secret key specs.
					SecretKey skey = kgen.generateKey();
					byte[] raw = skey.getEncoded();

					document.setDocumentId(documentID);
					document.setTitle(title);
					document.setCreatedBy(userID);
					document.setCreatedOn(allocatedOn);
					//document.setIsPlaceHolder(true);
					document.setAllocatedOn(allocatedOn);
					document.setLatestVersionId("-1");
					document.setUniqueKey(asBase64(raw));
					// Status ID = 2 implies it is in read-write mode. Status ID = 1 implies it is in read only mode
					document.setStatusId(2);
					mapper.save(document);
					result = resultString(document);
				}
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		return result;
	}

	@Override
	public String listVisibleDocuments(AuthenticationCredentials credentials, int userID, UserType userType)
	{
		String result = null;

		try
		{
			result = authenticateUser(credentials);
			JSONArray resultSet = new JSONArray();

			if (isValidUser)
			{
				logger.info("Listing visible documents for :" + credentials.getUsername());
				DynamoDBScanExpression scanExpression = new DynamoDBScanExpression();
				addUserFilterCondition(scanExpression, userID);
				if (userType == UserType.ProvenanceAuditor)
					addIsAuditorFilterCondition(scanExpression);
				else
					addIsUserFilterCondition(scanExpression);

				List<UserDocumentMapping> mapping = mapper.scan(UserDocumentMapping.class, scanExpression);
				List<Document> availableDocument = new ArrayList<Document>();

				for (UserDocumentMapping map : mapping )
					availableDocument.add(mapper.load(Document.class, map.getDocumentID()));
				for (Document document : availableDocument)
					resultSet.put(document.asJSONObject(KEYSIZE));
				logger.info("Found "+availableDocument.size() + " documents");
				result = resultString(resultSet);
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		return result;
	}

	@Override
	public String getAllDocumentDetails(AuthenticationCredentials credentials)
	{
		String result = null;

		try
		{
			result = authenticateUser(credentials);
			JSONArray resultSet = new JSONArray();

			if (isValidUser)
			{
				HashMap<String, Condition> scanFilter = new HashMap<String, Condition>();
				scanFilter.put("DocumentID", new Condition()
						.withComparisonOperator(ComparisonOperator.NE.toString())
						.withAttributeValueList(new AttributeValue().withN("0")));

				ScanRequest scanRequest = new ScanRequest("Document").withScanFilter(scanFilter);
				ScanResult scanResult = dynamoDB.scan(scanRequest);

				List<Integer> documentIDs = new ArrayList<Integer>();
				for (Map<String, AttributeValue> item : scanResult.getItems())
					documentIDs.add(Integer.parseInt(item.get("DocumentID").getN()));

				/*
					for (Integer document : documentIDs)
					{
						JSONObject object = new JSONObject();
						object.put("DocumentID", document);
						// TO DO: Find all document version for this document-id and pass them as an array
						resultSet.put(object);
					}
				*/

				List<Document> availableDocument = new ArrayList<Document>();

				for (Integer document : documentIDs)
					availableDocument.add(mapper.load(Document.class, document));
				for (Document document : availableDocument)
					resultSet.put(document.asJSONObject(KEYSIZE));
				logger.info("Found "+availableDocument.size() + " documents");
				result = resultString(resultSet);
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		return result;
	}

	public String addDocumentToPlaceHolderList(AuthenticationCredentials credentials, int documentID)
	{
		String result;

		try
		{
			result = authenticateUser(credentials);

			if (isValidUser)
			{
				Document itemRetreived = mapper.load(Document.class, documentID);

				if (itemRetreived != null)
				{
					//itemRetreived.setIsPlaceHolder(true);
					result = resultString(true);
				}
				else
					result = resultString(false);
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, false);
		}
		return result;
	}

	@Override
	public String getDocumentEditors(AuthenticationCredentials credentials, int documentID)
	{
		JSONObject resultObject = new JSONObject();
		String result;

		try
		{
			result = authenticateUser(credentials);

			if (isValidUser)
			{
				DynamoDBScanExpression scanExpression = new DynamoDBScanExpression();
				addDocumentFilterCondition(scanExpression, documentID);
				addIsUserFilterCondition(scanExpression);

				List<UserDocumentMapping> mapping = mapper.scan(UserDocumentMapping.class, scanExpression);
				List<Integer> editors = new ArrayList<Integer>();

				for (UserDocumentMapping map : mapping)
					editors.add(map.getUserID());

				JSONArray resultSet = new JSONArray(editors);
				resultObject.put("result", true);
				resultObject.put("payload", resultSet);
				result = resultObject.toString();
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		return result;
	}

	@Override
	public String getDocumentAuditors(AuthenticationCredentials credentials, Integer documentID)
	{
		String result = null;

		try
		{
			result = authenticateUser(credentials);

			if (isValidUser)
			{
				DynamoDBScanExpression scanExpression = new DynamoDBScanExpression();
				addDocumentFilterCondition(scanExpression, documentID);
				addIsAuditorFilterCondition(scanExpression);

				List<UserDocumentMapping> mapping = mapper.scan(UserDocumentMapping.class, scanExpression);
				List<Integer> auditors = new ArrayList<Integer>();

				for (UserDocumentMapping map : mapping)
					auditors.add(map.getUserID());

				// kwa: returns something unexpected here
				result = getPublicKey(credentials, auditors, true);
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		return result;
	}

	@Override
	public String addAuditors(AuthenticationCredentials credentials, Integer documentID, List<Integer> auditors)
	{
		String result = null;

		try
		{
			result = authenticateUser ( credentials );

			if (isValidUser)
			{
				// TO DO : Work on refining this.
				for (Integer userID : auditors)
				{
					if (!isUserPresent(userID, documentID))
					{
						logger.info("User not present for document. Adding record as auditor");
						UserDocumentMapping userDocumentMapping = new UserDocumentMapping();
						userDocumentMapping.setDocumentID(documentID);
						userDocumentMapping.setUserID(userID);
						userDocumentMapping.setIsAuditor(true);
						logger.info("Adding Provenance Auditor");
						mapper.save(userDocumentMapping);
						logger.info("Added Provenance Auditor");
					}
					else
					{
						logger.info("User present for document. Updating record as auditor");
						UserDocumentMapping itemRetrieved = mapper.load(UserDocumentMapping.class, documentID, userID);
						itemRetrieved.setIsAuditor(true);
						mapper.save(itemRetrieved);
					}
				}
				result = resultString(true);
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		return result;
	}

	@Override
	public String deleteAuditors(AuthenticationCredentials credentials, Integer documentID, List<Integer> auditors)
	{
		String result = null;

		try
		{
			result = authenticateUser(credentials);

			if (isValidUser)
			{
				for (Integer userID : auditors)
				{
					DynamoDBScanExpression scanExpression = new DynamoDBScanExpression();
					addUserFilterCondition(scanExpression, userID);
					addDocumentFilterCondition(scanExpression, documentID);
					// TO DO: Must differentiate between a admin and auditor. Should not remove admin. Currently no admins used
					addIsAuditorFilterCondition(scanExpression);

					List<UserDocumentMapping> mapping = mapper.scan(UserDocumentMapping.class, scanExpression );

					for (UserDocumentMapping map : mapping)
					{
						if (!map.getIsUser())
							// Not a user and soon not an auditor, so can delete record
							mapper.delete(map);
						else
						{
							map.setIsAuditor(false);
							mapper.save(map);
						}
					}
				}
				result = resultString(true);
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, false);
		}
		return result;
	}

	@Override
	public String shareDocument(AuthenticationCredentials credentials, Integer documentID, List<Integer> usersToShare)
	{
		String result = null;

		try
		{
			result = authenticateUser(credentials);

			if (isValidUser)
			{
				for (Integer userID : usersToShare)
				{
					if (!isUserPresent(userID, documentID))
					{
						logger.info("User not present for document. Adding record for user");
						UserDocumentMapping userDocumentMapping = new UserDocumentMapping();
						userDocumentMapping.setDocumentID(documentID);
						userDocumentMapping.setUserID(userID);
						userDocumentMapping.setIsUser(true);
						userDocumentMapping.setIsAuditor(false);

						logger.info("Sharing document with Provenance User");
						mapper.save (userDocumentMapping);
						logger.info("Shared document with id - " + documentID + " with user id = " + userID);
					}
					else
					{
						logger.info("User present for document. Updating record for user "+userID+" for document "+documentID);
						UserDocumentMapping itemRetrieved = mapper.load(UserDocumentMapping.class, userID, documentID);
						itemRetrieved.setIsUser(true);
						mapper.save(itemRetrieved);
					}
				}
				result = resultString(true);
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		return result;
	}

	@Override
	public String revokeSharePrivilege(AuthenticationCredentials credentials, Integer documentID, List<Integer> usersToRevoke)
	{
		String result = null;

		try
		{
			result = authenticateUser(credentials);

			if (isValidUser)
			{
				for (Integer userID : usersToRevoke)
				{
					if (isUserPresent(userID, documentID))
					{
						UserDocumentMapping itemRetrieved = mapper.load(UserDocumentMapping.class, documentID, userID);
						itemRetrieved.setIsUser(false);
						mapper.save(itemRetrieved);
					}
					else
					{
						// kwa: Why is this.  The documentID, userID combination is not present at all
						/*
						DeleteItemRequest deleteItemRequest = new DeleteItemRequest()
								.withTableName("UserDocumentMapping")
								.withKey(new Key()
									.withHashKeyElement(new AttributeValue().withN(userID.toString()))
									.withRangeKeyElement(new AttributeValue().withN(documentID.toString())));
						*/
						Map<String, AttributeValue> key = new HashMap<String, AttributeValue>();
						key.put("UserID", new AttributeValue(userID.toString()));
						key.put("DocumentID", new AttributeValue(documentID.toString()));
						DeleteItemRequest deleteItemRequest = new DeleteItemRequest("UserDocumentMapping", key);
						dynamoDB.deleteItem (deleteItemRequest);
					}
				}
				result = resultString(true);
			}
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		return result;
	}

	private void addFilterCondition(DynamoDBScanExpression scanExpression, String name, int value)
	{
		scanExpression.addFilterCondition(name, new Condition()
				.withComparisonOperator(ComparisonOperator.EQ)
				.withAttributeValueList(new AttributeValue().withN(Integer.toString(value))));
	}

	private void addDocumentFilterCondition(DynamoDBScanExpression scanExpression, int documentID)
	{
		addFilterCondition(scanExpression, "DocumentID", documentID);
	}

	private void addUserFilterCondition(DynamoDBScanExpression scanExpression, int userID)
	{
		addFilterCondition(scanExpression, "UserID", userID);
	}

	private void addIsAuditorFilterCondition(DynamoDBScanExpression scanExpression)
	{
		addFilterCondition(scanExpression, "IsAuditor", 1);
	}

	private void addIsUserFilterCondition(DynamoDBScanExpression scanExpression)
	{
		addFilterCondition(scanExpression, "IsUser", 1);
	}

	private void addIsPlaceHolderFilterCondition(DynamoDBScanExpression scanExpression)
	{
		// Might this work with 1, depending on DB configuration
		scanExpression.addFilterCondition("IsPlaceholder", new Condition()
				.withComparisonOperator(ComparisonOperator.EQ)
				.withAttributeValueList(new AttributeValue().withS(Boolean.toString(true))));
	}

	protected String resultString(Document document)
	{
		return resultString(true, document.asJSONObject(KEYSIZE));
	}

	private boolean isUserPresent(int userID, int documentID) throws Exception
	{
		DynamoDBScanExpression scanExpression = new DynamoDBScanExpression();
		addUserFilterCondition(scanExpression, userID);
		addDocumentFilterCondition(scanExpression, documentID);
		/*
		if (userType == UserType.ProvenanceUser)
		{
			logger.info("Executing isUserPresent for Provenance User");
			addIsUserFilterCondition(scanExpression);
		}
		if (userType == UserType.ProvenanceAuditor)
		{
			logger.info("Executing isUserPresent for Provenance Auditor");
			addIsAuditorFilterCondition(scanExpression);
		}
		*/
		int recordCount = mapper.count(UserDocumentMapping.class, scanExpression);

		return recordCount > 0;
	}
}
/* **************************************************************************/
