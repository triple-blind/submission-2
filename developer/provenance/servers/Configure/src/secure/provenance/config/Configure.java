/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/Configure/src/secure/provenance/config/Configure.java#1 $
*****************************************************************************
Package
****************************************************************************/
package secure.provenance.config;
/* ***************************************************************************
Imports
****************************************************************************/
import java.beans.XMLDecoder;
import java.beans.XMLEncoder;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
/* ***************************************************************************
Class
****************************************************************************/
public class Configure
{
	protected static final String FILE_FILENAME = "../cfg/config.xml";
	protected static final String JAR_FILENAME = "./cfg/config.xml";
	
	// This is duplicate code copied from BaseLocalSQLiteDataAccessor
	// and should be refactored
	protected String getFilename()
	{
		return isJar() ? JAR_FILENAME : FILE_FILENAME;
	}
	
	protected boolean isJar() {
		String className = "/" + getClass().getName().replace('.', '/') + ".class";
		String resourceName = getClass().getResource(className).toString();
		return !resourceName.startsWith("file:");
	}
	
	public void serialize(Config config) throws FileNotFoundException
	{
	     XMLEncoder xmlEncoder = new XMLEncoder(new BufferedOutputStream(new FileOutputStream(getFilename())));
	     xmlEncoder.writeObject(config);
	     xmlEncoder.close();				
	}
	
	public Config deserialize() throws FileNotFoundException
	{
		XMLDecoder xmlDecoder = new XMLDecoder(new BufferedInputStream(new FileInputStream(getFilename())));
		Config config = (Config) (xmlDecoder.readObject());
		xmlDecoder.close();
		return config;
	}
	
	public static DeveloperConfig newDeveloperConfig()
	{
		DeveloperConfig instance = new DeveloperConfig();
		
		instance.setFirstName("");
		instance.setLastName("");
		instance.setEmailAddress("");
		instance.setPassword("");
		return instance;
	}
	
	public static ProvenanceServerConfig newProvenanceServerConfig()
	{
		ProvenanceServerConfig instance = new ProvenanceServerConfig();
		
		instance.setAddress("");
		instance.setPort(-1);
		return instance;
	}
	
	public static TrustedServerConfig newTrustedServerConfig()
	{
		TrustedServerConfig instance = new TrustedServerConfig();
		
		instance.setAddress("");
		instance.setPort(-1);
		return instance;
	}
	
	public static Config newConfig() {
		Config instance = new Config();
		
		instance.setDeveloperConfig(newDeveloperConfig());
		instance.setProvenanceServerConfig(newProvenanceServerConfig());
		instance.setTrustedServerConfig(newTrustedServerConfig());
		return instance;
	}
	
	public static Config oldConfig() {
		try
		{	
			return new Configure().deserialize();
		} catch (FileNotFoundException e)
		{
			e.printStackTrace();
			return null;
		}
	}
	
	// Write out an empty configuration file for people to edit
	public static void main(String[] args) throws FileNotFoundException {
		try
		{			
			new Configure().serialize(newConfig());
		}
		catch (Exception exception)
		{
			System.out.println("It didn't work!");
		}
	}
}
/* **************************************************************************/
