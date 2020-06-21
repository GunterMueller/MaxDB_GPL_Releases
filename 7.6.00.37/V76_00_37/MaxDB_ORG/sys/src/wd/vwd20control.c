/*!**********************************************************************

  module: vwd20control.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Özgen

  special area: WebAgent

  description:  Main control functions for WebAgent

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




************************************************************************/

#define MODULE_NAME "vwd20Control"

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "heo670.h"
#include "hwd09util.h"
#include "hwd15text.h"
#include "hwd20control.h"
#include "hwd23wa.h"
#include "hwd25log.h"
#include "hwd27excl.h"
#include "hwd30ses.h"
#include "hwd32hses.h"
#include "hwd37wses.h"
#include "hwd39sqlconn.h"
#include "hwd40user.h"
#include "hwd91mime.h"
#include "SAPDB/Registry/Reg_Registry.h"

#ifdef WIN32
#define HAVE_COM_CLASSES
#endif

#ifdef HAVE_COM_CLASSES
#include "hwd96wacom.h"
#endif

/****************************************************************************
 ***
 *** Module
 ***
 ****************************************************************************/

sapdbwa_TraceDefModule ("vwd20control.c")

/*!**********************************************************************

  prototypes and constants for internal functions

************************************************************************/
/* <-- Crash handling */
#ifdef WIN32
    DWORD WriteException(LPEXCEPTION_POINTERS ec, twd25LogP log);

    /*
    #define TRY
    #define CATCH_BLOCK
    */
    #define TRY      \
       __try
    #define CATCH    \
       __except (WriteException(GetExceptionInformation(), wd20WAControl.waLog)) {;}
#else
    #define TRY
    #define CATCH
#endif
/* Crash handling --> */

/* session pool name for dbfs command line tools */
#define DBFS_POOL_NAME "dbfspool"

/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118259
#define USE_SQLCLASSES
*/

#ifdef WIN32

#define DEFAULT_LOGFILE "c:\\temp\\webagent.log"
#define DEFAULT_CONF_LOGFILE "c:\\temp\\waconf.log"
#define DEFAULT_DBFS_LOGFILE "c:\\temp\\dbfs.log"

#else

#ifdef BIT64
#define DEFAULT_LOGFILE "/tmp/webagent64.log"
#define DEFAULT_CONF_LOGFILE "/tmp/waconf64.log"
#define DEFAULT_DBFS_LOGFILE "/tmp/dbfs64.log"
#else
#define DEFAULT_LOGFILE "/tmp/webagent32.log"
#define DEFAULT_CONF_LOGFILE "/tmp/waconf32.log"
#define DEFAULT_DBFS_LOGFILE "/tmp/dbfs32.log"
#endif

#endif

#define DEFAULT_MIME_TYPES_FILENAME "mime.types"

/****************************************************************************
 ***
 *** Registry Sections
 ***
 ****************************************************************************/

#ifdef REG_WIN32

#define WEBAGENT_REGISTRY "SOFTWARE\\SAP\\SAP DBTech\\WebAgent76"

#define ADMIN_REG_SECTION_GLOBAL			WEBAGENT_REGISTRY"\\Global"
#define ADMIN_REG_SECTION_SESSION_POOLS		WEBAGENT_REGISTRY"\\SessionPools"
#define ADMIN_REG_SECTION_SERVICES			WEBAGENT_REGISTRY"\\Services"
#define ADMIN_REG_SECTION_COMSERVICES		WEBAGENT_REGISTRY"\\COMServices"
#define ADMIN_REG_SECTION_RESOURCES			WEBAGENT_REGISTRY"\\Resources"
#define ADMIN_REG_SECTION_GENERAL_SETTINGS	WEBAGENT_REGISTRY"\\GeneralSettings"
#define ADMIN_REG_SECTION_HTTP	            WEBAGENT_REGISTRY"\\HTTP"
#else

#define WEBAGENT_REGISTRY "/usr/spool/sql/ini/WebAgent75.ini"

#ifdef BIT64
#define ADMIN_REG_SECTION_GLOBAL			"BIT64\\Global"
#define ADMIN_REG_SECTION_SESSION_POOLS		"BIT64\\SessionPools"
#define ADMIN_REG_SECTION_SERVICES			"BIT64\\Services"
#define ADMIN_REG_SECTION_COMSERVICES		"BIT64\\COMServices"
#define ADMIN_REG_SECTION_RESOURCES			"BIT64\\Resources"
#define ADMIN_REG_SECTION_GENERAL_SETTINGS	"BIT64\\GeneralSettings"
#define ADMIN_REG_SECTION_HTTP	            "HTTP"
#else
#define ADMIN_REG_SECTION_GLOBAL			"BIT32\\Global"
#define ADMIN_REG_SECTION_SESSION_POOLS		"BIT32\\SessionPools"
#define ADMIN_REG_SECTION_SERVICES			"BIT32\\Services"
#define ADMIN_REG_SECTION_COMSERVICES		"BIT32\\COMServices"
#define ADMIN_REG_SECTION_RESOURCES			"BIT32\\Resources"
#define ADMIN_REG_SECTION_GENERAL_SETTINGS	"BIT32\\GeneralSettings"
#define ADMIN_REG_SECTION_HTTP	            "HTTP"
#endif

#endif

/****************************************************************************
 ***
 *** Registry Keys
 ***
 ****************************************************************************/

#define ADMIN_REG_KEY_WA_ROOT						"documentRoot"
#define ADMIN_REG_KEY_MIME_TYPES					"MIMETypes"

#define ADMIN_REG_KEY_LOGFILE						"LogFile"
#define ADMIN_REG_KEY_CONF_LOGFILE					"ConfLogFile"

#define ADMIN_REG_KEY_EXIT_FUNCTION					"ExitFunction"
#define ADMIN_REG_KEY_INIT_FUNCTION					"InitFunction"
#define ADMIN_REG_KEY_LIBRARY						"Library"
#define ADMIN_REG_KEY_LIBRARY_TYPE					"LibraryType"
#define ADMIN_REG_KEY_LOGFILE						"LogFile"
#define ADMIN_REG_KEY_LOG_WITH_INFO                 "logWithInfo"
#define ADMIN_REG_KEY_SERVICE_FUNCTION				"ServiceFunction"
#define ADMIN_REG_KEY_SERVICE_NAME					"ServiceName"
#define ADMIN_REG_KEY_SESSION_POOL					"SessionPool"
#define ADMIN_REG_KEY_CLASS_ID				        "ClassId"
#define ADMIN_REG_KEY_SERVER_DB				        "serverDB"
#define ADMIN_REG_KEY_SERVER_NODE			        "serverNode"
#define ADMIN_REG_KEY_USER_ID				        "userId"
#define ADMIN_REG_KEY_PASSWORD				        "password"
#define ADMIN_REG_KEY_DATASOURCE			        "datasource"
#define ADMIN_REG_KEY_DRIVER				        "driver"
#define ADMIN_REG_KEY_AUTOCOMMIT			        "autocommit"
#define ADMIN_REG_KEY_POOL_TYPE				        "poolType"
#define ADMIN_REG_KEY_POOL_SIZE				        "poolSize"
#define ADMIN_REG_KEY_ISOLATION				        "isolation"
#define ADMIN_REG_KEY_SQL_TRACE_FILENAME	        "sqlTraceFilename"
#define ADMIN_REG_KEY_SERVICE_START			        "serviceStart"
#define ADMIN_REG_KEY_SERVICE_WITH_SSL		        "withSSL"
#define ADMIN_REG_KEY_SERVICE_SSL_URL		        "sslURL"
#define ADMIN_REG_KEY_WEB_SESSION_TIMEOUT	        "webSessionTimeout"
#define ADMIN_REG_KEY_DEVELOPER_MODE		        "developerMode"
#define ADMIN_REG_KEY_SERVICE_COOKIE_PATH_PREFIX	"useFastCGIForCookiePath"
#define ADMIN_REG_KEY_LOGFILE_HTTP_REQUESTS         "LogDirectory"
#define ADMIN_REG_KEY_LOGFILE_HTTP_ERRORS           "LogDirectory"
#define ADMIN_REG_KEY_ADMIN_USER_ID                 "AdminUserName"
#define ADMIN_REG_KEY_ADMIN_USER_PASSWORD           "AdminUserPassword"

/****************************************************************************
 ***
 *** Services
 ***
 ****************************************************************************/

#define ADMIN_SERVICE_NAME						"waadmin.wa"
#define ADMIN_SERVICE							"Service"
#define ADMIN_SERVICE_HEADER					"Header"
#define ADMIN_SERVICE_NAVIGATION				"Navigation"
#define ADMIN_SERVICE_NAVIGATION_REFRESH		"Refresh"
#define ADMIN_SERVICE_PICTURES					"Pictures"
#define ADMIN_SERVICE_INIT_DETAILS				"InitDetails"
#define ADMIN_SERVICE_SHOW_LOG_SETTINGS			"ShowLogSettings"
#define ADMIN_SERVICE_UPDATE_LOG_SETTINGS		"UpdateLogSettings"
#define ADMIN_SERVICE_SHOW_RESOURCE_SETTINGS	"ShowResourceSettings"
#define ADMIN_SERVICE_SHOW_GENERAL_SETTINGS		"ShowGeneralSettings"
#define ADMIN_SERVICE_UPDATE_RESOURCE_SETTINGS	"UpdateResourceSettings"
#define ADMIN_SERVICE_UPDATE_GENERAL_SETTINGS	"UpdateGeneralSettings"
#define ADMIN_SERVICE_SERVICE					"Service"
#define ADMIN_SERVICE_SERVICE_UPDATE			"ServiceUpdate"
#define ADMIN_SERVICE_SHOW_LOGFILE				"ShowLogFile"
#define ADMIN_SERVICE_SHOW_HTTP_REQUESTS		"ShowHTTPRequests"
#define ADMIN_SERVICE_SHOW_HTTP_ERRORS			"ShowHTTPErrors"
#define ADMIN_SERVICE_SHOW_CONF_LOGFILE			"ShowConfLogFile"
#define ADMIN_SERVICE_NEW_PARAMETER				"NewParameter"
#define ADMIN_SERVICE_NEW_SERVICE				"NewService"
#define ADMIN_SERVICE_CREATE_NEW_SERVICE		"CreateNewService"
#define ADMIN_SERVICE_DELETE_SERVICE			"DeleteService"
#define ADMIN_SERVICE_DELETE_PARAMETERS			"DeleteParameters"
#define ADMIN_SERVICE_COMSERVICE				"COMService"
#define ADMIN_SERVICE_SHOW_SESSION_POOL			"ShowSessionPool"
#define ADMIN_SERVICE_NEW_SESSION_POOL			"NewSessionPool"
#define ADMIN_SERVICE_CREATE_NEW_SESSION_POOL	"CreateNewSessionPool"
#define ADMIN_SERVICE_DELETE_SESSION_POOL		"DeleteSessionPool"
#define ADMIN_SERVICE_UPDATE_SESSION_POOL		"UpdateSessionPool"
#define ADMIN_SERVICE_UPDATE_COMSERVICE			"UpdateCOMService"
#define ADMIN_SERVICE_DELETE_COMSERVICE			"DeleteCOMService"
#define ADMIN_SERVICE_NEW_COMPARAMETER			"NewCOMParameter"
#define ADMIN_SERVICE_NEW_COMPARAMETER			"NewCOMParameter"
#define ADMIN_SERVICE_DELETE_COMPARAMETERS		"DeleteCOMParameters"
#define ADMIN_SERVICE_NEW_COMSERVICE			"NewCOMService"
#define ADMIN_SERVICE_CREATE_NEW_COMSERVICE		"CreateNewCOMService"
#define ADMIN_SERVICE_CLEAR_LOG_FILE			"ClearLogFile"
#define ADMIN_SERVICE_CLEAR_LOG_FILE_HTTP_REQUESTS  "ClearLogFileHTTPRequests"
#define ADMIN_SERVICE_CLEAR_LOG_FILE_HTTP_ERRORS	"ClearLogFileHTTPErrors"
#define ADMIN_SERVICE_SHOW_STATUS				"ShowStatus"

#define ADMIN_SERVICE_PARAM_NAME				"Name"
#define ADMIN_SERVICE_PARAM_MESSAGES			"Messages"
#define ADMIN_SERVICE_PARAM_OUTPUT              "Output"

/****************************************************************************
 ***
 *** HTML Templates
 ***
 ****************************************************************************/

#define ADMIN_TEMPLATE_NEW_SERVICE_PARAMETER		"WADefineNewServiceParameter.htm"
#define ADMIN_TEMPLATE_DELETE_SERVICE_PARAMETER		"WAShowServiceParametersToDelete.htm"
#define ADMIN_TEMPLATE_SHOW_SERVICE_TO_DELETE		"WAShowServiceToDelete.htm"
#define ADMIN_TEMPLATE_SHOW_SERVICE					"WAShowService.htm"
#define ADMIN_TEMPLATE_SHOW_SESSION_POOL			"WAShowSessionPool.htm"
#define ADMIN_TEMPLATE_SHOW_SESSION_POOL_TO_DELETE	"WAShowSessionPoolToDelete.htm"
#define ADMIN_TEMPLATE_DEFINE_NEW_SERVICE			"WADefineNewService.htm"
#define ADMIN_TEMPLATE_DEFINE_NEW_SESSION_POOL		"WADefineNewSessionPool.htm"

/****************************************************************************
 ***
 *** Buttons
 ***
 ****************************************************************************/

#define ADMIN_EVENT								"WAServiceEvent"
#define ADMIN_ACTION							"WAServiceAction"

#define ADMIN_ACTION_UPDATE_SERVICE						"UpdateService"
#define ADMIN_ACTION_DEFINE_NEW_SERVICE_PARAMETER		"DefineNewServiceParameter"
#define ADMIN_ACTION_DEFINE_NEW_SERVICE					"DefineNewService"
#define ADMIN_ACTION_CREATE_NEW_SERVICE					"CreateNewService"
#define ADMIN_ACTION_SHOW_SERVICE_PARAMETERS_TO_DELETE	"ShowServiceParametersToDelete"
#define ADMIN_ACTION_SHOW_SERVICE_TO_DELETE				"ShowServiceToDelete"
#define ADMIN_ACTION_START_SERVICE						"StartService"
#define ADMIN_ACTION_LOAD_SERVICE						"LoadService"
#define ADMIN_ACTION_UNLOAD_SERVICE						"UnloadService"
#define ADMIN_ACTION_RELOAD_SERVICE						"ReloadService"
#define ADMIN_ACTION_CREATE_NEW_SERVICE_PARAMETER		"CreateNewServiceParameter"
#define ADMIN_ACTION_DELETE_SERVICE_PARAMETERS			"DeleteServiceParameters"
#define ADMIN_ACTION_DELETE_SERVICE						"DeleteService"

#define ADMIN_ACTION_UPDATE_COM_SERVICE						"UpdateCOMService"
#define ADMIN_ACTION_DEFINE_NEW_COM_SERVICE_PARAMETER		"DefineNewCOMServiceParameter"
#define ADMIN_ACTION_SHOW_COM_SERVICE_PARAMETERS_TO_DELETE	"ShowCOMServiceParametersToDelete"
#define ADMIN_ACTION_SHOW_COM_SERVICE_TO_DELETE				"ShowCOMServiceToDelete"

#define ADMIN_ACTION_UPDATE_SESSION_POOL					"UpdateSessionPool"
#define ADMIN_ACTION_UPDATE_SESSION_POOL					"UpdateSessionPool"
#define ADMIN_ACTION_SHOW_SESSION_POOL_TO_DELETE			"ShowSessionPoolToDelete"
#define ADMIN_ACTION_DEFINE_NEW_SESSION_POOL				"DefineNewSessionPool"
#define ADMIN_ACTION_CREATE_NEW_SESSION_POOL				"CreateNewSessionPool"

#define ADMIN_BUTTON_CREATE						"Create"
#define ADMIN_BUTTON_UPDATE						"Update"
#define ADMIN_BUTTON_DELETE						"Delete"
#define ADMIN_BUTTON_NEW_PARAMETER				"New Parameter"
#define ADMIN_BUTTON_DELETE_PARAMETERS			"Delete Parameters"
#define ADMIN_BUTTON_DELETE_SERVICE				"Delete Service"
#define ADMIN_BUTTON_LOAD_SERVICE				"Load Service"
#define ADMIN_BUTTON_UNLOAD_SERVICE				"Unload Service"
#define ADMIN_BUTTON_RELOAD_SERVICE				"Reload Service"
#define ADMIN_BUTTON_START_SERVICE				"Start Service"

/****************************************************************************
 ***
 *** Checkboxes
 ***
 ****************************************************************************/

#define ADMIN_CHECKBOX_PARAMETER					"parameter"

/****************************************************************************
 ***
 *** Text fields
 ***
 ****************************************************************************/

#define ADMIN_TEXT_NEW_SESSION_POOL_NAME		"newSessionPoolName"
#define ADMIN_TEXT_NEW_COMSERVICE_NAME			"newCOMServiceName"
#define ADMIN_TEXT_WA_ROOT						"documentRoot"

#define ADMIN_VALUE_LOGFILE						"LogFile"
#define ADMIN_VALUE_CONF_LOGFILE				"ConfLogFile"
#define ADMIN_VALUE_PICTURES					"Pictures"
#define ADMIN_VALUE_MIME_TYPES					"MIMETypes"
#define ADMIN_VALUE_EXIT_FUNCTION				"ExitFunction"
#define ADMIN_VALUE_INIT_FUNCTION				"InitFunction"
#define ADMIN_VALUE_LIBRARY						"Library"
#define ADMIN_VALUE_LIBRARY_TYPE				"LibraryType"
#define ADMIN_VALUE_SERVICE_FUNCTION			"ServiceFunction"
#define ADMIN_VALUE_SERVICE_NAME				"ServiceName"
#define ADMIN_VALUE_SESSION_POOL				"SessionPool"
#define ADMIN_VALUE_PARAMETER_NAME				"ParameterName"
#define ADMIN_VALUE_PARAMETER_DEFAULT_VALUE		"DefaultValue"
#define ADMIN_VALUE_NEW_SERVICE_NAME			"NewServiceName"

/****************************************************************************
 ***
 *** Labels
 ***
 ****************************************************************************/

#define ADMIN_LABEL_SESSION_POOL_NAME_TITLE			                "Session Pool"
#define ADMIN_LABEL_SERVICE_NAME_TITLE				                "Service"
#define ADMIN_LABEL_SERVICE_USER_PARAMETER_TITLE	                "User defined parameters"
#define ADMIN_LABEL_NEW_PARAMETER_TITLE				                "New parameter for service"
#define ADMIN_LABEL_DELETE_PARAMETER_TITLE			                "Parameters of service"

/****************************************************************************
 ***
 *** Input boxes
 ***
 ****************************************************************************/

#define ADMIN_INPUT_GLOBAL_SETTINGS_LOGFILE_TITLE						"Log Filename"
#define ADMIN_INPUT_GLOBAL_SETTINGS_LOGFILE_NAME						"LogFile"
#define ADMIN_INPUT_GLOBAL_SETTINGS_CONF_LOGFILE_TITLE					"Configuration Log Filename"
#define ADMIN_INPUT_GLOBAL_SETTINGS_CONF_LOGFILE_NAME					"ConfLogFile"
#define ADMIN_INPUT_RESOURCE_SETTINGS_WA_ROOT_TITLE						"WebAgent Document Root"
#define ADMIN_INPUT_RESOURCE_SETTINGS_WA_ROOT_NAME						"documentRoot"
#define ADMIN_INPUT_RESOURCE_SETTINGS_MIME_TYPES_TITLE					"MIME Types"
#define ADMIN_INPUT_RESOURCE_SETTINGS_MIME_TYPES_NAME					"MIMETypes"
#define ADMIN_INPUT_GENERAL_SETTINGS_ADMIN_USER_ID_TITLE				"User Name"
#define ADMIN_INPUT_GENERAL_SETTINGS_ADMIN_USER_ID_NAME					"AdminUserName"
#define ADMIN_INPUT_GENERAL_SETTINGS_ADMIN_USER_PASSWORD_TITLE			"Password"
#define ADMIN_INPUT_GENERAL_SETTINGS_ADMIN_USER_PASSWORD_NAME			"AdminUserPassword"
#define ADMIN_INPUT_SESSION_POOL_NAME_TITLE								"Name of the new session pool"
#define ADMIN_INPUT_SESSION_POOL_NAME_NAME								"newSessionPoolName"
#define ADMIN_INPUT_SHOW_SESSION_POOL_SERVER_DB_TITLE					"Server DB"
#define ADMIN_INPUT_SHOW_SESSION_POOL_SERVER_DB_NAME					"serverDB"
#define ADMIN_INPUT_SHOW_SESSION_POOL_SERVER_NODE_TITLE					"Server Node"
#define ADMIN_INPUT_SHOW_SESSION_POOL_SERVER_NODE_NAME					"serverNode"
#define ADMIN_INPUT_SHOW_SESSION_POOL_USER_ID_TITLE						"User Id"
#define ADMIN_INPUT_SHOW_SESSION_POOL_USER_ID_NAME						"userId"
#define ADMIN_INPUT_SHOW_SESSION_POOL_PASSWORD_TITLE					"User Password"
#define ADMIN_INPUT_SHOW_SESSION_POOL_PASSWORD_NAME						"password"
#define ADMIN_INPUT_SHOW_SESSION_POOL_DATASOURCE_TITLE					"ODBC Datasource"
#define ADMIN_INPUT_SHOW_SESSION_POOL_DATASOURCE_NAME					"datasource"
#define ADMIN_INPUT_SHOW_SESSION_POOL_DRIVER_TITLE						"ODBC Driver"
#define ADMIN_INPUT_SHOW_SESSION_POOL_DRIVER_NAME						"driver"
#define ADMIN_INPUT_SHOW_SESSION_POOL_SIZE_TITLE						"Pool Size"
#define ADMIN_INPUT_SHOW_SESSION_POOL_SIZE_NAME							"poolSize"
#define ADMIN_INPUT_SHOW_SESSION_POOL_SQL_TRACE_FILENAME_TITLE			"SQL Trace Filename"
#define ADMIN_INPUT_SHOW_SESSION_POOL_SQL_TRACE_FILENAME_NAME			"sqlTraceFilename"
#define ADMIN_INPUT_SERVICE_NAME_TITLE									"Logical name of the new service"
#define ADMIN_INPUT_SERVICE_NAME_NAME									"Name"
#define ADMIN_INPUT_SERVICE_EXIT_FUNCTION_TITLE							"Exit Function"
#define ADMIN_INPUT_SERVICE_EXIT_FUNCTION_NAME							"ExitFunction"
#define ADMIN_INPUT_SERVICE_INIT_FUNCTION_TITLE							"Init Function"
#define ADMIN_INPUT_SERVICE_INIT_FUNCTION_NAME							"InitFunction"
#define ADMIN_INPUT_SERVICE_LIBRARY_TITLE								"Library"
#define ADMIN_INPUT_SERVICE_LIBRARY_NAME								"Library"
#define ADMIN_INPUT_SERVICE_LOGFILE_TITLE								"Log Filename"
#define ADMIN_INPUT_SERVICE_LOGFILE_NAME								"LogFile"
#define ADMIN_INPUT_SERVICE_SERVICE_FUNCTION_TITLE						"Service Function"
#define ADMIN_INPUT_SERVICE_SERVICE_FUNCTION_NAME						"ServiceFunction"
#define ADMIN_INPUT_SERVICE_SERVICE_NAME_TITLE							"Service Name"
#define ADMIN_INPUT_SERVICE_SERVICE_NAME_NAME							"ServiceName"
#define ADMIN_INPUT_SERVICE_SSL_URL_TITLE								"SSL URL"
#define ADMIN_INPUT_SERVICE_SSL_URL_NAME								"sslURL"
#define ADMIN_INPUT_SERVICE_WEB_SESSION_TIMEOUT_TITLE					"Default Web Session Timeout"
#define ADMIN_INPUT_SERVICE_WEB_SESSION_TIMEOUT_NAME					"webSessionTimeout"
#define ADMIN_INPUT_NEW_PARAMETER_TITLE									"Parameter Name"
#define ADMIN_INPUT_NEW_PARAMETER_NAME									"ParameterName"
#define ADMIN_INPUT_NEW_PARAMETER_DEFAULT_VALUE_TITLE					"Default Value"
#define ADMIN_INPUT_NEW_PARAMETER_DEFAULT_VALUE_NAME					"DefaultValue"

/****************************************************************************
 ***
 *** Lists
 ***
 ****************************************************************************/

#define ADMIN_LIST_SHOW_SESSION_POOL_AUTOCOMMIT_TITLE					"Autocommit"
#define ADMIN_LIST_SHOW_SESSION_POOL_AUTOCOMMIT_NAME					"autocommit"
#define ADMIN_LIST_SHOW_SESSION_POOL_TYPE_TITLE							"Pool Type"
#define ADMIN_LIST_SHOW_SESSION_POOL_TYPE_NAME							"poolType"
#define ADMIN_LIST_SERVICE_LIBRARY_TYPE_TITLE							"Library Type"
#define ADMIN_LIST_SERVICE_LIBRARY_TYPE_NAME							"LibraryType"
#define ADMIN_LIST_SERVICE_SESSION_POOL_TITLE							"Session Pool"
#define ADMIN_LIST_SERVICE_SESSION_POOL_NAME							"SessionPool"

/****************************************************************************
 ***
 *** Checkboxes
 ***
 ****************************************************************************/

#define ADMIN_CHECKBOX_GENERAL_SETTINGS_DEVELOPER_MODE_TITLE			"Developer Mode"
#define ADMIN_CHECKBOX_GENERAL_SETTINGS_DEVELOPER_MODE_NAME				"developerMode"
#define ADMIN_CHECKBOX_SERVICE_START_TYPE_TITLE							"Start automatic"
#define ADMIN_CHECKBOX_SERVICE_START_TYPE_NAME							"serviceStart"
#define ADMIN_CHECKBOX_SERVICE_WITH_SSL_TITLE							"With SSL"
#define ADMIN_CHECKBOX_SERVICE_WITH_SSL_NAME							"withSSL"
#define ADMIN_CHECKBOX_SERVICE_COOKIE_PATH_PREFIX_TITLE					"Use FastCGI prefix for cookie path"
#define ADMIN_CHECKBOX_SERVICE_COOKIE_PATH_PREFIX_NAME					"useFastCGIForCookiePath"
#define ADMIN_CHECKBOX_GLOBAL_SETTINGS_LOG_WITH_INFO_TITLE              "Log info messages"
#define ADMIN_CHECKBOX_GLOBAL_SETTINGS_LOG_WITH_INFO_NAME               "logWithInfo"

/****************************************************************************
 ***
 *** Common text defines
 ***
 ****************************************************************************/

#define ADMIN_LIBRARY_TYPE_C					    "C"
#define ADMIN_LIBRARY_TYPE_C_VALUE				    "C"
#define ADMIN_LIBRARY_TYPE_CPP					    "CPP"
#define ADMIN_LIBRARY_TYPE_CPP_VALUE			    "CPP"

#define ADMIN_AUTOCOMMIT_ON						    "On"
#define ADMIN_AUTOCOMMIT_ON_VALUE				    "On"
#define ADMIN_AUTOCOMMIT_OFF					    "Off"
#define ADMIN_AUTOCOMMIT_OFF_VALUE				    "Off"

#define ADMIN_POOL_TYPE_ODBC					    "ODBC"
#define ADMIN_POOL_TYPE_ODBC_VALUE				    "ODBC"
#define ADMIN_POOL_TYPE_SQL_CLASS				    "SQLClass"
#define ADMIN_POOL_TYPE_SQL_CLASS_VALUE			    "SQLClass"

#define ADMIN_SERVICE_START_ON_VALUE			    "1"
#define ADMIN_SERVICE_START_OFF_VALUE			    "0"

#define ADMIN_SERVICE_COOKIE_PATH_PREFIX_ON_VALUE	"1"
#define ADMIN_SERVICE_COOKIE_PATH_PREFIX_OFF_VALUE	"0"

#define ADMIN_SERVICE_WITH_SSL_ON_VALUE			    "1"
#define ADMIN_SERVICE_WITH_SSL_OFF_VALUE		    "0"

#define ADMIN_DEVELOPER_MODE_ON_VALUE			    "1"
#define ADMIN_DEVELOPER_MODE_OFF_VALUE			    "0"

#define ADMIN_LOG_WITH_INFO_ON_VALUE			    "1"
#define ADMIN_LOG_WITH_INFO_OFF_VALUE			    "0"

#define ADMIN_WAADMIN							    "WAADMIN"

#define ADMIN_JS_NAVIGATION						    "Navigation.js"

#define ADMIN_EMPTY_ITEM						    ""

#define ADMIN_DBFS_SESSION_POOL_NAME			    "DBFSPool"

#define ADMIN_OUTPUT_MSGSONLY                       "MsgOnly"

/****************************************************************************
 ***
 *** Common defines
 ***
 ****************************************************************************/

#define ADMIN_SESSION_ID_INIT_VALUE				100
#define REPLY_BUFFER_LEN						4096
#define MAX_STATUS_LEN							4096
#define MAX_FILENAME_LEN                        1024

/****************************************************************************
 ***
 *** Types
 ***
 ****************************************************************************/

typedef struct st_wa_control					*twd20WebAgentControlP;

typedef struct st_wa_service_list_item			 twd20ServiceListItem;
typedef struct st_wa_service_list_item			*twd20ServiceListItemP;

typedef struct st_wa_session_pool_list_item		 twd20SessionPoolListItem;
typedef struct st_wa_session_pool_list_item		*twd20SessionPoolListItemP;

typedef struct st_wa_service_description		 twd20_ServiceDescription;
typedef struct st_wa_parameter					 twd20_Parameter;

typedef struct st_wa_session_pool_description	 twd20_SessionPool;

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_RegistrySectionExists( Reg_RegistryP	 registry,
									   const char		*section,
									   sapdbwa_Bool		*found		);

sapdbwa_Bool wd20_RegistryKeyExists( Reg_RegistryP	 registry,
								   const char	*section,
								   const char	*key,
								   sapdbwa_Bool	*found		);

void wd20_InitControl();

sapdbwa_Bool wd20_LoadAllSessionPools( twd20WebAgentControlP wa );

sapdbwa_Bool wd20_UnloadAllSessionPools( twd20WebAgentControlP	wa );

sapdbwa_Bool wd20_LoadSessionPool( twd20WebAgentControlP	 wa,
								 const char				*sessionPool  );

sapdbwa_Bool wd20_UnloadSessionPool( twd20WebAgentControlP	 wa,
								   const char				*sessionPool);

sapdbwa_Bool wd20_AddSessionPoolToList( twd20SessionPoolListItemP	*firstSessionPool,
									  twd30SessionPoolP			 sessionPool       );

sapdbwa_Bool wd20_DeleteSessionPoolFromList( twd20SessionPoolListItemP	*firstSessionPool,
										   const char					*sessionPool       );

twd20SessionPoolListItemP wd20_FindSessionPoolInList( twd20SessionPoolListItemP	 firstSessionPool,
													  const char				*sessionPool       );

void wd20_SendHorizontalLineRow( sapdbwa_HttpReplyP	rep,
								 sapdbwa_UInt2	colNo );

void wd20_SendLabelRow( sapdbwa_HttpReplyP rep,
						const char	*title,
						const char	*label,
						bool		 bold   );

void wd20_SendOpenListRow( sapdbwa_HttpReplyP	 rep,
						   const char	*title,
						   const char	*name  );

void wd20_SendAddListRow( sapdbwa_HttpReplyP	 rep,
						  const char	*value,
						  bool			 selected );

void wd20_SendCloseListRow( sapdbwa_HttpReplyP	rep );

void wd20_SendCheckBoxRow( sapdbwa_HttpReplyP  rep,
						   const char	      *title,
						   const char	      *name,
						   const char	      *value,
						   bool			       checked,
						   sapdbwa_Bool	       multiColumn );

void wd20_SendInputRow( sapdbwa_HttpReplyP rep,
						const char	*title,
						const char	*name,
						const char	*value );

void wd20_SendSubmitFunction( sapdbwa_HttpReplyP  rep,
						      const char	     *action );

void wd20_SendCommonSubmitFunction( sapdbwa_HttpReplyP  rep );

void wd20_SendButtonStart( sapdbwa_HttpReplyP  rep );

void wd20_SendButtonEnd( sapdbwa_HttpReplyP  rep );

void wd20_SendButton( sapdbwa_HttpReplyP  rep,
					  const char	     *action,
					  const char	     *buttonText );

void wd20_SendTableRowStart( sapdbwa_HttpReplyP  rep );

void wd20_SendTableRowEnd( sapdbwa_HttpReplyP  rep );

void wd20_SendTableColumnStart( sapdbwa_HttpReplyP  rep );

void wd20_SendTableColumnEnd( sapdbwa_HttpReplyP  rep );

void wd20_SendBlankTableRow( sapdbwa_HttpReplyP  rep );

void wd20_SendTableSeparatorLine( sapdbwa_HttpReplyP  rep );

sapdbwa_Bool wd20_ShowNewService( sapdbwa_HttpReplyP		 rep,
								  twd20WebAgentControlP		 wa,
								  twd20_ServiceDescription	*serviceDescription,
								  const char				*message			);

sapdbwa_Bool wd20_ShowNewSessionPool( sapdbwa_HttpReplyP	 rep,
									  twd20WebAgentControlP  wa,
									  twd20_SessionPool		*sessionPool,
									  const char			*message	 );

void wd20_InitSessionPoolDescription ( twd20_SessionPool	*sessionPool );

sapdbwa_Bool wd20_GetSessionPoolDescription ( sapdbwa_HttpRequestP		 req,
											twd20_SessionPool	*sessionPool );

sapdbwa_Bool wd20_ShowServiceDescription( twd20_ServiceDescription	*serviceDescription,
						  				  twd20WebAgentControlP		 wa,
										  sapdbwa_HttpReplyP		 rep,
										  const char				*message );

sapdbwa_Bool wd20_FreeServiceDescription( twd20_ServiceDescription	*serviceDescription );

sapdbwa_Bool wd20_GetServiceDescription( sapdbwa_HttpRequestP			 req,
									   twd20_ServiceDescription	*serviceDescription );

twd20_Parameter* wd20_AddParameterToServiceDescription( twd20_ServiceDescription	*serviceDescription,
														const char					*name,
														const char					*value				);

void wd20_InitServiceDescription( twd20_ServiceDescription	*serviceDescription );

sapdbwa_Bool wd20_ShowStatus( sapdbwa_HttpRequestP	req,
							sapdbwa_HttpReplyP	rep  );

sapdbwa_Bool wd20_GetTimeValue( const char	*inputTime,
							  char			*outputTime,
							  sapdbwa_Int4	 outputTimeLen );

sapdbwa_Bool wd20_IsSSLURL( const char	*sslURL );

sapdbwa_Bool wd20_GotoService( twd20WebAgentControlP	wa,
							 sapdbwa_HttpRequestP			req,
							 sapdbwa_HttpReplyP			rep  );

sapdbwa_Bool wd20_RestartService( twd20WebAgentControlP	wa,
								sapdbwa_HttpRequestP			req,
								sapdbwa_HttpReplyP			rep  );

sapdbwa_Bool wd20_StartService( twd20WebAgentControlP	wa,
							  sapdbwa_HttpRequestP		req,
							  sapdbwa_HttpReplyP			rep  );

sapdbwa_Bool wd20_StopService( twd20WebAgentControlP	wa,
							 sapdbwa_HttpRequestP			req,
							 sapdbwa_HttpReplyP			rep  );

sapdbwa_Bool wd20_LoadService( twd20WebAgentControlP	 wa,
							 const char				*service  );

sapdbwa_Bool wd20_UnloadService( twd20WebAgentControlP	 wa,
							   const char				*service );

sapdbwa_Bool wd20_LoadAllServices( twd20WebAgentControlP	wa	);

sapdbwa_Bool wd20_UnloadAllServices( twd20WebAgentControlP	wa );

sapdbwa_Bool wd20_AddServiceToList( twd20ServiceListItemP	*firstService,
								    twd40UserDllP			 userDll       );

sapdbwa_Bool wd20_DeleteServiceFromList( twd20ServiceListItemP	*firstService,
									   const char				*service       );

twd20ServiceListItemP wd20_FindServiceInList( twd20ServiceListItemP	 firstService,
									   const char		*service       );

twd20ServiceListItemP wd20_GetServiceByLib( twd20ServiceListItemP	firstService,
											HANDLE					libraryHandle );

twd20ServiceListItemP wd20_GetServiceById( twd20ServiceListItemP	firstService,
                                           sapdbwa_UInt4              serviceId );

sapdbwa_Bool wd20ServiceStart( const char	*serviceName,
							 sapdbwa_Bool	 start	      );

sapdbwa_Bool wd20_ClearLogFile( sapdbwa_HttpReplyP			rep,
							  twd20WebAgentControlP wa   );

sapdbwa_Bool wd20_ClearLogFileHTTPRequests( sapdbwa_HttpRequestP  req,
                                            sapdbwa_HttpReplyP	  rep,
							                twd20WebAgentControlP wa   );

sapdbwa_Bool wd20_ClearLogFileHTTPErrors( sapdbwa_HttpRequestP  req,
                                          sapdbwa_HttpReplyP	rep,
							              twd20WebAgentControlP wa   );

sapdbwa_Bool wd20_SendNavigationScript( sapdbwa_HttpReplyP			rep,
									  twd20WebAgentControlP	wa   );

sapdbwa_Bool wd20_NewCOMService( sapdbwa_HttpReplyP	rep  );

sapdbwa_Bool wd20_CreateNewCOMService( twd20WebAgentControlP	wa,
									 sapdbwa_HttpRequestP			req,
									 sapdbwa_HttpReplyP			rep  );

sapdbwa_Bool wd20_UpdateCOMService( sapdbwa_HttpRequestP	req,
								  sapdbwa_HttpReplyP		rep  );

sapdbwa_Bool wd20_DeleteCOMServiceGetConfirmation( sapdbwa_HttpRequestP	req,
												 sapdbwa_HttpReplyP	rep  );

sapdbwa_Bool wd20_DeleteCOMService( sapdbwa_HttpRequestP	req,
								  sapdbwa_HttpReplyP		rep  );

sapdbwa_Bool wd20_NewCOMParameter( sapdbwa_HttpRequestP		req,
								 sapdbwa_HttpReplyP		rep  );

sapdbwa_Bool wd20_CreateNewCOMParameter( sapdbwa_HttpRequestP	req,
									   sapdbwa_HttpReplyP		rep  );

sapdbwa_Bool wd20_ShowCOMParametersToDelete( sapdbwa_HttpRequestP	req,
										   sapdbwa_HttpReplyP		rep  );

sapdbwa_Bool wd20_DeleteCOMParameters( sapdbwa_HttpRequestP	req,
									 sapdbwa_HttpReplyP	rep  );

sapdbwa_Bool wd20_IsSessionPoolDependentParameter( const char	*parameter );

sapdbwa_Bool wd20_IsSessionPoolStandardParameter( const char	*parameter );

sapdbwa_Bool wd20_IsCOMServiceDependentParameter( const char	*parameter );

sapdbwa_Bool wd20_DeleteSessionPool( sapdbwa_HttpRequestP	req,
								     sapdbwa_HttpReplyP		rep  );

sapdbwa_Bool wd20_DeleteSessionPoolGetConfirmation ( sapdbwa_HttpRequestP	req,
												     sapdbwa_HttpReplyP		rep,
													 twd20WebAgentControlP	wa	);

sapdbwa_Bool wd20_UpdateSessionPool( twd20WebAgentControlP	wa,
								   sapdbwa_HttpRequestP			req,
								   sapdbwa_HttpReplyP				rep  );

sapdbwa_Bool wd20_CreateNewSessionPool( twd20WebAgentControlP wa,
									  sapdbwa_HttpRequestP		req,
									  sapdbwa_HttpReplyP			rep  );

sapdbwa_Bool wd20_NewSessionPool( sapdbwa_HttpReplyP	rep,
								  twd20WebAgentControlP wa	);

void wd20_ShowSessionPool( twd20WebAgentControlP wa,
						   sapdbwa_HttpRequestP		 req,
						   sapdbwa_HttpReplyP			 rep,
						   const char			*name,
						   const char			*message );

void wd20_ShowCOMService( twd20WebAgentControlP	wa,
						  sapdbwa_HttpRequestP		req,
					      sapdbwa_HttpReplyP			rep  );

sapdbwa_Bool wd20_ShowParametersToDelete( sapdbwa_HttpRequestP	req,
										  sapdbwa_HttpReplyP	rep,
										  twd20WebAgentControlP	wa	);

sapdbwa_Bool wd20_DeleteParameters( twd20WebAgentControlP	wa,
								  sapdbwa_HttpRequestP		req,
								  sapdbwa_HttpReplyP			rep  );

sapdbwa_Bool wd20_DeleteService( sapdbwa_HttpRequestP	req,
							   sapdbwa_HttpReplyP		rep  );

sapdbwa_Bool wd20_DeleteServiceGetConfirmation( sapdbwa_HttpRequestP	req,
											    sapdbwa_HttpReplyP		rep,
												twd20WebAgentControlP	wa  );

sapdbwa_Bool wd20_CreateNewService( twd20WebAgentControlP	wa,
								  sapdbwa_HttpRequestP		req,
								  sapdbwa_HttpReplyP			rep  );

sapdbwa_Bool wd20_NewService( sapdbwa_HttpReplyP	rep,
							  twd20WebAgentControlP	wa  );

sapdbwa_Bool wd20_IsServiceStandardParameter( const char	*parameter );

sapdbwa_Bool wd20_IsCOMServiceStandardParameter( const char	*parameter );

sapdbwa_Bool wd20_IsServiceDependentParameter( const char	*parameter );

sapdbwa_Bool wd20_NewServiceParameter( sapdbwa_HttpRequestP		req,
                                       sapdbwa_HttpReplyP		rep,
								       twd20WebAgentControlP	wa	);

sapdbwa_Bool wd20_CreateNewParameter( twd20WebAgentControlP	wa,
									sapdbwa_HttpRequestP			req,
									sapdbwa_HttpReplyP			rep  );

const char* wd20_GetHTMLParameter( sapdbwa_HttpRequestP	 req,
								   const char		*parameterName );

void wd20_SendServerError( sapdbwa_HttpReplyP rep );

sapdbwa_Bool wd20_GetRegistryValue( Reg_RegistryP	 registry,
								  const char	*section,
								  const char	*key,
								  char			*value,
								  sapdbwa_Int4	 valueSize,
								  const char	*defaultValue );

sapdbwa_Bool wd20_DeleteRegistrySection( const char	*section );

sapdbwa_Bool wd20_DeleteRegistryKey( const char	*section,
								   const char	*key	  );

void wd20_ApiCleanUp();

sapdbwa_Bool wd20_ReadGlobalParameters( twd20WebAgentControlP wa,
                                      Reg_RegistryP			registry );


sapdbwa_Bool wd20_CheckHttpApi( twd20WebAgentControlP wa );


twd40UserDllP wd20_FindUserDll( twd20WebAgentControlP  wa,
                                char                  *uri );

#ifdef HAVE_COM_CLASSES
sapdbwa_Bool wd20_CreateComClasses( twd20WebAgentControlP wa,
                                  Reg_RegistryP			registry );
sapdbwa_Bool wd20_InitComClasses( twd20WebAgentControlP	wa,
                                Reg_RegistryP			registry );

void wd20_FreeComClasses( twd20WebAgentControlP wa );

twd96ComClassP wd20_FindComClass( twd20WebAgentControlP  wa,
                                  char                  *uri );
#endif /* HAVE_COM_CLASSES */

sapdbwa_Bool wd20_IsPrefix( char       *str1,
                          char       *str2,
                          sapdbwa_Int4 *prefixLength );


sapdbwa_Bool wd20_MakeArray( Reg_RegistryP   registry,
                           const char	  *section,
                           char			***arrayP,
                           sapdbwa_UInt4	  *numElemP,
                           twd26ErrP	   err		);

void wd20_FreeArray( char **array, sapdbwa_UInt4 numElem );

sapdbwa_Bool wd20_StringsAreUnique( char         **array,
                                  sapdbwa_UInt4    n,
                                  sapdbwa_UInt4   *nonUnique );

void wd20_GetConf( twd20WebAgentControlP  wa,
                   char                  *textBuffer,
                   sapdbwa_UInt4            bufferLen );

sapdbwa_Bool wd20_GetOdbcIsolation( const char *poolName,
                                  const char *isolationStr,
                                  sapdbwa_Int4 *isolation,
                                  twd26ErrP   err );

sapdbwa_Bool wd20_GetSqlIsolation( const char *poolName,
                                 const char *isolationStr,
                                 sapdbwa_Int4 *isolation,
                                 twd26ErrP   err );

sapdbwa_Bool wd20_SetRegistryKey ( const char	*section,
								 const char	*key,
								 const char	*value	  );

sapdbwa_Int2 wd20_HandleWAAdminRequest( sapdbwa_HttpRequestP		req,
                                      sapdbwa_HttpReplyP			rep,
									  twd20WebAgentControlP	wa   );

sapdbwa_Bool wd20_SendText( sapdbwa_HttpReplyP	rep,
						    sapdbwa_Int4		groupId,
						    sapdbwa_Int4		textId   );

sapdbwa_Bool wd20_ShowLogFile( sapdbwa_HttpReplyP			rep,
							 twd20WebAgentControlP	wa   );

sapdbwa_Bool wd20_ShowLogFileHTTPRequests( sapdbwa_HttpReplyP		rep,
							               twd20WebAgentControlP	wa   );

sapdbwa_Bool wd20_ShowLogFileHTTPErrors( sapdbwa_HttpReplyP	    rep,
							             twd20WebAgentControlP	wa   );

sapdbwa_Bool wd20_ShowConfLogFile( sapdbwa_HttpReplyP rep );

sapdbwa_Bool wd20_ShowHeader( sapdbwa_HttpReplyP rep );

sapdbwa_Bool wd20_ShowInitDetails( sapdbwa_HttpReplyP	 rep,
								 const char		*message );

sapdbwa_Bool wd20_ShowPicture( sapdbwa_HttpRequestP			req,
                             sapdbwa_HttpReplyP			rep,
							 twd20WebAgentControlP	wa   );

void wd20_ShowMain( sapdbwa_HttpReplyP			rep,
				    twd20WebAgentControlP	wa );

void wd20_ShowLogSettings( sapdbwa_HttpReplyP	 rep,
						   const char	*message );

void wd20_ShowResourceSettings( sapdbwa_HttpReplyP	 rep,
								const char		*message );

sapdbwa_Bool wd20_UpdateResourceSettings( sapdbwa_HttpRequestP	req,
										sapdbwa_HttpReplyP	rep );

void wd20_ShowGeneralSettings( sapdbwa_HttpReplyP	 rep,
							   const char		*message );

sapdbwa_Bool wd20_UpdateGeneralSettings( sapdbwa_HttpRequestP	req,
									   sapdbwa_HttpReplyP	rep );

sapdbwa_Bool wd20_ShowService( twd20WebAgentControlP	 wa,
							   sapdbwa_HttpRequestP		 req,
							   sapdbwa_HttpReplyP		 rep,
							   const char				*name,
							   const char				*message,
							   const char				*newBrowser );

sapdbwa_Bool wd20_ShowMsgOnly ( twd20WebAgentControlP	 wa,
							    sapdbwa_HttpRequestP	 req,
							    sapdbwa_HttpReplyP		 rep,
							    const char				*message );

void wd20_ShowNavigation( sapdbwa_HttpRequestP	req,
						  sapdbwa_HttpReplyP		rep );

sapdbwa_Bool wd20_UpdateLogSettings( sapdbwa_HttpRequestP	req,
									  sapdbwa_HttpReplyP		rep );

sapdbwa_Bool wd20_UpdateService( twd20WebAgentControlP	wa,
							   sapdbwa_HttpRequestP			req,
							   sapdbwa_HttpReplyP				rep  );

sapdbwa_UInt4 wd20_WAAdminShowFile( sapdbwa_HttpReplyP rep, const char *fileName );

void wd20_SendHtmlHeader( sapdbwa_HttpReplyP rep, const char *title );

void wd20_SendHtmlTrailer( sapdbwa_HttpReplyP rep );

void wd20_StrCatMax( char           *destination,
                     const char     *source,
                     sapdbwa_UInt4   maxLen );

sapdbwa_Bool wd20_EmptyLog( char            *filename,
                            tsp00_Int4        *file,
                            twd27ExclP       fileExcl,
                            sapdbwa_Bool     makeCopy );

sapdbwa_Bool wd20_IsLogFileEmpty( char  *filename );

/*!**********************************************************************

  Chapter: st_wa_control

  description:
  
************************************************************************/

typedef struct st_wa_TemplateValueList			sapdbwa_TemplateValueList;
typedef struct st_wa_TemplateValueListItem		sapdbwa_TemplateValueListItem;
typedef struct st_wa_TemplateValueTable			sapdbwa_TemplateValueTable;
typedef struct st_wa_TemplateValueTableRow		sapdbwa_TemplateValueTableRow;
typedef struct st_wa_TemplateValueTableColumn	sapdbwa_TemplateValueTableColumn;

sapdbwa_Bool CreateTemplateValueList ( sapdbwa_TemplateValueList	**valueList );

sapdbwa_Bool CreateTemplateValueListItem ( sapdbwa_TemplateValueListItem **valueListItem,
									       const char					  *name,
									       const char					  *value,
									       sapdbwa_TemplateValueTable	  *table		);

sapdbwa_Bool CreateTemplateValueTable ( sapdbwa_TemplateValueTable	**table );

sapdbwa_Bool CreateTemplateValueTableRow ( sapdbwa_TemplateValueTableRow	**row );

sapdbwa_Bool CreateTemplateValueTableColumn ( sapdbwa_TemplateValueTableColumn	**column,
										      const char						 *value   );

sapdbwa_Bool DropTemplateValueList ( sapdbwa_TemplateValueList	*valueList );

sapdbwa_Bool DropTemplateValueListItem ( sapdbwa_TemplateValueListItem	*listItem );

sapdbwa_Bool DropTemplateValueTable ( sapdbwa_TemplateValueTable	*table );

sapdbwa_Bool DropTemplateValueTableRow ( sapdbwa_TemplateValueTableRow	*row );

sapdbwa_Bool DropTemplateValueTableColumn ( sapdbwa_TemplateValueTableColumn	*column );

sapdbwa_Bool AddValueToTemplateValueList ( sapdbwa_TemplateValueList	*valueList,
										   const char					*name,
										   const char					*value     );

sapdbwa_Bool AddTableToTemplateValueList ( sapdbwa_TemplateValueList	*valueList,
										  const char					*name,
										  sapdbwa_TemplateValueTable	*table  );

sapdbwa_Bool AddRowToTemplateValueTable ( sapdbwa_TemplateValueTable	*table,
										  sapdbwa_TemplateValueTableRow	*row   );

sapdbwa_Bool AddColumnToTemplateValueTableRow ( sapdbwa_TemplateValueTableRow		*row,
												sapdbwa_TemplateValueTableColumn	*column );

sapdbwa_Bool FindValueInTemplateValueList ( sapdbwa_TemplateValueList	 *valueList,
										    const char					 *name,
											char						**value      );

sapdbwa_Bool FindTableInTemplateValueList ( sapdbwa_TemplateValueList	  *valueList,
										    const char					  *tableName,
											sapdbwa_TemplateValueTable	 **table      );

sapdbwa_Bool WriteTemplate( sapdbwa_HttpReplyP			 rep,
						    const char					*templateName,
							sapdbwa_TemplateValueList	*templateValueList,
							twd26ErrP					 err				);

sapdbwa_Bool SendTemplate( sapdbwa_HttpReplyP			 rep,
						   sapdbwa_TemplateValueList	*valueList,
						   char							*templateStream,
						   twd26ErrP					 err			);

sapdbwa_Bool SendTemplateVariable( sapdbwa_HttpReplyP			 rep,
								   sapdbwa_TemplateValueList	*valueList,
								   char							*templateVariableBegin,
								   char							*templateVariableEnd   );

sapdbwa_Bool SendTemplateTable( sapdbwa_HttpReplyP			 rep,
								sapdbwa_TemplateValueList	*valueList,
								char						*tableBegin,
								char						*tableEnd,
								twd26ErrP					 err		);

sapdbwa_Bool SendTemplateTableRow( sapdbwa_HttpReplyP				 rep,
								   sapdbwa_TemplateValueTableRow	*row,
								   char								*templateRow );

struct st_wa_TemplateValueList {
	sapdbwa_TemplateValueListItem	*listItem;
};

struct st_wa_TemplateValueListItem {
	char							*name;			/* Name of this list item as string.					*/
	char							*value;			/* Value to add if list item is not an object.			*/
	sapdbwa_TemplateValueTable		*table;			/* Table description if list item is a table object.	*/
	sapdbwa_TemplateValueListItem	*nextListItem;	/* Next list item.										*/
};

struct st_wa_TemplateValueTable {
	sapdbwa_TemplateValueTableRow	*row;
};

struct st_wa_TemplateValueTableRow {
	sapdbwa_TemplateValueTableColumn	*column;
	sapdbwa_TemplateValueTableRow		*nextRow;
};

struct st_wa_TemplateValueTableColumn {
	char								*value;
	sapdbwa_TemplateValueTableColumn	*nextColumn;
};

/*==============================================================================================*/

static struct st_wa_control {
    twd20ApiType                apiType;
    char                        iniFile[MAX_FILENAME_LEN+1];                        /* Used ini file / registry path */
    char                        regSectionGlobal[REG_MAX_SECTION_LEN+1];            /* Used registry section */
    char                        regSectionSessionPool[REG_MAX_SECTION_LEN+1];       /* Used registry section */
    char                        regSectionServices[REG_MAX_SECTION_LEN+1];          /* Used registry section */
    char                        regSectionCOMServices[REG_MAX_SECTION_LEN+1];       /* Used registry section */
    char                        regSectionResource[REG_MAX_SECTION_LEN+1];          /* Used registry section */
    char                        regSectionGeneralSettings[REG_MAX_SECTION_LEN+1];   /* Used registry section */
    char                        regSectionHTTP[REG_MAX_SECTION_LEN+1];              /* Used registry section */
    twd20SessionPoolListItemP	sessionPoolList;

    sapdbwa_UInt4			    serviceId;
    twd20ServiceListItemP	    serviceList;

#ifdef HAVE_COM_CLASSES    
    sapdbwa_UInt4	            nComClasses;	/* number of ComClasses in array comClasses	*/
    twd96ComClassP             *comClasses;
#endif
    
    twd37WebSessionContP	    sessionCont;	/* One container for all active WebSessions	*/
    twd25LogP				    waLog;			/* used to write log messages				*/
    twd25LogP				    confLog;		/* used to write the actual config			*/
    twd26ErrP				    waErr;			/* used to store error information			*/

	char			            documentRoot[MAX_FILENAME_LEN_WD00];	    /* Path to the root folder for WebAgent documents 	*/
	char			            mimeTypesFilename[MAX_FILENAME_LEN_WD00];	/* MIME types filename (path included)				*/
	twd91MimeTypesP	            mimeTypes;									/* MIME types										*/

	sapdbwa_Bool                developerMode;	/* If sapdbwa_True, developer mode is set with more functionaliy	*/

    char                        adminUserId[REG_MAX_VALUE_LEN+1];
    char                        adminUserPassword[REG_MAX_VALUE_LEN+1];

    twd27ExclP		            excl;			/* guarantee exclusive access to this structure				*/
    sapdbwa_Bool	            initialized;	/* flag that signals if wd20ApiInit was called successfully	*/
} wd20WAControl;

struct st_wa_service_list_item {
	twd40UserDllP			service;
	twd20ServiceListItemP	nextService;
};

struct st_wa_session_pool_list_item {
	twd30SessionPoolP			sessionPool;
	twd20SessionPoolListItemP	nextSessionPool;
};

struct st_wa_service_description {
	char			 service					[REG_MAX_VALUE_LEN+1];
	char			 withSSL					[REG_MAX_VALUE_LEN+1];
	char			 sslURL						[REG_MAX_VALUE_LEN+1];
	char			 serviceStart				[REG_MAX_VALUE_LEN+1];
	char			 useFastCGIForCookiePath	[REG_MAX_VALUE_LEN+1];
	char			 exitFunction				[REG_MAX_VALUE_LEN+1];
	char			 initFunction				[REG_MAX_VALUE_LEN+1];
	char			 library					[MAX_FILENAME_LEN_WD00];
	char			 libraryType				[REG_MAX_VALUE_LEN+1];
	char			 logFile					[MAX_FILENAME_LEN_WD00];
	char			 serviceFunction			[REG_MAX_VALUE_LEN+1];
	char			 serviceName				[REG_MAX_VALUE_LEN+1];
	char			 sessionPool				[REG_MAX_VALUE_LEN+1];
	char			 webSessionTimeout			[REG_MAX_VALUE_LEN+1];
	twd20_Parameter	*parameterList;
};

struct st_wa_parameter {
	char			 name	[REG_MAX_VALUE_LEN+1];
	char			 value	[REG_MAX_VALUE_LEN+1];
	twd20_Parameter	*nextParameter;
};

struct st_wa_session_pool_description {
	char			 name				[REG_MAX_VALUE_LEN+1];
	char			 serverDB			[REG_MAX_VALUE_LEN+1];
	char			 serverNode			[REG_MAX_VALUE_LEN+1];
	char			 userId				[REG_MAX_VALUE_LEN+1];
	char			 password			[REG_MAX_VALUE_LEN+1];
	char			 datasource			[REG_MAX_VALUE_LEN+1];
	char			 driver				[REG_MAX_VALUE_LEN+1];
	char			 autocommit			[REG_MAX_VALUE_LEN+1];
	char			 type				[REG_MAX_VALUE_LEN+1];
	char			 size				[REG_MAX_VALUE_LEN+1];
	char			 sqlTraceFilename	[MAX_FILENAME_LEN_WD00];
};

/* For wd20ApiDBFSInit. Only used for dbfs command-line tools. */
static twd30SessionPoolP	wd20DBFSSessionPool = NULL;
static sapdbwa_SessionPoolP	wd20DBFSPoolHandle = NULL;

/****************************************************************************
 ***
 *** wd20GetMIMEType
 ***
 ****************************************************************************/

const char* wd20GetMIMEType( const char *extension ) {
	return wd91GetMimeType ( wd20WAControl.mimeTypes, extension );
}

/****************************************************************************
 ***
 *** wd20ApiInit
 ***
 ****************************************************************************/

sapdbwa_Bool wd20ApiInit( const char *iniFile, twd20ApiType apiType )
{

    sapdbwa_Bool	ok = sapdbwa_True;
	Reg_RegistryP	registry;
	twd25LogP		defaultLog;

	sapdbwa_TraceDefFunction ("wd20ApiInit")

    wd20WAControl.apiType = apiType;

    /* Set ini file */
    if (iniFile && strlen(iniFile)) {
        strncpy(wd20WAControl.iniFile, iniFile, MAX_FILENAME_LEN);
        wd20WAControl.iniFile[MAX_FILENAME_LEN] = '\0';
#ifdef WIN32
        sp77sprintf(wd20WAControl.regSectionGlobal, REG_MAX_SECTION_LEN, "%s\\%s", iniFile, "Global");
        sp77sprintf(wd20WAControl.regSectionSessionPool, REG_MAX_SECTION_LEN, "%s\\%s", iniFile, "SessionPools");
        sp77sprintf(wd20WAControl.regSectionServices, REG_MAX_SECTION_LEN, "%s\\%s", iniFile, "Services");
        sp77sprintf(wd20WAControl.regSectionCOMServices, REG_MAX_SECTION_LEN, "%s\\%s", iniFile, "COMServices");
        sp77sprintf(wd20WAControl.regSectionResource, REG_MAX_SECTION_LEN, "%s\\%s", iniFile, "Resources");
        sp77sprintf(wd20WAControl.regSectionGeneralSettings, REG_MAX_SECTION_LEN, "%s\\%s", iniFile, "GeneralSettings");
        sp77sprintf(wd20WAControl.regSectionHTTP, REG_MAX_SECTION_LEN, "%s\\%s", iniFile, "HTTP");
#else
        strcpy(wd20WAControl.regSectionGlobal, ADMIN_REG_SECTION_GLOBAL);
        strcpy(wd20WAControl.regSectionSessionPool, ADMIN_REG_SECTION_SESSION_POOLS);
        strcpy(wd20WAControl.regSectionServices, ADMIN_REG_SECTION_SERVICES);
        strcpy(wd20WAControl.regSectionCOMServices, ADMIN_REG_SECTION_COMSERVICES);
        strcpy(wd20WAControl.regSectionResource, ADMIN_REG_SECTION_RESOURCES);
        strcpy(wd20WAControl.regSectionGeneralSettings, ADMIN_REG_SECTION_GENERAL_SETTINGS);
        strcpy(wd20WAControl.regSectionHTTP, ADMIN_REG_SECTION_HTTP);
#endif
    } else {
        strcpy(wd20WAControl.iniFile, WEBAGENT_REGISTRY);
        strcpy(wd20WAControl.regSectionGlobal, ADMIN_REG_SECTION_GLOBAL);
        strcpy(wd20WAControl.regSectionSessionPool, ADMIN_REG_SECTION_SESSION_POOLS);
        strcpy(wd20WAControl.regSectionServices, ADMIN_REG_SECTION_SERVICES);
        strcpy(wd20WAControl.regSectionCOMServices, ADMIN_REG_SECTION_COMSERVICES);
        strcpy(wd20WAControl.regSectionResource, ADMIN_REG_SECTION_RESOURCES);
        strcpy(wd20WAControl.regSectionGeneralSettings, ADMIN_REG_SECTION_GENERAL_SETTINGS);
        strcpy(wd20WAControl.regSectionHTTP, ADMIN_REG_SECTION_HTTP);
    }

    /* initialize pools for request-,reply- and wa-handle */
    wd21CreateRequestHandlePool();
    wd22CreateReplyHandlePool();
    wd23CreateHandlePool();

    /* initialize for usage of SQL-Class */
    wd39SqlInit();

#ifdef HAVE_COM_CLASSES
    /* initialize for usage of SQL-Class */
    wd96ComInit();
#endif

	wd20_InitControl();
    
	/* Create default log */
	/* With ReadGlobalParameters the log file may be changed! */
	/* Drop this log object if ReadGlobalParameters processed properly! */
    defaultLog = wd25CreateLog( DEFAULT_LOGFILE );

    if (!wd27InitExcl( &wd20WAControl.excl ) || !wd27BegExcl( wd20WAControl.excl )) {
		wd25WriteLnLogMsg( defaultLog, "Control struct could not be locked!" );
        return sapdbwa_False;
    }

	wd20WAControl.serviceId = ADMIN_SESSION_ID_INIT_VALUE;
    wd20WAControl.waErr = wd26CreateErr();
    if (wd20WAControl.waErr == NULL) {
		wd25WriteLnLogMsg( defaultLog, "Error handler could not be initialized!" );
        ok = sapdbwa_False;
    }
	
	if ( ok ) {
        wd20WAControl.sessionCont = wd37CreateWebSessionCont();
        if (wd20WAControl.sessionCont == NULL) {
			wd25WriteLnLogMsg( defaultLog, "WebSession handler could not be initialized!" );
            ok = sapdbwa_False;
        };
    };

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
		wd25WriteLnLogMsg( defaultLog, "Registry could not be read!" );
		ok = sapdbwa_False;
	}

	/* Read settings */
	if ( ok )
		ok = wd20_ReadGlobalParameters( &wd20WAControl, registry );
	if (! ok )
		wd25WriteLnLogMsg( defaultLog, "Global settings could not be read!" );
	else {
		char	buffer[1024];
		sp77sprintf( buffer, 1024, "For more WebAgent log entries see also: %s",
					 wd25GetLogFilename( wd20WAControl.waLog ));
		wd25WriteLnLogMsg( defaultLog, buffer );

		wd25DestroyLog( defaultLog );
		defaultLog = NULL;
	}

	/* Load all session pools */
    if ( ok )
		ok = wd20_LoadAllSessionPools( &wd20WAControl );

	/* Load all auto start services */
	if ( ok )
		ok = wd20_LoadAllServices( &wd20WAControl );

	/* Load all COM classes */
#ifdef HAVE_COM_CLASSES     
    if ( ok )
		ok = wd20_CreateComClasses( &wd20WAControl, registry );
	if ( ok )
		ok = wd20_InitComClasses( &wd20WAControl, registry );
#endif

	if ( ok ) {
        char textBuffer[8000];
        
        wd20WAControl.initialized = sapdbwa_True;

        wd25WriteLogMsg( wd20WAControl.waLog, "Scanning ini-file: Success\n" );

        /* write actual configuration in extra log file */
        wd20_GetConf( &wd20WAControl,
                      textBuffer,
                      sizeof( textBuffer ));

        wd25EmptyLog(wd20WAControl.confLog, sapdbwa_False );
        wd25WriteLogMsg( wd20WAControl.confLog, textBuffer );

        /* set random seed */
        srand( (unsigned) time( NULL ) );
		sapdbwa_TraceAdd( "Random started.");
	} else {
        /* Initializing failure*/
		if ( wd20WAControl.waLog ) {
			wd25WriteLogMsg( wd20WAControl.waLog,
							 "Scanning ini-file: Failure\n" );
			wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );
		} else if ( defaultLog ) {
			wd25WriteLogMsg( defaultLog, "Scanning ini-file: Failure\n" );
			wd26LogErr( wd20WAControl.waErr, defaultLog );
		}
		sapdbwa_TraceAdd( "Random not started.");
    };

	/* Close registry */
	Reg_CloseRegistry( registry );
    
    wd27EndExcl( wd20WAControl.excl );
    
    return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20ApiReInit
 ***
 ****************************************************************************/

sapdbwa_Bool wd20ApiReInit( const char *iniFile )
{

	sapdbwa_TraceDefFunction ("wd20ApiReInit")

	if (!wd20WAControl.initialized) {
        return wd20ApiInit( iniFile, wd20WAControl.apiType );
    } else {
        return sapdbwa_True;;
    }; /* else */

}

/****************************************************************************
 ***
 *** wd20ApiDBFSInit
 ***
 ****************************************************************************/

sapdbwa_SessionPoolP wd20ApiDBFSInit( twd26ErrP err ) {

	twd20SessionPoolListItemP	currentListItem;

	sapdbwa_TraceDefFunction ("wd20ApiDBFSInit")

	/* Inits */
    wd20WAControl.apiType = API_DBFS_WD20;    
	wd20_InitControl();
    wd20WAControl.waErr         = err;

	/* Create log object */
    wd20WAControl.waLog         = wd25CreateLog( DEFAULT_DBFS_LOGFILE );
    if (wd20WAControl.waLog == NULL) {
        return NULL;
    };

	/* Load session pool */
	if ( !wd20_LoadSessionPool( &wd20WAControl, ADMIN_DBFS_SESSION_POOL_NAME )) {
        wd26LogErr( err, wd20WAControl.waLog );

		return NULL;
	}

	/* Get session pool object */
	currentListItem = wd20_FindSessionPoolInList( wd20WAControl.sessionPoolList, ADMIN_DBFS_SESSION_POOL_NAME );
	if ( !currentListItem || !currentListItem->sessionPool ) {
        wd26LogErr( err, wd20WAControl.waLog );

		return NULL;
	}

	/* Create pool handle */
	wd20DBFSSessionPool = currentListItem->sessionPool;
    wd20DBFSPoolHandle = wd32CreatePoolHandle( wd20DBFSSessionPool, 0 );
    if (wd20DBFSPoolHandle == NULL) {
        wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
        wd26LogErr( err, wd20WAControl.waLog );

		return NULL;
    };

    return wd20DBFSPoolHandle;

}

/****************************************************************************
 ***
 *** wd20ApiExit
 ***
 ****************************************************************************/

void wd20ApiExit()
{

	sapdbwa_TraceDefFunction ("wd20ApiExit")

    wd27BegExcl( wd20WAControl.excl );
    wd20_ApiCleanUp();
    wd22DestroyReplyHandlePool();
    wd21DestroyRequestHandlePool();
    wd23DestroyHandlePool();    
    wd27EndExcl( wd20WAControl.excl );
    wd27ExitExcl( wd20WAControl.excl );
    wd39SqlFinish();

}

/****************************************************************************
 ***
 *** wd20ApiDBFSExit
 ***
 ****************************************************************************/

void wd20ApiDBFSExit()
{

	sapdbwa_TraceDefFunction ("wd20ApiDBFSExit")

    if (wd20DBFSSessionPool != NULL) {
		wd20_UnloadSessionPool( &wd20WAControl, ADMIN_DBFS_SESSION_POOL_NAME );
    }; /* if */

    if (wd20DBFSPoolHandle != NULL) {
        wd32DestroyPoolHandle( wd20DBFSPoolHandle );
    }; /* if */

    if (wd20WAControl.waLog != NULL) {
        wd25DestroyLog( wd20WAControl.waLog );
    }; /* if */

    /* do not destroy waErr it is is owned by twd101DBFS */

}

/****************************************************************************
 ***
 *** wd20CallServiceFunc
 ***
 ****************************************************************************/

sapdbwa_Int2 wd20CallServiceFunc( twd21HttpRequestP req,
                                sapdbwa_HttpReplyP      rep )
{

        char           *uri = NULL;
    twd40UserDllP   userDll = NULL;
#ifdef HAVE_COM_CLASSES
    twd96ComClassP  comClass = NULL;
#endif    
    sapdbwa_Handle    waHandle;
    sapdbwa_Int2      serviceFuncRetcode = sapdbwa_RTC_Ok;
    sapdbwa_Bool      useWebSessions = sapdbwa_False;
    twd37SessionId  newSessionId;
    sapdbwa_WebSessionP  webSession   = NULL;
    sapdbwa_Bool      timedOut = sapdbwa_False; /* did our web-session time out ? */
    sapdbwa_Int4      prefixLen;
	char		   *sslURL = NULL;

	sapdbwa_TraceDefFunction ("wd20CallServiceFunc")

/*printf(">> wd20CallServiceFunc()");*/

/*printf(">> wd20CallServiceFunc()::sapdbwa_GetRequestURI()");*/
    uri = (char*) sapdbwa_GetRequestURI( req );
    if (*uri == URLPATH_SEP_WD00) {
        uri++; /* step over '/' */
    };
    /* check for special waadmin request */
/*printf(">> wd20CallServiceFunc()::wd20_IsPrefix()");*/
    if (wd20_IsPrefix( ADMIN_SERVICE_NAME, uri, &prefixLen )) {
        /* handle special URI for status info */
        twd26ErrP err = wd26CreateErr();

        if (err != NULL) {
            /* req and rep don't have own error object */
            wd21SetErr( req, err );
            wd22SetErr( rep, err );

			/* Handle admin request */
            serviceFuncRetcode = wd20_HandleWAAdminRequest( req, rep, &wd20WAControl );

            wd26DestroyErr( err );
        } else {
            /* memory allocation error */
            serviceFuncRetcode = sapdbwa_RTC_Error;
        };
        
        return serviceFuncRetcode;
    }; /* if */

/*printf(">> wd20CallServiceFunc()::if (!wd20WAControl.initialized)");*/
    if (!wd20WAControl.initialized) {
        /* we are not initialized (possibly scanning of ini-file failed) */
        return sapdbwa_RTC_Error;
    }; /* if */
    
    /* determine UselDll that has to handle the request */
#if 0    
    if (wd20WAControl.apiType == API_FCGI_WD20) {
        /* skip wafcgi.fcgi/ */
        while (*uri != '\0' && *uri != URLPATH_SEP_WD00) {
            uri++;
        };
        if (*uri == URLPATH_SEP_WD00) {
            uri++;
        };
    };
#endif    
/*printf(">> wd20CallServiceFunc()::wd20_FindUserDll()");*/
    userDll = wd20_FindUserDll( &wd20WAControl, uri );
    if (userDll == NULL) {
#ifdef HAVE_COM_CLASSES
        comClass = wd20_FindComClass( &wd20WAControl, uri );
        if (comClass == NULL)
#endif            
        /* anybody wants to handle this request let the server handle it */
        wd25WriteLogInfoMsg(wd20WAControl.waLog, uri, sapdbwa_True);
        wd25WriteLogInfoMsg(wd20WAControl.waLog, "Not handled by a WebAgent service", sapdbwa_False);

        return sapdbwa_RTC_Ignored;
    }; /* if */

    /* set cookie if web sessions should be used */
#ifdef HAVE_COM_CLASSES
    if (userDll == NULL) {
        useWebSessions = sapdbwa_False;
    } else
#endif    
/*printf(">> wd20CallServiceFunc()::wd40GetUseWebSessions()");*/
    useWebSessions = wd40GetUseWebSessions( userDll );
    if (useWebSessions) {
        const char  *oldSessionId = NULL;
        char         cookiePath[MAX_NAME_LEN_WD00+2];

        /* Check timeout value of all open web sessions */
        wd37CheckTimeout(wd20WAControl.sessionCont);

        oldSessionId = sapdbwa_GetCookieValue( req, SID_COOKIE_WD00 );
        if (oldSessionId != NULL) {
            webSession   = wd37GetWebSession( wd20WAControl.sessionCont,
                                              oldSessionId, wd40GetUserDllId( userDll ), &timedOut );
        }; /* if */

        if (webSession == NULL) {
            /* May be that a web session should be started so set cookie. */
            wd37GetNewSessionId( wd20WAControl.sessionCont, newSessionId );
        } else {
            /* the session-id may have changed (if WANewSessionId was called ) */
            wd37GetSessionId( webSession, newSessionId );
        }; /* else */

		/* If FastCGI will be used and there is no user defined restriction	*/
		/* to use the FastCGI prefix for cookie pathes use 'wafcgi.fcgi'	*/
		/* as the prefix for cookie pathes									*/
        if ( wd20WAControl.apiType == API_FCGI_WD20 && wd40UseFastCGIForCookiePath( userDll )) {
            strcpy( cookiePath, "/wafcgi.fcgi/" );
        } else {
            strcpy( cookiePath, "/" );
        };
		
        strcat( cookiePath, wd40GetPathPrefix( userDll ));
/*printf(">> wd20CallServiceFunc()::sapdbwa_SetCookie()");*/
        if (!sapdbwa_SetCookie( rep, SID_COOKIE_WD00, newSessionId,
                          NULL, cookiePath, NULL, sapdbwa_False )) {
            wd26SetErr( wd20WAControl.waErr, ERR_CANT_SET_COOKIE_WD26,
                        SID_COOKIE_WD00, NULL );
            wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );
            return sapdbwa_RTC_Error;
        }; /* if */
        sapdbwa_SetHeader( rep, "Cache-control", "no-cache=\"Set-cookie\"" );
    }; /* if */
    
    /* create sapdbwa_Handle that is passed to the UserDll */
/*printf(">> wd20CallServiceFunc()::wd23CreateHandle()");*/
    waHandle = wd23CreateHandle( userDll,
                                     wd20WAControl.sessionCont,
                                     webSession,
                                     &newSessionId[0],
                                     timedOut );
    if (waHandle == NULL) {
        /* memory allocation error */
        wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, NULL, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );
        return sapdbwa_RTC_Error;
    }; /* if */
    
    /* req and rep report errors to the error object in wa */
    wd21SetErr( req, wd23GetErr( waHandle ) );
    wd22SetErr( rep, wd23GetErr( waHandle ) );
    
#ifdef HAVE_COM_CLASSES
    if (comClass != NULL) {
        /* call ServiceFunc in ComClass */
        if (!wd96CallServiceFunc( comClass,
                                  waHandle, req, rep,
                                  &serviceFuncRetcode,
                                  wd20WAControl.waLog,
                                  wd20WAControl.waErr )) {
            wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );
        }; /* else */        
    } else
#endif

	/* Check security */
/*printf(">> wd20CallServiceFunc()::wd40GetPropertyWithSSL()");*/
	if ( wd40GetPropertyWithSSL( userDll ) && !sapdbwa_IsSecure( req )) {
		if (( sslURL = wd40GetPropertySSLURL( userDll )) != NULL && strlen( sslURL ) > 0 ) {
			sapdbwa_InitHeader( rep, sapdbwa_Status_Redirect, NULL, NULL, NULL, NULL, sslURL );
			sapdbwa_SendHeader( rep );

			wd23DestroyHandle( waHandle );

			return sapdbwa_RTC_Accepted;
		} else {
			wd26SetErr( wd20WAControl.waErr, ERR_NO_SSL_URL_SPECIFIED_WD26, NULL, NULL );
			wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

			sapdbwa_InitHeader( rep, sapdbwa_Status_NotFound, NULL, NULL, NULL, NULL, NULL );
			sapdbwa_SendHeader( rep );

			wd23DestroyHandle( waHandle );

			return sapdbwa_RTC_Accepted;
		}
	}

    /* call ServiceFunc in UserDll */
/*printf(">> wd20CallServiceFunc()::wd40CallServiceFunc");*/
    TRY {
    if (!wd40CallServiceFunc( userDll,
                              waHandle, req, rep,
                              &serviceFuncRetcode,
                              wd20WAControl.waLog,
                              wd20WAControl.waErr )) {
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );
    }; /* if */
    } CATCH;
/*printf(">> wd20CallServiceFunc()::wd23DestroyHandle()");*/
    wd23DestroyHandle( waHandle );
    
	sapdbwa_TraceAdd( "Exiting function." );

/*printf("<< wd20CallServiceFunc()");*/
    return serviceFuncRetcode;

} /* wd20CallServiceFunc */



sapdbwa_Bool wd20CallSessionDataDestr( sapdbwa_UInt4       serviceId,
                                       sapdbwa_Destructor  destructor,
                                       void               *data )
{
    sapdbwa_Bool          ok = sapdbwa_True;
    sapdbwa_Handle        waHandle = NULL;
    twd20ServiceListItemP serviceItem = NULL;

	sapdbwa_TraceDefFunction ("wd20CallSessionDataDestr")

    serviceItem = wd20_GetServiceById( wd20WAControl.serviceList, serviceId );
    if (serviceItem != NULL) {
        /* create sapdbwa_Handle that is passed to the UserDll */
        waHandle = wd23CreateHandle( serviceItem->service,
                                       wd20WAControl.sessionCont,
                                       NULL,
                                       NULL,
                                       sapdbwa_False );
        
        TRY {
        ok = wd40CallSessionDataDestr( serviceItem->service,
                                       waHandle,
                                       destructor,
                                       data,
                                       wd20WAControl.waLog,
                                       wd20WAControl.waErr );
        } CATCH;

        wd23DestroyHandle( waHandle );
    } else {
        /* service with given id does not exist */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_INTERNAL_WD26,
                    "wd20CallSessionDataDestr: "
                    "Service with given id does not exist.",
                    NULL );
        
        ok = sapdbwa_False;
    };

    return ok;
} /* wd20CallSessionDataDestr */

sapdbwa_Bool WA_GetParam( sapdbwa_Handle    wa,
                        const char  *key,
                        char        *valueBuf,
                        sapdbwa_Int4   bufLen )
{
	char* p=0;

	*p = 123;

	return sapdbwa_True;
}

sapdbwa_Bool WINAPI WA_WriteLogMsg( sapdbwa_Handle wa, const char *msg )
{
	char* p=0;

	*p = 123;

	return sapdbwa_True;
}

sapdbwa_Bool wd20GetParam( const char   *section,
                           const char   *key,
                           char         *valueBuf,
                           sapdbwa_Int4  bufLen,
                           sapdbwa_Bool  isCOMService )
{

    Reg_RegistryP	registry = NULL;
	char		    currentSection [REG_MAX_SECTION_LEN+1];

	sapdbwa_TraceDefFunction ("wd20GetParam")
    
	/* Get current section */
	if ( isCOMService ) {
		sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wd20WAControl.regSectionCOMServices, section );
	} else {
		sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wd20WAControl.regSectionServices, section );
	}
    
	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile ))
		return sapdbwa_False;

	/* Get registry entry */
    if ( !Reg_GetRegistryKey( registry, currentSection, key, valueBuf, bufLen )) {
		Reg_CloseRegistry( registry );
		return sapdbwa_False;
	}

	/* Close registry */
	Reg_CloseRegistry( registry );

    return sapdbwa_True;

}


void wd20WriteLogMsg( char *msg )
{

	sapdbwa_TraceDefFunction ("wd20WriteLogMsg")

    if (wd20WAControl.initialized && wd20WAControl.waLog != NULL) {
        wd25WriteLogMsg( wd20WAControl.waLog, msg );
    }; /* if */

} /* wd20WriteLogMsg */


/*!**********************************************************************

  EndChapter: st_wa_control

************************************************************************/

/*!**********************************************************************

  Chapter: st_wa_control internal functions

************************************************************************/


void wd20_ApiCleanUp()
{

	sapdbwa_TraceDefFunction ("wd20_ApiCleanUp")

	if ( wd20WAControl.mimeTypes ) {
		wd91DestroyMimeTypes( wd20WAControl.mimeTypes );
        wd20WAControl.mimeTypes = NULL;
    };

    /* sessionpools have to be freed before unloading the user-dlls
     * because user-data destructors code lies in the user-dlls */
    wd20_UnloadAllSessionPools( &wd20WAControl );
    if ( wd20WAControl.sessionCont != NULL ) {
        wd37DestroyWebSessionCont(  wd20WAControl.sessionCont );
        wd20WAControl.sessionCont = NULL;
    }; /* if */

    wd20_UnloadAllServices( &wd20WAControl );

#ifdef HAVE_COM_CLASSES
    wd20_FreeComClasses( &wd20WAControl );
#endif

    wd26DestroyErr( wd20WAControl.waErr );

    if ( wd20WAControl.waLog != NULL ) {
        wd25WriteLogMsg( wd20WAControl.waLog,
                         "WebAgent exiting  --  Good Bye\n" );
    }; /* if */

    if ( wd20WAControl.waLog != NULL ) {
        wd25DestroyLog( wd20WAControl.waLog );
        wd20WAControl.waLog = NULL;
    };

    if ( wd20WAControl.confLog != NULL ) {
        wd25DestroyLog( wd20WAControl.confLog );
        wd20WAControl.confLog = NULL;
    };

#ifdef HAVE_COM_CLASSES
    wd96ComExit();
#endif    

    wd20WAControl.initialized = sapdbwa_False;

}


/****************************************************************************
 ***
 *** wd20_ReadGlobalParameters
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ReadGlobalParameters( twd20WebAgentControlP   wa,
                                        Reg_RegistryP           registry )

{
    char			logFilename		[MAX_FILENAME_LEN_WD00+1];
	char			developerMode	[REG_MAX_VALUE_LEN+1];
	char			logWithInfo     [REG_MAX_VALUE_LEN+1];
    sapdbwa_Int4    pathLen = 0;

	sapdbwa_TraceDefFunction ("wd20_ReadGlobalParameters")

	/* Get log filename */
    logFilename[0] = '\0';
	if ( !wd20_GetRegistryValue( registry, wa->regSectionGlobal, ADMIN_REG_KEY_LOGFILE,
								 logFilename, sizeof(logFilename), DEFAULT_LOGFILE )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionGlobal, ADMIN_REG_KEY_LOGFILE );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}
    
	/* Create log */
    wa->waLog = wd25CreateLog( logFilename );
    if (wa->waLog == NULL) {
        wd26SetErr( wa->waErr, ERR_MEMORY_WD26, NULL, NULL );
        
        return sapdbwa_False;
    };

    /* Get configuration log filename */
    logFilename[0] = '\0';
	if ( !wd20_GetRegistryValue( registry, wa->regSectionGlobal, ADMIN_REG_KEY_CONF_LOGFILE,
								 logFilename, sizeof(logFilename), DEFAULT_CONF_LOGFILE )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionGlobal, ADMIN_REG_KEY_CONF_LOGFILE );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Create configuration log */
    wa->confLog = wd25CreateLog( logFilename );
    if (wa->confLog == NULL) {
        wd26SetErr( wa->waErr, ERR_MEMORY_WD26, NULL, NULL );
        return sapdbwa_False;
    };

	/* Get path of document root */
    wa->documentRoot[0] = '\0';
	if ( !wd20_GetRegistryValue( registry, wa->regSectionResource, ADMIN_REG_KEY_WA_ROOT,
								 wa->documentRoot, sizeof( wa->documentRoot ), "" )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionResource, ADMIN_REG_KEY_WA_ROOT );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Replace slashes */
	wd09BackSlashToSlash( wa->documentRoot );

	/* Set path */
	pathLen = strlen(wa->documentRoot);
	/* In case of using path with a name or another path, you have to add a slash! */
	/* The slash is not added by default, because there might be situations where you just want the  path */
	if ( pathLen > 0 && wa->documentRoot[pathLen-1] == '/' )
		wa->documentRoot[pathLen-1] = '\0';

	/* Get path of mime types file */
    wa->mimeTypesFilename[0] = '\0';
	if ( !wd20_GetRegistryValue( registry, wa->regSectionResource, ADMIN_REG_KEY_MIME_TYPES,
								 wa->mimeTypesFilename, sizeof( wa->mimeTypesFilename ), DEFAULT_MIME_TYPES_FILENAME )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionResource, ADMIN_REG_KEY_MIME_TYPES );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Get MIME type */
	wa->mimeTypes = wd91CreateMimeTypes( wa->mimeTypesFilename );

	/* Get developer mode */
    developerMode[0] = '\0';
	if ( !wd20_GetRegistryValue( registry, wa->regSectionGeneralSettings, ADMIN_REG_KEY_DEVELOPER_MODE,
								 developerMode, sizeof( developerMode ), "0"  )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionGeneralSettings, ADMIN_REG_KEY_DEVELOPER_MODE );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Set developer mode */
	if ( strcmp( developerMode, ADMIN_DEVELOPER_MODE_ON_VALUE ) == 0 ) {
		wa->developerMode = sapdbwa_True;
    } else {
		wa->developerMode = sapdbwa_False;
	}

	/* Get log with info mode */
    logWithInfo[0] = '\0';
	if ( !wd20_GetRegistryValue( registry, wa->regSectionGlobal, ADMIN_REG_KEY_LOG_WITH_INFO,
								 logWithInfo, sizeof(logWithInfo), "0"  )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionGlobal, ADMIN_REG_KEY_LOG_WITH_INFO);

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Set log with info mode */
	if ( strcmp(logWithInfo, ADMIN_DEVELOPER_MODE_ON_VALUE ) == 0 ) {
        wd25SetLogInfoFlag(wa->waLog, sapdbwa_True);
    } else {
        wd25SetLogInfoFlag(wa->waLog, sapdbwa_False);
	}

	/* Get admin user and password */
    wa->adminUserId[0] = '\0';
	if (!wd20_GetRegistryValue(registry, wa->regSectionGeneralSettings, ADMIN_REG_KEY_ADMIN_USER_ID,
                               wa->adminUserId, sizeof(wa->adminUserId), "")) {
        wd26SetErr(wa->waErr,
                   ERR_READ_REGISTRY_WD26, wa->regSectionGeneralSettings, ADMIN_REG_KEY_ADMIN_USER_ID);
		sapdbwa_TraceAdd( "Exiting unexpected!" )

		return sapdbwa_False;
	}
    
    wa->adminUserPassword[0] = '\0';
	if (!wd20_GetRegistryValue(registry, wa->regSectionGeneralSettings, ADMIN_REG_KEY_ADMIN_USER_PASSWORD,
                               wa->adminUserPassword, sizeof(wa->adminUserPassword), "")) {
        wd26SetErr(wa->waErr,
                   ERR_READ_REGISTRY_WD26, wa->regSectionGeneralSettings, ADMIN_REG_KEY_ADMIN_USER_PASSWORD);
		sapdbwa_TraceAdd( "Exiting unexpected!" )

		return sapdbwa_False;
	}
    
    return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20_FindUserDll
 ***
 ****************************************************************************/

twd40UserDllP wd20_FindUserDll( twd20WebAgentControlP  wa,
                                char                  *uri )
{
    sapdbwa_UInt4		iUserDll;
    twd40UserDllP	userDll = NULL;
    sapdbwa_Int4		prefixLength = 0;
    sapdbwa_Int4		maxPrefixLength = 0;
	twd20ServiceListItemP	currentService;

	sapdbwa_TraceDefFunction ("wd20_FindUserDll")
    
	currentService = wa->serviceList;
    while ( currentService ) {
        if ( wd20_IsPrefix( wd40GetPathPrefix( currentService->service ), uri, &prefixLength )) {
            if (prefixLength > maxPrefixLength) {
                userDll = currentService->service;
                maxPrefixLength = prefixLength;
            };
        };

		currentService = currentService->nextService;
    };

    return userDll;
}


sapdbwa_Bool wd20_IsPrefix( char       *str1,
                          char       *str2,
                          sapdbwa_Int4 *prefixLength )
{
    sapdbwa_UInt4 i = 0;

	sapdbwa_TraceDefFunction ("wd20_IsPrefix")

    if (strlen(str1) > strlen(str2))
        return sapdbwa_False;
    
    while (i<strlen(str1) && toupper(str1[i])==toupper(str2[i])) {
        i++;
    }; /* while */
    *prefixLength = i;
    
    return (i == strlen(str1));
} /* wd20_IsPrefix */


#define ARRAY_ALLOC_INC  10

sapdbwa_Bool wd20_MakeArray( Reg_RegistryP     registry,
                             const char       *section,
                             char           ***arrayP,
                             sapdbwa_UInt4    *numElemP,
                             twd26ErrP         err )
{
    char		**array = NULL;
    sapdbwa_Bool	  allocatOK;
    sapdbwa_Bool	  makeArrayOK = sapdbwa_True;
    sapdbwa_UInt4	  allocArraySize = 0;
    sapdbwa_UInt4	  actIndex;
    char		  key[MAX_INI_PAR_LEN_WD00+1];
    sapdbwa_Int4	  len;
	sapdbwa_Bool	  eos;

	sapdbwa_TraceDefFunction ("wd20_MakeArray")
    
    sapdbwa_SQLALLOCAT( ARRAY_ALLOC_INC * sizeof(char*),
                   (sapdbwa_UInt1**) &array, &allocatOK );
    if (!allocatOK) {
        /* memory alloc failure */
        wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
        return sapdbwa_False;
    }; /* if */
    allocArraySize = ARRAY_ALLOC_INC;

    if (!Reg_EnumRegistrySections( registry, section, NULL )) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) array );
        wd26SetErr( err, ERR_READ_REGISTRY_WD26, section, NULL );
        
		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
    };

    actIndex = 0;
    do {
        if ( !Reg_GetNextSection( registry, NULL, 0, &key[0], sizeof(key), &eos ))
			break;
            
        len = strlen( key );
        if (len>0) {
            if (actIndex+1 > allocArraySize) {
                /* realloc array */
                allocArraySize += ARRAY_ALLOC_INC;
                sapdbwa_SQLREALLOCAT( allocArraySize * sizeof(char*),
                                 (sapdbwa_UInt1**) &array, &allocatOK );
                if (!allocatOK) {
                    /* memory alloc failure */
                    wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
                    return sapdbwa_False;
                };
            };
            sapdbwa_SQLALLOCAT( len * sizeof(char) + 1,
                           (sapdbwa_UInt1**) &array[actIndex],
                           &allocatOK );
            if (!allocatOK) {
                wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
				return sapdbwa_False;
            }; /* if */
            strcpy( array[actIndex], key );
            actIndex++;
        };
    } while ( !eos );

    *arrayP   = array;
    *numElemP = actIndex;
    
    return sapdbwa_True;

}


sapdbwa_Bool wd20_StringsAreUnique( char         **array,
                                  sapdbwa_UInt4    n,
                                  sapdbwa_UInt4   *nonUnique )
{

    sapdbwa_UInt4 i,j;

	sapdbwa_TraceDefFunction ("wd20_StringsAreUnique")

    for (i=1; i < n; i++) {
        for (j=0; j < i; j++) {
            if (strcmp( array[i], array[j] ) == 0) {
                *nonUnique = j;
                return sapdbwa_False;
            }; /* if */
        }; /* for */
    }; /* for */

    return sapdbwa_True;;
} /* wd20_StringsAreUnique */


void wd20_FreeArray( char **array, sapdbwa_UInt4 numElem )
{
    sapdbwa_UInt4 actIndex;

	sapdbwa_TraceDefFunction ("wd20_FreeArray")

    for (actIndex=0; actIndex<numElem; actIndex++) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) array[actIndex] );
    }; /* for */
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) array );
} /* wd20_FreeArray */


void wd20_GetConf( twd20WebAgentControlP  wa,
                   char                  *textBuffer,
                   sapdbwa_UInt4            bufferLen )
{
    sapdbwa_UInt4		freeSize = bufferLen;
    sapdbwa_UInt4		iPool;
    sapdbwa_UInt4		iUserDll;
    sapdbwa_UInt4		iComClass;
    sapdbwa_UInt4		oldLen;
    sapdbwa_Bool		ok = sapdbwa_True;

	twd20ServiceListItemP		currentService = NULL;
	twd20SessionPoolListItemP	currentSessionPool = NULL;

	sapdbwa_TraceDefFunction ("wd20_GetConf")
    
    textBuffer[0] = '\0';
    wd09AddString( textBuffer, "Configuration:\n", &freeSize );
    if (wa->initialized) {
        wd09AddString( textBuffer, "\nSessionPools:\n", &freeSize );

		currentSessionPool = wa->sessionPoolList;
		while( currentSessionPool && ok ) {
            ok = wd30GetConf( currentSessionPool->sessionPool, textBuffer, &freeSize );
            wd09AddString( textBuffer, "\n", &freeSize );
			currentSessionPool = currentSessionPool->nextSessionPool;
        };

        if (ok) {
            wd09AddString( textBuffer, "\nServices:\n", &freeSize );
			currentService = wa->serviceList;
			while ( currentService && ok ) {
                ok = wd40GetConf( currentService->service, textBuffer, &freeSize );
                wd09AddString( textBuffer, "\n", &freeSize );
				currentService = currentService->nextService;
            };
#ifdef HAVE_COM_CLASSES            
            if (ok) {
                wd09AddString( textBuffer, "\nComClasses:\n", &freeSize );
                for (iComClass=0;
                     iComClass<wa->nComClasses && ok;
                     iComClass++) {
                    ok = wd96GetConf( wa->comClasses[iComClass],
                                      textBuffer,
                                      &freeSize );
                    wd09AddString( textBuffer, "\n", &freeSize );
                }; /* for */
            }; /* if */
#endif
        }; /* if */
    } else {
        wd09AddString( textBuffer, "Not initialized!\n", &freeSize );
    }; /* else */
} /* wd20_GetConf */


sapdbwa_Bool wd20_GetIsolation( const char *poolName,
                              const char *isolationStr,
                              sapdbwa_Int4 *isolation,
                              twd26ErrP   err )
{
    sapdbwa_Bool ok = sapdbwa_True;

	sapdbwa_TraceDefFunction ("wd20_GetIsolation")
    
    if (strlen( isolationStr ) == 0) {
        *isolation = SQL_TXN_READ_COMMITTED;
    } else if (stricmp( isolationStr, "SQL_TXN_READ_UNCOMMITTED" ) == 0) {
        *isolation = SQL_TXN_READ_UNCOMMITTED;
    } else if (stricmp( isolationStr, "SQL_TXN_READ_COMMITTED" ) == 0) {
        *isolation = SQL_TXN_READ_COMMITTED;
    } else if (stricmp( isolationStr, "SQL_TXN_REPEATABLE_READ" ) == 0) {
        *isolation = SQL_TXN_REPEATABLE_READ;
    } else if (stricmp( isolationStr, "SQL_TXN_SERIALIZABLE" ) == 0) {
        *isolation = SQL_TXN_SERIALIZABLE;
    } else {
        /* unknown isolation */
        wd26SetErr( err, ERR_UNKNOWN_ISOLATION_WD26, poolName, NULL );
        ok = sapdbwa_False;
    }; /* else */

    return ok;
} /* wd20_GetOdbcIsolation */

/****************************************************************************
 ***
 *** wd20_HandleWAAdminRequest
 ***
 ****************************************************************************/

sapdbwa_Int2 wd20_HandleWAAdminRequest( sapdbwa_HttpRequestP    req,
                                        sapdbwa_HttpReplyP      rep,
									    twd20WebAgentControlP   wa   )
{

	const char	*service = NULL;
	const char	*userName = NULL;
	const char	*password = NULL;
    SAPDB_Bool   authorizationOk = SAPDB_FALSE;

	sapdbwa_TraceDefFunction ("wd20_HandleWAAdminRequest")

	/* Authorization */
    authorizationOk = sapdbwa_GetAuthorization(req, &userName, &password);

    if (authorizationOk == SAPDB_FALSE || strcmp(userName, wa->adminUserId) || strcmp(password, wa->adminUserPassword)) {
	    sapdbwa_InitHeader(rep, 401, "text/html", NULL, NULL, NULL, NULL );
	    sapdbwa_SetHeader ( rep, "WWW-Authenticate", "Basic realm=""SAP DB Web Server Administration""");
	    sapdbwa_SendHeader( rep );

        return sapdbwa_RTC_Accepted;
    }

	/* Identify request */
    service = wd20_GetHTMLParameter( req, ADMIN_SERVICE );

	/* Create response */
    if (service[0] != '\0') {
        if ( stricmp( service, ADMIN_SERVICE_NAVIGATION ) == 0 ) {
			wd20_ShowNavigation( req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_HEADER ) == 0 ) {
			wd20_ShowHeader( rep );
        } else if ( stricmp( service, ADMIN_SERVICE_PICTURES ) == 0 ) {
			wd20_ShowPicture ( req, rep, wa );
        } else if ( stricmp( service, ADMIN_SERVICE_INIT_DETAILS ) == 0 ) {
			wd20_ShowInitDetails ( rep, NULL );
        } else if ( stricmp( service, ADMIN_SERVICE_SHOW_LOGFILE ) == 0 ) {
			wd20_ShowLogFile ( rep, wa );
        } else if ( stricmp( service, ADMIN_SERVICE_SHOW_HTTP_REQUESTS ) == 0 ) {
			wd20_ShowLogFileHTTPRequests ( rep, wa );
        } else if ( stricmp( service, ADMIN_SERVICE_SHOW_HTTP_ERRORS ) == 0 ) {
			wd20_ShowLogFileHTTPErrors ( rep, wa );
        } else if ( stricmp( service, ADMIN_SERVICE_SHOW_CONF_LOGFILE ) == 0 ) {
			wd20_ShowConfLogFile ( rep );
        } else if ( stricmp( service, ADMIN_SERVICE_SHOW_LOG_SETTINGS ) == 0 ) {
			wd20_ShowLogSettings ( rep, NULL );
        } else if ( stricmp( service, ADMIN_SERVICE_UPDATE_LOG_SETTINGS ) == 0 ) {
			wd20_UpdateLogSettings ( req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_SHOW_RESOURCE_SETTINGS ) == 0 ) {
			wd20_ShowResourceSettings ( rep, NULL );
        } else if ( stricmp( service, ADMIN_SERVICE_SHOW_GENERAL_SETTINGS ) == 0 ) {
			wd20_ShowGeneralSettings ( rep, NULL );
        } else if ( stricmp( service, ADMIN_SERVICE_UPDATE_RESOURCE_SETTINGS ) == 0 ) {
			wd20_UpdateResourceSettings ( req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_UPDATE_GENERAL_SETTINGS ) == 0 ) {
			wd20_UpdateGeneralSettings ( req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_SERVICE) == 0 ) {
			wd20_ShowService ( wa, req, rep, NULL, "", NULL );
        } else if ( stricmp( service, ADMIN_ACTION_UPDATE_SERVICE ) == 0 ) {
		    service = wd20_GetHTMLParameter( req, ADMIN_ACTION );
			if ( stricmp( service, ADMIN_ACTION_UPDATE_SERVICE ) == 0 ) {
				wd20_UpdateService( wa, req, rep );
			} else if ( stricmp( service, ADMIN_ACTION_DEFINE_NEW_SERVICE_PARAMETER ) == 0 ) {
				wd20_NewServiceParameter( req, rep, wa );
			} else if ( stricmp( service, ADMIN_ACTION_SHOW_SERVICE_PARAMETERS_TO_DELETE ) == 0 ) {
				wd20_ShowParametersToDelete( req, rep, wa );
			} else if ( stricmp( service, ADMIN_ACTION_SHOW_SERVICE_TO_DELETE ) == 0 ) {
				wd20_DeleteServiceGetConfirmation( req, rep, wa );
			} else if ( stricmp( service, ADMIN_ACTION_LOAD_SERVICE ) == 0 ) {
				wd20_StartService( wa, req, rep );
			} else if ( stricmp( service, ADMIN_ACTION_UNLOAD_SERVICE ) == 0 ) {
				wd20_StopService( wa, req, rep );
			} else if ( stricmp( service, ADMIN_ACTION_RELOAD_SERVICE ) == 0 ) {
				wd20_RestartService( wa, req, rep );
			} else if ( stricmp( service, ADMIN_ACTION_START_SERVICE ) == 0 ) {
				wd20_GotoService( wa, req, rep );
			}
        } else if ( stricmp( service, ADMIN_ACTION_CREATE_NEW_SERVICE_PARAMETER ) == 0 ) {
			wd20_CreateNewParameter ( wa, req, rep );
        } else if ( stricmp( service, ADMIN_ACTION_DEFINE_NEW_SERVICE ) == 0 ) {
			wd20_NewService ( rep, wa );
        } else if ( stricmp( service, ADMIN_ACTION_CREATE_NEW_SERVICE ) == 0 ) {
			wd20_CreateNewService ( wa, req, rep );
        } else if ( stricmp( service, ADMIN_ACTION_DELETE_SERVICE ) == 0 ) {
			wd20_DeleteService ( req, rep );
        } else if ( stricmp( service, ADMIN_ACTION_DELETE_SERVICE_PARAMETERS ) == 0 ) {
			wd20_DeleteParameters ( wa, req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_COMSERVICE ) == 0 ) {
			wd20_ShowCOMService ( wa, req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_NEW_COMSERVICE ) == 0 ) {
			wd20_NewCOMService ( rep );
        } else if ( stricmp( service, ADMIN_SERVICE_CREATE_NEW_COMSERVICE ) == 0 ) {
			wd20_CreateNewCOMService ( wa, req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_UPDATE_COMSERVICE ) == 0 ) {
		    service = wd20_GetHTMLParameter( req, ADMIN_ACTION );
			if ( stricmp( service, ADMIN_ACTION_UPDATE_COM_SERVICE ) == 0 ) {
				wd20_UpdateCOMService( req, rep );
			} else if ( stricmp( service, ADMIN_ACTION_DEFINE_NEW_COM_SERVICE_PARAMETER ) == 0 ) {
				wd20_NewCOMParameter( req, rep );
			} else if ( stricmp( service, ADMIN_ACTION_SHOW_COM_SERVICE_PARAMETERS_TO_DELETE ) == 0 ) {
				wd20_ShowCOMParametersToDelete( req, rep );
			} else if ( stricmp( service, ADMIN_ACTION_SHOW_COM_SERVICE_TO_DELETE ) == 0 ) {
				wd20_DeleteCOMServiceGetConfirmation( req, rep );
			}
        } else if ( stricmp( service, ADMIN_SERVICE_NEW_COMPARAMETER ) == 0 ) {
			wd20_CreateNewCOMParameter ( req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_DELETE_COMSERVICE ) == 0 ) {
			wd20_DeleteCOMService ( req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_DELETE_COMPARAMETERS ) == 0 ) {
			wd20_DeleteCOMParameters ( req, rep );
        } else if ( stricmp( service, ADMIN_SERVICE_SHOW_SESSION_POOL ) == 0 ) {
			wd20_ShowSessionPool ( wa, req, rep, NULL, NULL );
        } else if ( stricmp( service, ADMIN_ACTION_DEFINE_NEW_SESSION_POOL ) == 0 ) {
			wd20_NewSessionPool( rep, wa );
        } else if ( stricmp( service, ADMIN_ACTION_CREATE_NEW_SESSION_POOL ) == 0 ) {
			wd20_CreateNewSessionPool( wa, req, rep );
        } else if ( stricmp( service, ADMIN_ACTION_UPDATE_SESSION_POOL ) == 0 ) {
		    service = wd20_GetHTMLParameter( req, ADMIN_ACTION );
			if ( stricmp( service, ADMIN_ACTION_UPDATE_SESSION_POOL ) == 0 ) {
				wd20_UpdateSessionPool( wa, req, rep );
			} else if ( stricmp( service, ADMIN_ACTION_SHOW_SESSION_POOL_TO_DELETE ) == 0 ) {
				wd20_DeleteSessionPoolGetConfirmation( req, rep, wa );
			}
        } else if ( stricmp( service, ADMIN_SERVICE_DELETE_SESSION_POOL ) == 0 ) {
			wd20_DeleteSessionPool ( req, rep );
		} else if ( stricmp( service, ADMIN_SERVICE_CLEAR_LOG_FILE ) == 0 ) {
			wd20_ClearLogFile( rep, wa );
		} else if ( stricmp( service, ADMIN_SERVICE_CLEAR_LOG_FILE_HTTP_REQUESTS ) == 0 ) {
			wd20_ClearLogFileHTTPRequests( req, rep, wa );
		} else if ( stricmp( service, ADMIN_SERVICE_CLEAR_LOG_FILE_HTTP_ERRORS ) == 0 ) {
			wd20_ClearLogFileHTTPErrors( req, rep, wa );
		} else if ( stricmp( service, ADMIN_SERVICE_SHOW_STATUS ) == 0 ) {
			wd20_ShowStatus( req, rep );
		}
    } else {
		wd20_ShowMain( rep, wa );
	}
    
	/* Write log, if log entry exists */
	wd26LogErr( wa->waErr, wa->waLog );

    return sapdbwa_RTC_Accepted;
}

/****************************************************************************
 ***
 *** wd20_ShowLogFile
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowLogFile( sapdbwa_HttpReplyP			rep,
							 twd20WebAgentControlP	wa   )
{

	char	fileName [MAX_FILENAME_LEN_WD00];

	sapdbwa_TraceDefFunction ("wd20_ShowLogFile")

	/* Get filename */
	if ( !wd20_GetRegistryValue( NULL, wa->regSectionGlobal,	ADMIN_REG_KEY_LOGFILE,
								 fileName, sizeof( fileName ), "" ) || fileName[0] == '\0' ) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_NotFound, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );
		sapdbwa_SendBody( rep, "File not found!", 0 );

		return sapdbwa_False;
	}

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText ( rep, 0, ADMIN_HTML_BEGIN );
	wd20_SendText ( rep, 0, ADMIN_HTML_BODY_BEGIN );

	if ( wd25IsLogFileEmpty( wa->waLog )) {
		wd20_SendText ( rep, 0, ADMIN_HTML_EMPTY_LOG_FILE );
	} else {
		wd20_SendText ( rep, 0, ADMIN_HTML_CLEAR_LOG_FILE );

		/* Read and send file */
		wd20_WAAdminShowFile ( rep, fileName );

		wd20_SendText ( rep, 0, ADMIN_HTML_CLEAR_LOG_FILE );
	}

	wd20_SendText ( rep, 0, ADMIN_HTML_BODY_END );
	wd20_SendText ( rep, 0, ADMIN_HTML_END );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_ShowLogFileHTTPRequests
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowLogFileHTTPRequests( sapdbwa_HttpReplyP		rep,
					  		               twd20WebAgentControlP	wa   )
{

	char	filename[MAX_FILENAME_LEN_WD00+1];
	char	logDirectory[MAX_FILENAME_LEN_WD00+1];

	sapdbwa_TraceDefFunction ("wd20_ShowLogFileHTTPRequests")

	/* Get filename */
	if ( !wd20_GetRegistryValue( NULL, wa->regSectionHTTP,	ADMIN_REG_KEY_LOGFILE_HTTP_REQUESTS,
								 logDirectory, sizeof( logDirectory ), "" ) || logDirectory[0] == '\0' ) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_NotFound, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );
		sapdbwa_SendBody( rep, "File not found!", 0 );

		return sapdbwa_False;
	}

    sp77sprintf(filename, MAX_FILENAME_LEN_WD00, "%s/httpreq.log", logDirectory);

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText ( rep, 0, ADMIN_HTML_BEGIN );
	wd20_SendText ( rep, 0, ADMIN_HTML_BODY_BEGIN );

	if ( wd20_IsLogFileEmpty( filename )) {
		wd20_SendText ( rep, 0, ADMIN_HTML_EMPTY_LOG_FILE_HTTP_REQUESTS );
	} else {
		wd20_SendText ( rep, 0, ADMIN_HTML_CLEAR_LOG_FILE_HTTP_REQUESTS );

		/* Read and send file */
		wd20_WAAdminShowFile ( rep, filename );

		wd20_SendText ( rep, 0, ADMIN_HTML_CLEAR_LOG_FILE_HTTP_REQUESTS );
	}

	wd20_SendText ( rep, 0, ADMIN_HTML_BODY_END );
	wd20_SendText ( rep, 0, ADMIN_HTML_END );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_ShowLogFileHTTPErrors
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowLogFileHTTPErrors( sapdbwa_HttpReplyP	rep,
					  		             twd20WebAgentControlP	wa   )
{

	char	logDirectory[MAX_FILENAME_LEN_WD00+1];
	char	filename[MAX_FILENAME_LEN_WD00+1];

	sapdbwa_TraceDefFunction ("wd20_ShowLogFileHTTPErrors")

	/* Get filename */
	if ( !wd20_GetRegistryValue( NULL, wa->regSectionHTTP, ADMIN_REG_KEY_LOGFILE_HTTP_ERRORS,
								 logDirectory, sizeof( logDirectory ), "" ) || logDirectory[0] == '\0' ) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_NotFound, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );
		sapdbwa_SendBody( rep, "File not found!", 0 );

		return sapdbwa_False;
	}

    sp77sprintf(filename, MAX_FILENAME_LEN_WD00, "%s/httperr.log", logDirectory);

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText ( rep, 0, ADMIN_HTML_BEGIN );
	wd20_SendText ( rep, 0, ADMIN_HTML_BODY_BEGIN );

	if ( wd20_IsLogFileEmpty( filename )) {
		wd20_SendText ( rep, 0, ADMIN_HTML_EMPTY_LOG_FILE_HTTP_ERRORS );
	} else {
		wd20_SendText ( rep, 0, ADMIN_HTML_CLEAR_LOG_FILE_HTTP_ERRORS );

		/* Read and send file */
		wd20_WAAdminShowFile ( rep, filename );

		wd20_SendText ( rep, 0, ADMIN_HTML_CLEAR_LOG_FILE_HTTP_ERRORS );
	}

	wd20_SendText ( rep, 0, ADMIN_HTML_BODY_END );
	wd20_SendText ( rep, 0, ADMIN_HTML_END );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_ShowConfLogFile
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowConfLogFile( sapdbwa_HttpReplyP rep )
{

	char	fileName [MAX_FILENAME_LEN_WD00];

	sapdbwa_TraceDefFunction ("wd20_ShowConfLogFile")

	/* Get filename */
	if ( !wd20_GetRegistryValue( NULL, wd20WAControl.regSectionGlobal, ADMIN_REG_KEY_CONF_LOGFILE,
								 fileName, sizeof( fileName ), "" ) || fileName[0] == '\0' ) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_NotFound, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		sapdbwa_SendBody( rep, "File not found!", 0 );

		return sapdbwa_False;
	}

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText ( rep, 0, ADMIN_HTML_BEGIN );
	wd20_SendText ( rep, 0, ADMIN_HTML_BODY_BEGIN );

	/* Read and send file */
	wd20_WAAdminShowFile ( rep, fileName );

	wd20_SendText ( rep, 0, ADMIN_HTML_BODY_END );
	wd20_SendText ( rep, 0, ADMIN_HTML_END );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_SendText
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_SendText( sapdbwa_HttpReplyP	rep,
							  sapdbwa_Int4	groupId,
							  sapdbwa_Int4	textId   )
{
	const char *text;

	sapdbwa_TraceDefFunction ("wd20_SendText")

	if ( wd15GetString( groupId, textId, &text )) {
		sapdbwa_SendBody( rep, text, 0 );
		return sapdbwa_True;;
	} else {
		sapdbwa_SendBody( rep, "Error occured.", 0 );
		return sapdbwa_False;
	}
}

/****************************************************************************
 ***
 *** wd20_ShowHeader
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowHeader( sapdbwa_HttpReplyP rep )
{

	sapdbwa_TraceDefFunction ("wd20_ShowHeader")

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText( rep, 0, ADMIN_HTML_MAIN_HEADER );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_ShowInitDetails
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowInitDetails( sapdbwa_HttpReplyP	 rep,
								 const char		*message )
{

	sapdbwa_TraceDefFunction ("wd20_ShowInitDetails")

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText( rep, 0, ADMIN_HTML_INIT_DETAILS_BEGIN );
	if ( message )
		sapdbwa_SendBody( rep, message, 0 );
	else {
		sapdbwa_SendBody( rep, "", 0 );
	}
	wd20_SendText( rep, 0, ADMIN_HTML_INIT_DETAILS_END );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_NewService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_NewService( sapdbwa_HttpReplyP	rep,
							  twd20WebAgentControlP	wa  )
{

	Reg_RegistryP	registry;
	sapdbwa_Bool	eos;
	char			listObject[REG_MAX_SECTION_LEN + 1];
	char			templateName [MAX_FILENAME_LEN_WD00 + 1];

	sapdbwa_TemplateValueList			*templateValueList = NULL;
	sapdbwa_TemplateValueTable			*templateTable = NULL;
	sapdbwa_TemplateValueTableRow		*templateRow = NULL;
	sapdbwa_TemplateValueTableColumn	*templateColumn = NULL;

	sapdbwa_TraceDefFunction ("wd20_NewService")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry,  wd20WAControl.iniFile )) {
        wd26SetErr( wd20WAControl.waErr,
                    ERR_OPEN_REGISTRY_WD26, wd20WAControl.iniFile, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

		wd20_SendServerError( rep );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
    }

	/* Create value list	*/
	CreateTemplateValueList( &templateValueList );

	AddValueToTemplateValueList( templateValueList, "ServiceName", "" );
	AddValueToTemplateValueList( templateValueList, "ServiceURI", "" );
	AddValueToTemplateValueList( templateValueList, "InitFunction", "" );
	AddValueToTemplateValueList( templateValueList, "ExitFunction", "" );
	AddValueToTemplateValueList( templateValueList, "ServiceFunction", "" );
	AddValueToTemplateValueList( templateValueList, "Library", "" );

	AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectC", "SELECTED");
	AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectCPP", "" );
	
	AddValueToTemplateValueList( templateValueList, "LogFilename", "" );

	/* Enumerate registry sections */
	if ( !Reg_EnumRegistrySections( registry, wa->regSectionSessionPool, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionSessionPool, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
	    Reg_CloseRegistry( registry );

		DropTemplateValueList( templateValueList );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Iterate registry keys */
	CreateTemplateValueTable ( &templateTable );

	CreateTemplateValueTableColumn ( &templateColumn, "SELECTED" );
	AddColumnToTemplateValueTableRow ( templateRow, templateColumn );

	CreateTemplateValueTableColumn ( &templateColumn, "" );
	AddColumnToTemplateValueTableRow ( templateRow, templateColumn );

	AddRowToTemplateValueTable( templateTable, templateRow );

	listObject[0]='\0';
	do {
		if ( !Reg_GetNextSection( registry, NULL, 0, listObject, sizeof( listObject ), &eos ))
			break;

		CreateTemplateValueTableRow ( &templateRow );

		CreateTemplateValueTableColumn ( &templateColumn, "" );
		AddColumnToTemplateValueTableRow ( templateRow, templateColumn );

		CreateTemplateValueTableColumn ( &templateColumn, listObject );
		AddColumnToTemplateValueTableRow ( templateRow, templateColumn );

		AddRowToTemplateValueTable( templateTable, templateRow );

		listObject[0]='\0';
	} while ( !eos );
	wd20_SendCloseListRow( rep );

	AddTableToTemplateValueList ( templateValueList, "SessionPools", templateTable );

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Stratup type */
	AddValueToTemplateValueList( templateValueList, "ServiceStartChecked", "" );

	/* Cookie prefix setting */
	AddValueToTemplateValueList( templateValueList, "UseFastCGIForCookiePathChecked", "CHECKED" );

	/* Secure type */
	AddValueToTemplateValueList( templateValueList, "WithSSLChecked", "" );

	/* SSL URL */
	AddValueToTemplateValueList( templateValueList, "SSLURL", "" );

	/* Web session timeout */
	AddValueToTemplateValueList( templateValueList, "WebSessionTimeout", "" );

	/* Send form */

	/* Build template name	*/
	sp77sprintf(templateName, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_DEFINE_NEW_SERVICE);

	/* Show confirmation page */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Add values to HTML template and send template as response	*/
	WriteTemplate( rep, templateName, templateValueList, wa->waErr );

	DropTemplateValueList( templateValueList );


	return sapdbwa_True;
}

/****************************************************************************
 ***
 *** wd20_ShowNewService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowNewService( sapdbwa_HttpReplyP		 rep,
								  twd20WebAgentControlP		 wa,
								  twd20_ServiceDescription	*serviceDescription,
								  const char				*message			)
{

	char			listObject [REG_MAX_VALUE_LEN + 1];
	Reg_RegistryP	registry;
	sapdbwa_Bool	eos;
	char			templateName [MAX_FILENAME_LEN_WD00 + 1];

	sapdbwa_TemplateValueList			*templateValueList = NULL;
	sapdbwa_TemplateValueTable			*templateTable = NULL;
	sapdbwa_TemplateValueTableRow		*templateRow = NULL;
	sapdbwa_TemplateValueTableColumn	*templateColumn = NULL;

	sapdbwa_TraceDefFunction ("wd20_ShowNewService")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        wd26SetErr( wd20WAControl.waErr,
                    ERR_OPEN_REGISTRY_WD26, wd20WAControl.iniFile, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

		wd20_SendServerError( rep );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
    }

	/* Create value list	*/
	CreateTemplateValueList( &templateValueList );

	if ( message ) {
		AddValueToTemplateValueList( templateValueList, "Status", message );
	} else {
		AddValueToTemplateValueList( templateValueList, "Status", "" );
	}

	AddValueToTemplateValueList( templateValueList, "ServiceName", serviceDescription->service );
	AddValueToTemplateValueList( templateValueList, "ServiceURI", serviceDescription->serviceName );
	AddValueToTemplateValueList( templateValueList, "InitFunction", serviceDescription->initFunction );
	AddValueToTemplateValueList( templateValueList, "ExitFunction", serviceDescription->exitFunction );
	AddValueToTemplateValueList( templateValueList, "ServiceFunction", serviceDescription->serviceFunction );
	AddValueToTemplateValueList( templateValueList, "Library", serviceDescription->library );

	if ( stricmp( serviceDescription->libraryType, ADMIN_LIBRARY_TYPE_C_VALUE ) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectC", "SELECTED");
		AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectCPP", "" );
	} else {
		AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectC", "");
		AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectCPP", "SELECTED" );
	}
	
	AddValueToTemplateValueList( templateValueList, "LogFilename", serviceDescription->logFile );

	/* Enumerate registry sections */
	if ( !Reg_EnumRegistrySections( registry, wa->regSectionSessionPool, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionSessionPool, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
	    Reg_CloseRegistry( registry );

		DropTemplateValueList( templateValueList );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Iterate registry keys */
	CreateTemplateValueTable ( &templateTable );

	if ( strcmp( "", serviceDescription->sessionPool ) == 0 ) {
		CreateTemplateValueTableColumn ( &templateColumn, "SELECTED" );
	} else {
		CreateTemplateValueTableColumn ( &templateColumn, "" );
	}
	AddColumnToTemplateValueTableRow ( templateRow, templateColumn );

	CreateTemplateValueTableColumn ( &templateColumn, "" );
	AddColumnToTemplateValueTableRow ( templateRow, templateColumn );

	AddRowToTemplateValueTable( templateTable, templateRow );

	listObject[0]='\0';
	do {
		if ( !Reg_GetNextSection( registry, NULL, 0, listObject, sizeof( listObject ), &eos ))
			break;

		CreateTemplateValueTableRow ( &templateRow );

		if ( strcmp( listObject, serviceDescription->sessionPool ) == 0 ) {
			CreateTemplateValueTableColumn ( &templateColumn, "SELECTED" );
			AddColumnToTemplateValueTableRow ( templateRow, templateColumn );
		} else {
			CreateTemplateValueTableColumn ( &templateColumn, "" );
			AddColumnToTemplateValueTableRow ( templateRow, templateColumn );
		}

		CreateTemplateValueTableColumn ( &templateColumn, listObject );
		AddColumnToTemplateValueTableRow ( templateRow, templateColumn );

		AddRowToTemplateValueTable( templateTable, templateRow );

		listObject[0]='\0';
	} while ( !eos );
	wd20_SendCloseListRow( rep );

	AddTableToTemplateValueList ( templateValueList, "SessionPools", templateTable );

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Stratup type */
	if ( strcmp( serviceDescription->serviceStart, ADMIN_SERVICE_START_ON_VALUE) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "ServiceStartChecked", "CHECKED" );
	} else {
		AddValueToTemplateValueList( templateValueList, "ServiceStartChecked", "" );
	}

	/* Cookie prefix setting */
	if ( strcmp( serviceDescription->useFastCGIForCookiePath, ADMIN_SERVICE_COOKIE_PATH_PREFIX_ON_VALUE) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "UseFastCGIForCookiePathChecked", "CHECKED" );
	} else {
		AddValueToTemplateValueList( templateValueList, "UseFastCGIForCookiePathChecked", "" );
	}

	/* Secure type */
	if ( strcmp( serviceDescription->withSSL, ADMIN_SERVICE_WITH_SSL_ON_VALUE) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "WithSSLChecked", "CHECKED" );
	} else {
		AddValueToTemplateValueList( templateValueList, "WithSSLChecked", "" );
	}

	/* SSL URL */
	AddValueToTemplateValueList( templateValueList, "SSLURL", serviceDescription->sslURL );

	/* Web session timeout */
	AddValueToTemplateValueList( templateValueList, "WebSessionTimeout", serviceDescription->webSessionTimeout );

	/* Send form */

	/* Build template name	*/
	sp77sprintf (templateName, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_DEFINE_NEW_SERVICE );

	/* Show confirmation page */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Add values to HTML template and send template as response	*/
	WriteTemplate( rep, templateName, templateValueList, wa->waErr );

	DropTemplateValueList( templateValueList );

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20_ShowParametersToDelete
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowParametersToDelete( sapdbwa_HttpRequestP	req,
										  sapdbwa_HttpReplyP	rep,
										  twd20WebAgentControlP	wa	)
{

	Reg_RegistryP	 registry;
	sapdbwa_Bool	 eok;
	const char		*serviceName = NULL;
	char			 currentSection	[REG_MAX_SECTION_LEN+1];
	char			 userParameter	[REG_MAX_VALUE_LEN+1];
	char			 userValue		[REG_MAX_VALUE_LEN+1];
	char			 templateName	[MAX_FILENAME_LEN_WD00 + 1];

	sapdbwa_TemplateValueList			*templateValueList = NULL;
	sapdbwa_TemplateValueTable			*templateTable = NULL;
	sapdbwa_TemplateValueTableRow		*templateRow = NULL;
	sapdbwa_TemplateValueTableColumn	*templateColumn = NULL;

	sapdbwa_TraceDefFunction ("wd20_ShowParametersToDelete")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        wd26SetErr( wd20WAControl.waErr,
                    ERR_OPEN_REGISTRY_WD26, wd20WAControl.iniFile, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

		wd20_SendServerError( rep );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
    }

	/* Get current section */
	serviceName = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionServices, serviceName);

	/* Enumerate registry keys */
	if ( !Reg_EnumRegistryKeys( registry, currentSection, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_READ_REGISTRY_WD26, currentSection, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
	}

	/* Create value list	*/
	CreateTemplateValueList( &templateValueList );
	if ( !templateValueList ) {
		/* Error handling	*/
	}

	/* Add values to value list	*/
	AddValueToTemplateValueList( templateValueList, "ServiceName", serviceName );

	/* Iterate registry keys */
	userParameter[0]='\0';
	userValue[0]='\0';
	CreateTemplateValueTable( &templateTable );
	do {
		if( !Reg_GetNextKey( registry, userParameter, sizeof( userParameter ), userValue, sizeof( userValue ), &eok ))
			break;

		if ( !wd20_IsServiceStandardParameter( userParameter ) && !wd20_IsServiceDependentParameter( userParameter )) {
			/* Add user parameter to value list	*/
			CreateTemplateValueTableRow( &templateRow );

			CreateTemplateValueTableColumn( &templateColumn, userParameter );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			CreateTemplateValueTableColumn( &templateColumn, userParameter );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			AddRowToTemplateValueTable( templateTable, templateRow );
		}

		userParameter[0]='\0';
		userValue[0]='\0';
	} while ( !eok );

	if ( templateRow ) {
		AddTableToTemplateValueList( templateValueList, "Parameters", templateTable );
	} else {
		DropTemplateValueTable( templateTable );
	}

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Send form */

	/* Build template name	*/
	sp77sprintf(templateName, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_DELETE_SERVICE_PARAMETER );

	/* Show confirmation page */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Add values to HTML template and send template as response	*/
	WriteTemplate( rep, templateName, templateValueList, wa->waErr );

	DropTemplateValueList( templateValueList );


	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20_DeleteParameters
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteParameters( twd20WebAgentControlP	wa,
								  sapdbwa_HttpRequestP		req,
								  sapdbwa_HttpReplyP			rep  )
{

	sapdbwa_Int4		 i = 0;
	sapdbwa_Int4		 count = 0;
    sapdbwa_StringSeqP	 valueList = sapdbwa_CreateStringSeq();
	const char		*service = NULL;
	const char		*value = NULL;
	const char		*message = NULL;
	char			 currentSection [REG_MAX_SECTION_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_DeleteParameters")

	/* Get current section */
	service = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionServices, service );

	/* Get list of values */
    if ( !sapdbwa_GetParameterValues( req, ADMIN_CHECKBOX_PARAMETER, valueList )) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Delete registry section */
	for ( i = 0 ; i < sapdbwa_GetNumElem( valueList ) ; i++ ) {
		if (( value = sapdbwa_GetStringByIndex( valueList, i )) != NULL ) {
			if ( !wd20_DeleteRegistryKey ( currentSection, value )) {
				wd20_SendServerError( rep );
				return sapdbwa_False;
			}

			count++;
		}
	}

	/* Show confirmation */
	if ( count ) {
		wd15GetString( 0, ADMIN_HTML_DELETE_PARAMETERS_CONFIRMATION, &message );
	} else {
		wd15GetString( 0, ADMIN_HTML_DELETE_PARAMETERS_NOTHING, &message );
	}

	wd20_ShowService( wa, req, rep, service, message, NULL );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_CreateNewService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_CreateNewService( twd20WebAgentControlP	wa,
								    sapdbwa_HttpRequestP	req,
								    sapdbwa_HttpReplyP		rep  )
{

	const char					*text = NULL;
	const char					*serviceName = NULL;
	const char					*serviceStart = NULL;
	const char					*sslURL = NULL;
	sapdbwa_Bool				 withSSLBool = sapdbwa_False;
	sapdbwa_Bool				 found = sapdbwa_False;
	char						 currentSection		[REG_MAX_SECTION_LEN+1];
	char						 serviceNameU		[REG_MAX_VALUE_LEN+1];
	char						 webSessionTimeout	[REG_MAX_VALUE_LEN+1];
	char						 status				[MAX_STATUS_LEN];
	twd20_ServiceDescription	 serviceDescription;

	sapdbwa_TraceDefFunction ("wd20_CreateNewService")

	/* Inits */
	status[0] = '\0';

	/* Get service description */
	wd20_GetServiceDescription( req, &serviceDescription );

	/* Get new service name */
	serviceName = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	/* Service already exists? */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionServices, serviceName);
	if ( !wd20_RegistrySectionExists( NULL, currentSection, &found )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, currentSection, NULL );
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Get upper case service name */
	strncpy( serviceNameU, serviceName, sizeof( serviceNameU ));
	wd09UpperCase( serviceNameU );

	/* If service exists */
	if ( found ) {
		/* Send exists message */
		if ( wd15GetString( 0, ADMIN_HTML_SERVICE_ALREADY_EXISTS_BEGIN, &text ))
			strcat( status, text );

		strcat( status, serviceNameU );

		if ( wd15GetString( 0, ADMIN_HTML_SERVICE_ALREADY_EXISTS_END, &text ))
			strcat( status, text );

		wd20_ShowNewService( rep, wa, &serviceDescription, status );
		wd20_FreeServiceDescription( &serviceDescription );

		return sapdbwa_True;;
	}

	/* If service not exists but service name is the WebAgent Admin service name */
	if ( !found && (strncmp( serviceNameU, ADMIN_WAADMIN, sizeof( ADMIN_WAADMIN )) == 0 )) {
		/* Send exists message */
		if ( wd15GetString( 0, ADMIN_HTML_RESERVED_SERVICE_NAME_BEGIN, &text ))
			strcat( status, text );

		strcat( status, serviceNameU );

		if ( wd15GetString( 0, ADMIN_HTML_RESERVED_SERVICE_NAME_END, &text ))
			strcat( status, text );

		wd20_ShowNewService( rep, wa, &serviceDescription, status );
		wd20_FreeServiceDescription( &serviceDescription );

		return sapdbwa_True;;
	}

	/* Get secure type */
	if ( strlen( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_WITH_SSL )) ) {
		withSSLBool = sapdbwa_True;
	} else {
		withSSLBool = sapdbwa_False;
	}

	/* Check SSL URL */
	sslURL = wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_SSL_URL );
	if ( withSSLBool ) {
		if ( !wd20_IsSSLURL( sslURL )) {
			wd15GetString( 0, ADMIN_HTML_SERVICE_MISSING_SSL_URL, &text );
			wd20_ShowNewService( rep, wa, &serviceDescription, text );
			wd20_FreeServiceDescription( &serviceDescription );

			return sapdbwa_True;;
		}
	}

	/* Get new section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionServices, serviceName );

	/* Update registry with new service */
	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_EXIT_FUNCTION,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_EXIT_FUNCTION ))) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_INIT_FUNCTION,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_INIT_FUNCTION ))) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_LIBRARY,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_LIBRARY ))) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	if ( strcmp( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_LIBRARY_TYPE ), ADMIN_LIBRARY_TYPE_C_VALUE ) == 0 ) {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_LIBRARY_TYPE, ADMIN_LIBRARY_TYPE_C )) {
			wd20_SendServerError( rep );
			wd20_FreeServiceDescription( &serviceDescription );
			return sapdbwa_False;
		}
	} else {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_LIBRARY_TYPE, ADMIN_LIBRARY_TYPE_CPP )) {
			wd20_SendServerError( rep );
			wd20_FreeServiceDescription( &serviceDescription );
			return sapdbwa_False;
		}
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_LOGFILE,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_LOGFILE ))) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_SERVICE_FUNCTION,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_FUNCTION ))) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_SERVICE_NAME,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_NAME ))) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_SESSION_POOL,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SESSION_POOL ))) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	/* Set startup type */
	serviceStart = wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_START );
	if ( strlen(serviceStart) ) {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_START, ADMIN_SERVICE_START_ON_VALUE )) {
			wd20_SendServerError( rep );
			wd20_FreeServiceDescription( &serviceDescription );
			return sapdbwa_False;
		}
	} else {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_START, ADMIN_SERVICE_START_OFF_VALUE )) {
			wd20_SendServerError( rep );
			wd20_FreeServiceDescription( &serviceDescription );
			return sapdbwa_False;
		}
	}

	/* Set secure type */
	if ( withSSLBool ) {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_WITH_SSL, ADMIN_SERVICE_WITH_SSL_ON_VALUE )) {
			wd20_SendServerError( rep );
			wd20_FreeServiceDescription( &serviceDescription );
			return sapdbwa_False;
		}
	} else {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_WITH_SSL, ADMIN_SERVICE_WITH_SSL_OFF_VALUE )) {
			wd20_SendServerError( rep );
			wd20_FreeServiceDescription( &serviceDescription );
			return sapdbwa_False;
		}
	}

	/* Set SSL URL */
	if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_SSL_URL, sslURL )) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	/* Set web session timeout */
	wd20_GetTimeValue( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_WEB_SESSION_TIMEOUT ), webSessionTimeout, sizeof(webSessionTimeout) );
	if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_WEB_SESSION_TIMEOUT, webSessionTimeout )) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	/* Show created service and send confirmation */
	wd15GetString( 0, ADMIN_HTML_SERVICE_CREATE_CONFIRMATION, &text );
	wd20_ShowService( wa, req, rep, serviceDescription.service, text, NULL );
	wd20_FreeServiceDescription( &serviceDescription );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_DeleteServiceGetConfirmation
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteServiceGetConfirmation( sapdbwa_HttpRequestP	req,
											    sapdbwa_HttpReplyP		rep,
												twd20WebAgentControlP	wa  )
{

	const char					*serviceName = NULL;
	char						 templateName[MAX_FILENAME_LEN_WD00 + 1];
	sapdbwa_TemplateValueList	*templateValueList = NULL;

	sapdbwa_TraceDefFunction ("wd20_DeleteServiceGetConfirmation")

	/* Get service name */
	serviceName = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	/* Show confirmation page */

	/* Create value list	*/
	CreateTemplateValueList( &templateValueList );
	if ( !templateValueList ) {
		/* Error handling	*/
	}

	/* Add values to value list	*/
	AddValueToTemplateValueList( templateValueList, "ServiceName", serviceName );

	/* Build template name	*/
	sp77sprintf(templateName, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_SHOW_SERVICE_TO_DELETE );

	/* Add values to HTML template and send template as response	*/
	WriteTemplate( rep, templateName, templateValueList, wa->waErr );

	DropTemplateValueList( templateValueList );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_DeleteService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteService( sapdbwa_HttpRequestP	req,
							     sapdbwa_HttpReplyP		rep	)
{

	char		 currentSection [REG_MAX_SECTION_LEN+1];
	const char	*service = NULL;
	const char	*text = NULL;
	char		 status	[MAX_STATUS_LEN];

	sapdbwa_TraceDefFunction ("wd20_DeleteService")

	/* Inits */
	status[0] = '\0';

	/* Get service */
	service = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	/* Get new section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wd20WAControl.regSectionServices, service);

	/* Delete registry section */
	if ( !wd20_DeleteRegistrySection( currentSection )) {
		wd20_SendServerError( rep );
	}

	/* Show confirmation page */
	if ( wd15GetString( 0, ADMIN_HTML_DELETE_SERVICE_CONFIRMATION, &text ))
		strcat( status, text );

	return wd20_ShowInitDetails( rep, status );

}

/****************************************************************************
 ***
 *** wd20_NewServiceParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_NewServiceParameter( sapdbwa_HttpRequestP		req,
                                       sapdbwa_HttpReplyP		rep,
								       twd20WebAgentControlP	wa	)
{
	const char					*serviceName = NULL;
	char						 templateName[MAX_FILENAME_LEN_WD00 + 1];
	sapdbwa_TemplateValueList	*templateValueList = NULL;

	sapdbwa_TraceDefFunction ("wd20_NewServiceParameter")

	/* Get service name */
	serviceName = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Send form */

	/* Create value list	*/
	CreateTemplateValueList( &templateValueList );
	if ( !templateValueList ) {
		/* Error handling	*/
	}

	/* Add values to value list	*/
	AddValueToTemplateValueList( templateValueList, "ServiceName", serviceName );

	/* Build template name	*/
	sp77sprintf(templateName, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_NEW_SERVICE_PARAMETER );

	/* Add values to HTML template and send template as response	*/
	WriteTemplate( rep, templateName, templateValueList, wa->waErr );

	DropTemplateValueList( templateValueList );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_CreateNewParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_CreateNewParameter( twd20WebAgentControlP	wa,
									sapdbwa_HttpRequestP			req,
									sapdbwa_HttpReplyP			rep  )
{
	char		 currentSection [REG_MAX_SECTION_LEN+1];
	const char	*service = NULL;
	const char	*parameterName = NULL;
	const char	*message = NULL;

	sapdbwa_TraceDefFunction ("wd20_CreateNewParameter")

	/* Get current section */
	service = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionServices, service );

	/* Get parameter name */
	parameterName = wd20_GetHTMLParameter( req, ADMIN_VALUE_PARAMETER_NAME );
	if ( strlen( parameterName ) < 1 || wd20_IsServiceStandardParameter( parameterName ) || wd20_IsServiceDependentParameter( parameterName )) {
		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		/* Send invalid parameter name message */
		wd20_SendText( rep, 0, ADMIN_HTML_INVALID_PARAMETER_NAME);

		return sapdbwa_True;;
	}

	/* Create new  */
	if ( !wd20_SetRegistryKey ( currentSection,
								parameterName,
								wd20_GetHTMLParameter( req, ADMIN_VALUE_PARAMETER_DEFAULT_VALUE ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Send confirmation */
	wd15GetString( 0, ADMIN_HTML_VALUE_UPDATE_CONFIRMATION, &message );
	wd20_ShowService( wa, req, rep, service, message, NULL );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_ShowPicture
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowPicture( sapdbwa_HttpRequestP			req,
                             sapdbwa_HttpReplyP			rep,
							 twd20WebAgentControlP	wa   )
{
	char			fileName [MAX_FILENAME_LEN_WD00];

	sapdbwa_TraceDefFunction ("wd20_ShowPicture")

	/* Get picture name */
	sp77sprintf(fileName, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_IMAGES,
			  wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME ));
	
	sapdbwa_SendFile( rep, fileName );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_SendNavigationScript
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_SendNavigationScript( sapdbwa_HttpReplyP			rep,
									  twd20WebAgentControlP	wa   )
{
	char	fileName [MAX_FILENAME_LEN_WD00];

	sapdbwa_TraceDefFunction ("wd20_SendNavigationScript")

	/* Get file name */
	sp77sprintf(fileName, MAX_FILENAME_LEN_WD00, "%s/%s/%s", wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_JAVA_SCRIPTS, ADMIN_JS_NAVIGATION );
	
	sapdbwa_SendTemplate( rep, fileName );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_IsServiceStandardParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_IsServiceStandardParameter( const char	*parameter )
{

	sapdbwa_TraceDefFunction ("wd20_IsServiceStandardParameter")

	/* Check parameter */
    if ( stricmp( parameter, ADMIN_REG_KEY_EXIT_FUNCTION )				== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_INIT_FUNCTION )				== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_LIBRARY )					== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_LIBRARY_TYPE )				== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_LOGFILE )					== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SERVICE_FUNCTION )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SERVICE_NAME )				== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SERVICE_START )				== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SERVICE_COOKIE_PATH_PREFIX )	== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SERVICE_WITH_SSL )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SERVICE_SSL_URL )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_WEB_SESSION_TIMEOUT )		== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SESSION_POOL )				== 0 ) return sapdbwa_True;;

	return sapdbwa_False;
}

/****************************************************************************
 ***
 *** wd20_IsServiceDependentParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_IsServiceDependentParameter( const char	*parameter )
{

	sapdbwa_TraceDefFunction ("wd20_IsServiceDependentParameter")

	/* Check parameter */
    if ( stricmp( parameter, ADMIN_SERVICE_SERVICE )		== 0 ||
		 stricmp( parameter, ADMIN_SERVICE_PARAM_NAME )		== 0 ||
		 stricmp( parameter, ADMIN_BUTTON_CREATE )			== 0 ||
		 stricmp( parameter, ADMIN_EVENT )					== 0 ||
		 stricmp( parameter, ADMIN_ACTION )					== 0 ) return sapdbwa_True;;

	return sapdbwa_False;
}

/****************************************************************************
 ***
 *** wd20_IsCOMServiceStandardParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_IsCOMServiceStandardParameter( const char	*parameter )
{

	sapdbwa_TraceDefFunction ("wd20_IsCOMServiceStandardParameter")

	/* Check parameter */
    if ( stricmp( parameter, ADMIN_REG_KEY_CLASS_ID )		== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_LOGFILE )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SERVICE_NAME )	== 0 ) return sapdbwa_True;;

	return sapdbwa_False;
}

/****************************************************************************
 ***
 *** wd20_IsCOMServiceDependentParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_IsCOMServiceDependentParameter( const char	*parameter )
{

	sapdbwa_TraceDefFunction ("wd20_IsCOMServiceDependentParameter")

	/* Check parameter */
    if ( stricmp( parameter, ADMIN_SERVICE_SERVICE )		== 0 ||
		 stricmp( parameter, ADMIN_SERVICE_PARAM_NAME )		== 0 ||
		 stricmp( parameter, ADMIN_ACTION )					== 0 ) return sapdbwa_True;;

	return sapdbwa_False;
}

/****************************************************************************
 ***
 *** wd20_IsSessionPoolStandardParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_IsSessionPoolStandardParameter( const char	*parameter )
{

	sapdbwa_TraceDefFunction ("wd20_IsSessionPoolStandardParameter")

	/* Check parameter */
    if ( stricmp( parameter, ADMIN_REG_KEY_SERVER_DB )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SERVER_NODE )		== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_USER_ID )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_PASSWORD )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_DATASOURCE )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_DRIVER )				== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_AUTOCOMMIT )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_POOL_TYPE )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_POOL_SIZE )			== 0 ||
		 stricmp( parameter, ADMIN_REG_KEY_SQL_TRACE_FILENAME )	== 0 ) return sapdbwa_True;;

	return sapdbwa_False;
}

/****************************************************************************
 ***
 *** wd20_IsSessionPoolDependentParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_IsSessionPoolDependentParameter( const char	*parameter )
{

	sapdbwa_TraceDefFunction ("wd20_IsSessionPoolDependentParameter")

	/* Check parameter */
    if ( stricmp( parameter, ADMIN_SERVICE_SHOW_SESSION_POOL )	== 0 ||
		 stricmp( parameter, ADMIN_SERVICE_PARAM_NAME )			== 0 ||
		 stricmp( parameter, ADMIN_ACTION )						== 0 ) return sapdbwa_True;;

	return sapdbwa_False;
}

/****************************************************************************
 ***
 *** wd20_ShowMain
 ***
 ****************************************************************************/
	
void wd20_ShowMain( sapdbwa_HttpReplyP			rep,
				    twd20WebAgentControlP	wa )
{

	sapdbwa_Int4		i = 0;

	sapdbwa_TraceDefFunction ("wd20_ShowMain")

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Send title */
	wd20_SendText( rep, 0, ADMIN_HTML_BEGIN );
	wd20_SendText( rep, 0, ADMIN_HTML_HEADER_BEGIN );
	wd20_SendText( rep, 0, ADMIN_HTML_TITLE );
	wd20_SendText( rep, 0, ADMIN_HTML_HEADER_END );

	/* Send basic js tree */
	wd20_SendText( rep, 0, ADMIN_JS_BEGIN );
	wd20_SendNavigationScript ( rep, wa );
	wd20_SendText( rep, 0, ADMIN_JS_REFRESH_NAVIGATION );
	wd20_SendText( rep, 0, ADMIN_JS_END );

	/* Send frameset */
	wd20_SendText( rep, 0, ADMIN_HTML_FRAMESET );

	wd20_SendText( rep, 0, ADMIN_HTML_END );

} /* wd20_ShowMain */

/****************************************************************************
 ***
 *** wd20_ShowNavigation
 ***
 ****************************************************************************/

void wd20_ShowNavigation( sapdbwa_HttpRequestP  req,
                          sapdbwa_HttpReplyP    rep )
{

	Reg_RegistryP   registry;
	sapdbwa_Bool    eos;
	sapdbwa_Int4    i = 0;
	sapdbwa_Int4    returnSize = 0;
	char            sessionPool[REG_MAX_VALUE_LEN+1];
	char            service[REG_MAX_VALUE_LEN+1];
	char            comService[REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_ShowNavigation")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        wd26SetErr( wd20WAControl.waErr,
                    ERR_OPEN_REGISTRY_WD26, wd20WAControl.iniFile, NULL );

		wd20_SendServerError( rep );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return;
    }

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Send title */
	if ( !wd20_SendText( rep, 0, ADMIN_HTML_BEGIN )) return;
	if ( !wd20_SendText( rep, 0, ADMIN_HTML_HEADER_BEGIN )) return;
	if ( !wd20_SendText( rep, 0, ADMIN_HTML_TITLE )) return;
	if ( !wd20_SendText( rep, 0, ADMIN_HTML_HEADER_END )) return;

	/* Send dynamic js tree */
	if ( !wd20_SendText( rep, 0, ADMIN_JS_BEGIN )) return;

	/*if ( stricmp( wd20_GetHTMLParameter( req, ADMIN_SERVICE_NAVIGATION_REFRESH ), "1" ) == 0 ) {
		wd20_SendText( rep, 0, ADMIN_JS_CLEAR_TREE );
		wd20_SendText( rep, 0, ADMIN_JS_TREE_SETTINGS );
	};*/
	wd20_SendText( rep, 0, ADMIN_JS_CLEAR_TREE );
	wd20_SendText( rep, 0, ADMIN_JS_TREE_SETTINGS );

	wd20_SendText( rep, 0, ADMIN_JS_TREE_CREATE_ROOT_FOLDERS );
#ifdef HAVE_COM_CLASSES
	wd20_SendText( rep, 0, ADMIN_JS_TREE_CREATE_ROOT_COM_FOLDERS );
#endif

	/* Add folder for global settings */
	if ( !wd20_SendText( rep, 0, ADMIN_JS_TREE_ADD_GLOBAL_SETTINGS )) return;

	/* Get session pools */
	if ( !wd20_SendText( rep, 0, ADMIN_JS_NEW_SESSIONPOOL )) return;

	/* Enumerate sections */
	if ( !Reg_EnumRegistrySections( registry, wd20WAControl.regSectionSessionPool, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_READ_REGISTRY_WD26, wd20WAControl.regSectionSessionPool, NULL );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return;
	}

	/* Iterate registry key */
	sessionPool[0]='\0';
	do {
		if ( !Reg_GetNextSection( registry, NULL, 0, sessionPool, sizeof( sessionPool ), &eos ))
			break;

		if ( !wd20_SendText( rep, 0, ADMIN_JS_TREE_ADD_SESSIONPOOL )) return;
		sapdbwa_SendBody( rep, sessionPool, 0 );
		if ( !wd20_SendText( rep, 0, ADMIN_HTML_SHOW_SESSIONPOOL)) return;
		sapdbwa_SendBody( rep, sessionPool, 0 );
		if ( !wd20_SendText( rep, 0, ADMIN_HTML_SHOW_END)) return;

		sessionPool[0]='\0';
	} while ( !eos );

	/* Get services */
	if ( !wd20_SendText( rep, 0, ADMIN_JS_NEW_SERVICE )) return;

	/* Enumerate sections */
	if ( !Reg_EnumRegistrySections( registry, wd20WAControl.regSectionServices, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_READ_REGISTRY_WD26, wd20WAControl.regSectionServices, NULL );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return;
	}

	/* Iterate registry sections */
	service[0]='\0';
	do {
		if ( !Reg_GetNextSection( registry, NULL, 0, service, sizeof( service ), &eos ))
			break;

		if ( !wd20_SendText( rep, 0, ADMIN_JS_TREE_ADD_SERVICE )) return;
		sapdbwa_SendBody( rep, service, 0 );
		if ( !wd20_SendText( rep, 0, ADMIN_HTML_SHOW_SERVICE)) return;
		sapdbwa_SendBody( rep, service, 0 );
		if ( !wd20_SendText( rep, 0, ADMIN_HTML_SHOW_END)) return;

		service[0]='\0';
	} while ( !eos );

#ifdef HAVE_COM_CLASSES
	/* Get COM services */
	if ( !wd20_SendText( rep, 0, ADMIN_JS_NEW_COM_SERVICE )) return;

	/* Enumerate sections */
	if ( !Reg_EnumRegistrySections( registry, wd20WAControl.regSectionCOMServices, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_READ_REGISTRY_WD26, wd20WAControl.regSectionCOMServices, NULL );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return;
	}

	/* Iterate registry key */
	comService[0]='\0';
	do {
		if ( !Reg_GetNextSection( registry, NULL, 0, comService, sizeof( comService ), &eos ))
			break;

		if ( !wd20_SendText( rep, 0, ADMIN_JS_TREE_ADD_COMSERVICE )) return;
		sapdbwa_SendBody( rep, comService, 0 );
		if ( !wd20_SendText( rep, 0, ADMIN_HTML_SHOW_COMSERVICE)) return;
		sapdbwa_SendBody( rep, comService, 0 );
		if ( !wd20_SendText( rep, 0, ADMIN_HTML_SHOW_END)) return;

		comService[0]='\0';
	} while ( !eos );
#endif

	wd20_SendText( rep, 0, ADMIN_JS_END );
	wd20_SendText( rep, 0, ADMIN_HTML_NAVIGATION );
	wd20_SendText( rep, 0, ADMIN_HTML_END );

	/* Close registry key */
    Reg_CloseRegistry( registry );
}

/****************************************************************************
 ***
 *** wd20_ShowGeneralSettings
 ***
 ****************************************************************************/

void wd20_ShowGeneralSettings( sapdbwa_HttpReplyP	 rep,
							   const char		*message )
{

	Reg_RegistryP	registry;
	char			developerMode[REG_MAX_VALUE_LEN+1];
    char            adminUserId[REG_MAX_VALUE_LEN+1];
    char            adminUserPassword[REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_ShowGeneralSettings")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile ))
		return;

	/* Get developer mode */
	developerMode[0]='\0';
	wd20_GetRegistryValue( registry, wd20WAControl.regSectionGeneralSettings,
						   ADMIN_REG_KEY_DEVELOPER_MODE,
						   developerMode, sizeof( developerMode ), "0"  );

	/* Get admins user id */
	adminUserId[0]='\0';
	wd20_GetRegistryValue(registry, wd20WAControl.regSectionGeneralSettings,
						  ADMIN_REG_KEY_ADMIN_USER_ID,
						  adminUserId, sizeof(adminUserId), "");

	/* Get admins user password */
	adminUserPassword[0]='\0';
	wd20_GetRegistryValue(registry, wd20WAControl.regSectionGeneralSettings,
						  ADMIN_REG_KEY_ADMIN_USER_PASSWORD,
						  adminUserPassword, sizeof(adminUserPassword), "");

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Build and send page */
	wd20_SendText( rep, 0, ADMIN_HTML_GENERAL_SETTINGS_FORM_BEGIN );
	if ( message )
		sapdbwa_SendBody( rep, message, 0 );
	else {
		sapdbwa_SendBody( rep, "", 0 );
	}
	wd20_SendText( rep, 0, ADMIN_HTML_GENERAL_SETTINGS_FORM_CONTINUE_1 );
	
	wd20_SendCheckBoxRow( rep,
						  ADMIN_CHECKBOX_GENERAL_SETTINGS_DEVELOPER_MODE_TITLE,
						  ADMIN_CHECKBOX_GENERAL_SETTINGS_DEVELOPER_MODE_NAME,
						  ADMIN_CHECKBOX_GENERAL_SETTINGS_DEVELOPER_MODE_NAME,
						  (bool) (strcmp( developerMode, ADMIN_DEVELOPER_MODE_ON_VALUE ) == 0),
						  sapdbwa_True);

	sapdbwa_SendBody( rep, "<tr><td colspan=\"2\" ><img src=\"/WARoot/Images/1x1.gif\" width=\"1\" height=\"20\"></td></tr>", 0 );
	sapdbwa_SendBody( rep, "<tr><td>Web Server Administrator</td></tr>", 0 );
	sapdbwa_SendBody( rep, "<tr><td colspan=\"2\" ><img src=\"/WARoot/Images/1x1.gif\" width=\"1\" height=\"5\"></td></tr>", 0 );

    wd20_SendInputRow(rep,
					  ADMIN_INPUT_GENERAL_SETTINGS_ADMIN_USER_ID_TITLE,
					  ADMIN_INPUT_GENERAL_SETTINGS_ADMIN_USER_ID_NAME,
					  adminUserId);
	
	wd20_SendInputRow(rep,
					  ADMIN_INPUT_GENERAL_SETTINGS_ADMIN_USER_PASSWORD_TITLE,
					  ADMIN_INPUT_GENERAL_SETTINGS_ADMIN_USER_PASSWORD_NAME,
					  adminUserPassword);

	/* Send horizontal line */
	wd20_SendHorizontalLineRow( rep, 2);
	
	wd20_SendText( rep, 0, ADMIN_HTML_GENERAL_SETTINGS_FORM_END );

}

/****************************************************************************
 ***
 *** wd20_UpdateGeneralSettings
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UpdateGeneralSettings( sapdbwa_HttpRequestP   req,
                                         sapdbwa_HttpReplyP     rep  )
{

	Reg_RegistryP    registry;
	const char      *developerMode = NULL;
	sapdbwa_Bool     rc = sapdbwa_False;
	const char      *message = NULL;
    const char      *adminUserId;
    const char      *adminUserPassword;

	sapdbwa_TraceDefFunction ("wd20_UpdateGeneralSettings")

	/* Set developer mode */
	developerMode = wd20_GetHTMLParameter( req, ADMIN_REG_KEY_DEVELOPER_MODE );
	if ( strlen(developerMode) < 1 ) {
		wd20_SetRegistryKey ( wd20WAControl.regSectionGeneralSettings,
							  ADMIN_REG_KEY_DEVELOPER_MODE,
							  ADMIN_DEVELOPER_MODE_OFF_VALUE );
	} else {
		wd20_SetRegistryKey ( wd20WAControl.regSectionGeneralSettings,
							  ADMIN_REG_KEY_DEVELOPER_MODE,
							  ADMIN_DEVELOPER_MODE_ON_VALUE );
	}

	/* Set admin user id */
	adminUserId = wd20_GetHTMLParameter(req, ADMIN_REG_KEY_ADMIN_USER_ID);
	wd20_SetRegistryKey(wd20WAControl.regSectionGeneralSettings,
						ADMIN_REG_KEY_ADMIN_USER_ID,
						adminUserId);

	/* Set admin user password */
	adminUserPassword = wd20_GetHTMLParameter(req, ADMIN_REG_KEY_ADMIN_USER_PASSWORD);
	wd20_SetRegistryKey(wd20WAControl.regSectionGeneralSettings,
						ADMIN_REG_KEY_ADMIN_USER_PASSWORD,
						adminUserPassword);

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Read global parameters */
	if ( wd27BegExcl( wd20WAControl.excl )) {
		if ( wd20_ReadGlobalParameters( &wd20WAControl, registry )) {
			rc = sapdbwa_True;
		}
		wd27EndExcl( wd20WAControl.excl );
	}

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Send confirmation */
	if ( rc )
		wd15GetString( 0, ADMIN_HTML_VALUE_UPDATE_CONFIRMATION, &message );
	else
		wd15GetString( 0, ADMIN_HTML_RESTART_WEBSERVER, &message );

	wd20_ShowGeneralSettings( rep, message );

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20_ShowResourceSettings
 ***
 ****************************************************************************/

void wd20_ShowResourceSettings( sapdbwa_HttpReplyP	 rep,
								const char		*message )
{

	Reg_RegistryP	registry;
	char			documentRoot	[MAX_FILENAME_LEN_WD00];
	char			picturesPath    [MAX_FILENAME_LEN_WD00];
	char			htmlPath		[MAX_FILENAME_LEN_WD00];
	char			stylesPath		[MAX_FILENAME_LEN_WD00];
	char			mimeTypesPath   [MAX_FILENAME_LEN_WD00];
	char			developerMode   [REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_ShowResourceSettings")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile ))
		return;

	/* Get document root path */
	if ( !wd20_GetRegistryValue( registry, wd20WAControl.regSectionResource, ADMIN_REG_KEY_WA_ROOT,
								 documentRoot, sizeof( documentRoot ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get mime types path */
	if ( !wd20_GetRegistryValue( registry, wd20WAControl.regSectionResource, ADMIN_REG_KEY_MIME_TYPES,
								 mimeTypesPath, sizeof( mimeTypesPath ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Build and send page */
	wd20_SendText( rep, 0, ADMIN_HTML_RESOURCE_SETTINGS_FORM_BEGIN );
	if ( message )
		sapdbwa_SendBody( rep, message, 0 );
	else {
		sapdbwa_SendBody( rep, "", 0 );
	}
	wd20_SendText( rep, 0, ADMIN_HTML_RESOURCE_SETTINGS_FORM_CONTINUE_1 );

	wd20_SendInputRow( rep,
					   ADMIN_INPUT_RESOURCE_SETTINGS_WA_ROOT_TITLE,
					   ADMIN_INPUT_RESOURCE_SETTINGS_WA_ROOT_NAME,
					   documentRoot );
	
	wd20_SendInputRow( rep,
					   ADMIN_INPUT_RESOURCE_SETTINGS_MIME_TYPES_TITLE,
					   ADMIN_INPUT_RESOURCE_SETTINGS_MIME_TYPES_NAME,
					   mimeTypesPath );
	

	/* Send horizontal line */
	wd20_SendHorizontalLineRow( rep, 2);
	
	wd20_SendText( rep, 0, ADMIN_HTML_RESOURCE_SETTINGS_FORM_END );

}

/****************************************************************************
 ***
 *** wd20_UpdateResourceSettings
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UpdateResourceSettings( sapdbwa_HttpRequestP	req,
										sapdbwa_HttpReplyP	rep  )
{

	Reg_RegistryP	 registry;
	const char		*developerMode = NULL;
	sapdbwa_Bool		 rc = sapdbwa_False;
	const char		*message = NULL;

	sapdbwa_TraceDefFunction ("wd20_UpdateResourceSettings")

	/* Set document root path */
	if ( !wd20_SetRegistryKey ( wd20WAControl.regSectionResource,
								ADMIN_REG_KEY_WA_ROOT,
								wd20_GetHTMLParameter( req, ADMIN_TEXT_WA_ROOT ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Set MIME types path */
	if ( !wd20_SetRegistryKey ( wd20WAControl.regSectionResource,
								ADMIN_REG_KEY_MIME_TYPES,
								wd20_GetHTMLParameter( req, ADMIN_VALUE_MIME_TYPES ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Read global parameters */
	if ( wd27BegExcl( wd20WAControl.excl )) {
		if ( wd20_ReadGlobalParameters( &wd20WAControl, registry )) {
			rc = sapdbwa_True;
		}
		wd27EndExcl( wd20WAControl.excl );
	}

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Send confirmation */
	if ( rc )
		wd15GetString( 0, ADMIN_HTML_VALUE_UPDATE_CONFIRMATION, &message );
	else
		wd15GetString( 0, ADMIN_HTML_RESTART_WEBSERVER, &message );

	wd20_ShowResourceSettings( rep, message );

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20_ShowLogSettings
 ***
 ****************************************************************************/

void wd20_ShowLogSettings( sapdbwa_HttpReplyP	 rep,
						   const char	*message )
{

	Reg_RegistryP	registry;
	char			logFilename		[MAX_FILENAME_LEN_WD00];
	char			confLogFilename [MAX_FILENAME_LEN_WD00];
	char			logWithInfo     [REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_ShowLogSettings")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile ))
		return;

	/* Get log filename */
	if ( !wd20_GetRegistryValue( registry, wd20WAControl.regSectionGlobal, ADMIN_REG_KEY_LOGFILE,
								 logFilename, sizeof( logFilename ), DEFAULT_LOGFILE  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get configuration log filename */
	if ( !wd20_GetRegistryValue( registry, wd20WAControl.regSectionGlobal, ADMIN_REG_KEY_CONF_LOGFILE,
								 confLogFilename, sizeof( confLogFilename ), DEFAULT_CONF_LOGFILE  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get developer mode */
	logWithInfo[0]='\0';
	wd20_GetRegistryValue( registry, wd20WAControl.regSectionGlobal,
						   ADMIN_REG_KEY_LOG_WITH_INFO,
						   logWithInfo, sizeof(logWithInfo), "0"  );

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Build and send page */
	wd20_SendText( rep, 0, ADMIN_HTML_GLOBAL_SETTINGS_FORM_BEGIN );
	if ( message )
		sapdbwa_SendBody( rep, message, 0 );
	else {
		sapdbwa_SendBody( rep, "", 0 );
	}
	wd20_SendText( rep, 0, ADMIN_HTML_GLOBAL_SETTINGS_FORM_CONTINUE_1 );

	wd20_SendInputRow( rep,
					   ADMIN_INPUT_GLOBAL_SETTINGS_LOGFILE_TITLE,
					   ADMIN_INPUT_GLOBAL_SETTINGS_LOGFILE_NAME,
					   logFilename );
	
	wd20_SendCheckBoxRow( rep,
						  ADMIN_CHECKBOX_GLOBAL_SETTINGS_LOG_WITH_INFO_TITLE,
						  ADMIN_CHECKBOX_GLOBAL_SETTINGS_LOG_WITH_INFO_NAME,
						  ADMIN_CHECKBOX_GLOBAL_SETTINGS_LOG_WITH_INFO_NAME,
						  (bool) (strcmp( logWithInfo, ADMIN_LOG_WITH_INFO_ON_VALUE ) == 0),
						  sapdbwa_True);

	wd20_SendInputRow( rep,
					   ADMIN_INPUT_GLOBAL_SETTINGS_CONF_LOGFILE_TITLE,
					   ADMIN_INPUT_GLOBAL_SETTINGS_CONF_LOGFILE_NAME,
					   confLogFilename );

	/* Send horizontal line */
	wd20_SendHorizontalLineRow( rep, 2);

	wd20_SendText( rep, 0, ADMIN_HTML_GLOBAL_SETTINGS_FORM_END );

} /* wd20_ShowLogSettings */

/****************************************************************************
 ***
 *** wd20_ShowMsgOnly
 ***
 ****************************************************************************/
sapdbwa_Bool wd20_ShowMsgOnly ( twd20WebAgentControlP	 wa,
							    sapdbwa_HttpRequestP		 req,
							    sapdbwa_HttpReplyP		 rep,
							    const char				*message )
{

	/* Show status */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

  sapdbwa_SendBody( rep, "<html><head><title>Status</title></head>",0);
  sapdbwa_SendBody( rep, "<body bgcolor=\"#DEDEC8\">", 0); 
  sapdbwa_SendBody( rep, message, 0 );
  sapdbwa_SendBody( rep, "<BR></P><HR><a href=\"javascript:history.back()\">Back</a>", 0);
  sapdbwa_SendBody( rep, "</body></html>", 0);

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_ShowService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowService( twd20WebAgentControlP	 wa,
							   sapdbwa_HttpRequestP		 req,
							   sapdbwa_HttpReplyP		 rep,
							   const char				*name,
							   const char				*message,
							   const char				*newBrowser )
{

	Reg_RegistryP	registry;
	sapdbwa_Bool	eos;
	sapdbwa_Bool	eok;

	char	withSSL					[REG_MAX_VALUE_LEN+1];
	char	sslURL					[REG_MAX_VALUE_LEN+1];
	char	serviceStart			[REG_MAX_VALUE_LEN+1];
	char	useFastCGIForCookiePath	[REG_MAX_VALUE_LEN+1];
	char	exitFunction			[REG_MAX_VALUE_LEN+1];
	char	initFunction			[REG_MAX_VALUE_LEN+1];
	char	library					[MAX_FILENAME_LEN_WD00+1];
	char	libraryType				[REG_MAX_VALUE_LEN+1];
	char	logFile					[MAX_FILENAME_LEN_WD00+1];
	char	serviceFunction			[REG_MAX_VALUE_LEN+1];
	char	serviceName				[REG_MAX_SECTION_LEN+1];
	char	sessionPool				[REG_MAX_SECTION_LEN+1];
	char	currentSection			[REG_MAX_SECTION_LEN+1];
	char	keyName					[REG_MAX_KEY_LEN+1];
	char	value					[REG_MAX_VALUE_LEN+1];
	char	sectionName				[REG_MAX_SECTION_LEN+1];
	char	webSessionTimeout		[REG_MAX_VALUE_LEN+1];
	char	templateFilename		[MAX_FILENAME_LEN_WD00+1];
    const char * service = NULL;

	sapdbwa_TemplateValueList			*templateValueList = NULL;
	sapdbwa_TemplateValueTable			*templateTable = NULL;
	sapdbwa_TemplateValueTableRow		*templateRow = NULL;
	sapdbwa_TemplateValueTableColumn	*templateColumn = NULL;

	sapdbwa_TraceDefFunction ("wd20_ShowService")

	/* MsgOnly mode */
    service = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_OUTPUT );
    if (stricmp(service, ADMIN_OUTPUT_MSGSONLY) == 0) {
      return wd20_ShowMsgOnly( wa, req, rep, message );
    } 

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile ))
		return sapdbwa_False;

	/* Get current section */
	if ( !name )
		name = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionServices, name);

	/* Get exit function */
	if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_EXIT_FUNCTION,
								 exitFunction, sizeof( exitFunction ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return sapdbwa_False;
	}

	/* Get init function */
	if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_INIT_FUNCTION,
								 initFunction, sizeof( initFunction ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return sapdbwa_False;
	}

	/* Get library */
	if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_LIBRARY,
								 library, sizeof( library ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return sapdbwa_False;
	}

	/* Get library type */
	if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_LIBRARY_TYPE,
								 libraryType, sizeof( libraryType ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return sapdbwa_False;
	}

	/* Get log file */
	if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_LOGFILE,
								 logFile, sizeof( logFile ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return sapdbwa_False;
	}

	/* Get service function */
	if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_SERVICE_FUNCTION,
								 serviceFunction, sizeof( serviceFunction ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return sapdbwa_False;
	}

	/* Get service name */
	if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_SERVICE_NAME,
								 serviceName, sizeof( serviceName ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return sapdbwa_False;
	}

	/* Get session pool */
	if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_SESSION_POOL,
								 sessionPool, sizeof( sessionPool ), ""  )) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		return sapdbwa_False;
	}

	/* Get startup type */
	wd20_GetRegistryValue( registry, currentSection,
						   ADMIN_REG_KEY_SERVICE_START, serviceStart, sizeof( serviceStart ), "1"  );

	/* Get cookie path prefix setting	*/
	wd20_GetRegistryValue( registry, currentSection,
						   ADMIN_REG_KEY_SERVICE_COOKIE_PATH_PREFIX,
						   useFastCGIForCookiePath, sizeof( useFastCGIForCookiePath ),
						   ADMIN_SERVICE_COOKIE_PATH_PREFIX_ON_VALUE  );

	/* Get secure type */
	wd20_GetRegistryValue( registry, currentSection,
						   ADMIN_REG_KEY_SERVICE_WITH_SSL, withSSL, sizeof( withSSL ), "0"  );

	/* Get SSL URL */
	wd20_GetRegistryValue( registry, currentSection,
						   ADMIN_REG_KEY_SERVICE_SSL_URL, sslURL, sizeof( sslURL ), ""  );

	/* Get web session timeout */
	wd20_GetRegistryValue( registry, currentSection,
						   ADMIN_REG_KEY_WEB_SESSION_TIMEOUT, webSessionTimeout, sizeof( webSessionTimeout ), "0"  );

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Build and send page */
	CreateTemplateValueList( &templateValueList );

	if ( message ) {
		AddValueToTemplateValueList( templateValueList, "Status", message );
	} else {
		AddValueToTemplateValueList( templateValueList, "Status", "" );
	}

	if ( newBrowser ) {
		AddValueToTemplateValueList( templateValueList, "NewBrowser", newBrowser );
	}
	else {
		AddValueToTemplateValueList( templateValueList, "NewBrowser", "" );
	}

	AddValueToTemplateValueList( templateValueList, "ServiceName", name );
	AddValueToTemplateValueList( templateValueList, "ServiceURI", serviceName );
	AddValueToTemplateValueList( templateValueList, "InitFunction", initFunction );
	AddValueToTemplateValueList( templateValueList, "ExitFunction", exitFunction );
	AddValueToTemplateValueList( templateValueList, "ServiceFunction", serviceFunction );
	AddValueToTemplateValueList( templateValueList, "Library", library );

	if ( strcmp( libraryType, ADMIN_LIBRARY_TYPE_C_VALUE ) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectC", "SELECTED" );
		AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectCPP", "" );
	} else {
		AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectC", "" );
		AddValueToTemplateValueList( templateValueList, "LibraryTypeSelectCPP", "SELECTED" );
	}

	AddValueToTemplateValueList( templateValueList, "LogFilename", logFile );

	/* Get session pools	*/
	if ( !Reg_EnumRegistrySections( registry, wa->regSectionSessionPool, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr, ERR_READ_REGISTRY_WD26,
					wa->regSectionSessionPool, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		DropTemplateValueList( templateValueList );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Create table template for session pools	*/
	CreateTemplateValueTable( &templateTable );

	/* Add default list item	*/
	CreateTemplateValueTableRow( &templateRow );

	if ( strcmp( "", sessionPool ) == 0 ) {
		CreateTemplateValueTableColumn( &templateColumn, "SELECTED" );
	} else {
		CreateTemplateValueTableColumn( &templateColumn, "" );
	}
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	CreateTemplateValueTableColumn( &templateColumn, "" );
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	AddRowToTemplateValueTable( templateTable, templateRow );

	/* Get available session pools	*/
	sectionName[0]='\0';
	do {
		if ( !Reg_GetNextSection( registry, NULL, 0, sectionName, sizeof( sectionName ), &eos ))
			break;

		CreateTemplateValueTableRow( &templateRow );

		if ( strcmp( sectionName, sessionPool ) == 0 ) {
			CreateTemplateValueTableColumn( &templateColumn, "SELECTED" );
		} else {
			CreateTemplateValueTableColumn( &templateColumn, "" );
		}
		AddColumnToTemplateValueTableRow( templateRow, templateColumn );

		CreateTemplateValueTableColumn( &templateColumn, sectionName );
		AddColumnToTemplateValueTableRow( templateRow, templateColumn );

		AddRowToTemplateValueTable( templateTable, templateRow );

		sectionName[0]='\0';
	} while ( !eos );

	AddTableToTemplateValueList( templateValueList, "SessionPools", templateTable );

	/* Stratup type */
	if ( strcmp( serviceStart, ADMIN_SERVICE_START_ON_VALUE) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "ServiceStartChecked", "CHECKED" );
	} else {
		AddValueToTemplateValueList( templateValueList, "ServiceStartChecked", "" );
	}

	/* Cookie path prefix setting	*/
	if ( strcmp( useFastCGIForCookiePath, ADMIN_SERVICE_COOKIE_PATH_PREFIX_ON_VALUE) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "UseFastCGIForCookiePathChecked", "CHECKED" );
	} else {
		AddValueToTemplateValueList( templateValueList, "UseFastCGIForCookiePathChecked", "" );
	}

	/* Secure type */
	if ( strcmp( withSSL, ADMIN_SERVICE_WITH_SSL_ON_VALUE) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "WithSSLChecked", "CHECKED" );
	} else {
		AddValueToTemplateValueList( templateValueList, "WithSSLChecked", "" );
	}

	/* SSL URL */
	AddValueToTemplateValueList( templateValueList, "SSLURL", sslURL );

	/* Web session timeout */
	AddValueToTemplateValueList( templateValueList, "WebSessionTimeout", webSessionTimeout );

	/* User defined parameters	*/
	if ( !Reg_EnumRegistryKeys( registry, currentSection, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr, ERR_READ_REGISTRY_WD26,
                    currentSection, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		DropTemplateValueList( templateValueList );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Iterate registry keys */
	keyName[0]='\0';
	value[0]='\0';
	CreateTemplateValueTable( &templateTable );
	do {
		if( !Reg_GetNextKey( registry, keyName, sizeof( keyName ), value, sizeof( value ), &eok ))
			break;

		if ( !wd20_IsServiceStandardParameter( keyName )) {
			/* Add user parameter */
			CreateTemplateValueTableRow( &templateRow );

			CreateTemplateValueTableColumn( &templateColumn, keyName );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			CreateTemplateValueTableColumn( &templateColumn, keyName );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			CreateTemplateValueTableColumn( &templateColumn, value );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			AddRowToTemplateValueTable( templateTable, templateRow );
		}

		keyName[0]='\0';
		value[0]='\0';
	} while ( !eok );

	AddTableToTemplateValueList( templateValueList, "Parameters", templateTable );

	/* Buttons	*/
	CreateTemplateValueTable( &templateTable );

	/* Button Update	*/
	CreateTemplateValueTableRow( &templateRow );

	CreateTemplateValueTableColumn( &templateColumn, ADMIN_ACTION_UPDATE_SERVICE );
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	CreateTemplateValueTableColumn( &templateColumn, ADMIN_BUTTON_UPDATE );
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	AddRowToTemplateValueTable( templateTable, templateRow );

	/* Button New Service Parameter	*/
	CreateTemplateValueTableRow( &templateRow );

	CreateTemplateValueTableColumn( &templateColumn, ADMIN_ACTION_DEFINE_NEW_SERVICE_PARAMETER );
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	CreateTemplateValueTableColumn( &templateColumn, ADMIN_BUTTON_NEW_PARAMETER );
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	AddRowToTemplateValueTable( templateTable, templateRow );

	/* Button Show Service Parameters to Delete	*/
	CreateTemplateValueTableRow( &templateRow );

	CreateTemplateValueTableColumn( &templateColumn, ADMIN_ACTION_SHOW_SERVICE_PARAMETERS_TO_DELETE );
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	CreateTemplateValueTableColumn( &templateColumn, ADMIN_BUTTON_DELETE_PARAMETERS );
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	AddRowToTemplateValueTable( templateTable, templateRow );

	/* Add buttons to template value list	*/
	AddTableToTemplateValueList( templateValueList, "Buttons", templateTable );

	/* Buttons2	*/
	CreateTemplateValueTable( &templateTable );

	/* Is service already loaded?	*/
	if ( wd20_FindServiceInList( wa->serviceList, name )) {
		if ( wa->developerMode ) {
			/* Button Start Service	*/
			CreateTemplateValueTableRow( &templateRow );

			CreateTemplateValueTableColumn( &templateColumn, ADMIN_ACTION_START_SERVICE );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			CreateTemplateValueTableColumn( &templateColumn, ADMIN_BUTTON_START_SERVICE );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			AddRowToTemplateValueTable( templateTable, templateRow );

			/* Button Unload Service	*/
			CreateTemplateValueTableRow( &templateRow );

			CreateTemplateValueTableColumn( &templateColumn, ADMIN_ACTION_UNLOAD_SERVICE );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			CreateTemplateValueTableColumn( &templateColumn, ADMIN_BUTTON_UNLOAD_SERVICE );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			AddRowToTemplateValueTable( templateTable, templateRow );

			/* Button Reload Service	*/
			CreateTemplateValueTableRow( &templateRow );

			CreateTemplateValueTableColumn( &templateColumn, ADMIN_ACTION_RELOAD_SERVICE );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			CreateTemplateValueTableColumn( &templateColumn, ADMIN_BUTTON_RELOAD_SERVICE );
			AddColumnToTemplateValueTableRow( templateRow, templateColumn );

			AddRowToTemplateValueTable( templateTable, templateRow );
		}
	} else {
		/* Button Load Service	*/
		CreateTemplateValueTableRow( &templateRow );

		CreateTemplateValueTableColumn( &templateColumn, ADMIN_ACTION_LOAD_SERVICE );
		AddColumnToTemplateValueTableRow( templateRow, templateColumn );

		CreateTemplateValueTableColumn( &templateColumn, ADMIN_BUTTON_LOAD_SERVICE );
		AddColumnToTemplateValueTableRow( templateRow, templateColumn );

		AddRowToTemplateValueTable( templateTable, templateRow );
	}

	/* Button Show Service to Delete	*/
	CreateTemplateValueTableRow( &templateRow );

	CreateTemplateValueTableColumn( &templateColumn, ADMIN_ACTION_SHOW_SERVICE_TO_DELETE );
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	CreateTemplateValueTableColumn( &templateColumn, ADMIN_BUTTON_DELETE_SERVICE );
	AddColumnToTemplateValueTableRow( templateRow, templateColumn );

	AddRowToTemplateValueTable( templateTable, templateRow );

	/* Add buttons to template value list	*/
	AddTableToTemplateValueList( templateValueList, "Buttons2", templateTable );

	/* Build template name	*/
	sp77sprintf(templateFilename, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_SHOW_SERVICE );

	/* Write template	*/
	WriteTemplate(rep, templateFilename, templateValueList, wa->waErr );

	/* Free value list	*/
	DropTemplateValueList( templateValueList );
	
	/* Close registry */
	Reg_CloseRegistry( registry );

	return sapdbwa_False;

}	

/****************************************************************************
 ***
 *** wd20_UpdateService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UpdateService( twd20WebAgentControlP	wa,
							     sapdbwa_HttpRequestP	req,
							     sapdbwa_HttpReplyP		rep  )
{

	sapdbwa_Int4			 i = 0;
    sapdbwa_StringSeqP		 keyList = sapdbwa_CreateStringSeq();
	const char				*key = NULL;
	const char				*sslURL = NULL;
	const char				*service = NULL;
	char					 currentSection				[REG_MAX_SECTION_LEN+1];
	char					 serviceStart				[REG_MAX_VALUE_LEN+1];
	char					 useFastCGIForCookiePath	[REG_MAX_VALUE_LEN+1];
	char					 withSSL					[REG_MAX_VALUE_LEN+1];
	char					 webSessionTimeout			[REG_MAX_VALUE_LEN+1];
	sapdbwa_Bool			 withSSLBool = sapdbwa_False;
	twd20ServiceListItemP	 foundService = NULL;
	const char				*text = NULL;
	char					 status	[MAX_STATUS_LEN];

	twd20_ServiceDescription	serviceDescription;

	sapdbwa_TraceDefFunction ("wd20_UpdateService")

	/* Inits */
	wd20_GetServiceDescription( req, &serviceDescription );

	status[0] = '\0';
	strcpy( serviceStart, ADMIN_SERVICE_START_OFF_VALUE );
	strcpy( useFastCGIForCookiePath, ADMIN_SERVICE_COOKIE_PATH_PREFIX_OFF_VALUE );
	strcpy( withSSL, ADMIN_SERVICE_WITH_SSL_OFF_VALUE );

	/* Get current section */
	service = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionServices, service );

	/* Get list of keys */
	if ( !sapdbwa_GetParameterNames( req, keyList )) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	/* Update registry */
	for ( i = 0 ; i < sapdbwa_GetNumElem( keyList ) ; i++ ) {
		if (( key = sapdbwa_GetStringByIndex( keyList, i )) != NULL ) {
			if ( !wd20_IsServiceDependentParameter( key )) {
				if ( strcmp( key, ADMIN_REG_KEY_SERVICE_START ) == 0 ) {
					strcpy( serviceStart, ADMIN_SERVICE_START_ON_VALUE );
				} else if ( strcmp( key, ADMIN_REG_KEY_SERVICE_COOKIE_PATH_PREFIX ) == 0 ) {
					strcpy( useFastCGIForCookiePath, ADMIN_SERVICE_COOKIE_PATH_PREFIX_ON_VALUE );
				} else if ( strcmp( key, ADMIN_REG_KEY_SERVICE_WITH_SSL ) == 0 ) {
					strcpy( withSSL, ADMIN_SERVICE_WITH_SSL_ON_VALUE );
					withSSLBool = sapdbwa_True;
				} else if ( strcmp( key, ADMIN_REG_KEY_SERVICE_SSL_URL ) == 0 ) {
						sslURL = wd20_GetHTMLParameter( req, key );
				} else {
					if ( !wd20_SetRegistryKey ( currentSection,
												key,
												wd20_GetHTMLParameter( req, key ))) {
						wd20_SendServerError( rep );
						wd20_FreeServiceDescription( &serviceDescription );
						return sapdbwa_False;
					}
				}
			}
		}
	}

	/* Update startup type */
	if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_START, serviceStart )) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	/* Update cookie path prefix setting	*/
	if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_COOKIE_PATH_PREFIX, useFastCGIForCookiePath )) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	/* Update web session timeout */
	wd20_GetTimeValue( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_WEB_SESSION_TIMEOUT ), webSessionTimeout, sizeof(webSessionTimeout) );
	if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_WEB_SESSION_TIMEOUT, webSessionTimeout )) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	/* Check SSL URL */
	if ( withSSLBool ) {
		if ( !wd20_IsSSLURL( sslURL )) {
			wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_WITH_SSL, ADMIN_SERVICE_WITH_SSL_OFF_VALUE );

			if ( wd15GetString( 0, ADMIN_HTML_SERVICE_MISSING_SSL_URL, &text ))
				strcat( status, text );

			wd20_ShowServiceDescription( &serviceDescription, wa, rep, status );
			wd20_FreeServiceDescription( &serviceDescription );

			return sapdbwa_True;;
		}
	}

	/* Update secure type */
	if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_WITH_SSL, withSSL )) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	/* Update SSL URL */
	if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_SSL_URL, sslURL)) {
		wd20_SendServerError( rep );
		wd20_FreeServiceDescription( &serviceDescription );
		return sapdbwa_False;
	}

	/* Update service if service is already loaded */
	foundService = wd20_FindServiceInList( wa->serviceList, service );
	if ( foundService ) {
		wd40SetPropertyWithSSL ( foundService->service, withSSLBool );
		wd40SetPropertySSLURL ( foundService->service, sslURL );
		wd40SetPropertyWebSessionTimeout ( foundService->service, atoi( webSessionTimeout ));
	}

	/* Send confirmation */
	if ( wd15GetString( 0, ADMIN_HTML_VALUE_UPDATE_CONFIRMATION, &text ))
		strcat( status, text );
	wd20_ShowService( wa, req, rep, serviceDescription.service, status, NULL );

	wd20_FreeServiceDescription( &serviceDescription );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_UpdateLogSettings
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UpdateLogSettings( sapdbwa_HttpRequestP	req,
								   sapdbwa_HttpReplyP		rep )
{

	const char	*message = NULL;
	const char  *logWithInfo = NULL;

	sapdbwa_TraceDefFunction ("wd20_UpdateLogSettings")

	/* Set log filename */
	if ( !wd20_SetRegistryKey ( wd20WAControl.regSectionGlobal,
								ADMIN_REG_KEY_LOGFILE,
								wd20_GetHTMLParameter( req, ADMIN_VALUE_LOGFILE ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	};

	/* Set configuration log filename */
	if ( !wd20_SetRegistryKey ( wd20WAControl.regSectionGlobal,
								ADMIN_REG_KEY_CONF_LOGFILE,
								wd20_GetHTMLParameter( req, ADMIN_VALUE_CONF_LOGFILE ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	};

	/* Set log with info */
	logWithInfo = wd20_GetHTMLParameter( req, ADMIN_REG_KEY_LOG_WITH_INFO);
	if ( strlen(logWithInfo) < 1 ) {
		wd20_SetRegistryKey ( wd20WAControl.regSectionGlobal,
							  ADMIN_REG_KEY_LOG_WITH_INFO,
							  ADMIN_LOG_WITH_INFO_OFF_VALUE );

        wd25SetLogInfoFlag(wd20WAControl.waLog, sapdbwa_False);
	} else {
		wd20_SetRegistryKey ( wd20WAControl.regSectionGlobal,
							  ADMIN_REG_KEY_LOG_WITH_INFO,
							  ADMIN_LOG_WITH_INFO_ON_VALUE );

        wd25SetLogInfoFlag(wd20WAControl.waLog, sapdbwa_True);
	}

	/* Send confirmation */
	wd15GetString( 0, ADMIN_HTML_VALUE_UPDATE_CONFIRMATION, &message );
	wd20_ShowLogSettings( rep, message );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** 
 ***
 ****************************************************************************/

sapdbwa_UInt4 wd20_WAAdminShowFile( sapdbwa_HttpReplyP rep, const char *fileName )
{
    FILE        *fin   = NULL;
    char         buf[1024];
    sapdbwa_UInt4  bytesRead = 0;
	sapdbwa_UInt4  fileLen = 0;
	char		 errorText [128];

	sapdbwa_TraceDefFunction ("wd20_WAAdminShowFile")

    if ((fin = fopen( fileName, "r")) != NULL) {
        sapdbwa_SendBody( rep, "<pre><code>", 0 );
        while (0 < (bytesRead = fread( buf, 1, sizeof(buf), fin))) {
            sapdbwa_SendBody( rep, buf, bytesRead );
			fileLen += bytesRead;
        };
        fclose( fin );
        sapdbwa_SendBody( rep, "</code></pre>", 0 );
    } else {
        /* file open error */
		errorText[0] = '\0';
		sp77sprintf(errorText, 128, "[%d] ", errno );
        sapdbwa_SendBody( rep, "Can not open file ", 0 );
        sapdbwa_SendBody( rep, errorText, 0 );
        sapdbwa_SendBody( rep, fileName, 0 );
        sapdbwa_SendBody( rep, "\n", 0 );
    };

	return fileLen;
} /* wd20_WAAdminShowFile */


void wd20_SendHtmlHeader( sapdbwa_HttpReplyP rep, const char *title )
{

	sapdbwa_TraceDefFunction ("wd20_SendHtmlHeader")

    sapdbwa_SendBody( rep, "<HTML><HEAD><TITLE>", 0 );
    sapdbwa_SendBody( rep, title, 0 );
    sapdbwa_SendBody( rep, "</TITLE></HEAD><BODY BGCOLOR=\"#80C0E4\">", 0);

} /* wd20_SendHtmlHeader */


void wd20_SendHtmlTrailer( sapdbwa_HttpReplyP rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendHtmlTrailer")

    sapdbwa_SendBody( rep, "</BODY></HTML>", 0 );

} /* wd20_SendHtmlTrailer */


#ifdef HAVE_COM_CLASSES

sapdbwa_Bool wd20_CreateComClasses( twd20WebAgentControlP   wa,
                                    Reg_RegistryP           registry )
{
    sapdbwa_Int4    copied = 0; /* Number of characters copied */
    char          **comClassesArray = NULL;
    sapdbwa_UInt4   actIndex;
    sapdbwa_UInt4   nComClasses;
    sapdbwa_UInt4   nonUnique;
    sapdbwa_Bool    allocatOK;

	sapdbwa_TraceDefFunction ("wd20_CreateComClasses")

    if (!wd20_MakeArray( registry,
                         wa->regSectionCOMServices,
                         &comClassesArray,
                         &nComClasses,
                         wa->waErr )
        || (nComClasses == 0)) {
        wa->nComClasses = 0;
        
        return sapdbwa_True;;
    } else {
        if (!wd20_StringsAreUnique( comClassesArray,
                                    nComClasses,
                                    &nonUnique )) {
            /* names must be unique */
            wd26SetErr( wa->waErr,
                        ERR_NON_UNIQUE_WD26,
                        comClassesArray[nonUnique], NULL );
            wd20_FreeArray( comClassesArray, nComClasses );
            return sapdbwa_False;
        }; /* if */
        /* create comClasses array */
        sapdbwa_SQLALLOCAT( nComClasses*sizeof(twd96ComClassP),
                    (sapdbwa_UInt1**) &wa->comClasses,
                    &allocatOK );
        if (!allocatOK) {
            /* memory alloc failure */
            wd26SetErr( wa->waErr, ERR_MEMORY_WD26, NULL, NULL );
            wd20_FreeArray( comClassesArray, nComClasses );
            return sapdbwa_False;
        }; /* if */
        for ( actIndex = 0; actIndex < nComClasses; actIndex++ ) {
            wa->comClasses[actIndex] =
                wd96CreateComClass( comClassesArray[actIndex],
                                    actIndex,
                                    wa->waErr );
            if (wa->comClasses[actIndex] == NULL) {
                /* memory alloc failure or name too long */
                wd20_FreeArray( comClassesArray, nComClasses );
                wa->nComClasses = actIndex;
                wd20_FreeComClasses( wa );
                return sapdbwa_False;
            }; /* if */
        }; /* for */
        wd20_FreeArray( comClassesArray, nComClasses );
        wa->nComClasses = nComClasses;
    }; /* else */

    return sapdbwa_True;;    
}


sapdbwa_Bool wd20_InitComClasses( twd20WebAgentControlP wa,
                                  Reg_RegistryP         registry )
{
    char	pathPrefix [REG_MAX_VALUE_LEN+1];
    char	logFile    [REG_MAX_VALUE_LEN+1];
    char	classId    [MAX_CLASSID_LEN_WD00+1];
	char	currentSection [REG_MAX_SECTION_LEN+1];
    
    sapdbwa_UInt4  iComClass;

	sapdbwa_TraceDefFunction ("wd20_InitComClasses")

    for (iComClass = 0; iComClass < wa->nComClasses; iComClass++ ) {
		/* Set current section */
		sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionCOMServices, wd96GetComClassName(wa->comClasses[iComClass]));

		/* Get prefix */
		pathPrefix[0] = '\0';
		if ( !wd20_GetRegistryValue( registry, currentSection,
									 ADMIN_REG_KEY_SERVICE_NAME,
									 pathPrefix, sizeof( pathPrefix ), "" ))
			return sapdbwa_False;

        if ( strlen(pathPrefix) == 0 ) {
            /* missing prefix */
            wd26SetErr( wa->waErr,
                        ERR_NO_PREFIX_WD26,
                        wd96GetComClassName(wa->comClasses[iComClass]), NULL );
            return sapdbwa_False;
        };

		/* Get class id */
        classId[0] = '\0';
		if ( !wd20_GetRegistryValue( registry, currentSection,
									 ADMIN_REG_KEY_CLASS_ID,
									 classId, sizeof( classId ), "" ))
            return sapdbwa_False;

		if ( strlen(classId) == 0 ) {
            /* missing class id */
            wd26SetErr( wa->waErr,
                        ERR_NO_CLASSID_WD26,
                        wd96GetComClassName(wa->comClasses[iComClass]), NULL );
            return sapdbwa_False;
        };

		/* Get log filename */
        logFile[0] = '\0';
		if ( !wd20_GetRegistryValue( registry, currentSection,
									 ADMIN_REG_KEY_LOGFILE,
									 logFile, sizeof( logFile ), "" ))
            return sapdbwa_False;

		/* Inits COM class */
        if (!wd96InitComClass( wa->comClasses[iComClass],
                               pathPrefix,
                               classId,
                               logFile,
                               wa->waErr )) {
            /* init failed */
            return sapdbwa_False;
        };
    };

    return sapdbwa_True;
}


void wd20_FreeComClasses( twd20WebAgentControlP wa )
{
    sapdbwa_UInt4 iComClass;

	sapdbwa_TraceDefFunction ("wd20_FreeComClasses")

    if (wa->comClasses != NULL) {
        for (iComClass=0; iComClass < wa->nComClasses; iComClass++) {
            if (wa->comClasses[iComClass] != NULL) {
                wd96DestroyComClass( wa->comClasses[iComClass] );
            }; /* if */
        }; /* for */
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) wa->comClasses );
        wa->comClasses = NULL;
        wa->nComClasses = 0;
    }; /* if */
} /* wd20_FreeComClasses */


twd96ComClassP wd20_FindComClass( twd20WebAgentControlP  wa,
                                  char                  *uri )
{
    sapdbwa_UInt4     iComClass;
    twd96ComClassP  comClass = NULL;
    char           *pureUri = uri;
    sapdbwa_Int4      prefixLength = 0;
    sapdbwa_Int4      maxPrefixLength = 0;

	sapdbwa_TraceDefFunction ("wd20_FindComClass")

    for (iComClass = 0; iComClass < wa->nComClasses; iComClass++ ) {
        if (wd20_IsPrefix( wd96GetPathPrefix(wa->comClasses[iComClass]),
                           pureUri,
                           &prefixLength )) {
            if (prefixLength > maxPrefixLength) {
                comClass = wa->comClasses[iComClass];
                maxPrefixLength = prefixLength;
            };
        };
    }; /* for */

    return comClass;
} /* wd20_FindComClass */

#endif /* HAVE_COM_CLASSES */

/****************************************************************************
 ***
 *** wd20_SendServerError
 ***
 ****************************************************************************/

void wd20_SendServerError( sapdbwa_HttpReplyP rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendServerError")

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_ServerError, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Send error */
	wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
}

/****************************************************************************
 ***
 *** wd20_GetHTMLParameter
 ***
 ****************************************************************************/

const char* wd20_GetHTMLParameter( sapdbwa_HttpRequestP	 req,
								   const char		*parameterName )
{

    sapdbwa_StringSeqP	 valueList = sapdbwa_CreateStringSeq();
	const char		     *value = NULL;

	sapdbwa_TraceDefFunction ("wd20_GetHTMLParameter")

    if ( sapdbwa_GetParameterValues( req, parameterName, valueList )) {
		if (( value = sapdbwa_GetStringByIndex( valueList, 0 )) != NULL ) {
			/* Drop objects */
			sapdbwa_DestroyStringSeq( valueList );

			return value;
		}
	}

	/* Drop objects */
	sapdbwa_DestroyStringSeq( valueList );

	return "";

}

/****************************************************************************
 ***
 *** wd20_GetRegistryValue
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_GetRegistryValue( Reg_RegistryP		 registry,
								  const char	*section,
								  const char	*key,
								  char			*value,
								  sapdbwa_Int4	 valueSize,
								  const char	*defaultValue )
{

	Reg_RegistryP		myRegistry = registry;

	sapdbwa_TraceDefFunction ("wd20_GetRegistryValue")

	/* Open registry if needed */
	if ( !myRegistry ) {
		if ( !Reg_OpenRegistry( &myRegistry, wd20WAControl.iniFile )) {
			/* Set default value */
			strcpy( value, defaultValue );

			return sapdbwa_False;
		}
	}

	/* Get registry value */
	if ( !Reg_GetRegistryKey( myRegistry, section, key, value, valueSize )) {
		if ( !registry )
			Reg_CloseRegistry( myRegistry );

		/* Set default value */
		strcpy( value, defaultValue );

		return sapdbwa_False;
	}

	/* Close registry */
    if ( !registry )
		Reg_CloseRegistry( myRegistry );

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20_RegistrySectionExists
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_RegistrySectionExists( Reg_RegistryP	 registry,
									   const char		*section,
									   sapdbwa_Bool		*found		)
{

	Reg_RegistryP	myRegistry = registry;

	sapdbwa_TraceDefFunction ("wd20_RegistrySectionExists")

	/* Inits */
	*found = sapdbwa_False;

	/* Open registry if needed */
	if ( !myRegistry ) {
		if ( !Reg_OpenRegistry( &myRegistry, wd20WAControl.iniFile ))
			return sapdbwa_False;
	}

	/* Search section */
	if ( !Reg_SectionExists( myRegistry, section, found )) {
		if ( !registry )
			Reg_CloseRegistry( myRegistry );

		return sapdbwa_False;
	}

	/* Close registry */
    if ( !registry )
		Reg_CloseRegistry( myRegistry );

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20_RegistryKeyExists
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_RegistryKeyExists( Reg_RegistryP	 registry,
								   const char	*section,
								   const char	*key,
								   sapdbwa_Bool	*found		)
{

	Reg_RegistryP	myRegistry = registry;

	sapdbwa_TraceDefFunction ("wd20_RegistryKeyExists")

	/* Inits */
	*found = sapdbwa_False;

	/* Open registry if needed */
	if ( !myRegistry ) {
		if ( !Reg_OpenRegistry( &myRegistry, wd20WAControl.iniFile ))
			return sapdbwa_False;
	}

	/* Search section */
	if ( !Reg_KeyExists( myRegistry, section, key, found )) {
		if ( !registry )
			Reg_CloseRegistry( myRegistry );

		return sapdbwa_False;
	}

	/* Close registry */
    if ( !registry )
		Reg_CloseRegistry( myRegistry );

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20_SetRegistryKey
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_SetRegistryKey ( const char	*section,
								 const char	*key,
								 const char	*value		)
{

	sapdbwa_TraceDefFunction ("wd20_SetRegistryKey")

	return Reg_SetRegistryKey(wd20WAControl.iniFile, section, key, value);

}

/****************************************************************************
 ***
 *** wd20_DeleteRegistrySection
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteRegistrySection( const char	*section )
{

	sapdbwa_TraceDefFunction ("wd20_DeleteRegistrySection")

	return Reg_DeleteRegistrySection ( wd20WAControl.iniFile, section );

}

/****************************************************************************
 ***
 *** wd20_DeleteRegistryKey
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteRegistryKey( const char	*section,
								   const char	*key	  )
{

	sapdbwa_TraceDefFunction ("wd20_DeleteRegistryKey")

	return Reg_DeleteRegistryKey ( wd20WAControl.iniFile, section, key );

}

/****************************************************************************
 ***
 *** wd20_ShowCOMService
 ***
 ****************************************************************************/

void wd20_ShowCOMService( twd20WebAgentControlP	wa,
						  sapdbwa_HttpRequestP		req,
					      sapdbwa_HttpReplyP			rep  )
{

	Reg_RegistryP	registry;
	sapdbwa_Bool		eok;
	sapdbwa_Bool		userParameterExists = sapdbwa_False;

	const char *name = NULL;

	char	currentSection	[REG_MAX_SECTION_LEN+1];
	char	classId			[REG_MAX_VALUE_LEN+1];
	char	logFile			[MAX_FILENAME_LEN_WD00];
	char	serviceName		[REG_MAX_VALUE_LEN+1];
	char	keyName			[REG_MAX_KEY_LEN+1];
	char	value			[REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_ShowCOMService")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        wd26SetErr( wa->waErr,
                    ERR_OPEN_REGISTRY_WD26, wd20WAControl.iniFile, NULL );

		wd20_SendServerError( rep );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return;
	}

	/* Get current section */
	name = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wd20WAControl.regSectionCOMServices, name);

	/* Get class id */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_CLASS_ID,
								 classId, sizeof( classId ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get log file */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_LOGFILE,
								 logFile, sizeof( logFile ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get service name */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SERVICE_NAME,
								 serviceName, sizeof( serviceName ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Build and send page */
	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_BEGIN );
    sapdbwa_SendBody( rep, name, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_CONTINUE_1 );

	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_FORM_BEGIN );
    sapdbwa_SendBody( rep, name, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_FORM_CONTINUE_1 );

	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_CLASS_ID_BEGIN );
    sapdbwa_SendBody( rep, classId, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_CLASS_ID_END );
	
	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_LOGFILE_BEGIN );
    sapdbwa_SendBody( rep, logFile, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_LOGFILE_END );
	
	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_SERVICE_NAME_BEGIN );
    sapdbwa_SendBody( rep, serviceName, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_SERVICE_NAME_END );
	

	/* Open registry key */
	if ( !Reg_EnumRegistryKeys( registry, currentSection, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr, ERR_READ_REGISTRY_WD26,
                    currentSection, NULL );
        wd26LogErr( wd20WAControl.waErr, wd20WAControl.waLog );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
		Reg_CloseRegistry( registry );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return;
	}

	/* Iterate registry keys */
	keyName[0]='\0';
	value[0]='\0';
	do {
		if ( !Reg_GetNextKey( registry, keyName, sizeof( keyName ), value, sizeof( value ), &eok ))
			break;

		if ( !wd20_IsCOMServiceStandardParameter( keyName ) && !wd20_IsCOMServiceDependentParameter( keyName )) {
			if ( !userParameterExists ) {
				wd20_SendHorizontalLineRow( rep, 2);
				wd20_SendLabelRow( rep, ADMIN_LABEL_SERVICE_USER_PARAMETER_TITLE, "", sapdbwa_False );
				userParameterExists = sapdbwa_True;
			}

			/* Send user parameter */
			wd20_SendInputRow( rep, keyName, keyName, value );
		}

		keyName[0]='\0';
		value[0]='\0';
	} while ( !eok );

	/* Send horizontal line */
	wd20_SendHorizontalLineRow( rep, 2);

	/* Close form */
	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_FORM_END );

	/* Close registry */
	Reg_CloseRegistry( registry );

}

/****************************************************************************
 ***
 *** wd20_ShowSessionPool
 ***
 ****************************************************************************/

void wd20_ShowSessionPool( twd20WebAgentControlP wa,
						   sapdbwa_HttpRequestP	 req,
						   sapdbwa_HttpReplyP	 rep,
						   const char			*name,
						   const char			*message )
{

	Reg_RegistryP	registry;

	char	currentSection		[REG_MAX_SECTION_LEN+1];
	char	serverDB			[REG_MAX_VALUE_LEN+1];
	char	serverNode			[REG_MAX_VALUE_LEN+1];
	char	userId				[REG_MAX_VALUE_LEN+1];
	char	password			[REG_MAX_VALUE_LEN+1];
	char	datasource			[REG_MAX_VALUE_LEN+1];
	char	driver				[REG_MAX_VALUE_LEN+1];
	char	autocommit			[REG_MAX_VALUE_LEN+1];
	char	poolType			[REG_MAX_VALUE_LEN+1];
	char	poolSize			[REG_MAX_VALUE_LEN+1];
	char	sqlTraceFilename	[MAX_FILENAME_LEN_WD00+1];
	char	templateFilename	[MAX_FILENAME_LEN_WD00+1];

	sapdbwa_TemplateValueList	*templateValueList = NULL;

	sapdbwa_TraceDefFunction ("wd20_ShowSessionPool")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        wd26SetErr( wa->waErr,
                    ERR_OPEN_REGISTRY_WD26, wd20WAControl.iniFile, NULL );

		wd20_SendServerError( rep );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return;
	}

	/* Get current section */
	if ( !name ) 
		name = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionSessionPool, name);

	/* Get server db */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SERVER_DB,
								 serverDB, sizeof( serverDB ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get server node */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SERVER_NODE,
								 serverNode, sizeof( serverNode ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get user id */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_USER_ID,
								 userId, sizeof( userId ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get password */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_PASSWORD,
								 password, sizeof( password ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get datasource */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_DATASOURCE,
								 datasource, sizeof( datasource ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get driver */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_DRIVER,
								 driver, sizeof( driver ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get autocommit */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_AUTOCOMMIT,
								 autocommit, sizeof( autocommit ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

#ifdef USE_SQLCLASSES
	/* Get pool type */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_POOL_TYPE,
								 poolType, sizeof( poolType ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}
#endif

	/* Get pool size */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_POOL_SIZE,
								 poolSize, sizeof( poolSize ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Get sql trace filename */
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SQL_TRACE_FILENAME,
								 sqlTraceFilename, sizeof( sqlTraceFilename ), ""  )) {

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		return;
	}

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Build and send page */
	CreateTemplateValueList( &templateValueList );

	if ( message ) {
		AddValueToTemplateValueList( templateValueList, "Status", message );
	} else {
		AddValueToTemplateValueList( templateValueList, "Status", "" );
	}

	AddValueToTemplateValueList( templateValueList, "SessionPoolName", name );
	AddValueToTemplateValueList( templateValueList, "ServerDB", serverDB );
	AddValueToTemplateValueList( templateValueList, "ServerNode", serverNode );
	AddValueToTemplateValueList( templateValueList, "UserId", userId );
	AddValueToTemplateValueList( templateValueList, "UserPassword", password );
	AddValueToTemplateValueList( templateValueList, "ODBCDatasource", datasource );
	AddValueToTemplateValueList( templateValueList, "ODBCDriver", driver );

	if ( stricmp( autocommit, ADMIN_AUTOCOMMIT_ON_VALUE ) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "AutocommitSelectedOn", "SELECTED" );
		AddValueToTemplateValueList( templateValueList, "AutocommitSelectedOff", "" );
	} else {
		AddValueToTemplateValueList( templateValueList, "AutocommitSelectedOn", "" );
		AddValueToTemplateValueList( templateValueList, "AutocommitSelectedOff", "SELECTED" );
	}

#ifdef USE_SQLCLASSES
	if ( stricmp( poolType, ADMIN_POOL_TYPE_ODBC_VALUE ) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "PoolTypeODBC", "SELECTED" );
		AddValueToTemplateValueList( templateValueList, "PoolTypeSQLClass", "" );
	} else {
		AddValueToTemplateValueList( templateValueList, "PoolTypeODBC", "" );
		AddValueToTemplateValueList( templateValueList, "PoolTypeSQLClass", "SELECTED" );
	}
#endif

	AddValueToTemplateValueList( templateValueList, "PoolSize", poolSize );
	AddValueToTemplateValueList( templateValueList, "SQLTraceFilename", sqlTraceFilename );

	/* Build template name	*/
	sp77sprintf(templateFilename, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_SHOW_SESSION_POOL );

	/* Write template	*/
	WriteTemplate( rep, templateFilename, templateValueList, wa->waErr );

	/* Free value list	*/
	DropTemplateValueList( templateValueList );

	/* Close registry */
	Reg_CloseRegistry( registry );

}

/****************************************************************************
 ***
 *** wd20_NewSessionPool
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_NewSessionPool( sapdbwa_HttpReplyP	rep,
								  twd20WebAgentControlP wa	)
{

	char	templateFilename	[MAX_FILENAME_LEN_WD00+1];

	sapdbwa_TemplateValueList	*templateValueList = NULL;

	sapdbwa_TraceDefFunction ("wd20_NewSessionPool")

	/* Build and send page */
	CreateTemplateValueList( &templateValueList );

	AddValueToTemplateValueList( templateValueList, "AutocommitSelectedOn", "SELECTED" );

#ifdef USE_SQLCLASSES
	AddValueToTemplateValueList( templateValueList, "PoolTypeODBC", "SELECTED" );
#endif

	/* Build template name	*/
	sp77sprintf(templateFilename, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_DEFINE_NEW_SESSION_POOL );

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Write template	*/
	WriteTemplate( rep, templateFilename, templateValueList, wa->waErr );

	/* Free value list	*/
	DropTemplateValueList( templateValueList );

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_ShowNewSessionPool
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowNewSessionPool( sapdbwa_HttpReplyP	 rep,
									  twd20WebAgentControlP  wa,
									  twd20_SessionPool		*sessionPool,
									  const char			*message	 )
{

	char	templateFilename	[MAX_FILENAME_LEN_WD00+1];

	sapdbwa_TemplateValueList	*templateValueList = NULL;

	sapdbwa_TraceDefFunction ("wd20_ShowNewSessionPool")

	/* Build and send page */
	CreateTemplateValueList( &templateValueList );

	if ( message ) {
		AddValueToTemplateValueList( templateValueList, "Status", message );
	} else {
		AddValueToTemplateValueList( templateValueList, "Status", "" );
	}

	AddValueToTemplateValueList( templateValueList, "SessionPoolName", sessionPool->name );
	AddValueToTemplateValueList( templateValueList, "ServerDB", sessionPool->serverDB );
	AddValueToTemplateValueList( templateValueList, "ServerNode", sessionPool->serverNode );
	AddValueToTemplateValueList( templateValueList, "UserId", sessionPool->userId );
	AddValueToTemplateValueList( templateValueList, "UserPassword", sessionPool->password );
	AddValueToTemplateValueList( templateValueList, "ODBCDatasource", sessionPool->datasource );
	AddValueToTemplateValueList( templateValueList, "ODBCDriver", sessionPool->driver );

	if ( stricmp( sessionPool->autocommit, ADMIN_AUTOCOMMIT_ON_VALUE ) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "AutocommitSelectedOn", "SELECTED" );
		AddValueToTemplateValueList( templateValueList, "AutocommitSelectedOff", "" );
	} else {
		AddValueToTemplateValueList( templateValueList, "AutocommitSelectedOn", "" );
		AddValueToTemplateValueList( templateValueList, "AutocommitSelectedOff", "SELECTED" );
	}

#ifdef USE_SQLCLASSES
	if ( stricmp( sessionPool->type, ADMIN_POOL_TYPE_ODBC_VALUE ) == 0 ) {
		AddValueToTemplateValueList( templateValueList, "PoolTypeODBC", "SELECTED" );
		AddValueToTemplateValueList( templateValueList, "PoolTypeSQLClass", "" );
	} else {
		AddValueToTemplateValueList( templateValueList, "PoolTypeODBC", "" );
		AddValueToTemplateValueList( templateValueList, "PoolTypeSQLClass", "SELECTED" );
	}
#endif

	AddValueToTemplateValueList( templateValueList, "PoolSize", sessionPool->size );
	AddValueToTemplateValueList( templateValueList, "SQLTraceFilename", sessionPool->sqlTraceFilename );

	/* Build template name	*/
	sp77sprintf(templateFilename, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_DEFINE_NEW_SESSION_POOL );

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Write template	*/
	WriteTemplate( rep, templateFilename, templateValueList, wa->waErr );

	/* Free value list	*/
	DropTemplateValueList( templateValueList );


	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_CreateNewSessionPool
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_CreateNewSessionPool( twd20WebAgentControlP	wa,
									    sapdbwa_HttpRequestP	req,
									    sapdbwa_HttpReplyP		rep  )
{

	const char			*sessionPoolName = NULL;
	sapdbwa_Bool			 found = sapdbwa_False;
	char				 currentSection   [REG_MAX_SECTION_LEN+1];
	char				 sessionPoolNameU [REG_MAX_VALUE_LEN+1];
	const char			*text = NULL;
	char				 status	[MAX_STATUS_LEN];
	twd20_SessionPool	 sessionPool;
	char				 section[REG_MAX_SECTION_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_CreateNewSessionPool")

	/* Inits */
	status[0] = '\0';

	/* Get session pool */
	wd20_GetSessionPoolDescription( req, &sessionPool );

	/* Get new service name */
	sessionPoolName = wd20_GetHTMLParameter( req, ADMIN_TEXT_NEW_SESSION_POOL_NAME );

	/* Session pool already exists? */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionSessionPool, sessionPoolName);
	if ( !wd20_RegistrySectionExists( NULL, currentSection, &found )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, currentSection, NULL );
		wd20_SendServerError( rep );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* If session pool exists */
	if ( found ) {
		/* Send exists message */
		strncpy( sessionPoolNameU, sessionPoolName, sizeof( sessionPoolNameU ));
		wd09UpperCase( sessionPoolNameU );

		if ( wd15GetString( 0, ADMIN_HTML_SESSION_POOL_ALREADY_EXISTS_BEGIN, &text ))
			strcat( status, text );

		strcat( status, sessionPoolNameU );

		if ( wd15GetString( 0, ADMIN_HTML_SESSION_POOL_ALREADY_EXISTS_END, &text ))
			strcat( status, text );

		wd20_ShowNewSessionPool( rep, wa, &sessionPool, status );

		return sapdbwa_True;;
	}

	/* Get new section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionSessionPool, sessionPoolName );

	/* Update registry with new session pool */
	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_SERVER_DB,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVER_DB))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_SERVER_NODE,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVER_NODE ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_USER_ID,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_USER_ID ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_PASSWORD,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_PASSWORD ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_DATASOURCE,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_DATASOURCE ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_DRIVER,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_DRIVER ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	if ( strcmp( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_AUTOCOMMIT ), ADMIN_AUTOCOMMIT_ON_VALUE ) == 0 ) {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_AUTOCOMMIT, ADMIN_AUTOCOMMIT_ON )) {
			wd20_SendServerError( rep );
			return sapdbwa_False;
		}
	} else {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_AUTOCOMMIT, ADMIN_AUTOCOMMIT_OFF )) {
			wd20_SendServerError( rep );
			return sapdbwa_False;
		}
	}

/*	PTS1121181 http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121181 Start Deletion
	if ( strcmp( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_POOL_TYPE ), ADMIN_POOL_TYPE_ODBC_VALUE ) == 0 ) {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_POOL_TYPE, ADMIN_POOL_TYPE_ODBC )) {
			wd20_SendServerError( rep );
			return sapdbwa_False;
		}
	} else {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_POOL_TYPE, ADMIN_POOL_TYPE_SQL_CLASS )) {
			wd20_SendServerError( rep );
			return sapdbwa_False;
		}
	}
	PTS1121181 http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121181 End Deletion */
	/* PTS1121181 http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121181 Start Insertion */
	if ( strcmp( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_POOL_TYPE ), ADMIN_POOL_TYPE_SQL_CLASS_VALUE ) == 0 ) {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_POOL_TYPE, ADMIN_POOL_TYPE_SQL_CLASS )) {
			wd20_SendServerError( rep );
			return sapdbwa_False;
		}
	} else {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_POOL_TYPE, ADMIN_POOL_TYPE_ODBC )) {
			wd20_SendServerError( rep );
			return sapdbwa_False;
		}
	}
	/* PTS1121181 http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1121181 End Insertion */

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_POOL_SIZE,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_POOL_SIZE ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_SQL_TRACE_FILENAME,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SQL_TRACE_FILENAME ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Load new session pool */
	if ( wd20_LoadSessionPool( wa, sessionPool.name )) {
		/* Show new session pool */
		wd15GetString( 0, ADMIN_HTML_SESSION_POOL_CREATE_CONFIRMATION, &text );
		wd20_ShowSessionPool( wa, req, rep, sessionPool.name, text );
	} else {
        wd26LogErr( wa->waErr, wa->waLog );

		/* Delete registry section */
		/*
		sp77sprintf( section, REG_MAX_SECTION_LEN+1, "%s\\%s",
					 wa->regSectionSessionPool, sessionPool.name );
		if ( !wd20_DeleteRegistrySection( section )) {
			wd20_SendServerError( rep );
		}
		

		wd15GetString( 0, ADMIN_HTML_SESSION_POOL_COULD_NOT_LOAD, &text );
		wd20_ShowNewSessionPool( rep, wa, &sessionPool, text );
		*/
		/* Show new session pool */
		wd15GetString( 0, ADMIN_HTML_SESSION_POOL_COULD_NOT_LOAD, &text );
		wd20_ShowSessionPool( wa, req, rep, sessionPool.name, text );
	}


	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_UpdateSessionPool
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UpdateSessionPool( twd20WebAgentControlP	wa,
								   sapdbwa_HttpRequestP			req,
								   sapdbwa_HttpReplyP				rep  )
{

	sapdbwa_Bool		 ok = sapdbwa_True;
	sapdbwa_Int4		 i = 0;
    sapdbwa_StringSeqP	 keyList = sapdbwa_CreateStringSeq();
	const char		*key = NULL;
	const char		*message = NULL;
	const char		*name = NULL;
	char			 currentSection [REG_MAX_SECTION_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_UpdateSessionPool")

	/* Get current section */
	name = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionSessionPool, name );

	/* Get list of keys */
	if ( !sapdbwa_GetParameterNames( req, keyList )) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Update registry */
	for ( i = 0 ; i < sapdbwa_GetNumElem( keyList ) ; i++ ) {
		if (( key = sapdbwa_GetStringByIndex( keyList, i )) != NULL ) {
			if ( wd20_IsSessionPoolStandardParameter( key )) {
				if ( !wd20_SetRegistryKey ( currentSection,
											key,
											wd20_GetHTMLParameter( req, key ))) {
					wd20_SendServerError( rep );
					return sapdbwa_False;
				}
			}
		}
	}

	/* Update session pool list */
	ok = wd20_UnloadSessionPool( wa, name );
	if ( ok ) ok = wd20_LoadSessionPool( wa, name );
	if ( !ok ) {
		/* Show error message */
		wd15GetString( 0, ADMIN_HTML_RELOAD_SESSION_POOL_FAILED, &message );
	}

	/* Send confirmation */
	if ( ok ) {
		wd15GetString( 0, ADMIN_HTML_VALUE_UPDATE_CONFIRMATION, &message );
	}

	/* Show current session pool settings */
	wd20_ShowSessionPool( wa, req, rep, name, message );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_DeleteSessionPoolGetConfirmation 
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteSessionPoolGetConfirmation ( sapdbwa_HttpRequestP	req,
												     sapdbwa_HttpReplyP		rep,
													 twd20WebAgentControlP	wa	)
{

	const char	*sessionPoolName = NULL;
	char		 templateFilename	[MAX_FILENAME_LEN_WD00+1];

	sapdbwa_TemplateValueList	*templateValueList = NULL;

	sapdbwa_TraceDefFunction ("wd20_DeleteSessionPoolGetConfirmation")

	/* Get session pool name */
	sessionPoolName = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	/* Show confirmation page */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Create value list	*/
	CreateTemplateValueList( &templateValueList );
	if ( !templateValueList ) {
		/* Error handling	*/
	}

	/* Add values to value list	*/
	AddValueToTemplateValueList( templateValueList, "SessionPoolName", sessionPoolName );

	/* Build template name	*/
	sp77sprintf(templateFilename, MAX_FILENAME_LEN_WD00, "%s/%s/%s",
			  wa->documentRoot, SAPDBWA_DOCUMENT_ROOT_HTML,
			  ADMIN_TEMPLATE_SHOW_SESSION_POOL_TO_DELETE );

	/* Add values to HTML template and send template as response	*/
	WriteTemplate( rep, templateFilename, templateValueList, wa->waErr );

	DropTemplateValueList( templateValueList );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_DeleteSessionPool
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteSessionPool( sapdbwa_HttpRequestP	req,
								   sapdbwa_HttpReplyP		rep  )
{

	const char	*message = NULL;
	char		 currentSection [REG_MAX_SECTION_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_DeleteSessionPool")

	/* Get new section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wd20WAControl.regSectionSessionPool, wd20_GetHTMLParameter(req, ADMIN_SERVICE_PARAM_NAME));

	/* Delete registry section */
	if ( !wd20_DeleteRegistrySection( currentSection )) {
		wd20_SendServerError( rep );
	}

	/* Show confirmation page */
	wd15GetString( 0, ADMIN_HTML_DELETE_SESSION_POOL_CONFIRMATION, &message );
	wd20_ShowInitDetails( rep, message );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_UpdateCOMService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UpdateCOMService( sapdbwa_HttpRequestP	req,
								  sapdbwa_HttpReplyP		rep  )
{

	sapdbwa_Int4		 i = 0;
    sapdbwa_StringSeqP	 keyList = sapdbwa_CreateStringSeq();
	const char		*key = NULL;
	char			 currentSection [REG_MAX_SECTION_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_UpdateCOMService")

	/* Get current section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s",
			  wd20WAControl.regSectionCOMServices,
			  wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME ));

	/* Get list of keys */
	if ( !sapdbwa_GetParameterNames( req, keyList )) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Update registry */
	for ( i = 0 ; i < sapdbwa_GetNumElem( keyList ) ; i++ ) {
		if (( key = sapdbwa_GetStringByIndex( keyList, i )) != NULL ) {
			if ( !wd20_IsCOMServiceDependentParameter( key )) {
				if ( !wd20_SetRegistryKey ( currentSection,
											key,
											wd20_GetHTMLParameter( req, key ))) {
					wd20_SendServerError( rep );
					return sapdbwa_False;
				}
			}
		}
	}

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText( rep, 0, ADMIN_HTML_VALUE_UPDATE_CONFIRMATION );

	return sapdbwa_True;;

}	/* wd20_UpdateCOMService */

/****************************************************************************
 ***
 *** wd20_NewCOMParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_NewCOMParameter( sapdbwa_HttpRequestP		req,
								 sapdbwa_HttpReplyP		rep  )
{
	const char	*serviceName = NULL;

	sapdbwa_TraceDefFunction ("wd20_NewCOMParameter")

	/* Get service name */
	serviceName = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Send form */
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMPARAMETER_FORM_BEGIN);
	sapdbwa_SendBody( rep, serviceName, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMPARAMETER_TITLE_END);
	sapdbwa_SendBody( rep, serviceName, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMPARAMETER_SERVICE_END);
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMPARAMETER_NAME);
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMPARAMETER_DEFAULT_VALUE);
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMPARAMETER_FORM_END);

	return sapdbwa_True;;
}

/****************************************************************************
 ***
 *** wd20_ShowCOMParametersToDelete
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowCOMParametersToDelete( sapdbwa_HttpRequestP	req,
										   sapdbwa_HttpReplyP		rep  )
{

	Reg_RegistryP	 registry;
	sapdbwa_Bool     eok;
	const char		*serviceName = NULL;
	char			 currentSection	[REG_MAX_SECTION_LEN+1];
	char			 userParameter	[REG_MAX_VALUE_LEN+1];
	char			 userValue		[REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_ShowCOMParametersToDelete")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        /* memory allocation error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_OPEN_REGISTRY_WD26, wd20WAControl.iniFile, NULL );

		wd20_SendServerError( rep );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
    }

	/* Get current section */
	serviceName = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wd20WAControl.regSectionCOMServices, serviceName );

	/* Enumerate keys */
	if ( !Reg_EnumRegistryKeys( registry, currentSection, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_READ_REGISTRY_WD26, currentSection, NULL );

		wd20_SendServerError( rep );
		Reg_CloseRegistry( registry );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
	}

	/* Show confirmation page */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMPARAMETERS_FORM_BEGIN );
	sapdbwa_SendBody( rep, serviceName, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMPARAMETERS_FORM_CONTINUE_1 );

	/* Iterate registry keys */
	userParameter[0]='\0';
	userValue[0]='\0';
	do {
		if ( !Reg_GetNextKey( registry, userParameter, sizeof( userParameter ), userValue, sizeof( userValue ), &eok ))
			break;

		if ( !wd20_IsCOMServiceStandardParameter( userParameter ) && !wd20_IsCOMServiceDependentParameter( userParameter )) {
			/* Show user parameter */
			wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMPARAMETERS_PARAMETER_BEGIN );
			sapdbwa_SendBody( rep, userParameter, 0 );
			wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMPARAMETERS_PARAMETER_CONTINUE_1 );
			sapdbwa_SendBody( rep, userParameter, 0 );
			wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMPARAMETERS_PARAMETER_END );
		}

		userParameter[0]='\0';
		userValue[0]='\0';
	} while ( !eok );

	/* Close registry key */
	Reg_CloseRegistry( registry );

	/* Close form */
	wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMPARAMETERS_FORM_END );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_DeleteCOMServiceGetConfirmation
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteCOMServiceGetConfirmation( sapdbwa_HttpRequestP	req,
												 sapdbwa_HttpReplyP	rep  )
{

	const char	*serviceName = NULL;

	sapdbwa_TraceDefFunction ("wd20_DeleteCOMServiceGetConfirmation")

	/* Get service name */
	serviceName = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	/* Show confirmation page */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMSERVICE_GET_CONFIRMATION_BEGIN );
	sapdbwa_SendBody( rep, serviceName, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMSERVICE_GET_CONFIRMATION_CONTINUE_1 );
	sapdbwa_SendBody( rep, serviceName, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMSERVICE_GET_CONFIRMATION_END );

	return sapdbwa_True;;

}	/* wd20_DeleteCOMServiceGetConfirmation */

/****************************************************************************
 ***
 *** wd20_CreateNewCOMParameter
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_CreateNewCOMParameter( sapdbwa_HttpRequestP	req,
									   sapdbwa_HttpReplyP		rep  )
{
	char		 currentSection [REG_MAX_SECTION_LEN+1];
	const char	*parameterName = NULL;

	sapdbwa_TraceDefFunction ("wd20_CreateNewCOMParameter")

	/* Get current section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s",
			 wd20WAControl.regSectionCOMServices,
			 wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME ));

	/* Get parameter name */
	parameterName = wd20_GetHTMLParameter( req, ADMIN_VALUE_PARAMETER_NAME );
	if ( strlen( parameterName ) < 1 || wd20_IsCOMServiceStandardParameter( parameterName ) || wd20_IsCOMServiceDependentParameter( parameterName )) {
		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		/* Send invalid parameter name message */
		wd20_SendText( rep, 0, ADMIN_HTML_INVALID_PARAMETER_NAME);

		return sapdbwa_True;;
	}

	/* Create new  */
	if ( !wd20_SetRegistryKey ( currentSection,
								parameterName,
								wd20_GetHTMLParameter( req, ADMIN_VALUE_PARAMETER_DEFAULT_VALUE ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Send confirmation */
	wd20_SendText( rep, 0, ADMIN_HTML_VALUE_UPDATE_CONFIRMATION);

	return sapdbwa_True;;
}	/* wd20_CreateNewCOMParameter */

/****************************************************************************
 ***
 *** wd20_DeleteCOMService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteCOMService( sapdbwa_HttpRequestP	req,
								  sapdbwa_HttpReplyP		rep  )
{

	char	currentSection [REG_MAX_SECTION_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_DeleteCOMService")

	/* Get new section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wd20WAControl.regSectionCOMServices, wd20_GetHTMLParameter(req, ADMIN_SERVICE_PARAM_NAME));

	/* Delete registry section */
	if ( !wd20_DeleteRegistrySection( currentSection )) {
		wd20_SendServerError( rep );
	}

	/* Show confirmation page */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMSERVICE_CONFIRMATION );

	return sapdbwa_True;;

}	/* wd20_DeleteCOMService */

/****************************************************************************
 ***
 *** wd20_DeleteCOMParameters
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteCOMParameters( sapdbwa_HttpRequestP	req,
									 sapdbwa_HttpReplyP	rep  )
{

	sapdbwa_Int4		 i = 0;
	sapdbwa_Int4		 count = 0;
    sapdbwa_StringSeqP	 valueList = sapdbwa_CreateStringSeq();
	const char		*value = NULL;
	char			 currentSection [REG_MAX_SECTION_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_DeleteCOMParameters")

	/* Get current section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s",
			  wd20WAControl.regSectionCOMServices,
			  wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME ));

	/* Get list of values */
    if ( !sapdbwa_GetParameterValues( req, ADMIN_CHECKBOX_PARAMETER, valueList )) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Delete registry section */
	for ( i = 0 ; i < sapdbwa_GetNumElem( valueList ) ; i++ ) {
		if (( value = sapdbwa_GetStringByIndex( valueList, i )) != NULL ) {
			if ( !wd20_DeleteRegistryKey ( currentSection, value )) {
				wd20_SendServerError( rep );
				return sapdbwa_False;
			}

			count++;
		}
	}

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	if ( count ) {
		wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMPARAMETERS_CONFIRMATION );
	} else {
		wd20_SendText( rep, 0, ADMIN_HTML_DELETE_COMPARAMETERS_NOTHING );
	}

	return sapdbwa_True;;

}	/* wd20_DeleteCOMParameters */

/****************************************************************************
 ***
 *** wd20_NewCOMService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_NewCOMService( sapdbwa_HttpReplyP	rep  )
{

	sapdbwa_TraceDefFunction ("wd20_NewCOMService")

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Send form */
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMSERVICE_FORM_BEGIN );
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMSERVICE_NAME );
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMSERVICE_CLASS_ID );
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMSERVICE_LOGFILE );
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMSERVICE_SERVICE_NAME );
	wd20_SendText( rep, 0, ADMIN_HTML_NEW_COMSERVICE_FORM_END);

	return sapdbwa_True;;
}	/* wd20_NewCOMService */

/****************************************************************************
 ***
 *** wd20_CreateNewCOMService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_CreateNewCOMService( twd20WebAgentControlP	wa,
									 sapdbwa_HttpRequestP			req,
									 sapdbwa_HttpReplyP			rep  )
{

	const char	*serviceName = NULL;
	sapdbwa_Bool	 found = sapdbwa_False;
	char		 currentSection [REG_MAX_SECTION_LEN+1];
	char		 serviceNameU   [REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_CreateNewCOMService")

	/* Get new service name */
	serviceName = wd20_GetHTMLParameter( req, ADMIN_TEXT_NEW_COMSERVICE_NAME );

	/* COM service already exists? */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionCOMServices, serviceName);
	if ( !wd20_RegistrySectionExists( NULL, currentSection, &found )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, currentSection, NULL );
		wd20_SendServerError( rep );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Get upper case service name */
	strncpy( serviceNameU, serviceName, sizeof( serviceNameU ));
	wd09UpperCase( serviceNameU );

	/* If service exists */
	if ( found ) {
		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		/* Send exists message */
		wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_ALREADY_EXISTS_BEGIN );
		sapdbwa_SendBody( rep, serviceNameU, 0 );
		wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_ALREADY_EXISTS_END );

		return sapdbwa_True;;
	}

	/* If service not exists but service name is the WebAgent Admin service name */
	if ( !found && (strncmp( serviceNameU, ADMIN_WAADMIN, sizeof( ADMIN_WAADMIN )) == 0 )) {
		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );

		/* Send exists message */
		wd20_SendText( rep, 0, ADMIN_HTML_RESERVED_COMSERVICE_NAME_BEGIN );
		sapdbwa_SendBody( rep, serviceNameU, 0 );
		wd20_SendText( rep, 0, ADMIN_HTML_RESERVED_COMSERVICE_NAME_END );

		return sapdbwa_True;;
	}

	/* Get new section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionCOMServices, serviceName );

	/* Update registry with new COM service */
	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_CLASS_ID,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_CLASS_ID ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_LOGFILE,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_LOGFILE ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	if ( !wd20_SetRegistryKey ( currentSection,
								ADMIN_REG_KEY_SERVICE_NAME,
								wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_NAME ))) {
		wd20_SendServerError( rep );
		return sapdbwa_False;
	}

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText( rep, 0, ADMIN_HTML_COMSERVICE_CREATE_CONFIRMATION );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_ClearLogFile
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ClearLogFile( sapdbwa_HttpReplyP			rep,
							  twd20WebAgentControlP wa   )
{

	const char	*message = NULL;

	sapdbwa_TraceDefFunction ("wd20_ClearLogFile")

	/* Clear log file */
	wd25EmptyLog(wa->waLog, sapdbwa_True);

	/* Send confirmation */
	wd15GetString( 0, ADMIN_HTML_LOG_FILE_CLEAR_CONFIRMATION, &message );
	wd20_ShowInitDetails( rep, message );

	return sapdbwa_True;;

}	/* wd20_ClearLogFile */

/****************************************************************************
 ***
 *** wd20_ClearLogFileHTTPRequests
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ClearLogFileHTTPRequests( sapdbwa_HttpRequestP  req,
                                            sapdbwa_HttpReplyP	  rep,
							                twd20WebAgentControlP wa   )
{

	char	     logDirectory[MAX_FILENAME_LEN_WD00+1];
	char	     filename [MAX_FILENAME_LEN_WD00+1];
	const char	*message = NULL;

	sapdbwa_TraceDefFunction ("wd20_ClearLogFileHTTPRequests")

    /* Get filename */
	if ( !wd20_GetRegistryValue( NULL, wa->regSectionHTTP,	ADMIN_REG_KEY_LOGFILE_HTTP_REQUESTS,
								 logDirectory, sizeof( logDirectory ), "" ) || logDirectory[0] == '\0' ) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_NotFound, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );
		sapdbwa_SendBody( rep, "File not found!", 0 );

		return sapdbwa_False;
	}

    sp77sprintf(filename, MAX_FILENAME_LEN_WD00, "%s/httpreq.log", logDirectory);

    /* Clear log file */
	wd20_EmptyLog(filename, &req->logDesc->logFile, req->logDesc->logExcl, sapdbwa_True);

	/* Send confirmation */
	wd15GetString( 0, ADMIN_HTML_LOG_FILE_CLEAR_CONFIRMATION, &message );
	wd20_ShowInitDetails( rep, message );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_ClearLogFileHTTPErrors
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ClearLogFileHTTPErrors( sapdbwa_HttpRequestP  req,
                                          sapdbwa_HttpReplyP	rep,
							              twd20WebAgentControlP wa   )
{

	char	     logDirectory[MAX_FILENAME_LEN_WD00+1];
	char	     filename [MAX_FILENAME_LEN_WD00+1];
	const char	*message = NULL;

	sapdbwa_TraceDefFunction ("wd20_ClearLogFileHTTPErrors")

    /* Get filename */
	if ( !wd20_GetRegistryValue( NULL, wa->regSectionHTTP,	ADMIN_REG_KEY_LOGFILE_HTTP_REQUESTS,
								 logDirectory, sizeof( logDirectory ), "" ) || logDirectory[0] == '\0' ) {

		/* Initialize reply header */
		sapdbwa_InitHeader( rep, sapdbwa_Status_NotFound, "text/html", NULL, NULL, NULL, NULL );
		sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
		sapdbwa_SendHeader( rep );
		sapdbwa_SendBody( rep, "File not found!", 0 );

		return sapdbwa_False;
	}

    sp77sprintf(filename, MAX_FILENAME_LEN_WD00, "%s/httperr.log", logDirectory);

	/* Clear log file */
	wd20_EmptyLog(filename, &req->logDesc->errFile, req->logDesc->errExcl, sapdbwa_True);

	/* Send confirmation */
	wd15GetString( 0, ADMIN_HTML_LOG_FILE_CLEAR_CONFIRMATION, &message );
	wd20_ShowInitDetails( rep, message );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20EnableService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20ServiceStart( const char	*serviceName,
							 sapdbwa_Bool	 start	      )
{

	char	currentSection [REG_MAX_SECTION_LEN+1];

	sapdbwa_TraceDefFunction ("wd20ServiceStart")

	/* Get current section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wd20WAControl.regSectionServices, serviceName );

	/* Update registry */
	if ( start ) {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_START, ADMIN_SERVICE_START_ON_VALUE )) {
			return sapdbwa_False;
		}
	}
	else {
		if ( !wd20_SetRegistryKey ( currentSection, ADMIN_REG_KEY_SERVICE_START, ADMIN_SERVICE_START_OFF_VALUE )) {
			return sapdbwa_False;
		}
	}

	return sapdbwa_True;;

}	/* wd20EnableService */

/****************************************************************************
 ***
 *** wd20_LoadService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_LoadService( twd20WebAgentControlP	 wa,
							 const char				*service  )
{

    char				iniValue			[MAX_INI_PAR_LEN_WD00+1];
    char				serviceName			[MAX_INI_PAR_LEN_WD00+1];
    char				sessionPool			[MAX_INI_PAR_LEN_WD00+1];
    char				library				[MAX_DLL_NAME_LEN_WD00+1];
    char				libraryType			[MAX_INI_PAR_LEN_WD00+1];
    char				initFunction		[MAX_FUNC_NAME_LEN_WD00+1];
    char				exitFunction		[MAX_FUNC_NAME_LEN_WD00+1];
    char				serviceFunction		[MAX_FUNC_NAME_LEN_WD00+1];
    char				logFile				[MAX_INI_PAR_LEN_WD00+1];
    char				withSSL				[MAX_INI_PAR_LEN_WD00+1];
    char				sslURL				[MAX_INI_PAR_LEN_WD00+1];
    char				webSessionTimeout	[MAX_INI_PAR_LEN_WD00+1];
    
	char				currentSection		[REG_MAX_SECTION_LEN+1];

	twd20SessionPoolListItemP	sessionPoolItem = NULL;
    twd30SessionPoolP			thePool = NULL;

	twd40UserDllP		newUserDll = NULL;
    twd40DllType		userDllType;
    sapdbwa_Handle		waHandle;
    sapdbwa_Int2		initFuncRetcode;
	sapdbwa_Bool		secureType = sapdbwa_False;
	sapdbwa_UInt4		webSessionTimeoutVal = 0;
	sapdbwa_Bool		useFastCGIForCookiePath = sapdbwa_True;
	Reg_RegistryP		registry;
    
	sapdbwa_TraceDefFunction ("wd20_LoadService")

	/* Get current section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionServices, service );

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        wd26SetErr( wa->waErr,
                    ERR_OPEN_REGISTRY_WD26, currentSection, NULL );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
	}

	/* Get service name */
	serviceName[0] = '\0';
	if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SERVICE_NAME, serviceName, sizeof( serviceName ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SERVICE_NAME );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( serviceName )
	}

	if ( strlen( serviceName ) == 0) {
        /* missing service name */
        wd26SetErr( wa->waErr, ERR_NO_SERVICE_NAME_WD26, currentSection, NULL );
		Reg_CloseRegistry( registry );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
    };

	/* Get session pool */
    sessionPool[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SESSION_POOL, sessionPool, sizeof( sessionPool ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SESSION_POOL );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}

    if ( strlen( sessionPool ) > 0) {
		sessionPoolItem = wd20_FindSessionPoolInList( wa->sessionPoolList, sessionPool );
		if ( sessionPoolItem && sessionPoolItem->sessionPool )
			thePool = sessionPoolItem->sessionPool;

        if ( !thePool) {
            /* no such pool */
            wd26SetErr( wa->waErr, ERR_NO_SUCH_POOL_WD26, sessionPool, NULL );
			Reg_CloseRegistry( registry );

			sapdbwa_TraceAdd( "Exiting unexpected!" )
            return sapdbwa_False;
        };
    };

	/* Get library */
    library[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_LIBRARY, library, sizeof( library ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_LIBRARY );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}

    if (strlen(library) == 0) {
        /* missing library */
        wd26SetErr( wa->waErr, ERR_NO_LIBRARY_WD26, service, NULL );
		Reg_CloseRegistry( registry );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
    };

	/* Get service function */
    serviceFunction[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SERVICE_FUNCTION, serviceFunction, sizeof( serviceFunction ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SERVICE_FUNCTION );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}

	/* Get init function */
    initFunction[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_INIT_FUNCTION, initFunction, sizeof( initFunction ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_INIT_FUNCTION );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}

	/* Get exit function */
    exitFunction[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_EXIT_FUNCTION, exitFunction, sizeof( exitFunction ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_EXIT_FUNCTION );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}

	/* Get library type */
    libraryType[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_LIBRARY_TYPE, libraryType, sizeof( libraryType ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_LIBRARY_TYPE );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}

    if ( stricmp( libraryType, ADMIN_LIBRARY_TYPE_CPP_VALUE ) == 0) {
        userDllType = DllTypeCPP;
    } else {
        userDllType = DllTypeC; /* C is default */
    };

	/* Get log file */
    logFile[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_LOGFILE, logFile, sizeof( logFile ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_LOGFILE );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}

	/* Get cookie path prefix setting */
    iniValue[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SERVICE_COOKIE_PATH_PREFIX,
								 iniValue, sizeof( iniValue ),
								 ADMIN_SERVICE_COOKIE_PATH_PREFIX_ON_VALUE )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SERVICE_COOKIE_PATH_PREFIX );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}
	if ( strcmp( iniValue, ADMIN_SERVICE_COOKIE_PATH_PREFIX_ON_VALUE ) == 0 ) {
		useFastCGIForCookiePath = sapdbwa_True;
	} else {
		useFastCGIForCookiePath = sapdbwa_False;
	}

	/* Get secure type */
    withSSL[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SERVICE_WITH_SSL, withSSL, sizeof( withSSL ), "0" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SERVICE_WITH_SSL );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}
	if ( strcmp( withSSL, ADMIN_SERVICE_WITH_SSL_ON_VALUE ) == 0 ) {
		secureType = sapdbwa_True;
	} else {
		secureType = sapdbwa_False;
	}

	/* SSL URL */
    sslURL[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_SERVICE_SSL_URL, sslURL, sizeof( sslURL ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SERVICE_SSL_URL );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}

	/* Web session timeout */
    webSessionTimeout[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection,
								 ADMIN_REG_KEY_WEB_SESSION_TIMEOUT, webSessionTimeout, sizeof( webSessionTimeout ), "0" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_WEB_SESSION_TIMEOUT );
		sapdbwa_TraceAdd( "Error: Read Registry!" )
		sapdbwa_TraceAdd( currentSection )
	}
	webSessionTimeoutVal = atoi( webSessionTimeout );

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Create user dll instance */
    if (( newUserDll = wd40CreateUserDll( service, wa->serviceId++, wa->waErr )) == NULL ) return sapdbwa_False;

	/* Init service */
    if (!wd40InitUserDll( newUserDll,
                          serviceName,
                          library,
                          thePool,
                          initFunction,
                          exitFunction,
                          serviceFunction,
                          logFile,
                          userDllType,
						  secureType,
						  sslURL,
						  webSessionTimeoutVal,
						  useFastCGIForCookiePath,
                          wa->waErr )) {

        /* init failed */
		wd40DestroyUserDll( newUserDll );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
    };

	/* Create WA handle */
	waHandle = wd23CreateHandle( newUserDll, NULL, NULL, NULL, sapdbwa_False );

    /* Load library */
    if ( !wd40LoadUserDll( newUserDll, wa->waErr )) {

        wd23DestroyHandle( waHandle );
		wd40DestroyUserDll( newUserDll );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
    };

    /* Call init function in library */
    TRY {
    if ( !wd40CallInitFunc( newUserDll, waHandle, &initFuncRetcode, wd20WAControl.waLog, wa->waErr )) {
        wd23DestroyHandle( waHandle );
		wd40DestroyUserDll( newUserDll );
        
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
    };
    } CATCH;

	/* Destroy WA handle */
    wd23DestroyHandle( waHandle );

	/* Add new service to service list */
	if ( !wd20_AddServiceToList ( &(wa->serviceList), newUserDll )) {
		wd40DestroyUserDll( newUserDll );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

    return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_UnloadService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UnloadService( twd20WebAgentControlP	 wa,
							   const char				*service )
{

    sapdbwa_Bool  retcode = sapdbwa_True;
    sapdbwa_Handle   waHandle;
    sapdbwa_Int2  exitFuncRetcode;

	twd20ServiceListItemP	 foundService = NULL;
	twd40UserDllP			 serviceToDelete = NULL;

	sapdbwa_TraceDefFunction ("wd20_UnloadService")

	/* Get service to delete */
	if (( foundService = wd20_FindServiceInList( wa->serviceList, service )) == NULL ) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_USERDLL_UNLOAD_FAILED_WD26, service, NULL );

		return sapdbwa_False;
	}

	serviceToDelete = foundService->service;
	if ( !serviceToDelete ) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_USERDLL_UNLOAD_FAILED_WD26, service, NULL );

		return sapdbwa_False;
	}

	/* Stop web session, if exists */
	if ( wa->sessionCont ) {
		wd37CloseServiceWebSession( wa->sessionCont, wd40GetUserDllId( serviceToDelete ));
	}

	/* Create wa handle */
    waHandle = wd23CreateHandle( serviceToDelete, NULL, NULL, NULL, sapdbwa_False );

    /* Call the exit function before unloading */
    TRY {
    if (!wd40CallExitFunc( serviceToDelete,
                           waHandle,
                           &exitFuncRetcode,
                           wa->waLog,
                           wa->waErr )) {
        if (wd26GetId( wa->waErr ) != ERR_USERDLL_NOT_INIT_WD26) {
            wd26LogErr( wa->waErr, wa->waLog );
            retcode = sapdbwa_False;
        };
    };
    } CATCH;

	/* Unload user dll */
    if (!wd40UnloadUserDll( serviceToDelete, wa->waErr )) {
        wd26LogErr( wa->waErr, wa->waLog );
        retcode = sapdbwa_False;
    };

	/* Destroy wa handle */
    wd23DestroyHandle( waHandle );

	/* Delete service from service list */
	wd20_DeleteServiceFromList ( &(wa->serviceList), service );

	/* Destroy user dll */
	wd40DestroyUserDll( serviceToDelete );

    return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_LoadAllServices
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_LoadAllServices( twd20WebAgentControlP	wa )
{
	const char FUNCTION_NAME  [] = "wd20_LoadAllServices";

	char			service			[REG_MAX_VALUE_LEN+1];
	char			serviceStart	[REG_MAX_VALUE_LEN+1];
	char			currentSection	[REG_MAX_SECTION_LEN+1];
	Reg_RegistryP	registry;
	sapdbwa_Bool		eos;

	sapdbwa_TraceDefFunction ("wd20_LoadAllServices")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        wd26SetErr( wa->waErr,
                    ERR_OPEN_REGISTRY_WD26, wa->regSectionServices, NULL );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
	}

	/* Open registry key */
	if ( !Reg_EnumRegistrySections( registry, wa->regSectionServices, NULL )) {
        wd26SetErr( wa->waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionServices, NULL );

		Reg_CloseRegistry( registry );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
	}

	/* Iterate services */
	service[0]='\0';
	do {
		if ( !Reg_GetNextSection( registry, NULL, 0, service, sizeof( service ), &eos ))
			break;

		if ( strlen( service ) > 0 ) {
			/* Get current section */
			sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionServices, service );

			serviceStart[0] = '\0';
			if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_SERVICE_START,
										 serviceStart, sizeof( serviceStart ), ADMIN_SERVICE_START_OFF_VALUE )) {
				/* Registry read error */
				wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SERVICE_START );
			}

			if ( strcmp( serviceStart, ADMIN_SERVICE_START_ON_VALUE ) == 0 ) {
				/* Load service */
				if ( !wd20_LoadService( wa, service )) {
					wd26LogErr( wa->waErr, wa->waLog);
				}
			}
		}

		service[0]='\0';
	} while ( !eos );

	/* Close registry */
	Reg_CloseRegistry( registry );

    return sapdbwa_True;

}

/****************************************************************************
 ***
 *** wd20_UnloadAllServices
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UnloadAllServices( twd20WebAgentControlP	wa )
{

	sapdbwa_Bool	rc = sapdbwa_True;

	sapdbwa_TraceDefFunction ("wd20_UnloadAllServices")

	while ( wa->serviceList ) {
		if ( !wd20_UnloadService( wa, wd40GetUserDllName( wa->serviceList->service )))
			rc = sapdbwa_False;
	}

    return rc;

}

/****************************************************************************
 ***
 *** wd20_AddServiceToList
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_AddServiceToList( twd20ServiceListItemP	*firstService,
								  twd40UserDllP			 userDll       )
{
	const char FUNCTION_NAME[] = "wd20_AddServiceToList";

	twd20ServiceListItemP	*currentItem;
	twd20ServiceListItemP	 currentService;
    sapdbwa_Bool				 allocatOk;

	sapdbwa_TraceDefFunction ("wd20_AddServiceToList")

	/* Find end of list */
	currentItem = firstService;
	currentService = *currentItem;
	while ( currentService != NULL ) {
		currentItem = &(currentService->nextService);
		currentService = *currentItem;
	}

	/* Allocate memory for new service item */
    sapdbwa_SQLALLOCAT( sizeof(twd20ServiceListItem), (sapdbwa_UInt1**) currentItem, &allocatOk );
	if ( !allocatOk ) {
        /* Memory alloc failure */
        wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );

		return sapdbwa_False;
	}

	/* Add new service to list */
	currentService = *currentItem;
	currentService->service = userDll;
	currentService->nextService = NULL;

    return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_DeleteServiceFromList
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteServiceFromList( twd20ServiceListItemP	*firstService,
									   const char				*service       )
{
	const char FUNCTION_NAME[] = "wd20_DeleteServiceFromList";

	twd20ServiceListItemP	*lastItem;
	twd20ServiceListItemP	*currentItem;
	twd20ServiceListItemP	 lastService;
	twd20ServiceListItemP	 currentService;
    sapdbwa_Bool				 found;

	sapdbwa_TraceDefFunction ("wd20_DeleteServiceFromList")

	/* Find service in list */
	found = sapdbwa_False;
	lastItem = firstService;
	currentItem = firstService;
	currentService = *firstService;
	while ( !found ) {
		if ( currentService && strcmp( wd40GetUserDllName( currentService->service ), service ) != 0 ) {
			lastItem = currentItem;
			currentItem = &(currentService->nextService);
			currentService = *currentItem;
		} else
			found = sapdbwa_True;
	}

	/* Service found? */
	if ( found ) {
		/* Set new list pointers */
		if ( currentItem == firstService ) {
			*firstService = currentService->nextService;
		} else {
			lastService = *lastItem;
			lastService->nextService = currentService->nextService;
		}

		/* Free memory for list item */
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) currentService );

	    return sapdbwa_True;;
	}

    return sapdbwa_False;

}

/****************************************************************************
 ***
 *** wd20_FindServiceInList
 ***
 ****************************************************************************/

twd20ServiceListItemP wd20_FindServiceInList( twd20ServiceListItemP	 firstService,
											  const char			*service       )
{
	const char FUNCTION_NAME[] = "wd20_FindServiceInList";

	twd20ServiceListItemP	currentService;
    sapdbwa_Bool				found;

	sapdbwa_TraceDefFunction ("wd20_FindServiceInList")

	/* Find service in list */
	found = sapdbwa_False;
	currentService = firstService;
	while ( !found ) {
		if ( currentService && service &&
			 strcmp( wd40GetUserDllName( currentService->service ), service ) != 0 ) {

			currentService = currentService->nextService;

		} else
			found = sapdbwa_True;
	}

	/* Service found? */
	if ( found ) {
		return currentService;
	} else {
		return NULL;
	}
}

/****************************************************************************
 ***
 *** wd20_GetServiceByLib
 ***
 ****************************************************************************/

twd20ServiceListItemP wd20_GetServiceByLib( twd20ServiceListItemP	firstService,
											HANDLE					libraryHandle )
{
	const char FUNCTION_NAME[] = "wd20_GetServiceByLib";

	twd20ServiceListItemP	currentService;
    sapdbwa_Bool				found;

	sapdbwa_TraceDefFunction ("wd20_GetServiceByLib")

	/* Find service in list */
	found = sapdbwa_False;
	currentService = firstService;
	while ( !found ) {
		if ( currentService && ( wd40GetUserDllLibHandle( currentService->service ) != libraryHandle )) {

			currentService = currentService->nextService;

		} else
			found = sapdbwa_True;
	}

	/* Service found? */
	if ( found ) {
		return currentService;
	} else {
		return NULL;
	}
}

/****************************************************************************
 ***
 *** wd20_GetServiceById
 ***
 ****************************************************************************/

twd20ServiceListItemP wd20_GetServiceById( twd20ServiceListItemP    firstService,
                                           sapdbwa_UInt4              serviceId )
{
	const char FUNCTION_NAME[] = "wd20_GetServiceById";

	twd20ServiceListItemP	currentService;
    sapdbwa_Bool				found;

	sapdbwa_TraceDefFunction ("wd20_GetServiceById")

	/* Find service in list */
	found = sapdbwa_False;
	currentService = firstService;
	while ( !found ) {
		if ( currentService
             && ( wd40GetUserDllId( currentService->service ) != serviceId )) {

			currentService = currentService->nextService;

		} else
			found = sapdbwa_True;
	}

	/* Service found? */
	if ( found ) {
		return currentService;
	} else {
		return NULL;
	}
}

/****************************************************************************
 ***
 *** wd20_StartService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_StartService( twd20WebAgentControlP	wa,
							  sapdbwa_HttpRequestP		req,
							  sapdbwa_HttpReplyP			rep  )
{

	const char	*service = NULL;
	const char	*text = NULL;
	char		 status	[MAX_STATUS_LEN];

	sapdbwa_TraceDefFunction ("wd20_StartService")

	/* Inits */
	status[0] = '\0';

	/* Get service */
	service = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );

	/* If service is already loaded */
	if ( wd20_FindServiceInList( wa->serviceList, service )) {
		if ( wd15GetString( 0, ADMIN_HTML_SERVICE_ALREADY_LOADED_BEGIN, &text ))
			strcat ( status, text );
		strcat( status, service );
		if ( wd15GetString( 0, ADMIN_HTML_SERVICE_ALREADY_LOADED_END, &text ))
			strcat ( status, text );
	} else {
		/* Start service */
		if ( wd20_LoadService( wa, service )) {
			if ( wd15GetString( 0, ADMIN_HTML_START_SERVICE_CONFIRMATION_BEGIN, &text ))
				strcat ( status, text );
			strcat ( status, service );
			if ( wd15GetString( 0, ADMIN_HTML_START_SERVICE_OK, &text ))
				strcat ( status, text );
		} else {
			if ( wd15GetString( 0, ADMIN_HTML_START_SERVICE_NOT_OK, &text ))
				strcpy ( status, text );
		}
	}

	return wd20_ShowService( wa, req, rep, service, status, NULL );

}

/****************************************************************************
 ***
 *** wd20_StopService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_StopService( twd20WebAgentControlP    wa,
                               sapdbwa_HttpRequestP     req,
                               sapdbwa_HttpReplyP       rep  )
{

	sapdbwa_Bool             rc = sapdbwa_False;
    sapdbwa_Bool             listBegin = sapdbwa_True;
	const char              *service = NULL;
	twd20ServiceListItemP    foundService = NULL;
	HANDLE                   libHandle;
	char                     serviceToStop[REG_MAX_VALUE_LEN+1];
	const char              *text = NULL;
	char                     status[MAX_STATUS_LEN];

	sapdbwa_TraceDefFunction ("wd20_StopService")

	/* Inits */
	status[0] = '\0';

	/* Get service */
	service = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
		
	/* Get service description */
	foundService = wd20_FindServiceInList( wa->serviceList, service );

	/* If service found */
	if ( foundService ) {
		libHandle = wd40GetUserDllLibHandle( foundService->service );
		strcpy( serviceToStop, service );

		/* Start confirmation */
		if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_BEGIN, &text ))
			strcat ( status, text );

		/* Stop service */
		rc = wd20_UnloadService( wa, serviceToStop);

		/* Confirmation */
		if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_NAME, &text ))
			strcat ( status, text );
		strcat( status, serviceToStop );

		if ( rc ) {
			if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_OK, &text ))
				strcat ( status, text );
		} else {
			if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_NOT_OK, &text ))
				strcat ( status, text );
		}

		/* If last service unload was ok, get next service which uses the same library */
		if ( rc ) {
		    foundService = wd20_GetServiceByLib( wa->serviceList, libHandle );
            listBegin = sapdbwa_True;

		    while ( foundService ) {
                if (listBegin) {
                    listBegin = sapdbwa_False;

		            if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_LIST_OF_SERVICES_TITLE, &text ))
                        wd20_StrCatMax(status, text, MAX_STATUS_LEN);
                }

                wd20_StrCatMax(status, wd40GetUserDllName(foundService->service), MAX_STATUS_LEN);
                wd20_StrCatMax(status, "<BR>", MAX_STATUS_LEN);

				foundService = wd20_GetServiceByLib( foundService->nextService, libHandle );
		    };

            if (listBegin) {
                wd20_StrCatMax(status, "<BR>", MAX_STATUS_LEN);
            }
        }

        if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_END, &text )) {
            wd20_StrCatMax(status, text, MAX_STATUS_LEN);
        }

		return wd20_ShowService( wa, req, rep, service, status, NULL );
	}

	/* Service not found */
    if ( wd15GetString( 0, ADMIN_HTML_SERVICE_NOT_FOUND, &text )) {
        wd20_StrCatMax(status, text, MAX_STATUS_LEN);
    }

	return wd20_ShowService( wa, req, rep, service, status, NULL );

}

/****************************************************************************
 ***
 ***	wd20GetDocumentRoot
 ***
 ****************************************************************************/

sapdbwa_Bool wd20GetDocumentRoot( char         *valueBuf,
							      sapdbwa_Int4  bufLen    )
{

	sapdbwa_TraceDefFunction ("wd20GetDocumentRoot")

	strncpy(valueBuf, wd20WAControl.documentRoot, bufLen );
	valueBuf[bufLen-1] = '\0';

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 ***	wd20_IsSSLURL
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_IsSSLURL( const char	*sslURL )
{

	char	sslURLCapitol	[REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_IsSSLURL")

	strncpy( sslURLCapitol, sslURL, sizeof(sslURLCapitol) );
	sslURLCapitol[sizeof(sslURLCapitol)-1] = '\0';

	wd09UpperCase( sslURLCapitol );

	if ( strlen( sslURLCapitol ) < 1 || strncmp( sslURLCapitol, "HTTPS://", 8 ))
		return sapdbwa_False;

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_RestartService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_RestartService( twd20WebAgentControlP	wa,
								  sapdbwa_HttpRequestP	req,
								  sapdbwa_HttpReplyP	rep  )
{

	sapdbwa_Bool			 rc = sapdbwa_False;
	const char				*service = NULL;
	twd20ServiceListItemP	 foundService = NULL;
	HANDLE					 libHandle;
	char					 serviceToStop	[REG_MAX_VALUE_LEN+1];
	char					 status			[MAX_STATUS_LEN];
	const char				*text = NULL;

	sapdbwa_TraceDefFunction ("wd20_RestartService")

	/* Inits */
	status[0] = '\0';

	/* Get service */
	service = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
		
	/* Get service description */
	foundService = wd20_FindServiceInList( wa->serviceList, service );

	/* If service found */
	if ( foundService ) {
		libHandle = wd40GetUserDllLibHandle( foundService->service );
		strcpy( serviceToStop, service );

		/* Start confirmation */
		if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_BEGIN, &text ))
			strcat( status, text );

		do {
			/* Stop service */
			rc = wd20_UnloadService( wa, serviceToStop);

			/* Confirmation */
			if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_NAME, &text ))
				strcat( status, text );

			strcat( status, serviceToStop );

			if ( rc ) {
				if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_OK, &text ))
					strcat( status, text );
			} else {
				if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_NOT_OK, &text ))
					strcat( status, text );
			}

			/* If last service unload was ok, get next service which uses the same library */
			foundService = NULL;
			if ( rc ) {
				foundService = wd20_GetServiceByLib( wa->serviceList, libHandle );

				if ( foundService ) {
					strcpy( serviceToStop, wd40GetUserDllName( foundService->service ));
				}
			}
		} while ( foundService );

		if ( wd15GetString( 0, ADMIN_HTML_STOP_SERVICE_CONFIRMATION_END, &text ))
			strcat( status, text );
	} else {
		/* Service not found */
		if ( wd15GetString( 0, ADMIN_HTML_SERVICE_NOT_FOUND, &text ))
			strcat( status, text );
	}

	if ( rc ) {
		/* Start service */
		rc = wd20_LoadService( wa, service);

		/* Confirmation */
		if ( wd15GetString( 0, ADMIN_HTML_HORIZONTAL_RULE, &text ))
			strcat( status, text );
		if ( wd15GetString( 0, ADMIN_HTML_START_SERVICE_CONFIRMATION_BEGIN, &text ))
			strcat( status, text );
		strcat( status, service );
		if ( rc ) {
			if ( wd15GetString( 0, ADMIN_HTML_START_SERVICE_OK, &text ))
				strcat( status, text );
		} else {
			if ( wd15GetString( 0, ADMIN_HTML_START_SERVICE_NOT_OK, &text ))
				strcat( status, text );
		}
	}

    return wd20_ShowService( wa, req, rep, service, status, NULL );
}

/****************************************************************************
 ***
 *** wd20_GotoService
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_GotoService( twd20WebAgentControlP	wa,
							   sapdbwa_HttpRequestP		req,
							   sapdbwa_HttpReplyP		rep  )
{

	sapdbwa_Bool			 rc = sapdbwa_False;
	const char				*service = NULL;
	twd20ServiceListItemP	 foundService = NULL;
	const char				*serviceName = NULL;
	char					 newBrowser [REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_GotoService")

	/* Inits */
	newBrowser[0] = '\0';

	/* Get service */
	service = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME );
		
	/* Get service description */
	foundService = wd20_FindServiceInList( wa->serviceList, service );

	/* If service found */
	if ( foundService ) {
		serviceName = wd40GetPathPrefix( foundService->service  );

		if ( serviceName ) {
			/* Get fcgi path to get the cookies */
			if ( wd20WAControl.apiType == API_FCGI_WD20 && wd40UseFastCGIForCookiePath( foundService->service )) {
				sp77sprintf(newBrowser, REG_MAX_VALUE_LEN, "/wafcgi.fcgi/%s", serviceName );
			} else {
				sp77sprintf(newBrowser, REG_MAX_VALUE_LEN, "/%s", serviceName );
			};
		}
	}

	return wd20_ShowService( wa, req, rep, service, "", newBrowser );

}

/****************************************************************************
 ***
 ***	wd20_IsSSLURL
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_GetTimeValue( const char	*inputTime,
							  char			*outputTime,
							  sapdbwa_Int4	 outputTimeLen )
{

	sapdbwa_Int4	maxLen = 9;
	sapdbwa_Int4	timeVal = 0;

	sapdbwa_TraceDefFunction ("wd20_GetTimeValue")

	if ( outputTimeLen < maxLen && outputTimeLen > 0 ) maxLen = outputTimeLen;

	strncpy( outputTime, inputTime, maxLen );
	outputTime[maxLen] = '\0';

	timeVal = atoi( outputTime );
	if ( timeVal < 0 ) timeVal = 0;

	outputTime[0]='\0';
	sp77sprintf(outputTime, outputTimeLen, "%u", timeVal );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_ShowStatus
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowStatus( sapdbwa_HttpRequestP	req,
							sapdbwa_HttpReplyP	rep  )
{

	const char		*messages = NULL;

	sapdbwa_TraceDefFunction ("wd20_ShowStatus")

	/* Get messages */
	messages = wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_MESSAGES );

	/* Show status */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	wd20_SendText( rep, 0, atoi(messages) );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_GetServiceDescription
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_GetServiceDescription( sapdbwa_HttpRequestP			 req,
									   twd20_ServiceDescription	*serviceDescription )
{

	sapdbwa_Int4				 i = 0;
    sapdbwa_StringSeqP			 keyList = sapdbwa_CreateStringSeq();
	const char				*key = NULL;

	sapdbwa_TraceDefFunction ("wd20_GetServiceDescription")

	/* Inits */
	wd20_InitServiceDescription( serviceDescription );

	/* Get service description */
	strcpy( serviceDescription->service,			wd20_GetHTMLParameter( req, ADMIN_SERVICE_PARAM_NAME ));
	strcpy( serviceDescription->sslURL,				wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_SSL_URL ));
	strcpy( serviceDescription->exitFunction,		wd20_GetHTMLParameter( req, ADMIN_REG_KEY_EXIT_FUNCTION ));
	strcpy( serviceDescription->initFunction,		wd20_GetHTMLParameter( req, ADMIN_REG_KEY_INIT_FUNCTION ));
	strcpy( serviceDescription->library,			wd20_GetHTMLParameter( req, ADMIN_REG_KEY_LIBRARY ));
	strcpy( serviceDescription->libraryType,		wd20_GetHTMLParameter( req, ADMIN_REG_KEY_LIBRARY_TYPE ));
	strcpy( serviceDescription->logFile,			wd20_GetHTMLParameter( req, ADMIN_REG_KEY_LOGFILE ));
	strcpy( serviceDescription->serviceFunction,	wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_FUNCTION ));
	strcpy( serviceDescription->serviceName,		wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_NAME ));
	strcpy( serviceDescription->sessionPool,		wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SESSION_POOL ));

	wd20_GetTimeValue( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_WEB_SESSION_TIMEOUT ),
					   serviceDescription->webSessionTimeout,
					   sizeof(serviceDescription->webSessionTimeout) );
	
	if ( strlen( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_WITH_SSL )) > 0 )
		strcpy( serviceDescription->withSSL, ADMIN_SERVICE_WITH_SSL_ON_VALUE );
	else
		strcpy( serviceDescription->withSSL, ADMIN_SERVICE_WITH_SSL_OFF_VALUE );

	if ( strlen( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_START )) > 0 )
		strcpy( serviceDescription->serviceStart, ADMIN_SERVICE_START_ON_VALUE );
	else
		strcpy( serviceDescription->serviceStart, ADMIN_SERVICE_START_OFF_VALUE );

	/* Cookie path prefix	*/
	if ( strlen( wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVICE_COOKIE_PATH_PREFIX )) > 0 )
		strcpy( serviceDescription->useFastCGIForCookiePath, ADMIN_SERVICE_COOKIE_PATH_PREFIX_ON_VALUE );
	else
		strcpy( serviceDescription->useFastCGIForCookiePath, ADMIN_SERVICE_COOKIE_PATH_PREFIX_OFF_VALUE );

	/* Get list of keys */
	if ( !sapdbwa_GetParameterNames( req, keyList ))
		return sapdbwa_False;

	/* Set service's parameter list */
	for ( i = 0 ; i < sapdbwa_GetNumElem( keyList ) ; i++ ) {
		if (( key = sapdbwa_GetStringByIndex( keyList, i )) != NULL ) {
			if ( !wd20_IsServiceDependentParameter( key ) && !wd20_IsServiceStandardParameter( key )) {
				if ( !wd20_AddParameterToServiceDescription( serviceDescription, key, wd20_GetHTMLParameter( req, key )))
					return sapdbwa_False;
			}
		}
	}

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_InitServiceDescription
 ***
 ****************************************************************************/

void wd20_InitServiceDescription( twd20_ServiceDescription	*serviceDescription )
{

	sapdbwa_TraceDefFunction ("wd20_InitServiceDescription")

	/* Inits */
	serviceDescription->service[0] = '\0';
	serviceDescription->sslURL[0] = '\0';
	serviceDescription->exitFunction[0] = '\0';
	serviceDescription->initFunction[0] = '\0';
	serviceDescription->library[0] = '\0';
	serviceDescription->libraryType[0] = '\0';
	serviceDescription->logFile[0] = '\0';
	serviceDescription->serviceFunction[0] = '\0';
	serviceDescription->serviceName[0] = '\0';
	serviceDescription->sessionPool[0] = '\0';
	serviceDescription->webSessionTimeout[0] = '\0';
	serviceDescription->withSSL[0] = '\0';
	serviceDescription->serviceStart[0] = '\0';
	serviceDescription->useFastCGIForCookiePath[0] = '\0';

	serviceDescription->parameterList = NULL;

}

/****************************************************************************
 ***
 *** wd20_AddParameterToServiceDescription
 ***
 ****************************************************************************/

twd20_Parameter* wd20_AddParameterToServiceDescription( twd20_ServiceDescription	*serviceDescription,
														const char					*name,
														const char					*value				)
{
	
    sapdbwa_Bool		 allocatOK = sapdbwa_False;
	twd20_Parameter	*currentParameter = NULL;
	twd20_Parameter	*lastParameter = NULL;

	sapdbwa_TraceDefFunction ("wd20_AddParameterToServiceDescription")

	/* Inits */
	currentParameter = serviceDescription->parameterList;
	lastParameter = serviceDescription->parameterList;

	/* Find last parameter */
	while( currentParameter ) {
		lastParameter = currentParameter;
		currentParameter = currentParameter->nextParameter;
	}

	/* Allocate new memory for ne parameter */
    sapdbwa_SQLALLOCAT( sizeof(twd20_Parameter),
                   (sapdbwa_UInt1**) &currentParameter,
                   &allocatOK );
    if (!allocatOK)
        return NULL;
	if ( !lastParameter )
		serviceDescription->parameterList = currentParameter;
	else
		lastParameter->nextParameter = currentParameter;

	/* Initialize new parameter */
	strcpy( currentParameter->name, name );
	strcpy( currentParameter->value, value );
	currentParameter->nextParameter = NULL;

	return currentParameter;

}

/****************************************************************************
 ***
 *** wd20_FreeServiceDescription
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_FreeServiceDescription( twd20_ServiceDescription	*serviceDescription )
{
	
    sapdbwa_Bool		 allocatOK = sapdbwa_False;
	twd20_Parameter	*currentParameter = NULL;
	twd20_Parameter	*nextParameter = NULL;

	sapdbwa_TraceDefFunction ("wd20_FreeServiceDescription")

	/* Inits */
	currentParameter = serviceDescription->parameterList;

	/* Free parameter list */
	while( currentParameter ) {
		nextParameter = currentParameter->nextParameter;

        sapdbwa_SQLFREE( (sapdbwa_UInt1*) currentParameter );
		currentParameter = nextParameter;
	}

	wd20_InitServiceDescription( serviceDescription );

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_GetSessionPool
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_GetSessionPoolDescription( sapdbwa_HttpRequestP		 req,
										   twd20_SessionPool	*sessionPool )
{

	sapdbwa_Int4				 i = 0;
    sapdbwa_StringSeqP			 keyList = sapdbwa_CreateStringSeq();
	const char				*key = NULL;

	sapdbwa_TraceDefFunction ("wd20_GetSessionPoolDescription")

	/* Inits */
	wd20_InitSessionPoolDescription( sessionPool );

	/* Get session pool */
	strcpy( sessionPool->name,				wd20_GetHTMLParameter( req, ADMIN_TEXT_NEW_SESSION_POOL_NAME ));
	strcpy( sessionPool->serverDB,			wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVER_DB ));
	strcpy( sessionPool->serverNode,		wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SERVER_NODE ));
	strcpy( sessionPool->userId,			wd20_GetHTMLParameter( req, ADMIN_REG_KEY_USER_ID ));
	strcpy( sessionPool->password,			wd20_GetHTMLParameter( req, ADMIN_REG_KEY_PASSWORD ));
	strcpy( sessionPool->datasource,		wd20_GetHTMLParameter( req, ADMIN_REG_KEY_DATASOURCE ));
	strcpy( sessionPool->driver,			wd20_GetHTMLParameter( req, ADMIN_REG_KEY_DRIVER ));
	strcpy( sessionPool->autocommit,		wd20_GetHTMLParameter( req, ADMIN_REG_KEY_AUTOCOMMIT ));
	strcpy( sessionPool->type,				wd20_GetHTMLParameter( req, ADMIN_REG_KEY_POOL_TYPE ));
	strcpy( sessionPool->size,				wd20_GetHTMLParameter( req, ADMIN_REG_KEY_POOL_SIZE ));
	strcpy( sessionPool->sqlTraceFilename,	wd20_GetHTMLParameter( req, ADMIN_REG_KEY_SQL_TRACE_FILENAME ));

	return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_InitSessionPoolDescription
 ***
 ****************************************************************************/

void wd20_InitSessionPoolDescription( twd20_SessionPool	*sessionPool )
{

	sapdbwa_TraceDefFunction ("wd20_InitSessionPoolDescription")

	/* Inits */
	sessionPool->name[0] = '\0';
	sessionPool->serverDB[0] = '\0';
	sessionPool->serverNode[0] = '\0';
	sessionPool->userId[0] = '\0';
	sessionPool->password[0] = '\0';
	sessionPool->datasource[0] = '\0';
	sessionPool->driver[0] = '\0';
	sessionPool->autocommit[0] = '\0';
	sessionPool->type[0] = '\0';
	sessionPool->size[0] = '\0';
	sessionPool->sqlTraceFilename[0] = '\0';

}

/****************************************************************************
 ***
 *** wd20_ShowServiceDescription
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_ShowServiceDescription( twd20_ServiceDescription	*serviceDescription,
										  twd20WebAgentControlP		 wa,
										  sapdbwa_HttpReplyP		 rep,
										  const char				*message )
{

	Reg_RegistryP	 registry;
	sapdbwa_Bool	 eos;
	sapdbwa_Bool	 userParameterExists = sapdbwa_False;
	twd20_Parameter	*currentParameter = NULL;
	twd20_Parameter	*nextParameter = NULL;
	char			 listObject	[REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_ShowServiceDescription")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        /* memory allocation error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_OPEN_REGISTRY_WD26, wd20WAControl.iniFile, NULL );

		wd20_SendServerError( rep );

        return sapdbwa_False;
    }

	/* Initialize reply header */
	sapdbwa_InitHeader( rep, sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL );
	sapdbwa_SetHeader ( rep, "Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
	sapdbwa_SendHeader( rep );

	/* Build and send page */
	wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_BEGIN );

	wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_ON_LOAD_BEGIN );
	if ( message )
		sapdbwa_SendBody( rep, message, 0 );
	else {
		sapdbwa_SendBody( rep, "", 0 );
	}
	wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_ON_LOAD_END );

	wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_FORM_BEGIN );
    sapdbwa_SendBody( rep, serviceDescription->service, 0 );
	wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_FORM_CONTINUE_1 );
	
	wd20_SendLabelRow(rep,
					  ADMIN_LABEL_SERVICE_NAME_TITLE,
					  serviceDescription->service,
					  sapdbwa_True );

	/* Send horizontal line */
	wd20_SendHorizontalLineRow( rep, 2);

	wd20_SendInputRow(rep,
					  ADMIN_INPUT_SERVICE_SERVICE_NAME_TITLE,
					  ADMIN_INPUT_SERVICE_SERVICE_NAME_NAME,
					  serviceDescription->serviceName );
	
	wd20_SendInputRow(rep,
					  ADMIN_INPUT_SERVICE_INIT_FUNCTION_TITLE,
					  ADMIN_INPUT_SERVICE_INIT_FUNCTION_NAME,
					  serviceDescription->initFunction );

	wd20_SendInputRow(rep,
					  ADMIN_INPUT_SERVICE_EXIT_FUNCTION_TITLE,
					  ADMIN_INPUT_SERVICE_EXIT_FUNCTION_NAME,
					  serviceDescription->exitFunction );
	
	wd20_SendInputRow(rep,
					  ADMIN_INPUT_SERVICE_SERVICE_FUNCTION_TITLE,
					  ADMIN_INPUT_SERVICE_SERVICE_FUNCTION_NAME,
					  serviceDescription->serviceFunction );
	
	wd20_SendInputRow(rep,
					  ADMIN_INPUT_SERVICE_LIBRARY_TITLE,
					  ADMIN_INPUT_SERVICE_LIBRARY_NAME,
					  serviceDescription->library );
	
	wd20_SendOpenListRow( rep,
						  ADMIN_LIST_SERVICE_LIBRARY_TYPE_TITLE,
						  ADMIN_LIST_SERVICE_LIBRARY_TYPE_NAME );

	if ( stricmp( serviceDescription->libraryType, ADMIN_LIBRARY_TYPE_C_VALUE ) == 0 ) {
		wd20_SendAddListRow( rep, ADMIN_LIBRARY_TYPE_C, sapdbwa_True );
	} else {
		wd20_SendAddListRow( rep, ADMIN_LIBRARY_TYPE_C, sapdbwa_False );
	}

	if ( stricmp( serviceDescription->libraryType, ADMIN_LIBRARY_TYPE_CPP_VALUE ) == 0 ) {
		wd20_SendAddListRow( rep, ADMIN_LIBRARY_TYPE_CPP, sapdbwa_True );
	} else {
		wd20_SendAddListRow( rep, ADMIN_LIBRARY_TYPE_CPP, sapdbwa_False );
	}

	wd20_SendCloseListRow( rep );
	
	wd20_SendInputRow(rep,
					  ADMIN_INPUT_SERVICE_LOGFILE_TITLE,
					  ADMIN_INPUT_SERVICE_LOGFILE_NAME,
					  serviceDescription->logFile );

	/* Enumerate sections */
	if ( !Reg_EnumRegistrySections( registry, wa->regSectionSessionPool, NULL )) {
        /* ini read error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_READ_REGISTRY_WD26, wa->regSectionSessionPool, NULL );

		wd20_SendText( rep, 0, ADMIN_HTML_ERROR );
	    Reg_CloseRegistry( registry );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

	/* Iterate registry keys */
	wd20_SendOpenListRow( rep,
						  ADMIN_LIST_SERVICE_SESSION_POOL_TITLE,
						  ADMIN_LIST_SERVICE_SESSION_POOL_NAME );
	wd20_SendAddListRow( rep, ADMIN_EMPTY_ITEM, sapdbwa_False );

	listObject[0]='\0';
	do {
		if ( !Reg_GetNextSection( registry, NULL, 0, listObject, sizeof( listObject ), &eos ))
			break;

		if ( strcmp( listObject, serviceDescription->sessionPool ) == 0 ) {
			wd20_SendAddListRow( rep, listObject, sapdbwa_True );
		} else {
			wd20_SendAddListRow( rep, listObject, sapdbwa_False );
		}

		listObject[0]='\0';
	} while ( !eos );

	wd20_SendCloseListRow( rep );

	/* Stratup type */
	if ( strcmp( serviceDescription->serviceStart, ADMIN_SERVICE_START_ON_VALUE) == 0 ) {
		wd20_SendCheckBoxRow( rep,
							  ADMIN_CHECKBOX_SERVICE_START_TYPE_TITLE,
							  ADMIN_CHECKBOX_SERVICE_START_TYPE_NAME,
							  ADMIN_CHECKBOX_SERVICE_START_TYPE_NAME,
							  sapdbwa_True, sapdbwa_True );
	} else {
		wd20_SendCheckBoxRow( rep,
							  ADMIN_CHECKBOX_SERVICE_START_TYPE_TITLE,
							  ADMIN_CHECKBOX_SERVICE_START_TYPE_NAME,
							  ADMIN_CHECKBOX_SERVICE_START_TYPE_NAME,
							  sapdbwa_False, sapdbwa_True );
	}

	/* Cookie path prefix */
	if ( strcmp( serviceDescription->useFastCGIForCookiePath, ADMIN_SERVICE_COOKIE_PATH_PREFIX_ON_VALUE) == 0 ) {
		wd20_SendCheckBoxRow( rep,
							  ADMIN_CHECKBOX_SERVICE_COOKIE_PATH_PREFIX_TITLE,
							  ADMIN_CHECKBOX_SERVICE_COOKIE_PATH_PREFIX_NAME,
							  ADMIN_CHECKBOX_SERVICE_COOKIE_PATH_PREFIX_NAME,
							  sapdbwa_True, sapdbwa_True );
	} else {
		wd20_SendCheckBoxRow( rep,
							  ADMIN_CHECKBOX_SERVICE_COOKIE_PATH_PREFIX_TITLE,
							  ADMIN_CHECKBOX_SERVICE_COOKIE_PATH_PREFIX_NAME,
							  ADMIN_CHECKBOX_SERVICE_COOKIE_PATH_PREFIX_NAME,
							  sapdbwa_False, sapdbwa_True );
	}

	/* Secure type */
	if ( strcmp( serviceDescription->withSSL, ADMIN_SERVICE_WITH_SSL_ON_VALUE) == 0 ) {
		wd20_SendCheckBoxRow( rep,
							  ADMIN_CHECKBOX_SERVICE_WITH_SSL_TITLE,
							  ADMIN_CHECKBOX_SERVICE_WITH_SSL_TITLE,
							  ADMIN_CHECKBOX_SERVICE_WITH_SSL_TITLE,
							  sapdbwa_True, sapdbwa_True );
	} else {
		wd20_SendCheckBoxRow( rep,
							  ADMIN_CHECKBOX_SERVICE_WITH_SSL_TITLE,
							  ADMIN_CHECKBOX_SERVICE_WITH_SSL_TITLE,
							  ADMIN_CHECKBOX_SERVICE_WITH_SSL_TITLE,
							  sapdbwa_False, sapdbwa_True );
	}

	/* SSL URL */
	wd20_SendInputRow(rep,
					  ADMIN_INPUT_SERVICE_SSL_URL_TITLE,
					  ADMIN_INPUT_SERVICE_SSL_URL_NAME,
					  serviceDescription->sslURL );

	/* Web session timeout */
	wd20_SendInputRow(rep,
					  ADMIN_INPUT_SERVICE_WEB_SESSION_TIMEOUT_TITLE,
					  ADMIN_INPUT_SERVICE_WEB_SESSION_TIMEOUT_NAME,
					  serviceDescription->webSessionTimeout );
	
	/* Iterate parameters */
	currentParameter = serviceDescription->parameterList;

	while( currentParameter ) {
		nextParameter = currentParameter->nextParameter;

		if ( !wd20_IsServiceStandardParameter( currentParameter->name )) {
			if ( !userParameterExists ) {
				wd20_SendHorizontalLineRow( rep, 2);
				wd20_SendLabelRow( rep, ADMIN_LABEL_SERVICE_USER_PARAMETER_TITLE, "", sapdbwa_False );
				userParameterExists = sapdbwa_True;
			}

			/* Send user parameter */
			wd20_SendInputRow( rep, currentParameter->name, currentParameter->name, currentParameter->value );
		}

		currentParameter = nextParameter;
	}

	/* Send horizontal line */
	wd20_SendHorizontalLineRow( rep, 2);

	/* Close form */
	wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_FORM_END_BEGIN );
	if ( wd20_FindServiceInList( wa->serviceList, serviceDescription->service )) {
		if ( wa->developerMode ) {
			wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_FORM_END_STOP_SERVICE );
			wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_FORM_END_RELOAD_SERVICE );
		}
	} else {
		wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_FORM_END_START_SERVICE );
	}
	wd20_SendText( rep, 0, ADMIN_HTML_SERVICE_FORM_END_END );

	/* Close registry key */
	Reg_CloseRegistry( registry );

	return sapdbwa_False;

}	

/****************************************************************************
 ***
 *** wd20_SendInputRow
 ***
 ****************************************************************************/

void wd20_SendInputRow( sapdbwa_HttpReplyP rep,
						const char	*title,
						const char	*name,
						const char	*value )
{

	sapdbwa_TraceDefFunction ("wd20_SendInputRow")

	/* Send input row */
	sapdbwa_SendBody( rep, "<tr>\n  <td>", 0 );
	sapdbwa_SendBody( rep, title, 0 );
	sapdbwa_SendBody( rep, ":</td>\n  <td><input type=\"text\" name=\"", 0 );
	sapdbwa_SendBody( rep, name, 0 );
	sapdbwa_SendBody( rep, "\" size=\"60\" value=\"", 0 );
	sapdbwa_SendBody( rep, value, 0 );
	sapdbwa_SendBody( rep, "\"></td>\n</tr>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendCheckBoxRow
 ***
 ****************************************************************************/

void wd20_SendCheckBoxRow( sapdbwa_HttpReplyP  rep,
						   const char		  *title,
						   const char	      *name,
						   const char	      *value,
						   bool			       checked,
						   sapdbwa_Bool	       multiColumn )
{

	sapdbwa_TraceDefFunction ("wd20_SendCheckBoxRow")

	/* Send input row */
	if ( multiColumn ) {
		sapdbwa_SendBody( rep, "<tr>\n  <td>", 0 );
		sapdbwa_SendBody( rep, title, 0 );
			sapdbwa_SendBody( rep, ":</td>\n  <td><input ", 0 );
		if ( checked ) 
			sapdbwa_SendBody( rep, "checked ", 0 );
		sapdbwa_SendBody( rep, "type=checkbox name=\"", 0 );
		sapdbwa_SendBody( rep, name, 0 );
		sapdbwa_SendBody( rep, "\" value=\"", 0 );
		sapdbwa_SendBody( rep, value, 0 );
		sapdbwa_SendBody( rep, "\"></td>\n</tr>\n", 0 );
	} else {
		sapdbwa_SendBody( rep, "<tr><td><input ", 0 );
		if ( checked ) 
			sapdbwa_SendBody( rep, "checked ", 0 );
		sapdbwa_SendBody( rep, "type=checkbox name=\"", 0 );
		sapdbwa_SendBody( rep, name, 0 );
		sapdbwa_SendBody( rep, "\" value=\"", 0 );
		sapdbwa_SendBody( rep, value, 0 );
		sapdbwa_SendBody( rep, "\">  ", 0 );
		sapdbwa_SendBody( rep, title, 0 );
		sapdbwa_SendBody( rep, "</td>\n</tr>\n", 0 );
	}

}

/****************************************************************************
 ***
 *** wd20_SendOpenListRow
 ***
 ****************************************************************************/

void wd20_SendOpenListRow( sapdbwa_HttpReplyP	 rep,
						   const char	*title,
						   const char	*name  )
{

	sapdbwa_TraceDefFunction ("wd20_SendOpenListRow")

	/* Send row */
	sapdbwa_SendBody( rep, "<tr>\n  <td>", 0 );
	sapdbwa_SendBody( rep, title, 0 );
	sapdbwa_SendBody( rep, ":</td>\n  <td>\n    <select name=\"", 0 );
	sapdbwa_SendBody( rep, name, 0 );
	sapdbwa_SendBody( rep, "\">\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendAddListRow
 ***
 ****************************************************************************/

void wd20_SendAddListRow( sapdbwa_HttpReplyP	 rep,
						  const char	*value,
						  bool			 selected )
{

	sapdbwa_TraceDefFunction ("wd20_SendAddListRow")

	/* Send row */
	if ( selected )
		sapdbwa_SendBody( rep, "      <option selected>", 0 );
	else
		sapdbwa_SendBody( rep, "      <option>", 0 );

	sapdbwa_SendBody( rep, value, 0 );
	sapdbwa_SendBody( rep, "</option>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendCloseListRow
 ***
 ****************************************************************************/

void wd20_SendCloseListRow( sapdbwa_HttpReplyP	rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendCloseListRow")

	/* Send row */
	sapdbwa_SendBody( rep, "    </select>\n  </td>\n</tr>", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendLabelRow
 ***
 ****************************************************************************/

void wd20_SendLabelRow( sapdbwa_HttpReplyP rep,
						const char	*title,
						const char	*label,
						bool		 bold   )
{

	sapdbwa_TraceDefFunction ("wd20_SendLabelRow")

	/* Send row */
	sapdbwa_SendBody( rep, "<TABLE cellSpacing=0 cellPadding=0 width=100% bgColor=#dedec8 border=0>\n", 0 );
	sapdbwa_SendBody( rep, " <TBODY>\n", 0 );
	sapdbwa_SendBody( rep, "  <TR>\n", 0 );
	sapdbwa_SendBody( rep, "   <TD bgcolor=\"#6780b8\" noWrap>&nbsp;&nbsp;", 0 );
	sapdbwa_SendBody( rep, title, 0 );
	sapdbwa_SendBody( rep, "&nbsp;&nbsp;</TD>\n", 0 );
	sapdbwa_SendBody( rep, "   <TD vAlign=top rowSpan=2><IMG src=\"/WARoot/Images/wacorner.gif\"></TD>\n", 0 );
	sapdbwa_SendBody( rep, "   <TD width=\"100%\" rowSpan=2><IMG src=\"/WARoot/Images/1x1.gif\"></TD>\n  </TR>\n", 0 );
	sapdbwa_SendBody( rep, " </TBODY>\n</TABLE>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendHorizontalLineRow
 ***
 ****************************************************************************/

void wd20_SendHorizontalLineRow( sapdbwa_HttpReplyP	rep,
								 sapdbwa_UInt2	    colNo )
{
	char	rowToSend[REPLY_BUFFER_LEN];

	sapdbwa_TraceDefFunction ("wd20_SendHorizontalLineRow")

	/* Send row */
	rowToSend[0] = '\0';
	sp77sprintf(rowToSend, REPLY_BUFFER_LEN, "<tr>\n  <td colspan=\"%d\"><img src=\"/WARoot/Images/1x1.gif\" width=\"1\" height=\"2\"></td>\n</tr>\n"\
					   "<tr>\n  <td colspan=\"%d\" bgcolor=\"darkgray\"><img src=\"/WARoot/Images/1x1.gif\" width=\"1\" height=\"3\"></td>\n</td>\n",
					   colNo, colNo );

	sapdbwa_SendBody( rep, rowToSend, 0 );

}

/****************************************************************************
 ***
 *** wd20_InitControl
 ***
 ****************************************************************************/

void wd20_InitControl()
{

	sapdbwa_TraceDefFunction ("wd20_InitControl")

    wd20WAControl.sessionPoolList		= NULL;

    wd20WAControl.serviceId				= ADMIN_SESSION_ID_INIT_VALUE;
    wd20WAControl.serviceList			= NULL;

#ifdef HAVE_COM_CLASSES    
    wd20WAControl.nComClasses			= 0;
    wd20WAControl.comClasses			= NULL;
#endif
    
    wd20WAControl.sessionCont			= NULL;
    wd20WAControl.waLog					= NULL;
    wd20WAControl.confLog				= NULL;
    wd20WAControl.waErr					= NULL;

	wd20WAControl.documentRoot		[0]	= '\0';
	wd20WAControl.mimeTypesFilename	[0]	= '\0';
	wd20WAControl.mimeTypes				= NULL;

	wd20WAControl.developerMode			= sapdbwa_False;

    wd20WAControl.excl					= NULL;
    wd20WAControl.initialized			= sapdbwa_False;
}

/****************************************************************************
 ***
 *** wd20_LoadAllSessionPools
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_LoadAllSessionPools( twd20WebAgentControlP wa )
{

	const char FUNCTION_NAME[] = "wd20_LoadAllSessionPools";

	Reg_RegistryP	registry;
	sapdbwa_Bool		eos;
	char			sessionPool	[REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction ("wd20_LoadAllSessionPools")

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile )) {
        /* memory allocation error */
        wd26SetErr( wd20WAControl.waErr,
                    ERR_OPEN_REGISTRY_WD26, wd20WAControl.iniFile, NULL );

        return sapdbwa_False;
    }

	/* Enumerate sections */
	if ( !Reg_EnumRegistrySections( registry, wa->regSectionSessionPool, NULL )) {
        /* ini read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, wa->regSectionSessionPool, NULL );
		Reg_CloseRegistry( registry );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
        return sapdbwa_False;
	}

	/* Iterate session pools */
	sessionPool[0]='\0';
	do {
		if ( !Reg_GetNextSection( registry, NULL, 0, sessionPool, sizeof( sessionPool ), &eos ))
			break;

		if ( strlen( sessionPool ) > 0 ) {
			/* Load session pool */
			if ( !wd20_LoadSessionPool( wa, sessionPool )) {
				wd26SetErr( wa->waErr, ERR_SESSION_POOL_LOAD_FAILED_WD26, sessionPool, NULL);
			}
		}

		sessionPool[0]='\0';
	} while ( !eos );

	/* Close registry key */
	Reg_CloseRegistry( registry );

    return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_UnloadAllSessionPools
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UnloadAllSessionPools( twd20WebAgentControlP	wa )
{

	sapdbwa_Bool	rc = sapdbwa_True;

	sapdbwa_TraceDefFunction ("wd20_UnloadAllSessionPools")

	while ( wa->sessionPoolList ) {
		if ( !wd20_UnloadSessionPool( wa, wd30GetName( wa->sessionPoolList->sessionPool )))
			rc = sapdbwa_False;
	}

    return rc;

}

/****************************************************************************
 ***
 *** wd20_LoadSessionPool
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_LoadSessionPool( twd20WebAgentControlP	 wa,
								 const char				*sessionPool  )
{

	char				name				[MAX_INI_PAR_LEN_WD00+1];
    char				dataSource			[MAX_INI_PAR_LEN_WD00+1];
    char				driver				[MAX_FILENAME_LEN_WD00+1];
    char				serverNode			[MAX_INI_PAR_LEN_WD00+1];
    char				serverDB			[MAX_INI_PAR_LEN_WD00+1];
    char				userId				[MAX_INI_PAR_LEN_WD00+1];
    char				password			[MAX_INI_PAR_LEN_WD00+1];
    char				size				[MAX_INI_PAR_LEN_WD00+1];
    char				autocommit			[MAX_INI_PAR_LEN_WD00+1];
    char				sqlTraceFilename	[MAX_FILENAME_LEN_WD00+1];
    char				type				[MAX_INI_PAR_LEN_WD00+1];
    /*
    char				isolation			[MAX_INI_PAR_LEN_WD00+1];
    */
    
	char				currentSection		[REG_MAX_SECTION_LEN+1];

	twd30SessionPoolP	newSessionPool;
    sapdbwa_Int4			newSize;
    bool				newAutocommit;
    sapdbwa_Int4			newIsolation;
	Reg_RegistryP		registry;

	sapdbwa_TraceDefFunction ("wd20_LoadSessionPool")

	/* Get current section */
	sp77sprintf(currentSection, REG_MAX_SECTION_LEN, "%s\\%s", wa->regSectionSessionPool, sessionPool);

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, wd20WAControl.iniFile ))
		return sapdbwa_False;

	/* Get datasource */
    dataSource[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_DATASOURCE, dataSource, sizeof( dataSource ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_DATASOURCE );
	}

	/* Get driver */
    driver[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_DRIVER, driver, sizeof( driver ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_DRIVER );
	}

	/* Get server node */
    serverNode[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_SERVER_NODE, serverNode, sizeof( serverNode ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SERVER_NODE );
	}

	/* Get server DB */
    serverDB[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_SERVER_DB, serverDB, sizeof( serverDB ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SERVER_DB );
	}

	/* Get user id */
    userId[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_USER_ID, userId, sizeof( userId ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_USER_ID );
	}

	/* Get password */
    password[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_PASSWORD, password, sizeof( password ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_PASSWORD );
	}

	/* Get pool size */
    size[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_POOL_SIZE, size, sizeof( size ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_POOL_SIZE );
	}
	newSize = atoi( size );

	/* Get pool type */
    type[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_POOL_TYPE, type, sizeof( type ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_POOL_TYPE );
	}

	/* Autocommit */
    autocommit[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_AUTOCOMMIT, autocommit, sizeof( autocommit ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_AUTOCOMMIT );
	}
	if ( strcmp( autocommit, ADMIN_AUTOCOMMIT_ON_VALUE ) == 0 )
		newAutocommit = sapdbwa_True;
	else
		newAutocommit = sapdbwa_False;

	/* SQL trace filename */
    sqlTraceFilename[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_SQL_TRACE_FILENAME, sqlTraceFilename, sizeof( sqlTraceFilename ), "" )) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_SQL_TRACE_FILENAME );
	}

	/* Isolation */
    /*
    isolation[0] = '\0';
    if ( !wd20_GetRegistryValue( registry, currentSection, ADMIN_REG_KEY_ISOLATION, isolation, sizeof( isolation ), "" )) {
        * Registry read error *
        wd26SetErr( wa->waErr, ERR_READ_REGISTRY_WD26, currentSection, ADMIN_REG_KEY_ISOLATION );
	}
    */
	if ( !wd20_GetIsolation( sessionPool, "", &newIsolation, wa->waErr )) {
		Reg_CloseRegistry( registry );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	};

	/* Close registry */
	Reg_CloseRegistry( registry );

	/* Create session pool */
    if (( newSessionPool = wd30CreateSessionPool( sessionPool, wa->waErr )) == NULL ) return sapdbwa_False;

	/* Init session pool */
	if ( strcmp( type, ADMIN_POOL_TYPE_ODBC_VALUE ) == 0 ) {
		if (!wd30InitSessionPool( newSessionPool,
								  dataSource,
								  driver,
								  serverNode,
								  serverDB,
								  userId,
								  password,
								  newSize,
								  newAutocommit,
								  newIsolation,
								  sqlTraceFilename,
								  wa->waErr )) {
			/* init failed */
			wd30DestroySessionPool( newSessionPool );

			sapdbwa_TraceAdd( "Exiting unexpected!" )
			return sapdbwa_False;
		}
	} else {
		if (!wd30InitSqlSessionPool( newSessionPool,
									 serverNode, serverDB,
									 userId, password,
									 newIsolation, newSize,
									 wa->waErr )) {
			/* init failed */
			wd30DestroySessionPool( newSessionPool );

			sapdbwa_TraceAdd( "Exiting unexpected!" )
			return sapdbwa_False;
		}
	}

	/* Add new session pool to list */
	if ( !wd20_AddSessionPoolToList ( &(wa->sessionPoolList), newSessionPool )) {
		wd30DestroySessionPool( newSessionPool );

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return sapdbwa_False;
	}

    return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_UnloadSessionPool
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_UnloadSessionPool( twd20WebAgentControlP	 wa,
								   const char				*sessionPool)
{

    sapdbwa_Bool  retcode = sapdbwa_True;
    sapdbwa_Handle   waHandle;
    sapdbwa_Int2  exitFuncRetcode;

	twd20SessionPoolListItemP	 foundSessionPool = NULL;
	twd30SessionPoolP			 sessionPoolToDelete = NULL;

	sapdbwa_TraceDefFunction ("wd20_UnloadSessionPool")

	/* Get session pool to delete */
	if (( foundSessionPool = wd20_FindSessionPoolInList( wa->sessionPoolList, sessionPool )) == NULL ) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_SESSION_POOL_UNLOAD_FAILED_WD26, sessionPool, NULL );

		return sapdbwa_False;
	}

	sessionPoolToDelete = foundSessionPool->sessionPool;
	if ( !sessionPoolToDelete ) {
        /* Registry read error */
        wd26SetErr( wa->waErr, ERR_SESSION_POOL_UNLOAD_FAILED_WD26, sessionPool, NULL );

		return sapdbwa_False;
	}

	/* Delete service from service list */
	wd20_DeleteSessionPoolFromList ( &(wa->sessionPoolList), sessionPool );

	/* Destroy session pool */
	wd30DestroySessionPool( sessionPoolToDelete );

    return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_AddSessionPoolToList
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_AddSessionPoolToList( twd20SessionPoolListItemP	*firstSessionPool,
									  twd30SessionPoolP			 sessionPool       )
{
	const char FUNCTION_NAME[] = "wd20_AddSessionPoolToList";

	twd20SessionPoolListItemP	*currentItem;
	twd20SessionPoolListItemP	 currentSessionPool;
    sapdbwa_Bool					 allocatOk;

	sapdbwa_TraceDefFunction ("wd20_AddSessionPoolToList")

	/* Find end of list */
	currentItem = firstSessionPool;
	currentSessionPool = *currentItem;
	while ( currentSessionPool != NULL ) {
		currentItem = &(currentSessionPool->nextSessionPool);
		currentSessionPool = *currentItem;
	}

	/* Allocate memory for new session pool item */
    sapdbwa_SQLALLOCAT( sizeof(twd20SessionPoolListItem), (sapdbwa_UInt1**) currentItem, &allocatOk );
	if ( !allocatOk ) {
        /* Memory alloc failure */
        wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );

		return sapdbwa_False;
	}

	/* Add new session pool to list */
	currentSessionPool = *currentItem;
	currentSessionPool->sessionPool = sessionPool;
	currentSessionPool->nextSessionPool = NULL;

    return sapdbwa_True;;

}

/****************************************************************************
 ***
 *** wd20_DeleteSessionPoolFromList
 ***
 ****************************************************************************/

sapdbwa_Bool wd20_DeleteSessionPoolFromList( twd20SessionPoolListItemP	*firstSessionPool,
										   const char					*sessionPool       )
{
	const char FUNCTION_NAME[] = "wd20_DeleteSessionPoolFromList";

	twd20SessionPoolListItemP	*lastItem;
	twd20SessionPoolListItemP	*currentItem;
	twd20SessionPoolListItemP	 lastSessionPool;
	twd20SessionPoolListItemP	 currentSessionPool;
    sapdbwa_Bool					 found;

	sapdbwa_TraceDefFunction ("wd20_DeleteSessionPoolFromList")

	/* Find session pool in list */
	found = sapdbwa_False;
	lastItem = firstSessionPool;
	currentItem = firstSessionPool;
	currentSessionPool = *firstSessionPool;
	while ( !found ) {
		if ( currentSessionPool && strcmp( wd30GetName( currentSessionPool->sessionPool ), sessionPool ) != 0 ) {
			lastItem = currentItem;
			currentItem = &(currentSessionPool->nextSessionPool);
			currentSessionPool = *currentItem;
		} else
			found = sapdbwa_True;
	}

	/* Session pool found? */
	if ( found ) {
		/* Set new list pointers */
		if ( currentItem == firstSessionPool ) {
			*firstSessionPool = currentSessionPool->nextSessionPool;
		} else {
			lastSessionPool = *lastItem;
			lastSessionPool->nextSessionPool = currentSessionPool->nextSessionPool;
		}

		/* Free memory for list item */
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) currentSessionPool );

	    return sapdbwa_True;;
	}

    return sapdbwa_False;

}

/****************************************************************************
 ***
 *** wd20_FindSessionPoolInList
 ***
 ****************************************************************************/

twd20SessionPoolListItemP wd20_FindSessionPoolInList( twd20SessionPoolListItemP	 firstSessionPool,
													  const char				*sessionPool       )
{
	const char FUNCTION_NAME[] = "wd20_FindSessionPoolInList";

	twd20SessionPoolListItemP	currentSessionPool;
    sapdbwa_Bool					found;

	sapdbwa_TraceDefFunction ("wd20_FindSessionPoolInList")

	/* Find session pool in list */
	found = sapdbwa_False;
	currentSessionPool = firstSessionPool;
	while ( !found ) {
		if ( currentSessionPool && sessionPool &&
			 strcmp( wd30GetName( currentSessionPool->sessionPool ), sessionPool ) != 0 ) {

			currentSessionPool = currentSessionPool->nextSessionPool;

		} else
			found = sapdbwa_True;
	}

	/* Session pool found? */
	if ( found ) {
		return currentSessionPool;
	} else {
		return NULL;
	}

}

/****************************************************************************
 ***
 *** wd20_SendSubmitFunction
 ***
 ****************************************************************************/

void wd20_SendSubmitFunction( sapdbwa_HttpReplyP  rep,
						      const char	     *action )
{

	sapdbwa_TraceDefFunction ("wd20_SendSubmitFunction")

	/* Send submit function */
	sapdbwa_SendBody( rep, "function ", 0 );
	sapdbwa_SendBody( rep, action, 0 );
	sapdbwa_SendBody( rep, "() {\n    document.myForm.Action.value = '", 0 );
	sapdbwa_SendBody( rep, action, 0 );
	sapdbwa_SendBody( rep, "';\n    document.myForm.submit();\n}\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendCommonSubmitFunction
 ***
 ****************************************************************************/

void wd20_SendCommonSubmitFunction( sapdbwa_HttpReplyP  rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendCommonSubmitFunction")

	/* Send common submit function */
	sapdbwa_SendBody( rep, "function CommonSubmit(ActionValue) {\n", 0 );
	sapdbwa_SendBody( rep, "    document.myForm.WAServiceAction.value = ActionValue;\n", 0 );
	sapdbwa_SendBody( rep, "    document.myForm.submit();\n}\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendButton
 ***
 ****************************************************************************/

void wd20_SendButton( sapdbwa_HttpReplyP  rep,
					  const char	     *action,
					  const char	     *buttonText )
{

	sapdbwa_TraceDefFunction ("wd20_SendButton")

	/* Send button */
	sapdbwa_SendBody( rep, "<TD ALIGN=\"right\"><IMG SRC=\"/WARoot/Images/btnl1.gif\"></TD>\n", 0 );
	sapdbwa_SendBody( rep, "<TD BACKGROUND=\"/WARoot/Images/btnb1.gif\" ALIGN=\"center\" NOWRAP>&nbsp;&nbsp;", 0 );
	sapdbwa_SendBody( rep, "<A HREF=\"javascript:CommonSubmit('", 0 );
	sapdbwa_SendBody( rep, action, 0 );
	sapdbwa_SendBody( rep, "')\">", 0 );
	sapdbwa_SendBody( rep, buttonText, 0 );
	sapdbwa_SendBody( rep, "</A>&nbsp;&nbsp;\n</TD>\n", 0 );
	sapdbwa_SendBody( rep, "<TD><IMG SRC=\"/WARoot/Images/btnr1.gif\"></TD>\n", 0 );
	sapdbwa_SendBody( rep, "<TD><IMG SRC=\"/WARoot/Images/1x1.gif\" WIDTH=\"3\"></TD>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendButtonStart
 ***
 ****************************************************************************/

void wd20_SendButtonStart( sapdbwa_HttpReplyP  rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendButtonStart")

	/* Send button */
	sapdbwa_SendBody( rep, "<TR><TD><IMG SRC=\"/WARoot/Images/1x1.gif\" WIDTH=\"10\"></TD>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendButtonEnd
 ***
 ****************************************************************************/

void wd20_SendButtonEnd( sapdbwa_HttpReplyP  rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendButtonEnd")

	/* Send button */
	sapdbwa_SendBody( rep, "</TR>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendTableRowStart
 ***
 ****************************************************************************/

void wd20_SendTableRowStart( sapdbwa_HttpReplyP  rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendTableRowStart")

	/* Send HTML */
	sapdbwa_SendBody( rep, "<TR>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendTableRowEnd
 ***
 ****************************************************************************/

void wd20_SendTableRowEnd( sapdbwa_HttpReplyP  rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendTableRowEnd")

	/* Send HTML */
	sapdbwa_SendBody( rep, "</TR>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendTableColumnStart
 ***
 ****************************************************************************/

void wd20_SendTableColumnStart( sapdbwa_HttpReplyP  rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendTableColumnStart")

	/* Send HTML */
	sapdbwa_SendBody( rep, "<TD>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendTableRowEnd
 ***
 ****************************************************************************/

void wd20_SendTableColumnEnd( sapdbwa_HttpReplyP  rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendTableColumnEnd")

	/* Send HTML */
	sapdbwa_SendBody( rep, "</TD>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendBlankTableRow
 ***
 ****************************************************************************/

void wd20_SendBlankTableRow( sapdbwa_HttpReplyP  rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendBlankTableRow")

	/* Send HTML */
	sapdbwa_SendBody( rep, "<TR><TD><IMG SRC=\"/WARoot/Images/1x1.gif\" HEIGHT=\"10\"></TD></TR>\n", 0 );

}

/****************************************************************************
 ***
 *** wd20_SendTableSeparatorLine
 ***
 ****************************************************************************/

void wd20_SendTableSeparatorLine( sapdbwa_HttpReplyP  rep )
{

	sapdbwa_TraceDefFunction ("wd20_SendTableSeparatorLine")

	/* Send HTML */
	sapdbwa_SendBody( rep, "<TR><TD><IMG SRC=\"/WARoot/Images/1x1.gif\" HEIGHT=\"5\"></TD></TR>\n", 0 );
	sapdbwa_SendBody( rep, "<TR><TD COLSPAN=5 bgcolor=\"#FDFDF3\"><IMG SRC=\"/WARoot/Images/1x1.gif\"></TD></TR>\n", 0 );
	sapdbwa_SendBody( rep, "<TR><TD COLSPAN=5 bgcolor=\"#CDCDBE\"><IMG SRC=\"/WARoot/Images/1x1.gif\"></TD></TR>\n", 0 );
	sapdbwa_SendBody( rep, "<TR><TD><IMG SRC=\"/WARoot/Images/1x1.gif\" HEIGHT=\"5\"></TD></TR>\n", 0 );

}

/*!**********************************************************************

  EndChapter: st_wa_control internal functions

************************************************************************/

/*!**********************************************************************

	HTML Template value list management
  
************************************************************************/

sapdbwa_Bool CreateTemplateValueList ( sapdbwa_TemplateValueList	**valueList )
{
	const char FUNCTION_NAME  [] = "CreateTemplateValueList";

	sapdbwa_Bool				 ok = sapdbwa_False;

	/* Allocat memory for the initial list item	*/
    sapdbwa_SQLALLOCAT( sizeof(sapdbwa_TemplateValueList), (sapdbwa_UInt1**) valueList, &ok );
	if ( !ok ) {
        /* Memory alloc failure */
        wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );

		return sapdbwa_False;
	}

	(*valueList)->listItem     = NULL;

	return sapdbwa_True;

}

sapdbwa_Bool CreateTemplateValueListItem ( sapdbwa_TemplateValueListItem **valueListItem,
									       const char					  *name,
									       const char					  *value,
									       sapdbwa_TemplateValueTable	  *table		)
{
	const char FUNCTION_NAME  [] = "CreateTemplateValueListItem";

	sapdbwa_Bool					 ok = sapdbwa_False;
	sapdbwa_TemplateValueListItem	*newListItem = NULL;

	/* Allocat memory for the initial list item	*/
    sapdbwa_SQLALLOCAT( sizeof(sapdbwa_TemplateValueListItem), (sapdbwa_UInt1**) valueListItem, &ok );
	if ( !ok ) {
        /* Memory alloc failure */
        wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );

		return sapdbwa_False;
	}

	newListItem = *valueListItem;

	/* Allocat memory for item name	*/
	if ( name ) {
		sapdbwa_SQLALLOCAT( strlen(name) + 1, (sapdbwa_UInt1**) &(newListItem->name), &ok );
		if ( !ok ) {
			/* Memory alloc failure */
			wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );
			sapdbwa_SQLFREE( (sapdbwa_UInt1*) newListItem );

			return sapdbwa_False;
		}

		strcpy( newListItem->name, name );
	} else {
		newListItem->name = NULL;
	}

	/* Allocat memory item value	*/
	if ( value ) {
		sapdbwa_SQLALLOCAT( strlen(value) + 1, (sapdbwa_UInt1**) &(newListItem->value), &ok );
		if ( !ok ) {
			/* Memory alloc failure */
			wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );
			sapdbwa_SQLFREE( (sapdbwa_UInt1*) newListItem->name );
			sapdbwa_SQLFREE( (sapdbwa_UInt1*) newListItem );

			return sapdbwa_False;
		}

		strcpy( newListItem->value, value );
	} else {
		newListItem->value = NULL;
	}

	newListItem->table = table;
	newListItem->nextListItem = NULL;

	return sapdbwa_True;

}

sapdbwa_Bool CreateTemplateValueTable ( sapdbwa_TemplateValueTable	**table )
{
	const char FUNCTION_NAME  [] = "CreateTemplateValueListItem";

	sapdbwa_Bool				 ok = sapdbwa_False;
	sapdbwa_TemplateValueTable	*newTable = NULL;

	/* Allocat memory for the initial list item	*/
    sapdbwa_SQLALLOCAT( sizeof(sapdbwa_TemplateValueTable), (sapdbwa_UInt1**) table, &ok );
	if ( !ok ) {
        /* Memory alloc failure */
        wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );

		return sapdbwa_False;
	}

	newTable = *table;
	newTable->row = NULL;

	return sapdbwa_True;

}

sapdbwa_Bool CreateTemplateValueTableRow ( sapdbwa_TemplateValueTableRow	**row )
{
	const char FUNCTION_NAME  [] = "CreateTemplateValueListItem";

	sapdbwa_Bool					 ok = sapdbwa_False;
	sapdbwa_TemplateValueTableRow	*newRow = NULL;

	/* Allocat memory for the initial list item	*/
    sapdbwa_SQLALLOCAT( sizeof(sapdbwa_TemplateValueTableRow), (sapdbwa_UInt1**) row, &ok );
	if ( !ok ) {
        /* Memory alloc failure */
        wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );

		return sapdbwa_False;
	}

	newRow = *row;
	newRow->column = NULL;
	newRow->nextRow = NULL;

	return sapdbwa_True;

}

sapdbwa_Bool CreateTemplateValueTableColumn ( sapdbwa_TemplateValueTableColumn	**column,
											  const char						 *value  )
{
	const char FUNCTION_NAME  [] = "CreateTemplateValueTableColumn";

	sapdbwa_Bool						 ok = sapdbwa_False;
	sapdbwa_TemplateValueTableColumn	*newColumn = NULL;

	/* Allocat memory for the initial list item	*/
    sapdbwa_SQLALLOCAT( sizeof(sapdbwa_TemplateValueTableColumn), (sapdbwa_UInt1**) column, &ok );
	if ( !ok ) {
        /* Memory alloc failure */
        wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );

		return sapdbwa_False;
	}

	newColumn = *column;

	/* Allocat memory column value	*/
	if ( value ) {
		sapdbwa_SQLALLOCAT( strlen(value) + 1, (sapdbwa_UInt1**) &(newColumn->value), &ok );
		if ( !ok ) {
			/* Memory alloc failure */
			wd26SetErr( wd20WAControl.waErr, ERR_MEMORY_WD26, MODULE_NAME, FUNCTION_NAME );
			sapdbwa_SQLFREE( (sapdbwa_UInt1*) newColumn );

			return sapdbwa_False;
		}

		strcpy( newColumn->value, value );
	} else {
		newColumn->value = NULL;
	}

	newColumn->nextColumn = NULL;

	return sapdbwa_True;

}

sapdbwa_Bool DropTemplateValueList ( sapdbwa_TemplateValueList	*valueList )
{
	const char FUNCTION_NAME  [] = "DropTemplateValueList";

	/* If list is empty return	*/
	if ( !valueList ) return sapdbwa_True;

	/* Drop value list	*/
	DropTemplateValueListItem( valueList->listItem );

	sapdbwa_SQLFREE( (sapdbwa_UInt1*) valueList );

	return sapdbwa_True;

}

sapdbwa_Bool DropTemplateValueListItem ( sapdbwa_TemplateValueListItem	*listItem )
{
	const char FUNCTION_NAME  [] = "DropTemplateValueListItem";

	sapdbwa_TemplateValueListItem	*listItemToDrop = NULL;
	sapdbwa_TemplateValueListItem	*nextListItem = NULL;

	/* If item is empty return	*/
	if ( !listItem ) return sapdbwa_True;

	/* Drop list item	*/
	listItemToDrop = listItem->nextListItem;
	while ( listItemToDrop ) {
		nextListItem = listItemToDrop->nextListItem;

		DropTemplateValueTable( listItemToDrop->table );

		if ( listItemToDrop->name ) {
			sapdbwa_SQLFREE( (sapdbwa_UInt1*) listItemToDrop->name );
		}
		if ( listItemToDrop->value ) {
			sapdbwa_SQLFREE( (sapdbwa_UInt1*) listItemToDrop->value );
		}
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) listItemToDrop );

		listItemToDrop = nextListItem;
	}

	DropTemplateValueTable( listItem->table );
	if ( listItem->name ) {
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) listItem->name );
	}
	if ( listItem->value ) {
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) listItem->value );
	}
	sapdbwa_SQLFREE( (sapdbwa_UInt1*) listItem );


	return sapdbwa_True;

}

sapdbwa_Bool DropTemplateValueTable ( sapdbwa_TemplateValueTable	*table )
{
	const char FUNCTION_NAME  [] = "DropTemplateValueTable";

	/* If table is empty return	*/
	if ( !table ) return sapdbwa_True;

	/* Drop table	*/
	DropTemplateValueTableRow( table->row );

	sapdbwa_SQLFREE( (sapdbwa_UInt1*) table );

	return sapdbwa_True;

}

sapdbwa_Bool DropTemplateValueTableRow ( sapdbwa_TemplateValueTableRow	*row )
{
	const char FUNCTION_NAME  [] = "DropTemplateValueTableRow";

	sapdbwa_TemplateValueTableRow	*rowToDrop = NULL;
	sapdbwa_TemplateValueTableRow	*nextRow = NULL;

	/* If row is empty return	*/
	if ( !row ) return sapdbwa_True;

	/* Drop rows	*/
	rowToDrop = row->nextRow;
	while ( rowToDrop ) {
		nextRow = rowToDrop->nextRow;

		DropTemplateValueTableColumn( rowToDrop->column );
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) rowToDrop );

		rowToDrop = nextRow;
	}

	DropTemplateValueTableColumn( row->column );
	sapdbwa_SQLFREE( (sapdbwa_UInt1*) row );

	return sapdbwa_True;

}

sapdbwa_Bool DropTemplateValueTableColumn ( sapdbwa_TemplateValueTableColumn	*column )
{
	const char FUNCTION_NAME  [] = "DropTemplateValueTableColumn";

	sapdbwa_TemplateValueTableColumn	*columnToDrop = NULL;
	sapdbwa_TemplateValueTableColumn	*nextColumn = NULL;

	/* If list is empty return	*/
	if ( !column ) return sapdbwa_True;

	/* Drop columns	*/
	columnToDrop = column->nextColumn;
	while ( columnToDrop ) {
		nextColumn = columnToDrop->nextColumn;

		if ( columnToDrop->value ) {
			sapdbwa_SQLFREE( (sapdbwa_UInt1*) columnToDrop->value );
		}
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) columnToDrop );

		columnToDrop = nextColumn;
	}

	if ( column->value ) {
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) column->value );
	}
	sapdbwa_SQLFREE( (sapdbwa_UInt1*) column );

	return sapdbwa_True;

}

sapdbwa_Bool AddValueToTemplateValueList ( sapdbwa_TemplateValueList	*valueList,
										   const char					*name,
										   const char					*value      )
{
	const char FUNCTION_NAME  [] = "AddValueToTemplateValueList";

	sapdbwa_TemplateValueListItem	*newListItem = NULL;
	sapdbwa_TemplateValueListItem	*currentListItem = NULL;
	sapdbwa_Bool					 ok = sapdbwa_False;

	if ( !valueList ) {
		/* Error handling	*/
		return sapdbwa_False;
	}

	/* Create list item	*/
	if ( !CreateTemplateValueListItem( &newListItem, name, value, NULL )) {
		/* Error handling	*/
		return sapdbwa_False;
	}

	/* Search end of list	*/
	if ( valueList->listItem ) {
		/* A list of items already exists	*/
		currentListItem = valueList->listItem;
		while ( currentListItem->nextListItem ) {
			currentListItem = currentListItem->nextListItem;
		}

		/* Add new list item at the end of list	*/
		currentListItem->nextListItem = newListItem;
	} else {
		/* The value list is empty, so insert as initial list item	*/
		valueList->listItem = newListItem;
	}

	return sapdbwa_True;

}

sapdbwa_Bool AddTableToTemplateValueList ( sapdbwa_TemplateValueList	*valueList,
										   const char					*name,
										   sapdbwa_TemplateValueTable	*table		)
{
	const char FUNCTION_NAME  [] = "AddTableToTemplateValueList";

	sapdbwa_TemplateValueListItem	*newListItem = NULL;
	sapdbwa_TemplateValueListItem	*currentListItem = NULL;
	sapdbwa_Bool					 ok = sapdbwa_False;

	if ( !valueList ) {
		/* Error handling	*/
		return sapdbwa_False;
	}

	/* Create list item	*/
	if ( !CreateTemplateValueListItem( &newListItem, name, NULL, table )) {
		/* Error handling	*/
		return sapdbwa_False;
	}

	/* Search end of list	*/
	if ( valueList->listItem ) {
		/* A list of items already exists	*/
		currentListItem = valueList->listItem;
		while ( currentListItem->nextListItem ) {
			currentListItem = currentListItem->nextListItem;
		}

		currentListItem->nextListItem = newListItem;
	} else {
		/* The value list is empty, so insert as initial list item	*/
		valueList->listItem = newListItem;
	}

	return sapdbwa_True;

}

sapdbwa_Bool AddRowToTemplateValueTable ( sapdbwa_TemplateValueTable	*table,
										  sapdbwa_TemplateValueTableRow	*row	)
{
	const char FUNCTION_NAME  [] = "AddRowToTemplateValueTable";

	sapdbwa_TemplateValueTableRow	*currentRow = NULL;
	sapdbwa_Bool					 ok = sapdbwa_False;

	if ( !table ) {
		/* Error handling	*/
		return sapdbwa_False;
	}

	/* Search end of list	*/
	if ( table->row ) {
		/* A list of rows already exists	*/
		currentRow = table->row;
		while ( currentRow->nextRow ) {
			currentRow = currentRow->nextRow;
		}

		/* Add new row at the end of row list	*/
		if ( currentRow == row ) {
			/* User error! Deadlock!	*/
			currentRow->nextRow = NULL;
		} else {
			currentRow->nextRow = row;
		}
	} else {
		/* The row list is empty, so insert as initial row	*/
		table->row = row;
	}

	return sapdbwa_True;

}

sapdbwa_Bool AddColumnToTemplateValueTableRow ( sapdbwa_TemplateValueTableRow		*row,
												sapdbwa_TemplateValueTableColumn	*column	)
{
	const char FUNCTION_NAME  [] = "AddColumnToTemplateValueTableRow";

	sapdbwa_TemplateValueTableColumn	*currentColumn = NULL;
	sapdbwa_Bool						 ok = sapdbwa_False;

	if ( !row ) {
		/* Error handling	*/
		return sapdbwa_False;
	}

	/* Search end of list	*/
	if ( row->column ) {
		/* A list of columns already exists	*/
		currentColumn = row->column;
		while ( currentColumn->nextColumn ) {
			currentColumn = currentColumn->nextColumn;
		}

		/* Add new column at the end of column list	*/
		if ( currentColumn == column ) {
			/* User error! Deadlock!	*/
			currentColumn->nextColumn = NULL;
		} else {
			currentColumn->nextColumn = column;
		}
	} else {
		/* The column list is empty, so insert as initial column	*/
		row->column = column;
	}

	return sapdbwa_True;

}

sapdbwa_Bool FindValueInTemplateValueList ( sapdbwa_TemplateValueList	 *valueList,
										    const char					 *name,
											char						**value      )
{
	const char FUNCTION_NAME  [] = "FindValueInTemplateValueList";

	sapdbwa_TemplateValueListItem	*currentListItem = NULL;

	/* Initial return value	*/
	*value = NULL;

	/* Start searching	*/
	currentListItem = valueList->listItem;
	while ( currentListItem ) {
		if ( strcmp(currentListItem->name, name) == 0 && currentListItem->value ) {
			/* Item found	*/
			*value = currentListItem->value;
			return sapdbwa_True;
		}

		/* Continue searching with next item	*/
		currentListItem = currentListItem->nextListItem;
	}

	return sapdbwa_True;

}

sapdbwa_Bool FindTableInTemplateValueList ( sapdbwa_TemplateValueList   *valueList,
										    const char				    *tableName,
											sapdbwa_TemplateValueTable **table      )
{
	const char FUNCTION_NAME  [] = "FindTableInTemplateValueList ";

	sapdbwa_TemplateValueListItem	*currentListItem = NULL;

	/* Initial return value	*/
	*table = NULL;

	/* Start searching	*/
	currentListItem = valueList->listItem;
	while ( currentListItem ) {
		if ( strcmp(currentListItem->name, tableName) == 0 && currentListItem->table ) {
			/* Item found	*/
			*table= currentListItem->table;

			return sapdbwa_True;
		}

		/* Continue searching with next item	*/
		currentListItem = currentListItem->nextListItem;
	}

	return sapdbwa_True;

}

sapdbwa_Bool WriteTemplate( sapdbwa_HttpReplyP			 rep,
						    const char					*templateName,
							sapdbwa_TemplateValueList	*templateValueList,
							twd26ErrP					 err				)
{
	const char FUNCTION_NAME  [] = "WriteTemplate";

	sapdbwa_Int4		 file = -1;
	tsp05_RteFileInfo    fInfo;
	tsp05_RteFileError	 fError;
	char				*buffer = NULL;
	tsp00_Longint			 readLen = 0;
	sapdbwa_Bool		 ok = sapdbwa_False;
	
	/* Determine file size	*/
	sqlfinfoc ( templateName, &fInfo, &fError );
	if ( fError.sp5fe_result != vf_ok ) {
		/* Error: Can not open file */

		return sapdbwa_False;
	}

	/* Allocat memory for buffer	*/
	sapdbwa_SQLALLOCAT( fInfo.sp5fi_size + 1, (sapdbwa_UInt1**) &buffer, &ok );
	if ( !ok ) {
		wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
		return sapdbwa_False;
	}; /* if */

	/* Open file for reading in binary mode */
	sqlfopenc( templateName, sp5vf_binary, sp5vf_read, sp5bk_buffered, &file, &fError );
	if ( fError.sp5fe_result != vf_ok ) {
		/* Error: Can not open file */

		sapdbwa_SQLFREE( (sapdbwa_UInt1*) buffer );

		return sapdbwa_False;
	}

	/* Read file into buffer	*/
	sqlfreadc ( file, buffer, fInfo.sp5fi_size, &readLen, &fError );
	buffer[readLen] = '\0';

	/* Close file	*/
	sqlfclosec( file, sp5vf_close_normal, &fError );

	/* Scan template, fill with values from value list and send as response	*/
	if ( readLen > 0 ) {
		if ( !SendTemplate( rep, templateValueList, buffer, err )) {
			/* Error handling	*/

			sapdbwa_SQLFREE( (sapdbwa_UInt1*) buffer );

			return sapdbwa_False;
		}
	}

	/* Free memory	*/
	sapdbwa_SQLFREE( (sapdbwa_UInt1*) buffer );

	return sapdbwa_True;

}

sapdbwa_Bool SendTemplate( sapdbwa_HttpReplyP			 rep,
						   sapdbwa_TemplateValueList	*valueList,
						   char							*templateStream,
						   twd26ErrP					 err			)
{
	const char FUNCTION_NAME  [] = "SendTemplate";

	#define MAX_VAR_NAME_LEN	128

	char				*templateVarBegin = NULL;
	char				*templateVarEnd = NULL;
	char				*templateTableBegin = NULL;
	char				*templateTableEnd = NULL;
	char				*bufferToScan = NULL;

	/* Checks	*/
	if ( !valueList || !templateStream ) {
		return sapdbwa_False;
	}

	/* Scan template */
	bufferToScan = templateStream;
	while ( bufferToScan ) {
		templateVarBegin = strstr( bufferToScan, "<!--§$WAVAL:" );
		templateTableBegin = strstr( bufferToScan, "<!--§$WATABLEBEGIN:" );

		if ( !templateTableBegin || (templateVarBegin && templateVarBegin < templateTableBegin )) {
			if ( templateVarBegin ) {
				/* Send template until variable part begins	*/
				sapdbwa_SendBody ( rep, bufferToScan, templateVarBegin - bufferToScan );

				/* Search end of variable part	*/
				templateVarEnd = strstr( templateVarBegin, "$§-->" );
				if ( !templateVarEnd ) {
					/* This is an error case. Variable begin should be closed by a variable end	*/
					sapdbwa_SendBody ( rep, templateVarBegin, 0 );

					return sapdbwa_False;

				}

				/* Write template variable	*/
				SendTemplateVariable( rep, valueList, templateVarBegin, templateVarEnd );

				/* Set next buffer to scan	*/
				bufferToScan = templateVarEnd + strlen( "$§-->" );
			} else {
				/* Send buffer like it is: no variables and no tables	*/
				if ( bufferToScan ) {
					if ( sapdbwa_SendBody ( rep, bufferToScan, 0 ) < 1) {
						/* Error: Sending reply */
						return sapdbwa_False;
					}
				}

				/* Set next buffer to scan	*/
				bufferToScan = NULL;
			}
		} else {
			/* Send template until variable part begins	*/
			sapdbwa_SendBody ( rep, bufferToScan, templateTableBegin - bufferToScan );

			/* Search template table end	*/
			templateTableEnd = strstr( templateTableBegin, "<!--§$WATABLEEND:" );
			if ( !templateTableEnd ) {
				/* This is an error case. Table begin should be closed by a table end	*/
				sapdbwa_SendBody ( rep, templateTableBegin, 0 );

				return sapdbwa_False;

			}
			/* Table handling	*/
			if ( !SendTemplateTable( rep, valueList, templateTableBegin, templateTableEnd, err )) {
				/* Error handling	*/
			}

			/* Find real table end	*/
			templateTableEnd = strstr( templateTableEnd, "$§-->" );
			if ( !templateTableEnd ) {
				/* Error Handling: Syntax error!	*/
				bufferToScan = NULL;
			} else {
				/* Set next buffer to scan	*/
				bufferToScan = templateTableEnd + strlen( "$§-->" );
			}
		}
	}

	return sapdbwa_True;

}

sapdbwa_Bool SendTemplateVariable( sapdbwa_HttpReplyP			 rep,
								   sapdbwa_TemplateValueList	*valueList,
								   char							*templateVariableBegin,
								   char							*templateVariableEnd   )
{
	const char FUNCTION_NAME  [] = "SendTemplateVariable";

	#define MAX_VAR_NAME_LEN	128

	char				 variableName[ MAX_VAR_NAME_LEN + 1 ];
	sapdbwa_UInt2		 variableNameLen = 0;
	char				*variableValue = NULL;

	/* Get variable name	*/
	variableNameLen = templateVariableEnd - templateVariableBegin - strlen("<!--§$WAVAL:");
	if ( variableNameLen >= MAX_VAR_NAME_LEN )
		variableNameLen = MAX_VAR_NAME_LEN;

	strncpy( variableName, templateVariableBegin + strlen("<!--§$WAVAL:"), variableNameLen );
	variableName[variableNameLen] = '\0';

	/* Get variable value	*/
	FindValueInTemplateValueList ( valueList, variableName, &variableValue );

	/* Send variable value	*/
	if ( variableValue ) {
		sapdbwa_SendBody ( rep, variableValue, 0 );
	}

	return sapdbwa_True;

}

sapdbwa_Bool SendTemplateTable( sapdbwa_HttpReplyP			 rep,
								sapdbwa_TemplateValueList	*valueList,
								char						*tableBegin,
								char						*tableEnd,
								twd26ErrP					 err		)
{
	const char FUNCTION_NAME  [] = "SendTemplateTable";

	#define MAX_TABLE_NAME_LEN	128

	char							*tableRow = NULL;
	char							*tableNameEnd = NULL;
	char							 tableName[ MAX_VAR_NAME_LEN + 1 ];
	sapdbwa_UInt2					 tableNameLen = 0;
	sapdbwa_UInt2					 tableLen = 0;
	sapdbwa_TemplateValueTable		*table = NULL;
	sapdbwa_TemplateValueTableRow	*currentTableRow = NULL;
	sapdbwa_Bool					 ok = sapdbwa_False;

	/* Get table name end pos	*/
	tableNameEnd = strstr( tableBegin, "$§-->" );
	if ( !tableNameEnd ) {
		/* Error Handling: Syntax error	*/
		return sapdbwa_False;
	}

	/* Determine table name length	*/
	tableNameLen = tableNameEnd - tableBegin - strlen("<!--§$WATABLEBEGIN:");
	if ( tableNameLen >= MAX_TABLE_NAME_LEN )
		tableNameLen = MAX_TABLE_NAME_LEN;

	/* Get table name	*/
	strncpy( tableName, tableBegin + strlen("<!--§$WATABLEBEGIN:"), tableNameLen );
	tableName[tableNameLen] = '\0';

	/* Determine table length	*/
	tableLen = tableEnd - tableBegin - strlen("<!--§$WATABLEBEGIN:") - tableNameLen - strlen("$§-->");

	/* Allocat memory for table row	*/
	sapdbwa_SQLALLOCAT( tableLen + 1, (sapdbwa_UInt1**) &tableRow, &ok );
	if ( !ok ) {
		wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
		return sapdbwa_False;
	};

	/* Get table row	*/
	strncpy( tableRow, tableBegin + strlen("<!--§$WATABLEBEGIN:") + tableNameLen + strlen("$§-->"), tableLen );
	tableRow[tableLen] = '\0';

	/* Find table in value list	*/
	FindTableInTemplateValueList ( valueList, tableName, &table );

	/* Send table rows	*/
	if ( table ) {
		currentTableRow = table->row;
		while ( currentTableRow ) {
			SendTemplateTableRow( rep, currentTableRow, tableRow );

			currentTableRow = currentTableRow->nextRow;
		}
	} else {
		/* Error handling: Table not found!	*/
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) tableRow );

		return sapdbwa_False;
	}

	sapdbwa_SQLFREE( (sapdbwa_UInt1*) tableRow );

	return sapdbwa_True;

}

sapdbwa_Bool SendTemplateTableRow( sapdbwa_HttpReplyP				 rep,
								   sapdbwa_TemplateValueTableRow	*row,
								   char								*templateRow )
{
	const char FUNCTION_NAME  [] = "SendTemplateTableRow";

	char								*columnPos = NULL;
	char								*nextColumnPos = NULL;
	sapdbwa_TemplateValueTableColumn	*currentColumn = NULL;

	char				 variableName[ MAX_VAR_NAME_LEN + 1 ];
	sapdbwa_UInt2		 variableNameLen = 0;
	char				*variableValue = NULL;

	/* Get column position	*/
	columnPos = strstr( templateRow, "<!--§$WATABLECOL$§-->" );

	/* No columns or syntax error	*/
	if ( !columnPos ) {
		sapdbwa_SendBody ( rep, templateRow, 0 );
		return sapdbwa_True;
	}

	/* Determine first column	*/
	currentColumn = row->column;

	/* Send static part of template	*/
	sapdbwa_SendBody ( rep, templateRow, columnPos - templateRow );

	/* Set next search start for next column	*/
	columnPos = columnPos + strlen("<!--§$WATABLECOL$§-->");

	/* Insert column values	*/
	while ( columnPos && currentColumn && currentColumn->value ) {
		/* Send column value	*/
		sapdbwa_SendBody ( rep, currentColumn->value, 0 );

		/* Determine next column	*/
		currentColumn = currentColumn->nextColumn;

		/* Determine next column pos in template	*/
		nextColumnPos = strstr( columnPos, "<!--§$WATABLECOL$§-->");

		/* Send static part between to columns	*/
		if ( nextColumnPos ) {
			/* There is another column	*/
			sapdbwa_SendBody ( rep, columnPos, nextColumnPos - columnPos );

			/* Set next search start for next column	*/
			columnPos = nextColumnPos + strlen("<!--§$WATABLECOL$§-->");
		} else {
			/* There are no more columns	*/
			sapdbwa_SendBody ( rep, columnPos, 0 );

			/* There is nothing more to send	*/
			columnPos = NULL;
		}
	}

	return sapdbwa_True;

}

/*===========================================================================*/

void wd20_StrCatMax( char           *destination,
                     const char     *source,
                     sapdbwa_UInt4   maxLen )
{

    if (strlen(destination) + strlen(source) <= maxLen) {
	    strcat (destination, source);
    }

}

/*===========================================================================*/

sapdbwa_Bool wd20_EmptyLog( char            *filename,
                            tsp00_Int4        *file,
                            twd27ExclP       fileExcl,
                            sapdbwa_Bool     makeCopy )
{

    tsp00_Int4    backupFile;
    char        backupFilename[1024] = "";
    tsp00_Longint readLen = 0;
    char        buffer[1024] = "";

    tsp00_Bool		    bOk;
    tsp05_RteFileError	err;

    if (!file || !*file || !fileExcl)
        return sapdbwa_False;

    wd27BegExcl(fileExcl);

    sqlfclosec(*file, sp5vf_close_normal, &err);

    sqlfopenc(filename, sp5vf_binary, sp5vf_read, sp5bk_unbuffered, file, &err);
 
    if (makeCopy == sapdbwa_True) {
        sp77sprintf(backupFilename, 1024, "%s.bak", filename);

        /* Copy log file */
        sqlfopenc(backupFilename, sp5vf_binary, sp5vf_write, sp5bk_unbuffered, &backupFile, &err);
        if (backupFile) {
            do {
                sqlfreadc(*file, buffer, 1024, &readLen, &err);
                if (readLen > 0) {
                    sqlfwritec(backupFile, buffer, readLen, &err);
                }
            } while (readLen == 1024);

            sqlfclosec(backupFile, sp5vf_close_normal, &err);
        }
    }

    sqlfclosec(*file, sp5vf_close_normal, &err);

    /* Empty log file */
    sqlfopenc(filename, sp5vf_binary, sp5vf_write, sp5bk_unbuffered, file, &err);
    sqlfclosec(*file, sp5vf_close_normal, &err);

    sqlfopenc(filename, sp5vf_binary, sp5vf_write, sp5bk_unbuffered, file, &err);

    wd27EndExcl(fileExcl);

    return sapdbwa_True;

}

/*===========================================================================*/

sapdbwa_Bool wd20_IsLogFileEmpty( char  *filename )
{

    tsp05_RteFileInfo   fileInfo;
    tsp05_RteFileError  err;

    sqlfinfoc (filename, &fileInfo, &err);

    if (fileInfo.sp5fi_size > 0) {
        return sapdbwa_False;
    } else {
        return sapdbwa_True;
    }
 
}

/*===========================================================================*/

#ifdef WIN32

void LogException( char const * const str, 
                   void * const       pOutContext )
{

    twd25LogP log = (twd25LogP) pOutContext;
    wd25WriteLnLogMsg(log, (char*)str);

}

/*===========================================================================*/

DWORD WriteException(LPEXCEPTION_POINTERS ec, twd25LogP log)
{
    eo670TraceContext traceContext;

    traceContext.threadHandle   = GetCurrentThread();
    traceContext.pThreadContext = ec->ContextRecord;
    eo670_CTraceContextStackOCB(&traceContext, LogException, log);

    return EXCEPTION_CONTINUE_SEARCH;
}

#endif

/*===========================================================================*/
