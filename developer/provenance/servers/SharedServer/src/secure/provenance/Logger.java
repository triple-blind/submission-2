/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/Logger.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
Imports
****************************************************************************/
import java.io.PrintWriter;
import java.io.StringWriter;
/* ***************************************************************************
Class
****************************************************************************/
public class Logger
{
	// Assume complete control of the interface
	protected org.apache.log4j.Logger logger;
	
	@SuppressWarnings("rawtypes")
	public static Logger getLogger(Class clazz)
	{
		return new Logger(clazz.getName());
	}
	
	public Logger(String name) {
		logger = org.apache.log4j.Logger.getLogger(name);
	}

	public void trace(String message)
	{
		String methodName = Thread.currentThread().getStackTrace()[2].getMethodName();
		
		trace(methodName, message);		
	}
		
	public void debug(String message)
	{
		logger.debug(message);
	}
	
	public void info(String message)
	{
		logger.info(message);
	}
	
	public void warn(String message)
	{
		logger.warn(message);
	}
	
	public void error(String message)
	{
		logger.error(message);
	}
	
	public void fatal(String message)
	{
		logger.error(message);
	}

	protected String format(String type, String methodName, String message)
	{
		return type + " in " + methodName + (message != null ? ": " + message : "");
	}
	
	public void trace()
	{
		String methodName = Thread.currentThread().getStackTrace()[2].getMethodName();
		
		trace(methodName, null);
	}
	
	protected void trace(String methodName, String message)
	{
//		if (!isDebugEnabled())
//			return;
		debug(format("Trace", methodName, message));
	}
	
	public void catchException(Exception exception)
	{
		catchException(exception, true);
	}
	
	public String getMethodName(Exception exception)
	{
		return exception.getStackTrace()[0].getMethodName();
	}
	
	public void catchException(Exception exception, boolean stackTrace)
	{
		String methodName = getMethodName(exception);
		String message = exception.getMessage();
		
		error(format("Exception", methodName, message));
		if (!stackTrace)
			return;
		
		StringWriter stringWriter = new StringWriter();
		PrintWriter printWriter = new PrintWriter(stringWriter);
		exception.printStackTrace(printWriter);
		error(stringWriter.toString());				
	}
}
/* **************************************************************************/
