/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/TrustedServer/src/secure/provenance/Utils.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.security.SecureRandom;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringWriter;
import java.math.BigInteger;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class Utils
{
  	public static String getRandomString()
	{
	    SecureRandom random = new SecureRandom();
	    return new BigInteger(130, random).toString(8);
	}

  	public static byte[] getBytesFromInputStream ( InputStream is )
  			throws IOException
  			{

  				// Get the size of the file
  				long length = is.available();

  				if ( length > Integer.MAX_VALUE )
  				{
  					// File is too large
  				}

  				// Create the byte array to hold the data
  				byte[] bytes = new byte[ ( int ) length];

  				// Read in the bytes
  				int offset = 0;
  				int numRead = 0;

  				while ( offset < bytes.length
  				&& ( numRead = is.read ( bytes, offset, bytes.length - offset ) ) >= 0 )
  				{
  					offset += numRead;
  				}

  				// Ensure all the bytes have been read in
  				if ( offset < bytes.length )
  				{
  					throw new IOException ( "Could not completely read file " );
  				}

  				// Close the input stream and return bytes
  				is.close();
  				return bytes;
  			}
  	

	public static byte[] hexStringToByteArray ( String s )
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
  	
  	public static String displayTextInputStream ( InputStream input ) throws IOException
	{
		BufferedReader reader = new BufferedReader ( new InputStreamReader ( input ) );
		StringWriter writer = new StringWriter();

		while ( true )
		{
			String line = reader.readLine();

			if ( line == null ) break;

			writer.write ( line );
		}

		System.out.println();

		return writer.toString();
	}
}
/* **************************************************************************/
