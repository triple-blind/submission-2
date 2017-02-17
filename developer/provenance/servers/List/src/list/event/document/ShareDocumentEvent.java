/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/event/document/ShareDocumentEvent.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list.event.document;
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
public class ShareDocumentEvent extends ProvenanceEvent
{
	protected ArrayList<String> eventData;
	
	public ShareDocumentEvent(String className, String eventUser, Document document, XPath xPath)
	{
		super(className, eventUser, document, xPath);
		
		eventData = newEventData();
	}
	
	protected ArrayList<String> newEventData()
	{
		return newStringListEventData("//ev/userEmails/item");		
	}
	
	public ArrayList<String> getEventData()
	{
		return eventData;
	}	
}
/* **************************************************************************/
