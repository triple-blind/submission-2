/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/event/ProvenanceEvent.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package list.event;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.ByteArrayInputStream;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

import list.event.document.CreateDocumentEvent;
import list.event.document.ExportDocumentEvent;
import list.event.document.ExtensionDocumentEvent;
import list.event.document.ListenDocumentEvent;
import list.event.document.MenuDocumentEvent;
import list.event.document.OpenDocumentEvent;
import list.event.document.PrintDocumentEvent;
import list.event.document.PrintJobDocumentEvent;
import list.event.document.SaveDocumentEvent;
import list.event.document.SendDocumentEvent;
import list.event.document.ShareDocumentEvent;
import list.event.text.CopyTextEvent;
import list.event.text.CutTextEvent;
import list.event.text.DeleteTextEvent;
import list.event.text.InsertTextEvent;
import list.event.text.PasteTextEvent;
import list.event.text.ReplaceTextEvent;
import list.event.text.WatermarkTextEvent;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class ProvenanceEvent
{
	public static class Holder<T>
	{
		protected T value;
		
		public Holder(T value) {
			this.value = value;
		}
		
		public void set(T value) {
			this.value=value;
		}
		
		public T get() {
			return value;
		}
	}	
	
	protected static final String DATE_FORMAT = "yyyy-MM-dd HH:mm:ss.SSS";
	
	protected Document document;
	protected XPath xPath;
		
	protected String className;
	protected String eventUser;
	protected String eventTime;
	
	protected static String newEventField(Exception exception)
	{
		return "[Exception: " + exception.getMessage() + "]";		
	}
	
	protected static String newEventType(Exception exception)
	{
		return newEventField(exception);
	}

	protected static String newEventTime(Exception exception)
	{
		return newEventField(exception);
	}

	protected static String newEventLocation(Exception exception)
	{
		return newEventField(exception);
	}
	
	protected String newStringEventLocation(String expression)
	{
		try
		{
			return evaluateStringXPathExpression(expression);
		}
		catch (Exception exception)
		{
			return newEventLocation(exception);
		}		
	}	
	
	protected static ArrayList<String> newArrayList(String value) {
		ArrayList<String> arrayList = new ArrayList<String>();
		
		arrayList.add(value);
		return arrayList;
	}
		
	protected static ArrayList<String> newEventData(Exception exception)
	{
		return newArrayList(newEventField(exception));		
	}

	protected ArrayList<String> newStringEventData(String expression) {
		try
		{
			return newArrayList(unescape(evaluateStringXPathExpression(expression)));
		}
		catch (Exception exception)
		{
			return newEventData(exception);
		}		
	}
	
	protected ArrayList<String> newStringListEventData(String expression) {
		try
		{
			ArrayList<String> strings = evaluateStringListXPathExpression(expression);
			
			return unescape(strings);
		}
		catch (Exception exception)
		{
			return newEventData(exception);
		}		
	}
	
	public static String evaluateStringXPathExpression(XPath xPath, Document document, String expression) throws XPathExpressionException
	{
		XPathExpression xPathExpression = xPath.compile(expression);
		
		return (String) xPathExpression.evaluate(document, XPathConstants.STRING);
	}
	
	public static ArrayList<String> evaluateStringListXPathExpression(XPath xPath, Document document, String expression) throws XPathExpressionException
	{
		XPathExpression xPathExpression = xPath.compile(expression);
		NodeList nodeList = (NodeList) xPathExpression.evaluate(document, XPathConstants.NODESET);
		ArrayList<String> stringList = new ArrayList<String>(nodeList.getLength());
		
		for (int i = 0; i < nodeList.getLength(); i++)
			stringList.add(nodeList.item(i).getTextContent());
		return stringList;
	}
	
	// Use byte[] so that is independent of coding scheme.  Let the XML decide.
	public static ProvenanceEvent newProvenanceEvent(String eventUser, byte[] xmlBytes)
	{
		try
		{
			//String xml = new String(xmlBytes, "UTF-8");
			Document document = DocumentBuilderFactory.newInstance().newDocumentBuilder()
					.parse(new InputSource(new ByteArrayInputStream(xmlBytes)));			
			XPath xPath = XPathFactory.newInstance().newXPath();
			String className = evaluateStringXPathExpression(xPath, document, "//ev/@class_name");	

			switch (className)
			{
				case "com::haathi::provenance::CreateDocumentProvenance":
					return new CreateDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::ExportDocumentProvenance":
					return new ExportDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::OpenDocumentProvenance":
					return new OpenDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::PrintDocumentProvenance":
					return new PrintDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::PrintJobDocumentProvenance":
					return new PrintJobDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::SaveDocumentProvenance":
					return new SaveDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::SendDocumentProvenance":
					return new SendDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::ShareDocumentProvenance":
					return new ShareDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::MenuDocumentProvenance":
					return new MenuDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::ListenDocumentProvenance":
					return new ListenDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::ExtensionDocumentProvenance":
					return new ExtensionDocumentEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::CopyTextProvenance":
					return new CopyTextEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::CutTextProvenance":
					return new CutTextEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::PasteTextProvenance":
					return new PasteTextEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::ReplaceTextProvenance":
					return new ReplaceTextEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::InsertTextProvenance":
					return new InsertTextEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::DeleteTextProvenance":
					return new DeleteTextEvent(className, eventUser, document, xPath);
				case "com::haathi::provenance::WatermarkTextProvenance":
					return new WatermarkTextEvent(className, eventUser, document, xPath);

				default:
					System.err.println(new String(xmlBytes));
					return new ProblemProvenanceEvent(className, eventUser, document, xPath, "[Unknown: " + className + "]");
			}
		}
		catch (Exception exception)
		{
			return new ProblemProvenanceEvent(null, eventUser, null, null, newEventType(exception));
		}
	}

	public String evaluateStringXPathExpression(String expression) throws XPathExpressionException
	{
		return evaluateStringXPathExpression(xPath, document, expression);
	}

	public ArrayList<String> evaluateStringListXPathExpression(String expression) throws XPathExpressionException
	{
		return evaluateStringListXPathExpression(xPath, document, expression);
	}
	
	public String asString(ArrayList<String> stringList)
	{
		StringBuffer stringBuffer = new StringBuffer();
		
		for (int i = 0; i < stringList.size(); i++)
		{
			if (i > 0)
				stringBuffer.append('\t');
			stringBuffer.append(stringList.get(i));
		}
		return stringBuffer.toString();
	}
	
	public int evaluateIntegerXPathExpression(String expression) throws XPathExpressionException
	{
		XPathExpression xPathExpression = xPath.compile(expression);	
		Double value = (Double) xPathExpression.evaluate(document, XPathConstants.NUMBER);
		
		return value.intValue();
	}
	
	protected String newEventType()
	{
		try
		{
			String[] parts = className.split("::");

			return parts[parts.length - 1];
		}
		catch (Exception exception)
		{
			return newEventType(exception);
		}	
	}
	
	protected Calendar newCalendar(int year, int month, int day, int hours, int minutes, int seconds, int nanoseconds)
	{
		// Provenance events are recorded GMT
		Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
		calendar.set(year, month, day, hours, minutes, seconds);
		calendar.set(Calendar.MILLISECOND, nanoseconds/1000000);
		
		return calendar;
	}
	
	protected int getTime(String ts, Holder<Integer> pos, char end) {
		int value = 0;

		while (ts.charAt(pos.get()) != end) {
			value = value * 10 + (ts.charAt(pos.get()) - '0');
			pos.set(pos.get() + 1);
		}
		pos.set(pos.get() + 1);
		return value;
	}
	
	protected int getTime(String ts, Holder<Integer> pos) {
		int value = 0;

		while (pos.get() < ts.length()) {
			value = value * 10 + (ts.charAt(pos.get()) - '0');
			pos.set(pos.get() + 1);
		}
		pos.set(pos.get() + 1);		
		return value;
	}

	protected String newEventTime()
	{
		try
		{
			String ts = evaluateStringXPathExpression("//ev/pc/ts/text()");
			Holder<Integer> pos = new Holder<Integer>(0);
			
			/* int weekday = */ getTime(ts, pos, '-');
			int year = getTime(ts, pos, '/');
			int month = getTime(ts, pos, '/') - 1;
			int day = getTime(ts, pos, ' ');
			int hours = getTime(ts, pos, ':');
			int minutes = getTime(ts, pos, ':');
			int seconds = getTime(ts, pos, '.');
			int nanoseconds = getTime(ts, pos);
			
			Date date = newCalendar(year, month, day, hours, minutes, seconds, nanoseconds).getTime();
			SimpleDateFormat simpleDateFormat = new SimpleDateFormat(DATE_FORMAT);
		
			// This should be local time now in the string
			return simpleDateFormat.format(date);
		}
		catch (Exception exception)
		{
			return newEventTime(exception);
		}		
	}
	
	public ProvenanceEvent(String className, String eventUser, Document document, XPath xPath)
	{
		this.className = className;
		this.eventUser = eventUser;		
		this.document = document;
		this.xPath = xPath;
		
		if (document == null || xPath == null)
			return;
	
		eventTime = newEventTime();
	}
	
	public String getEventTime()
	{
		return eventTime;
	}
	
	public String getEventUser()
	{
		return eventUser != null ? eventUser : "?";
	}
	
	public String getEventType()
	{
		return className != null ? newEventType() : "?";
	}
	
	public String getEventLocation()
	{
		return "";
	}
	
	public ArrayList<String> getEventData()
	{
		return new ArrayList<String>();
	}
	
	protected ArrayList<String> unescape(ArrayList<String> strings) {
		ArrayList<String> unescapedStrings = new ArrayList<String>();
		
		for (String string: strings)
			unescapedStrings.add(unescape(string));
		return unescapedStrings;
	}
	
	protected String unescape(String text)
	{
		// Haathi escapes some XML data so that it survives boost
		StringBuffer stringBuffer = new StringBuffer();
		
		for (int i = 0; i < text.length(); i++)
			if (text.charAt(i) == '\\' && i + 1 < text.length())
				switch (text.charAt(++i)) {
					case 't':
						stringBuffer.append("\t");
						break;
					case 'n':
						stringBuffer.append("\n");
						break;
					case 'r':
						stringBuffer.append("\r");
						break;
					case '\\':
						stringBuffer.append("\\");
						break;
					default:
						stringBuffer.append(text.charAt(i - 1));
						stringBuffer.append(text.charAt(i));
				}
			else
				stringBuffer.append(text.charAt(i));
		return stringBuffer.toString();
	}	
}
/* **************************************************************************/
