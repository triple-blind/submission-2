/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/event/text/PasteTextEvent.java#1 $
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
public class PasteTextEvent extends ProvenanceEvent
{
	protected String eventLocation;	
	protected ArrayList<String> eventData;
	
	public PasteTextEvent(String className, String eventUser, Document document, XPath xPath)
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
		return newStringEventData("//ev/txt/text()");
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
