/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autoType2/haathi4j/BugModify/src/only/ModifyBug.java#1 $
 *****************************************************************************
 Package
 ****************************************************************************/
package only;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import com.sun.star.beans.PropertyValue;
import com.sun.star.comp.helper.Bootstrap;
import com.sun.star.comp.helper.BootstrapException;
import com.sun.star.frame.XComponentLoader;
import com.sun.star.frame.XController;
import com.sun.star.frame.XDesktop;
import com.sun.star.frame.XModel;
import com.sun.star.io.IOException;
import com.sun.star.lang.EventObject;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.text.XText;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.Exception;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.util.XModifiable;
import com.sun.star.util.XModifyBroadcaster;
import com.sun.star.util.XModifyListener;
import com.sun.star.view.XSelectionSupplier;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class ModifyBug implements XModifyListener {
	protected XComponent savedXComponent;
	
	public ModifyBug() {
	}

	@Override
	public void disposing(EventObject eventObject) {
		// TODO: remove listener
	}
	
	protected void getSelections(XComponent xComponent) {
		XModel xModel = (XModel) UnoRuntime.queryInterface(XModel.class, xComponent); 
		XController xController = xModel.getCurrentController();
		XSelectionSupplier xSelectionSupplier = (XSelectionSupplier) UnoRuntime.queryInterface(XSelectionSupplier.class, xController);
		
		xSelectionSupplier.getSelection(); // Including this line makes it hang!
	}
		
	@Override
	public void modified(EventObject eventObject) {
		getSelections(savedXComponent);
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
	
	protected XTextDocument newXTextDocument(Object desktop) throws IOException, IllegalArgumentException {
		XComponentLoader xComponentLoader = (XComponentLoader) UnoRuntime.queryInterface(XComponentLoader.class, desktop);				
		XComponent document = xComponentLoader.loadComponentFromURL("private:factory/swriter", "_blank", 0, new PropertyValue[0]);
		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, document);
		
		return xTextDocument;
	}
	
	public void run() {
		try {
			XComponentContext xComponentContext = newXComponentContext();
			Object desktop = newDesktop(xComponentContext);
			XDesktop xDesktop = (XDesktop)  UnoRuntime.queryInterface(XDesktop.class, desktop);
			
			{
					XTextDocument xTextDocument = newXTextDocument(desktop);
					XText xText = xTextDocument.getText();
					XComponent xComponent = (XComponent) UnoRuntime.queryInterface(XComponent.class, xTextDocument);
					XModifiable xModifiable = (XModifiable) UnoRuntime.queryInterface(XModifiable.class, xComponent);
					XModifyBroadcaster xModifyBroadcaster = (XModifyBroadcaster) UnoRuntime.queryInterface(XModifyBroadcaster.class, xComponent);
				
					savedXComponent = xComponent;
					xModifyBroadcaster.addModifyListener(this);
					
					getSelections(savedXComponent); // It (getSelection) can be called directly
					getSelections(savedXComponent); // and repeatedly
					
					xText.setString("hello"); // It can _not_ be called indirectly
					xModifiable.setModified(true); // It can't be called indirectly like this, either.
					
					xModifyBroadcaster.removeModifyListener(this);
					savedXComponent = null;
					xTextDocument.dispose();		
			}
			xDesktop.terminate();
		}
		catch (BootstrapException e) {
			e.printStackTrace();
		}
		catch (Exception e) {
			System.out.println("Exception...");
		}
	}
	
	public static void main(String[] args) {
		ModifyBug modifyBug = new ModifyBug();
		
		modifyBug.run();
	}
}
/* **************************************************************************/
