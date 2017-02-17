/* ***************************************************************************
$Header: //MyDataDepot/Projects/provenance-autoType2/servers/List/src/list/config/Configure.java#1 $
*****************************************************************************
Package
****************************************************************************/
package list.config;
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
import java.io.InputStream;

import list.config.TrustedServerConfig;
/* ***************************************************************************
Class
****************************************************************************/
public class Configure
{
	protected static final String FILE_FILENAME = "./cfg/config.xml";
	protected static final String JAR_FILENAME = "/cfg/config.xml";

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
	
	protected InputStream newInputStream() throws FileNotFoundException
	{
		if (isJar())
			return ClassLoader.class.getResourceAsStream(getFilename());
		return new FileInputStream(getFilename());
	}
	
	public Config deserialize() throws FileNotFoundException
	{	
		InputStream inputStream = newInputStream();
		XMLDecoder xmlDecoder = new XMLDecoder(new BufferedInputStream(inputStream));
		Config config = (Config) (xmlDecoder.readObject());
		
		xmlDecoder.close();
		return config;
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
