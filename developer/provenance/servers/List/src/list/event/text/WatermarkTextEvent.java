/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/event/text/WatermarkTextEvent.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list.event.text;
/* ***************************************************************************
Imports
****************************************************************************/
import java.util.ArrayList;

import javax.xml.xpath.XPath;

import list.event.ProvenanceEvent;

import org.w3c.dom.Document;
/* ***************************************************************************
Class
****************************************************************************/
public class WatermarkTextEvent extends ProvenanceEvent
{
	protected String eventLocation;
	protected ArrayList<String> eventData;
	
	public WatermarkTextEvent(String className, String eventUser, Document document, XPath xPath)
	{
		super(className, eventUser, document, xPath);
		
		eventLocation = newEventLocation();
		eventData = newEventData();
	}

	protected String newEventLocation()
	{
		return newStringEventLocation("//ev/pos/text()");		
	}
	
	protected ArrayList<String> newEventData()
	{
		try
		{
			String before = unescape(evaluateStringXPathExpression("//ev/old/text()"));
			String after = unescape(evaluateStringXPathExpression("//ev/new/text()"));
			
			ArrayList<String> result = new ArrayList<String>();
			result.add(before);
			result.add(after);
			return result;
		}
		catch (Exception exception)
		{
			return newEventData(exception);
		}		
	}

	public String getEventLocation()
	{
		return eventLocation;
	}
	
	public ArrayList<String> getEventData()
	{
		return eventData;
	}
}
/* **************************************************************************/
