/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/FormatTimer/src/only/FormatTimer.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.comp.helper.Bootstrap;
import com.sun.star.comp.helper.BootstrapException;
import com.sun.star.container.XEnumeration;
import com.sun.star.container.XEnumerationAccess;
import com.sun.star.container.XNameAccess;
import com.sun.star.container.XNameContainer;
import com.sun.star.frame.XComponentLoader;
import com.sun.star.frame.XDesktop;
import com.sun.star.frame.XModel;
import com.sun.star.io.IOException;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.style.XStyle;
import com.sun.star.style.XStyleFamiliesSupplier;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextRangeCompare;
import com.sun.star.ucb.XFileIdentifierConverter;
import com.sun.star.uno.Exception;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class FormatTimer {
	protected XComponent savedXComponent;
	protected String[] args;
	protected boolean validSyntax;
	protected String pathToDocOdt;
	protected boolean useStyles;
	
	public FormatTimer(String[] args) {
		this.args = args;
		validSyntax = validateSyntax();
		if (!validSyntax)
			printUsage();
	}
	
	protected boolean validateSyntax() {
		if (args.length != 2)
			return false;
		pathToDocOdt = args[1];
		if (args[0].equals("style"))
			useStyles = true;
		else if (args[0].equals("attribute"))
			useStyles = false;
		else
			return false;
		return true;
	}
	
	protected boolean isValid() {
		return validSyntax;
	}
	
	void printUsage() {
		System.err.println(
				"Usage: " + this.getClass().getName() +  "style|attribute <doc.odt>\n" +
				"Where: doc.odt is the OpenOffice text document to format and time."
		);
	}
	
	protected XComponentContext newXComponentContext() throws BootstrapException {
		XComponentContext xComponentContext = Bootstrap.bootstrap();
		
		return xComponentContext;		
	}
	
	protected Object newDesktop(XComponentContext xComponentContext) throws Exception {
		XMultiComponentFactory xMultiComponentFactory = xComponentContext.getServiceManager();
		Object desktop = xMultiComponentFactory.createInstanceWithContext("com.sun.star.frame.Desktop", xComponentContext);
		
		return desktop;
	}
	
	protected String getFileURLFromSystemPath(XComponentContext xComponentContext, String systemPath) throws Exception {
		XMultiComponentFactory xMultiComponentFactory = xComponentContext.getServiceManager();
		Object fileContentProvider = xMultiComponentFactory.createInstanceWithContext("com.sun.star.ucb.FileContentProvider", xComponentContext);
		XFileIdentifierConverter xFileIdentifierConverter = (XFileIdentifierConverter) UnoRuntime.queryInterface(XFileIdentifierConverter.class, fileContentProvider);

		// This is a hack!  The documentation sucks.
		String baseUrl = xFileIdentifierConverter.getFileURLFromSystemPath("", System.getProperty("user.dir"));
		String fileUrl = xFileIdentifierConverter.getFileURLFromSystemPath(baseUrl, pathToDocOdt);
		return baseUrl + "/" + fileUrl;
	}
	
	protected XTextDocument loadXTextDocument(Object desktop, String fileUrl) throws IOException, IllegalArgumentException {
		XComponentLoader xComponentLoader = (XComponentLoader) UnoRuntime.queryInterface(XComponentLoader.class, desktop);
		XComponent document = xComponentLoader.loadComponentFromURL(fileUrl, "_blank", 0, new PropertyValue[0]);
		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, document);
		
		return xTextDocument;
	}
	
	protected double traverseStandard(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
				timer.start();
				XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
				lockController(xTextDocument);

				XText xText = xTextDocument.getText();
				XTextRangeCompare xTextRangeCompare = (XTextRangeCompare) UnoRuntime.queryInterface(XTextRangeCompare.class, xText);

				XTextCursor documentXTextCursor = xText.createTextCursor();
				documentXTextCursor.gotoEnd(false);
				
				XTextCursor characterXTextCursor = xText.createTextCursor();
				characterXTextCursor.gotoStart(false);
				
				while (xTextRangeCompare.compareRegionEnds(characterXTextCursor, documentXTextCursor) != 0) {
					characterXTextCursor.goRight((short) 1,  true);
					
					characterXTextCursor.collapseToEnd();
				}
				unlockController(xTextDocument);
				timer.stop();
				xTextDocument.dispose();
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}

	protected XStyle newXStyle(XTextDocument xTextDocument) throws Exception {
		XMultiServiceFactory xMultiServiceFactory = (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		Object characterStyle = xMultiServiceFactory.createInstance("com.sun.star.style.CharacterStyle");
		XStyle xStyle = (XStyle) UnoRuntime.queryInterface(XStyle.class, characterStyle);

		return xStyle;
	}
	
	protected void addStyles(XTextDocument xTextDocument) throws Exception {
		XStyleFamiliesSupplier xStyleFamiliesSupplier = (XStyleFamiliesSupplier) UnoRuntime.queryInterface(XStyleFamiliesSupplier.class, xTextDocument);
		XNameAccess xNameAccess = xStyleFamiliesSupplier.getStyleFamilies();
		Object nameContainer = xNameAccess.getByName("CharacterStyles");
		XNameContainer xNameContainer = (XNameContainer) UnoRuntime.queryInterface(XNameContainer.class, nameContainer);
		XStyle xStyle;
		
		xStyle = newXStyle(xTextDocument);
		xNameContainer.insertByName("1", xStyle);
	
		xStyle = newXStyle(xTextDocument);
		xNameContainer.insertByName("2", xStyle);
		
		xStyle = newXStyle(xTextDocument);
		xNameContainer.insertByName("3", xStyle);
	}
	
	protected double runProvenanceStyles(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop) UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
					timer.start();
					XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
					lockController(xTextDocument);
					
					addStyles(xTextDocument);

					XText xText = xTextDocument.getText();
					XTextRangeCompare xTextRangeCompare = (XTextRangeCompare) UnoRuntime.queryInterface(XTextRangeCompare.class, xText);

					XTextCursor documentXTextCursor = xText.createTextCursor();
					documentXTextCursor.gotoEnd(false);
					
					XTextCursor characterXTextCursor = xText.createTextCursor();
					characterXTextCursor.gotoStart(false);
		
					int style = 0;
					
					while (xTextRangeCompare.compareRegionEnds(characterXTextCursor, documentXTextCursor) != 0) {
						characterXTextCursor.goRight((short) 1,  true);
						characterXTextCursor.getString();
						
						XPropertySet xPropertySet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, characterXTextCursor);
						String styleString = "" + (style + 1);
						
						xPropertySet.setPropertyValue("CharStyleName", styleString);
						style = (style + 1) % 3;
						
						characterXTextCursor.collapseToEnd();
					}
					unlockController(xTextDocument);
					timer.stop();
					xTextDocument.dispose();		
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}

	protected double runProvenanceNewStyles(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
				timer.start();
				XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
				lockController(xTextDocument);

				addStyles(xTextDocument);

				XTextCursor xTextCursor = xTextDocument.getText().createTextCursor();
				xTextCursor.gotoStart(false);

				XPropertySet xPropertySet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);

				int style = 0;
				
				while (xTextCursor.goRight((short) 1,  true)) {
					String styleString = "" + (style + 1);
					
					xPropertySet.setPropertyValue("CharStyleName", styleString);
					xTextCursor.collapseToEnd();
					style = (style + 1) % 3;
				}
				unlockController(xTextDocument);
				timer.stop();
				xTextDocument.dispose();
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}
	
	protected double runProvenanceAttributes(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop) UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
					timer.start();
					XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
					lockController(xTextDocument);

					XText xText = xTextDocument.getText();
					XTextRangeCompare xTextRangeCompare = (XTextRangeCompare) UnoRuntime.queryInterface(XTextRangeCompare.class, xText);

					XTextCursor documentXTextCursor = xText.createTextCursor();
					documentXTextCursor.gotoEnd(false);
					
					XTextCursor characterXTextCursor = xText.createTextCursor();
					characterXTextCursor.gotoStart(false);
		
					int[] colors = new int[] { 0x00ff0000, 0x0000ff00, 0x000000ff };
					int style = 0;
					
					while (xTextRangeCompare.compareRegionEnds(characterXTextCursor, documentXTextCursor) != 0) {
						characterXTextCursor.goRight((short) 1,  true);
						// If somehow got the string beforehand, don't have to check it
						// Does it get next character anyway?
						//String s = characterXTextCursor.getString();
						
						XPropertySet xPropertySet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, characterXTextCursor);

						// This is just an example, but it probably has to be a real property
						xPropertySet.setPropertyValue("CharColor", colors[style]);
						style = (style + 1) % 3;
						
						characterXTextCursor.collapseToEnd();
						//characterXTextCursor.goRight((short) 1,  false);
					}
					unlockController(xTextDocument);
					timer.stop();
					xTextDocument.dispose();		
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}
	
	protected double runProvenanceNewAttributes(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
				timer.start();
				XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
				lockController(xTextDocument);

				XTextCursor xTextCursor = xTextDocument.getText().createTextCursor();
				xTextCursor.gotoStart(false);

				int[] colors = new int[] { 0x00ff0000, 0x0000ff00, 0x000000ff };
				int style = 0;

				// This can stay outside the loop.  The xPropertySet applies itself to the characterXTextCursor somehow.
				// Can I make all the property sets that are needed in advance and somehow assign them?
				// There can be lots of cursors and ranges, one for each character.  Can they be reused?
				// Will their position be adjusted on insert?  If so, then might be expensive to insert.
				XPropertySet xPropertySet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);

				while (xTextCursor.goRight((short) 1,  true)) {
					xPropertySet.setPropertyValue("CharColor", colors[style]);
					xTextCursor.collapseToEnd();
					style = (style + 1) % 3;
				}
				unlockController(xTextDocument);
				timer.stop();
				xTextDocument.dispose();
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}
	
	protected void lockController(XTextDocument xTextDocument) {
		XModel xModel = (XModel) UnoRuntime.queryInterface(XModel.class, xTextDocument);

		xModel.lockControllers();
	}

	protected void unlockController(XTextDocument xTextDocument) {
		XModel xModel = (XModel) UnoRuntime.queryInterface(XModel.class, xTextDocument);

		xModel.unlockControllers();
	}
	
	protected double runStandard(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
				timer.start();
				XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
				lockController(xTextDocument);
				
				XText xText = xTextDocument.getText();
				XTextRangeCompare xTextRangeCompare = (XTextRangeCompare) UnoRuntime.queryInterface(XTextRangeCompare.class, xText);

				XTextCursor documentXTextCursor = xText.createTextCursor();
				documentXTextCursor.gotoEnd(false);
				
				XTextCursor characterXTextCursor = xText.createTextCursor();
				characterXTextCursor.gotoStart(false);
					
				while (xTextRangeCompare.compareRegionEnds(characterXTextCursor, documentXTextCursor) != 0) {
					characterXTextCursor.goRight((short) 1,  true);
					characterXTextCursor.getString();
					characterXTextCursor.collapseToEnd();
				}
				unlockController(xTextDocument);
				timer.stop();
				xTextDocument.dispose();		
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}

	protected double runRetype(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
				timer.start();
				XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
				lockController(xTextDocument);
				
				XText xText = xTextDocument.getText();
				String text = xText.getString();
				int count = text.length();
				
				xText.setString("");
				XTextCursor characterXTextCursor = xText.createTextCursor();
				characterXTextCursor.gotoStart(false);
					
				// Might break up into smaller strings first
				for (int i = 0; i < count; i++) {
					characterXTextCursor.setString(text.substring(i, i + 1));
					characterXTextCursor.goRight((short) 1, false);
					// Set to true and do formatting
				}		
				unlockController(xTextDocument);
				timer.stop();
				xTextDocument.dispose();		
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}

	protected double runRestyle(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
				timer.start();
				XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
				lockController(xTextDocument);
				
				addStyles(xTextDocument);

				XText xText = xTextDocument.getText();
				String text = xText.getString();
				int count = text.length();
				
				xText.setString("");
				XTextCursor characterXTextCursor = xText.createTextCursor();
				characterXTextCursor.gotoStart(false);
					
				int style = 0;
				// Might break up into smaller strings first
				for (int i = 0; i < count; i++) {
					characterXTextCursor.setString(text.substring(i, i + 1));
					characterXTextCursor.goRight((short) 1, true);
					
					XPropertySet xPropertySet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, characterXTextCursor);
					String styleString = "" + (style + 1);
					
					xPropertySet.setPropertyValue("CharStyleName", styleString);
					style = (style + 1) % 3;
					
					characterXTextCursor.goRight((short) 1, false);
				}		
				unlockController(xTextDocument);
				timer.stop();
				xTextDocument.dispose();
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}

	protected double runReattribute(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
				timer.start();
				XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
				lockController(xTextDocument);

				XText xText = xTextDocument.getText();
				String text = xText.getString();
				int count = text.length();
				
				xText.setString("");
				XTextCursor characterXTextCursor = xText.createTextCursor();
				characterXTextCursor.gotoStart(false);

				
				int[] colors = new int[] { 0x00ff0000, 0x0000ff00, 0x000000ff };
				int style = 0;
			
				// Might break up into smaller strings first
				for (int i = 0; i < count; i++) {
					characterXTextCursor.setString(text.substring(i, i + 1));
					characterXTextCursor.goRight((short) 1, true);
					
					XPropertySet xPropertySet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, characterXTextCursor);
					
					xPropertySet.setPropertyValue("CharColor", colors[style]);
					style = (style + 1) % 3;
					
					characterXTextCursor.goRight((short) 1, false);
				}		
				unlockController(xTextDocument);
				timer.stop();
				xTextDocument.dispose();
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}

	protected double traverseTextPortions(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
				timer.start();
				XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
				lockController(xTextDocument);

				XText xText = xTextDocument.getText();	
				XEnumerationAccess xParagraphEnumerationAccess = (XEnumerationAccess) UnoRuntime.queryInterface(XEnumerationAccess.class, xText);
				XEnumeration xParagraphEnumeration = xParagraphEnumerationAccess.createEnumeration();
	
				int[] colors = new int[] { 0x00ff0000, 0x0000ff00, 0x000000ff };
				int style = 0;

				while (xParagraphEnumeration.hasMoreElements()) {			
					XEnumerationAccess xEnumerationAccess1 = (XEnumerationAccess) UnoRuntime.queryInterface(XEnumerationAccess.class, xParagraphEnumeration.nextElement());
					XEnumeration xEnumeration1 = xEnumerationAccess1.createEnumeration();
					
					while (xEnumeration1.hasMoreElements()) {
						XPropertySet xPropertySet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xEnumeration1.nextElement());						

						xPropertySet.setPropertyValue("CharColor", colors[style]);
						style = (style + 1) % 3;
					}			
					style = (style + 1) % 3;
				}
				unlockController(xTextDocument);
				timer.stop();
				xTextDocument.dispose();
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}
	
	protected double runLoad(XComponentContext xComponentContext) {
		Timer timer = new Timer();

		try {
			String fileUrl = getFileURLFromSystemPath(xComponentContext, pathToDocOdt);
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);

			try {
				timer.start();
				XTextDocument xTextDocument = loadXTextDocument(desktop, fileUrl);
				timer.stop();
				xTextDocument.dispose();		
			}
			finally {
				xDesktop.terminate();
			}
		}
		catch (Exception e) {
			System.out.println("Exception: " + e.getMessage());
		}	
		catch (java.lang.Exception e) {
			System.out.println("Exception: " + e.getMessage());			
		}
		return timer.getElapsedTime();
	}
	
	public void run() {
		if (!isValid())
			return;
		
		try {
			XComponentContext xComponentContext = null;
			
//			xComponentContext = newXComponentContext();
//			double loadTime = runLoad(xComponentContext);
//			System.out.println(" Load only: " + loadTime);
//
//			xComponentContext = newXComponentContext();
//			double traverseTime = traverseStandard(xComponentContext);
//			System.out.println("  Traverse: " + traverseTime);
//			
			xComponentContext = newXComponentContext();
			double provenanceTime = useStyles ? runProvenanceStyles(xComponentContext) : runProvenanceAttributes(xComponentContext);
			System.out.println("Provenance: " + provenanceTime);

//			xComponentContext = newXComponentContext();
//			double provenanceNewTime = useStyles ? runProvenanceNewStyles(xComponentContext) : runProvenanceNewAttributes(xComponentContext);
//			System.out.println("Provenance: " + provenanceNewTime + " new");
			
//			xComponentContext = newXComponentContext();
//			double retypeTime = runRetype(xComponentContext);
//			System.out.println("    Retype: " + retypeTime);
			
//			xComponentContext = newXComponentContext();
//			double restyleTime = useStyles ? runRestyle(xComponentContext) : runReattribute(xComponentContext);
//			System.out.println("   Restyle: " + restyleTime);

//			xComponentContext = newXComponentContext();
//			double traverseTime = traverseTextPortions(xComponentContext);
//			System.out.println("  Traverse: " + traverseTime);
		}
		catch (BootstrapException e) {
			e.printStackTrace();
		}
	}
	
	public static void main(String[] args) {
		FormatTimer formatTimer = new FormatTimer(args);
		
		formatTimer.run();
	}
}
/* **************************************************************************/
