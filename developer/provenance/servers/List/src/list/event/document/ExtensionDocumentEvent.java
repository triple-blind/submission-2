/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/event/document/ExtensionDocumentEvent.java#1 $
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
public class ExtensionDocumentEvent extends ProvenanceEvent
{	
	protected ArrayList<String> eventData;
	
	public ExtensionDocumentEvent(String className, String eventUser, Document document, XPath xPath)
	{
		super(className, eventUser, document, xPath);
		
		eventData = newEventData();		
	}
	
	protected ArrayList<String> newEventData()
	{
		try
		{
			String ext = unescape(evaluateStringXPathExpression("//ev/n/text()"));
			String extName = unescape(evaluateStringXPathExpression("//ev/dn/text()"));			
			String ver = unescape(evaluateStringXPathExpression("//ev/v/text()"));
			String extId = unescape(evaluateStringXPathExpression("//ev/r/text()"));
			String status = unescape(evaluateStringXPathExpression("//ev/ok/text()"));
			ArrayList<String> result = new ArrayList<String>();
			result.add(ext);
			result.add(extName);
			result.add(ver);
			result.add(extId);
			result.add(status);			
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
