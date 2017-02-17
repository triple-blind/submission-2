/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/FormatTimer/src/only/Timer.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/

/* ***************************************************************************
 Class
 ****************************************************************************/
public class Timer {
	protected long startTime;
	protected long elapsedTime;
	protected boolean stopped;
	protected int count;
	
	public Timer() {
		elapsedTime = 0;
		stopped = true;
		count = 0;
	}
	
	public void start() throws Exception {
		if (!stopped)
			throw new Exception("Timer already started!");
		stopped = false;
		startTime = System.currentTimeMillis();
	}
	
	public void stop() throws Exception {
		if (stopped)
			throw new Exception("Timer already stopped!");
		stopped = true;
		
		long stopTime = System.currentTimeMillis();
		
		elapsedTime += stopTime - startTime;
		count++;
	}
	
	public double getElapsedTime() {
		return (double) elapsedTime / 1000;
	}
}
/* **************************************************************************/
