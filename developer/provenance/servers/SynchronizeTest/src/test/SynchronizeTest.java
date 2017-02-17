/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/servers/SynchronizeTest/src/test/SynchronizeTest.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package test;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import static org.junit.Assert.*;

import org.junit.Test;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class SynchronizeTest {

	class Callee
	{
		protected StringBuffer stringBuffer;
		
		public Callee(StringBuffer stringBuffer)
		{
			this.stringBuffer = stringBuffer;
		}
		
		void call(String message)
		{
			stringBuffer.append("[");
			System.out.print("[" + message);
			try
			{
				Thread.sleep(1000);
			}
			catch (InterruptedException e)
			{
				System.out.println("Interrupted");
			}
			stringBuffer.append("]");
			System.out.println("]");
		}
	}
	
	class Caller implements Runnable
	{
		protected Object lock;
		protected Callee callee;
		protected String message;
		public Thread thread;
		
		public Caller(Object lock, Callee callee, String message)
		{
			this.lock = lock;
			this.callee = callee;
			this.message = message;
			thread = new Thread(this);
			thread.start();
		}
		
		public void run()
		{
			synchronized(lock)
			{
				callee.call(message);
			}
		}
	}
	
	@Test
	public void testCalleeSynchronizer()
	{
		StringBuffer stringBuffer = new StringBuffer();
		
		Callee callee = new Callee(stringBuffer);
		// These will not always print in the same order
		Caller caller1 = new Caller(callee, callee, "Hello");
		Caller caller2 = new Caller(callee, callee, "Synchronized");
		Caller caller3 = new Caller(callee, callee, "World");
		
		try
		{
			caller1.thread.join();
			caller2.thread.join();
			caller3.thread.join();
		}
		catch (InterruptedException e)
		{
			System.out.println("Interrupted");
		}
		assertEquals("[][][]", stringBuffer.toString());
	}

	@Test
	public void testOtherSynchronizer()
	{
		String lock = "Hello";
		StringBuffer stringBuffer = new StringBuffer();
		
		Callee callee = new Callee(stringBuffer);
		// These will not always print in the same order
		Caller caller1 = new Caller(lock, callee, "Hello");
		Caller caller2 = new Caller(lock, callee, "Synchronized");
		Caller caller3 = new Caller(lock, callee, "World");
		
		try
		{
			caller1.thread.join();
			caller2.thread.join();
			caller3.thread.join();
		}
		catch (InterruptedException e)
		{
			System.out.println("Interrupted");
		}
		assertEquals("[][][]", stringBuffer.toString());
	}
}
/* **************************************************************************/
