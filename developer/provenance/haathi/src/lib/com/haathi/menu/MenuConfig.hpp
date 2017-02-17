/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/menu/MenuConfig.hpp#1 $

/*!
 @header configuration.hpp
 
 @author Aaron Gibson
 @date February 11, 2012
 
 @description 
 This header file defines all of the necessary names and macros to configure this extension.
 The information provided here MUST match the descriptions outlined in the various XML files
 bundled with the widget.
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__MENU__MENU_CONFIG_HPP
#	define COM__HAATHI__MENU__MENU_CONFIG_HPP
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
// ProvenanceMenu Fields
//===============================================================================

// This must match the implementation of the service, as defined in "ProtocolHandler.xcu"
#define PROVENANCE_MENU_IMPLEMENTATIONNAME "vnd.com.provenance.haathi.Impl.ProtocolHandler"

// This must match the service name, as defined in "ProtocolHandler.xcu"
#define PROVENANCE_MENU_SERVICENAME "vnd.com.provenance.haathi.ProtocolHandler"

// This must match the "name" of "OfficeMenuBar" as defined in "Addons.xcu". This is used by the 
// XDispatch service to actually decipher which menu item was selected.
#define PROVENANCE_MENU_DISPATCH_URL "vnd.com.provenance.haathi:"

#define PROVENANCE_PREPEND(x) PROVENANCE_MENU_DISPATCH_URL x

#define PROVENANCE_MENU_ABOUT_URL     PROVENANCE_PREPEND("about")
#define PROVENANCE_MENU_LOGIN_URL     PROVENANCE_PREPEND("login")
#define PROVENANCE_MENU_NEW_URL       PROVENANCE_PREPEND("new")
#define PROVENANCE_MENU_OPEN_URL      PROVENANCE_PREPEND("open")
#define PROVENANCE_MENU_SHARE_URL     PROVENANCE_PREPEND("share")
#define PROVENANCE_MENU_HISTORY_URL   PROVENANCE_PREPEND("history")
#define PROVENANCE_MENU_EXPLOIT_URL   PROVENANCE_PREPEND("exploit")
#define PROVENANCE_MENU_WATERMARK_URL PROVENANCE_PREPEND("watermark")
#define PROVENANCE_MENU_AUDIT_URL     PROVENANCE_PREPEND("audit")
#define PROVENANCE_MENU_LOGGER_URL    PROVENANCE_PREPEND("showlogger")
#define PROVENANCE_MENU_LOGOUT_URL    PROVENANCE_PREPEND("logout")
#define PROVENANCE_MENU_AUDITOR_URL   PROVENANCE_PREPEND("auditor")
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


