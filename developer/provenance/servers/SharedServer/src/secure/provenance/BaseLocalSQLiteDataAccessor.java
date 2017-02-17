/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SharedServer/src/secure/provenance/BaseLocalSQLiteDataAccessor.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package secure.provenance;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.File;
import java.net.URISyntaxException;

import com.almworks.sqlite4java.SQLiteConnection;
import com.almworks.sqlite4java.SQLiteException;
import com.almworks.sqlite4java.SQLiteStatement;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class BaseLocalSQLiteDataAccessor extends BaseDataAccessor
{
	public class UnauthenticatedCommandProcessor
	{
		public String process(SQLiteConnection db) throws Exception
		{
			return resultString(false);
		}
	}

	protected String database;
	protected Logger logger;

	public BaseLocalSQLiteDataAccessor(String fileDatabase, String jarDatabase, Logger logger)
	{
		database = isJar() ? jarDatabase : fileDatabase;
	}
	
	public BaseLocalSQLiteDataAccessor(String database, Logger logger)
	{
		this(database, database, logger);
	}
	
	protected boolean isJar() {
		String className = "/" + getClass().getName().replace('.', '/') + ".class";
		String resourceName = getClass().getResource(className).toString();
		return !resourceName.startsWith("file:");
	}
	
	protected SQLiteConnection newSQLiteConnection() throws SQLiteException, URISyntaxException
	{
		File file = new File(database);
		SQLiteConnection db = new SQLiteConnection(file);
		
		db.open(true);
		return db;
	}
	
	protected void execute(SQLiteStatement st) throws SQLiteException
	{
		if (st.step())
			st.stepThrough();
	}
	
	protected void dispose(SQLiteConnection db, SQLiteStatement st)
	{
		dispose(st);
		dispose(db);
	}
	
	protected void dispose(SQLiteConnection db)
	{
		if (db != null)
			db.dispose();
	}
	
	protected void dispose(SQLiteStatement st)
	{
		if (st != null)
			st.dispose();
	}

	protected String processTransaction(SQLiteConnection db, UnauthenticatedCommandProcessor processor)
	{
		String result = null;
		SQLiteStatement st = null;
		
		try
		{
			try
			{
				st = db.prepare("BEGIN TRANSACTION;");
				execute(st);
				dispose(st);
			}
			catch (Exception ex1)
			{
				result = catchException(ex1, logger, true);				
				return result;
			}
			
			try
			{
				result = processor.process(db);
			}
			catch (Exception ex2)
			{
				result = catchException(ex2, logger, true);
				try
				{
					st = db.prepare("ROLLBACK;");
					execute(st);
				}
				catch (Exception ex2a)
				{
				}
				return result;
			}	

			try
			{
				st = db.prepare("COMMIT;");
				execute(st);
			}
			catch (Exception ex3)
			{
				result = catchException(ex3, logger, true);				
				return result;
			}
		}
		finally
		{
			dispose(st);
		}		
		return result;
	}
	
	protected String processUnauthenticatedCommand(UnauthenticatedCommandProcessor processor, boolean transact)
	{
		String result = null;
		SQLiteConnection db = null;

		try
		{
			db = newSQLiteConnection();
			if (transact)
				result = processTransaction(db, processor);
			else
				result = processor.process(db);
		}
		catch (Exception ex)
		{
			result = catchException(ex, logger, true);
		}
		finally
		{
			dispose(db);
		}		
		return result;
	}
	
	protected String processUnauthenticatedCommand(UnauthenticatedCommandProcessor processor)
	{
		return processUnauthenticatedCommand(processor, true);
	}
}
/* **************************************************************************/
