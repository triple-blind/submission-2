/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/retired/servers/SharedServer/src/secure/provenance/BaseEC2DynamoDataAccessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import com.amazonaws.auth.BasicAWSCredentials;
/* ***************************************************************************
 Imports
 ****************************************************************************/

/* ***************************************************************************
 Class
 ****************************************************************************/
public class BaseEC2DynamoDataAccessor extends BaseDataAccessor
{
	protected BasicAWSCredentials newBasicAWSCredentials() throws IOException
	{
		InputStream inputStream = null;
		
		try
		{
			Properties properties = new Properties();
			inputStream = getClass().getClassLoader().getResourceAsStream("AwsCredentials.properties");
			properties.load(inputStream);
			
			String accessKey = properties.getProperty("accessKey");
			String secretKey = properties.getProperty("secretKey");
			
			return new BasicAWSCredentials(accessKey, secretKey);
		}
		catch (IOException ex)
		{
			ex.printStackTrace();
			throw ex;
		}
		finally
		{
			if (inputStream != null)
			{
				try
				{
					inputStream.close();
				}
				catch (IOException ex)
				{
				}
			}
			
		}
	}
		
/*kwa		private static String asHex ( byte buf[] )
		{
			StringBuffer strbuf = new StringBuffer ( buf.length * 2 );
			int i;

			for ( i = 0; i < buf.length; i++ )
			{
				if ( ( ( int ) buf[i] & 0xff ) < 0x10 )
					strbuf.append ( "0" );

				strbuf.append ( Long.toString ( ( int ) buf[i] & 0xff, 16 ) );
			}

			return strbuf.toString();
			
			//return new String(Base64.encodeBase64(buf));
		}
*/	

/*kwa		private static String asHex ( byte buf[] )
		{
			StringBuffer strbuf = new StringBuffer ( buf.length * 2 );
			int i;

			for ( i = 0; i < buf.length; i++ )
			{
				if ( ( ( int ) buf[i] & 0xff ) < 0x10 )
					strbuf.append ( "0" );

				strbuf.append ( Long.toString ( ( int ) buf[i] & 0xff, 16 ) );
			}

			return strbuf.toString();
		}


		private static byte[] hexStringToByteArray ( String s )
		{
			int len = s.length();
			byte[] data = new byte[len / 2];

			for ( int i = 0; i < len; i += 2 )
			{
				data[i / 2] = ( byte ) ( ( Character.digit ( s.charAt ( i ), 16 ) << 4 )
				                         + Character.digit ( s.charAt ( i+1 ), 16 ) );
			}

			return data;
		}
*/
}
/* **************************************************************************/
