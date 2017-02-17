/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/DateFormat.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class DateFormat extends SimpleDateFormat {
	private static final long serialVersionUID = 1L;

	public DateFormat()
	{
		// These will be sorted, so use MM instead of MMM!		
		super("yyyy-MM-dd HH:mm:ss.SSS");
		setTimeZone(TimeZone.getTimeZone("GMT"));
	}
	
	public String today()
	{
		return format(new Date());
	}
	
	String toLongString(String date, long defaultValue) {
		long value = defaultValue;
		
		if (date != null)		
			try {
				// Divide by 1000 to convert from milliseconds to seconds for client
				value = parse(date).getTime() / 1000;
			}
			catch (ParseException e) {
			}
		// Because cJSON only does ints and not longs, these are converted to strings
		// and on the other side atol is used to convert them back to longs		
		return Long.toString(value);
	}
}
/* **************************************************************************/
