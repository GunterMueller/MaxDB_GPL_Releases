/*!**********************************************************************

  module: hwd20control.h

  -----------------------------------------------------------------------

  
  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Initialisation and control functions

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1998-2005 SAP AG-2000



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#ifndef HWD20CONTROL_H
#define HWD20CONTROL_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd05wases.h"
#include "hwd21req.h"
#include "hwd22rep.h"
#include "hwd26err.h"

/* kind of web server */
typedef enum {
    API_NSAPI_WD20,
    API_IIS_WD20,
    API_APACHE_WD20,
    API_APACHE_2_WD20,
    API_CGI_WD20,
    API_FCGI_WD20,
    API_WASIM_WD20,
    API_DBFS_WD20,
    API_TESTAPI_WD20,
	API_SAPDBWWW_WD20
} twd20ApiType;


#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: API control functions

  description: These are the top-level funtions of the WebAgent for
               initialisation (reading of ini-file etc.), destruction
               and calling the UserDll-service function.
  
************************************************************************/

/*!**********************************************************************
  Function: wd20ApiInit

  description: Initialize the WebAgent.
  
  This function reads the WebAgent.ini file, builds up the sessionpools
  and other datastructures that persist during the lifetime of the
  WebAgent and loads the UserDll's specified in the ini file. After
  loading a UserDll the UserDll's initialisation-function is called.
  Any kind of informational or error messages are written to the log file
  specified in WebAgent.ini.

  This function is typically called once from the WebServer
  specific initialization function (GetHttpFilterVersion for MS, ...).

  arguments:
    iniFile  [in]  The name (including absolute path) of the ini file.
                   If this is NULL the file WebAgent.ini from the winnt
                   directory is used.
                    
  return value: true if initialization was successful and false if not.
  */

sapdbwa_Bool wd20ApiInit( const char *iniFile, twd20ApiType  apiType );


/*!**********************************************************************
  Function: wd20ApiDBFSInit

  description: Initialize the WebAgent for DBFS command-line utils.

  This function reads the [DBFS] section of the WebAgent.ini file and
  builds up a sessionpool with the given parameters.

  This function is called by the command-line utilities dbformat, dbcat,
  dbmkdir, dbcpin, dbcpout, ...

  arguments:
    err  [out]  In the case of an error this is set.
    
  return value: The sessionpool used
  */

sapdbwa_SessionPoolP wd20ApiDBFSInit( twd26ErrP err );


/*!**********************************************************************
  Function: wd20ApiExit

  description: Free resources of the WebAgent.

  The UserDll's are unloaded and dynamically allocated memory is freed.
  This function is called by the DllMain-function (vwd61dllmain.c) when
  the WebAgent DLL is unloaded.

  arguments:
    
  return value: void
  */
void wd20ApiExit();


/*!**********************************************************************
  Function: wd20ApiDBFSExit

  description: Free resources of the WebAgent.

  Dynamically allocated memory is freed and the sessionpool for DBFS
  command-line utilities is destroyed.

  arguments:
    
  return value: void
  */

void wd20ApiDBFSExit();


/*!**********************************************************************
  Function: wd20CallServiceFunc

  description: Call service function in UserDll.
                
  First the URI of the request is used to determine the UserDll that
  has to handle this request (each UserDll handles requests with a
  specific path-prefix). If no such UserDll exists sapdbwa_RTC_IGNORED is
  returned else the service function in the UserDll is called with
  req, rep as arguments and the return code of this call is returned.

  This function is called by the Web-Server specific filter-/service
  -function that is called by the Web-Server for incoming requests.
  The request- (req) and reply-handles (rep) have to be build up
  by this server specific function (HttpFilterProc for MS).

  arguments:
    req  [in]  Description of the incoming request.
    rep  [in]  Reply handle used by the service function to send data
               (header and body) back to the webserver.
               
  return value: <UL>
                <LI> sapdbwa_RTC_ERROR: An error occured while handling the
                     request.
                <LI> sapdbwa_RTC_IGNORED: No UserDll was configured to handle
                     this request or the UserDll service function
                     returned sapdbwa_RTC_IGNORED.
                <LI> sapdbwa_RTC_ACCEPTED: The UserDll service function
                     returned sapdbwa_RTC_ACCEPTED.
                </UL>
  */
sapdbwa_Int2 wd20CallServiceFunc( twd21HttpRequestP req,
                                sapdbwa_HttpReplyP     rep );


/*!**********************************************************************
  Function: wd20CallSessionDataDestr
      
  description: Call destructor function for session-data in WebSession.

  arguments:
  serviceId  [in]  Id of service that implements the destructor.
  destructor [in]  Destructor function of service for the UserData.
  data       [in]  Pointer to the data to destroy.
                       
  return value: true for success, false for failure.
*/    
sapdbwa_Bool wd20CallSessionDataDestr( sapdbwa_UInt4    serviceId,
                                     sapdbwa_Destructor  destructor,
                                     void          *data );

    
/*!**********************************************************************
  Function: wd20GetParam
      
  description: Get parameter from the WebAgent.ini file.

  arguments:
  section		[in]  The section of the parameter.
  key			[in]  The name of the parameter to retrieve.
  valueBuf		[out] Buffer for the parameter value.
  bufLen		[in]  Length of valueBuf.
  isCOMService	[in]  True if searched parameter is an COM service parameter
    
  return value:
  true if the param exists and is not empty, else false.
*/
sapdbwa_Bool wd20GetParam( const char *section,
                         const char *key,
                         char       *valueBuf,
                         sapdbwa_Int4      bufLen,
						 sapdbwa_Bool	 isCOMService );

    
/*!**********************************************************************
  Function: wd20WriteLogMsg

  description: Write log message to the configured log file.

  This function can be used by the Web-Server specific functions
  (vwd51ns, vwd52ms, vwd53ap) to write log messages.

  arguments:
    msg   [in]  The log message to write.
    
  return value: void
  */
void wd20WriteLogMsg( char *msg );


/*!**********************************************************************
  Function: wd20GetMIMEType

  description: Get the MIME type to an according file extension .

  arguments:
    extension [in]  The extension of the file.
    
  return value: The MIME type according to an extension.
  */
const char* wd20GetMIMEType( const char *extension );


/*!**********************************************************************
  Function: wd20ServiceStart

  description:	Updates the service property serviceStart in the registry
				with given start value.

  arguments:
    serviceName [in]	Name of the service to start
    start		[in]	True if service should be started next time,
						False if service should not be started next time.
    
  return value: True if service property is updated otherwise false.
  */
sapdbwa_Bool wd20ServiceStart( const char	*serviceName,
							 sapdbwa_Bool	 start	      );


/*!**********************************************************************
  Function: wd20GetDocumentRoot

  description: Returns the WebAgent root document folder

  arguments:
  valueBuf  [out] Buffer for the folder.
  bufLen    [in]  Length of buffer.

  return value:
  true if folder exists, otherwise false
*/
sapdbwa_Bool wd20GetDocumentRoot( char         *valueBuf,
							      sapdbwa_Int4  bufLen    );


/*!**********************************************************************

  EndChapter: API control functions

************************************************************************/
#if defined(__cplusplus)
}
#endif

#endif
