/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/extension/ExtensionConfig.hpp#1 $
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__EXTENSION__EXTENSION_CONFIG_HPP
#	define COM__HAATHI__EXTENSION__EXTENSION_CONFIG_HPP
/******************************************************************************
Include
******************************************************************************/

/******************************************************************************
Define
******************************************************************************/
/*!
 These defines MUST match the names described in:
 description.xml
 manifest.xml
 ProtocolHandler.xcu 
 Addons.xcu
 
 These macros are defined for convenience, since it is really confusing to determine what path
 exactly that OpenOffice is looking for. Anyway, if you make sure that the macros here match
 the ones in the xml files, then you should be fine.
 */

//===============================================================================
// ProvenanceManager Fields
//===============================================================================
// This must match "identifier" in description.xml
#define PROVENANCE_REFERENCE_URL "com.provenance.extensions.haathi"
// This must match the name of the service as defined in the manifest.xml file.
#define PROVENANCE_MANAGER_SERVICENAME "haathi.ProvenanceManager"
// This must match the name of the implementaiton as defined in....
#define PROVENANCE_MANAGER_IMPLEMENTATIONNAME "vnd.haathi.ProvenanceManager"
/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
Class
******************************************************************************/
		
/******************************************************************************
Namespace
******************************************************************************/

/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/


