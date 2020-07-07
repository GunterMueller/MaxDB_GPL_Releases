/*!**********************************************************************

  module: hwd96wacom.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  COM functions

  see also:

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef HWD96WACOM_H
#define HWD96WACOM_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd03waapi.h"
#include "hwd21req.h"
#include "hwd22rep.h"
#include "hwd26err.h"
#include "hwd30ses.h"

#if defined(__cplusplus)
extern "C" {
#endif

    
sapdbwa_Bool wd96ComInit();

void wd96ComExit();

/*!**********************************************************************

  Chapter: twd96ComClassP

  description: This type is used to handle ComClasses. A ComClass is
               configured in the WebAgent.ini file. The ComClass has to
               implement the interface WebAgentCOM (see vwd95wacom.idl).
  
************************************************************************/

typedef struct st_com_class *twd96ComClassP;

/*!**********************************************************************
  Function: wd96CreateComClass

  description: Create new ComClass description.

  arguments:
    name  [in]  The logical name of the ComClass. This is the unique
                name from the WebAgent.ini file.
    id    [in]  A unique id for the ComClass.
    err   [out] If the function returnes NULL the error type and
                message are set in err.
    
  return value: A pointer to the created ComClassP or NULL if the
                creation failed because of insufficient memory resources
                or if the name is too long. */

twd96ComClassP wd96CreateComClass( char        *name,
                                   sapdbwa_UInt4  id,
                                   twd26ErrP    err );


/*!**********************************************************************
  Function: wd96DestroyComClass

  description: Free resources of the comClass.

  arguments:
    comClass  [inout]  Pointer to comClass that should be freed.
    
  return value: void

  NOTE: The comClass is NOT unloaded before it is destroyed. This should
        be done with wd96UnloadcomClass before destroying it. */

void wd96DestroyComClass( twd96ComClassP comClass );


/*!**********************************************************************
  Function: wd96CallExitFunc

  description: Call the exit function in the comClass.

  arguments:
    comClass  [in]  Pointer to the comClass
    wa       [in]  sapdbwa_Handle that is passsed to the exit
                   function in the comClass.
    exitRet  [out] Return code of the exit function.
    log      [in]  If an exception occurs a message is written to this
                   log.
    err      [out] If the function returnes false the error type and
                   message are set in err.
                   
  return value: true if the comClass-exit-function finished. false if
                the comClass-init-function could not be called or if it
                returned throwed an exception or if it returned
                sapdbwa_RTC_ERROR. */

sapdbwa_Bool wd96CallExitFunc( twd96ComClassP  comClass,
                             sapdbwa_Handle       wa,
                             sapdbwa_Int2     *exitRet,
                             twd25LogP       log,
                             twd26ErrP       err      );


/*!**********************************************************************
  Function: wd96CallInitFunc

  description: Call the initialization function in the comClass.

  arguments:
    comClass  [in]  Pointer to the comClass
    wa       [in]  sapdbwa_Handle that is passsed to the initialization
                   function in the comClass.
    initRet  [out] Return code of the init function.
    log      [in]  If an exception occurs a message is written to this
                   log.
    err      [out] If the function returnes false the error type and
                   message are set in err.
                   
  return value: true if the comClass-init-function finished. false if
                the comClass-init-function could not be called or if it
                throwed an exception or if it returned sapdbwa_RTC_ERROR.*/

sapdbwa_Bool wd96CallInitFunc( twd96ComClassP  comClass,
                             sapdbwa_Handle       wa,
                             sapdbwa_Int2     *initRet,
                             twd25LogP       log,
                             twd26ErrP       err );


/*!**********************************************************************
  Function: wd96CallServiceFunc

  description: Call the service function in the comClass.

  arguments:
    comClass [in]    Pointer to the comClass
    wa       [in]    sapdbwa_Handle that is passsed to the service
                     function in the comClass.
    request  [in]    Description of the incoming http-request.
    reply    [inout] Reply-handle used to send data to the webserver.
    svcRet   [out]   Return-code of the service function (sapdbwa_RTC_IGNORED,
                     sapdbwa_RTC_ACCEPTED or sapdbwa_RTC_ERROR).
    log      [in]    If an exception occurs a message is written to this
                     log.
    err      [out]   Error information if function returns false.
    
  return value: true if the service functions finished. false if the
                service function could not be called or throwd an
                exception or if it returned sapdbwa_RTC_ERROR. If the return
                code is false err contains the reason for the failure. */

sapdbwa_Bool wd96CallServiceFunc( twd96ComClassP   comClass,
                                sapdbwa_Handle        wa,
                                sapdbwa_HttpRequestP  request,
                                sapdbwa_HttpReplyP    reply,
                                sapdbwa_Int2      *svcRet,
                                twd25LogP        log,
                                twd26ErrP        err );


/* Get configuration of com-class */
sapdbwa_Bool wd96GetConf( twd96ComClassP  comClass,
                        char           *textBuffer,
                        sapdbwa_UInt4    *bufferLen );


/*!**********************************************************************
  Function: wd96GetPathPrefix

  description: Return the path-prefix that describes which reuqests are
               handled by the comClass.

  arguments:
    comClass  [in]    Pointer to the comClass

  return value: Pointer to path-prefix. */
  
char* wd96GetPathPrefix( twd96ComClassP comClass );


/*!**********************************************************************
  Function: wd96GetComClassId

  description: Return the unique id of the comClass.

  arguments:
    comClass  [in]    Pointer to the comClass

  return value: The id of the comClass. */

sapdbwa_UInt4 wd96GetComClassId( twd96ComClassP comClass );


/*!**********************************************************************
  Function: wd96GetComClassName

  description: Return the logical name of the comClass.

  arguments:
    comClass  [in]    Pointer to the comClass

  return value: Pointer to the name. */

char* wd96GetComClassName( twd96ComClassP comClass );

/*!**********************************************************************
  Function: wd96InitComClass

  description: Set properties of the comClass.

  arguments:
    comClass     [in]    Pointer to the comClass
    pathPrefix  [in]    The pathPrefix that describes which requests are
                        handled by this comClass.
    dllName     [in]    The filename (including the absolute path) of
                        the comClass's dynamic link library.
    sessionPool [in]    Pointer to the session-pool that should be used
                        by the comClass.
    initFunc    [in]    Name of the initialization function in the
                        comClass.
    serviceFunc [in]    Name of the service-function in the comClass.
    err         [out]   When an error occurs the error code and message
                        are stored in err.
                        
  return value: true if the comClass could be initialized false if not.
                If false is returned the error information can be
                retrieved from err. */

sapdbwa_Bool wd96InitComClass( twd96ComClassP     comClass,
                             char              *pathPrefix,
                             char              *classId,
                             char              *logFile,
                             twd26ErrP          err );


/*!**********************************************************************
  Function: wd96WriteLogMsg

  description: Writes a log message to the file that was configured in
               webagent.ini file.

  arguments:
    comClass  [in]    Pointer to the comClass
    msg       [in]    The message that should be written to the log file.
    
  return value: true if the message could be written false else.*/

sapdbwa_Bool wd96WriteLogMsg( twd96ComClassP comClass, const char *msg );

#if defined(__cplusplus)
}
#endif

/*!**********************************************************************

  EndChapter: twd96ComClassP

************************************************************************/
#endif
