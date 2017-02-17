/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/event/ProblemProvenanceEvent.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list.event;
/* ***************************************************************************
Imports
****************************************************************************/
import javax.xml.xpath.XPath;

import org.w3c.dom.Document;
/* ***************************************************************************
Class
****************************************************************************/
public class ProblemProvenanceEvent extends ProvenanceEvent
{	
	protected String message;
	
	public ProblemProvenanceEvent(String className, String eventUser, Document document, XPath xPath, String message)
	{
		super(className, eventUser, document, xPath);
		this.message = message;
	}
	
	public String getEventType()
	{
		return message;
	}
	
	public String getEventTime()
	{
		return "-";
	}
}
/* **************************************************************************/
