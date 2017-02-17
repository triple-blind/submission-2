/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SQLiteTest/src/test/SQLiteTest.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package test;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import static org.junit.Assert.*;

import java.io.File;

import org.junit.Test;

import com.almworks.sqlite4java.SQLite;
import com.almworks.sqlite4java.SQLiteConnection;
import com.almworks.sqlite4java.SQLiteException;
import com.almworks.sqlite4java.SQLiteStatement;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class SQLiteTest {

	@Test
	public void testLibraryVersionNumber()
	{
		String version = SQLite.getLibraryVersion();
		System.out.println("LibraryVersion: " + version);
		assertEquals("282", version);
	}
	
	@Test
	public void testSQLiteVersionNumber()
	{
		int version;

		try
		{
			version = SQLite.getSQLiteVersionNumber();
			System.out.println("SQLiteVersionNumber: " + version);
			assertEquals(3007010, version);
		}
		catch (SQLiteException e) {
			fail("Exception thrown.");
		}
	}

	@Test
	public void testThreadSafety()
	{
		try
		{
			boolean threadSafe = SQLite.isThreadSafe();
			System.out.println("ThreadSafe: " + threadSafe);
			assertTrue(threadSafe);
		}
		catch (SQLiteException e) {
			fail("Exception thrown.");
		}
	}
	
	protected void execute(SQLiteConnection db, String command) throws SQLiteException
	{
		SQLiteStatement st = null;
		
		try
		{
			st = db.prepare(command);
			st.step();	
		}
		finally
		{
			if (st != null)
				st.dispose();
		}
	}
	
	@Test
	public void testCommit()
	{
		SQLiteConnection db = null;
		SQLiteStatement st = null;

		try
		{
			int initial = 9;
			int expected = 11;
			
			db = new SQLiteConnection();
			db.open(true);
			
			execute(db, "CREATE TABLE [Test] ([Name] TEXT NOT NULL, [Value] INT DEFAULT 0);");
			execute(db, "INSERT INTO [Test] ([Name], [Value]) VALUES ('Hello', " + initial + ");");
			execute(db, "BEGIN TRANSACTION;");
			execute(db, "UPDATE [Test] SET [Value] = " + expected + " WHERE [Name] = 'Hello';");
			execute(db, "COMMIT;");
			st = db.prepare("SELECT [Value] FROM [Test] WHERE [Name] = 'Hello';");
			st.step();
			
			int actual = st.columnInt(0);
			assertEquals(expected, actual);
		}
		catch (SQLiteException e)
		{
			fail("Exception thrown.");
		}
		finally
		{
			if (st != null)
				st.dispose();
			if (db != null)
				db.dispose();			
		}
	}
	
	@Test
	public void testRollback()
	{
		SQLiteConnection db = null;
		SQLiteStatement st = null;

		try
		{
			int initial = 9;
			int expected = 11;
			
			db = new SQLiteConnection();
			db.open(true);
			
			execute(db, "CREATE TABLE [Test] ([Name] TEXT NOT NULL, [Value] INT DEFAULT 0);");
			execute(db, "INSERT INTO [Test] ([Name], [Value]) VALUES ('Hello', " + initial + ");");
			execute(db, "BEGIN TRANSACTION;");
			execute(db, "UPDATE [Test] SET [Value] = " + expected + " WHERE [Name] = 'Hello';");
			execute(db, "ROLLBACK;");
			st = db.prepare("SELECT [Value] FROM [Test] WHERE [Name] = 'Hello';");
			st.step();
			
			int actual = st.columnInt(0);
			assertEquals(initial, actual);
		}
		catch (SQLiteException e)
		{
			fail("Exception thrown.");
		}
		finally
		{
			if (st != null)
				st.dispose();
			if (db != null)
				db.dispose();			
		}		
	}
	
	public abstract class Synchronizer implements Runnable
	{
		protected String filename;
		protected Object lock;
		protected SQLiteConnection db;
		
		public Synchronizer(String filename, Object lock)
		{
			this.filename = filename;
			this.lock = lock;
		}
		
		protected void execute(SQLiteConnection db, String command) throws SQLiteException
		{
			if (db == null)
				return;
			
			SQLiteStatement st = null;
			
			try
			{
				st = db.prepare(command);
				st.step();	
			}
			finally
			{
				if (st != null)
					st.dispose();
			}
		}		
	}
	
	public class Writer extends Synchronizer
	{
		protected int value;
		
		public Writer(String filename, Object lock, int value)
		{
			super(filename, lock);
			this.value = value;
		}
		
		@Override
		public void run()
		{
			if (lock != null)
			{
				synchronized(lock)
				{
					System.out.println("Before writer");
					innerRun();
					System.out.println("After writer");
				};
			}
			else
			{
				System.out.println("Before writer");
				innerRun();
				System.out.println("After writer");
			}
		}

		public void innerRun() {
			SQLiteConnection db = null;

			try
			{
				db = new SQLiteConnection(new File(filename));
				db.open();
				db.setBusyTimeout(5000); // Leave enough time for reader to finish
				execute(db, "UPDATE [Test] SET [Value] = " + value + " WHERE [Name] = 'Hello';");
				System.out.println("Wrote " + value);
			}
			catch (SQLiteException e)
			{
				e.printStackTrace();
			}
			finally
			{
				if (db != null)
					db.dispose();
			}
		}
	}
	
	public class Reader extends Synchronizer
	{
		protected int value;
		
		public Reader(String filename, Object lock)
		{
			super(filename, lock);
		}
		
		public int getValue()
		{
			return value;
		}

		@Override
		public void run()
		{
			if (lock != null)
			{
				synchronized(lock)
				{
					System.out.println("Before reader");
					innerRun();
					System.out.println("After reader");
				};
			}
			else
			{
				System.out.println("Before reader");
				innerRun();
				System.out.println("After reader");
			}
		}
		
		public void innerRun() {
			SQLiteConnection db = null;
			SQLiteStatement st = null;

			try
			{
				db = new SQLiteConnection(new File(filename));
				db.open();
				execute(db, "BEGIN TRANSACTION;");
				st = db.prepare("SELECT [Value] FROM [Test] WHERE [Name] = 'Hello';");
				st.step();
				value = st.columnInt(0);
				st.dispose();
				System.out.println("Read " + value);
				
				Thread.sleep(1000);
				
				st = db.prepare("SELECT [Value] FROM [Test] WHERE [Name] = 'Hello';");
				st.step();
				value = st.columnInt(0);
				execute(db, "COMMIT;");
				System.out.println("Read " + value);
			}
			catch (Exception e)
			{
				e.printStackTrace();
			}
			finally
			{
				if (st != null)
					st.dispose();
				if (db != null)
					db.dispose();
			}
		}		
	}
	
	public void testLock(Object lock)
	{
		String filename = "test.db";
		SQLiteConnection db = null;

		try
		{
			int initial = 9;
			int expected = 11;
			
			File file = new File(filename);
			file.delete();
			db = new SQLiteConnection(file);
			db.open(true);
			
			execute(db, "CREATE TABLE [Test] ([Name] TEXT NOT NULL, [Value] INT DEFAULT 0);");
			execute(db, "INSERT INTO [Test] ([Name], [Value]) VALUES ('Hello', " + initial + ");");

			Reader reader = new Reader(filename, lock);
			Writer writer = new Writer(filename, lock, expected);
			
			Thread readerThread = new Thread(reader);
			Thread writerThread = new Thread(writer);
			
			readerThread.start();
			// Let it start the transaction
			Thread.sleep(100);
			// Change the value
			writerThread.start();
			// Wait for all to stop
			readerThread.join();
			writerThread.join();
			
			assertEquals("Should read initial value.", initial, reader.getValue());
			reader.run();
			assertEquals("Should read final value.", expected, reader.getValue());
		}
		catch (Exception e)
		{
			fail("Exception thrown.");
		}
		finally
		{
			if (db != null)
				db.dispose();			
		}		
	}
	
	@Test
	public void testSynchronizedLock()
	{
		System.out.println("TestSynchronizedLock");
		testLock(new Object());
	}
	
	@Test
	public void testTransactionLock()
	{
		System.out.println("TestTransactionLock");
		testLock(null);
	}
}
/* **************************************************************************/
