/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/event/document/ListenDocumentEvent.java#1 $
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
public class ListenDocumentEvent extends ProvenanceEvent
{	
	protected ArrayList<String> eventData;
	
	public ListenDocumentEvent(String className, String eventUser, Document document, XPath xPath)
	{
		super(className, eventUser, document, xPath);
		
		eventData = newEventData();		
	}
	
	protected ArrayList<String> newEventData()
	{
		try
		{
			String ch = unescape(evaluateStringXPathExpression("//ev/ch/text()"));
			String msg = unescape(evaluateStringXPathExpression("//ev/msg/text()"));			
			
			ArrayList<String> result = new ArrayList<String>();
			
			result.add(ch);
			result.add(msg);			
			return result;
		}
		catch (Exception exception)
		{
			return newEventData(exception);
		}		
	}
	
	public ArrayList<String> getEventData()
	{
		return eventData;
	}		
}
/* **************************************************************************/
