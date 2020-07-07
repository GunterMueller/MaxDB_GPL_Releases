/*


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




*/

/*! 
  -----------------------------------------------------------------------------
 
  module: hcn14.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV, SteffenS

  special area: Database Manager-API
 
  description:  API for Database Manager Client Applications
                
                This is the API for Database Manager Client Applications for executing
                DBMServer commands. About DBMServer commands (the contents of the 
                packets mentioned below) see 
                <a href="http://p26326/Tools/Control/do_bvcn1.doc">Database Manager (CLI)</a>.
                Use this API by linking vcn14.cpp to your application (all platforms) or
                by using dbmapi.dll (NT only).
                

  version:      7.3.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------
*/

#ifndef _HCN14_H_
#define _HCN14_H_

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */
#if defined (_WIN32) && !defined(NOWINAPI)
  #include <windows.h>
  #define WINAPI_CN14 WINAPI
#else
  #define WINAPI_CN14
#endif

#include "gsp00.h"
#include "geo47.h"

/* ------------------------------------------------------------------
 * types, constants and some stuff
 * ------------------------------------------------------------------
 */

/*! Declaration: DBMAPI-Constants */

#define DBMAPI_OK_CN14           ((tsp00_Int4)    0)    /* everything works fine */
#define DBMAPI_USR_UNKNOWN_CN14  ((tsp00_Int4)   -1)    /* user not exist */
#define DBMAPI_USR_FALSE_CN14    ((tsp00_Int4)   -2)    /* wrong username password combination */
#define DBMAPI_MEMORY_CN14       ((tsp00_Int4)   -3)    /* memory error */
#define DBMAPI_COMMERR_CN14      ((tsp00_Int4)   -4)    /* tsp01_CommErr is set */
#define DBMAPI_TOSMALL_CN14      ((tsp00_Int4)   -5)    /* communication buffer to small */
#define DBMAPI_INVSESSION_CN14   ((tsp00_Int4)   -6)    /* invalid session */
#define DBMAPI_NOMOREDATA_CN14   ((tsp00_Int4)   -7)    /* no more data in packet */
#define DBMAPI_RELEASED_CN14     ((tsp00_Int4)   -8)    /* session released */
#define DBMAPI_XUSERKEY_CN14     ((tsp00_Int4)   -9)    /* user key too long */
#define DBMAPI_XUSER_CN14        ((tsp00_Int4)  -10)    /* xuser API error */
#define DBMAPI_R3TP_CN14         ((tsp00_Int4)  -11)    /* R/3 TP error */
#define DBMAPI_USC2_CN14         ((tsp00_Int4)  -12)    /* USC2 to ASCII conversion error */
#define DBMAPI_URI_CN14          ((tsp00_Int4)  -13)    /* URI calculation error */
#define DBMAPI_NULLPARAM_CN14    ((tsp00_Int4)  -14)    /* URI calculation error */
#define DBMAPI_NOT_OK_CN14       ((tsp00_Int4) -100)    /* common error */

#define DBMAPI_USE_OUTURI_CN14_NO   ((tsp00_Int4) 0)
#define DBMAPI_USE_OUTURI_CN14_YES  ((tsp00_Int4) 1)

#define DBMAPI_ENCRYPTION_SSL_AVAILABLE_NO   ((tsp00_Int4) 0)
#define DBMAPI_ENCRYPTION_SSL_AVAILABLE_YES  ((tsp00_Int4) 1)

/*! EndDeclaration: DBMAPI-Constants */


/* Default DBMServer Program */
#define DEFAULT_DBMPGM_CN14      ((_TCHAR *)   _T("dbmsrv"))

/* Default Replication Manager Server Program */
#define DEFAULT_RPMPGM_CN14      ((_TCHAR *)   _T("lserver"))

/* Version independence switch for Loader */
#define RPM_VERSION_INDEPENDENCE

#define RPM_PROTOCOL_SIZE_CN14       4*sizeof(char) /*This corresponds to PROTOCOL_SIZE_LS00*/
#define RPM_ERR_PROTOCOL_SIZE_CN14   4*sizeof(char) /*This corresponds to ProtHeaderLength_ls00*/
#define RPM_REPLY_BUFFER_LEN_CN14    1024           /*This corresponds to MAX_REPLY_LENGTH_LS00 in gls00.h*/

/*!  
  Chapter: Connecting and disconnecting
 */

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14connectDBM

  -----------------------------------------------------------------------------

  description:  connect to DBMServer
  
                This function connects the client application with the specified 
                server.

                This function is UCS2 enabled for the usage as dbmapi.dll with 
                the DBMGui.

  arguments:    servernode [IN] - A pointer to a zero terminated string with the
                                  name of the server node. This string may be empty
                                  or the name of the local host for special local 
                                  communication. For real local TCP/IP communication 
                                  this string should be set to "localhost".
                dbname [IN]     - A pointer to a zero terminated string with the
                                  database name. This string maybe empty. If a 
                                  database ist spedcified the DBMServer receives this
                                  name as a special argument. The XServer checks the
                                  version of the database and starts the correct
                                  DBMServer.
                dbroot [IN]     - A pointer to a zero terminated string with an 
                                  installation path. The DBMServer at this path will
                                  be used. If there is a dbname specified, this 
                                  parameter will be ignored.
                sessionOUT [OUT]- A pointer to a buffer for the session pointer. 
                                  The returned pointer should be used for any 
                                  subsequent DBM-API call.
                errtext [OUT]   - A pointer to a buffer where a description is
                                  available in case of errors.
                                  Caller must provide enough memory (at least for a tsp00_ErrTextc)

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 WINAPI_CN14 cn14connectDBM (
    const char*               servernode, 
    const char*               dbname,     
    const char*               dbroot,     
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext
    );

externC tsp00_Int4 cn14connectDBM_fixedSizeParameters (
    const tsp00_NodeIdc       VAR_ARRAY_REF servernode, 
    const tsp00_DbNamec       VAR_ARRAY_REF dbname,     
    const tsp00_VFilenamec    VAR_ARRAY_REF dbroot,     
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext     
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14encryptionMethodSSLAvailable

  -----------------------------------------------------------------------------

  description:  Check, whether encryption method SSL is available

  arguments:    errtext [OUT]   - A pointer to a buffer where a description is
                                  available in case of errors
                                  Caller must provide enough memory (at least for a tsp00_ErrTextc)

  returnvalue:  1, if SSL is available
                0, if SSL is not available
                negative value, if the check failed

                If returnvalue is != 1, the errtext contains an error message
*/
externC tsp00_Int4 WINAPI_CN14 cn14encryptionMethodSSLAvailable( char* errtext );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14connectDBMUsr

  -----------------------------------------------------------------------------

  description:  connect to DBMServer with user
  
                This function connects the client application with the specified 
                server and executes a authorization with the user

                This function is UCS2 enabled for the usage as dbmapi.dll with 
                the DBMGui.

  arguments:    servernode [IN] - A zero terminated string with the
                                  name of the server node. This string may be empty
                                  or the name of the local host for special local 
                                  communication. For real local TCP/IP communication 
                                  this string should be set to "localhost".
                dbname [IN]     - A zero terminated string with the
                                  database name. This string maybe empty. If a 
                                  database ist spedcified the DBMServer receives this
                                  name as a special argument. The XServer checks the
                                  version of the database and starts the correct
                                  DBMServer.
                dbroot [IN]     - A zero terminated string with an 
                                  installation path. The DBMServer at this path will
                                  be used. If there is a dbname specified, this 
                                  parameter will be ignored.
                user   [IN]     - A zero terminated string with the username and
                                  password (username","password")
                sessionOUT [OUT]- A pointer to a buffer for the session pointer. 
                                  The returned pointer should be used for any 
                                  subsequent DBM-API call.
                errtext [OUT]   - A pointer to a buffer where a description is
                                  available in case of errors
                                  Caller must provide enough memory (at least for a tsp00_ErrTextc)

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 WINAPI_CN14 cn14connectDBMUsr (
    const char*               servernode, 
    const char*               dbname,     
    const char*               dbroot,     
    const char*               user,     
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext
    );

externC tsp00_Int4 cn14connectDBMUsr_fixedSizeParameters (
    const tsp00_NodeIdc       VAR_ARRAY_REF servernode, 
    const tsp00_DbNamec       VAR_ARRAY_REF dbname,     
    const tsp00_VFilenamec    VAR_ARRAY_REF dbroot,     
    const tsp00_C64c          VAR_ARRAY_REF user,     
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext     
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14connectDBMTp

  -----------------------------------------------------------------------------

  description:  connect to DBMServer with getting user data from R/3 tp
  
                This function connects the client application with the specified 
                server and executes a authorization with the user provided
                by tp

  arguments:    servernode [IN] - A zero terminated string with the
                                  name of the server node. This string may be empty
                                  or the name of the local host for special local 
                                  communication. For real local TCP/IP communication 
                                  this string should be set to "localhost".
                dbname [IN]     - A zero terminated string with the
                                  database name. This string maybe empty. If a 
                                  database ist spedcified the DBMServer receives this
                                  name as a special argument. The XServer checks the
                                  version of the database and starts the correct
                                  DBMServer.
                dbroot [IN]     - A zero terminated string with an 
                                  installation path. The DBMServer at this path will
                                  be used. If there is a dbname specified, this 
                                  parameter will be ignored.
                sysid   [IN]    - A zero terminated string with the R/3 system id
                conn    [IN]    - A zero terminated string with the R/3 connection id
                profile [IN]    - A zero terminated string with the R/3 tp profile
                sessionOUT [OUT]- A pointer to a buffer for the session pointer. 
                                  The returned pointer should be used for any 
                                  subsequent DBM-API call.
                errtext [OUT]   - A pointer to a buffer where a description is
                                  available in case of errors

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4  cn14connectDBMTp (
    const char*               servernode, 
    const char*               dbname,     
    const char*               dbroot,     
    const char*               sysid,     
    const char*               conn,     
    const char*               profile,
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext     
    );

externC tsp00_Int4  cn14connectDBMTp_fixedSizeParameters (
    const tsp00_NodeIdc       VAR_ARRAY_REF servernode, 
    const tsp00_DbNamec       VAR_ARRAY_REF dbname,     
    const tsp00_VFilenamec    VAR_ARRAY_REF dbroot,     
    const tsp00_C8c           VAR_ARRAY_REF sysid,     
    const tsp00_C30c          VAR_ARRAY_REF conn,     
    const tsp00_Pathc         VAR_ARRAY_REF profile,
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext     
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14connectRPM

  -----------------------------------------------------------------------------

  description:  connect to Replication Manager Server
  
                This function connects the client application with the specified 
                server.

  arguments:    servernode [IN] - A pointer to a zero terminated string with the
                                  name of the server node. This string may be empty
                                  or the name of the local host for special local 
                                  communication. For real local TCP/IP communication 
                                  this string should be set to "localhost".
                dbname [IN]     - A pointer to a zero terminated string with the
                                  database name. This string maybe empty. If a 
                                  database ist spedcified the DBMServer receives this
                                  name as a special argument. The XServer checks the
                                  version of the database and starts the correct
                                  DBMServer.
                dbroot [IN]     - A pointer to a zero terminated string with an 
                                  installation path. The DBMServer at this path will
                                  be used. If there is a dbname specified, this 
                                  parameter will be ignored.
                pszData [OUT]   - contains return string (result of set-command)
                                  if not set to NULL; caller must provide enough
                                  space
                sessionOUT [OUT]- A pointer to a buffer for the session pointer. 
                                  The returned pointer should be used for any 
                                  subsequent DBM-API call.
                errtext [OUT]   - A pointer to a buffer where a description is
                                  available in case of errors

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4  cn14connectRPM (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    char                    * pszData,
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext
    );

externC tsp00_Int4  cn14connectRPM_fixedSizeParameters (
    const tsp00_NodeIdc       VAR_ARRAY_REF servernode,
    const tsp00_DbNamec       VAR_ARRAY_REF dbname,
    const tsp00_VFilenamec    VAR_ARRAY_REF dbroot,
    char                    * pszData,
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14connect

  -----------------------------------------------------------------------------

  description:  connect to server
  
                This function connects the client application with the specified 
                server. There is a special parameter for specifying a server 
                program.

  arguments:    servernode [IN] - A pointer to a zero terminated string with the
                                  name if the server node. This string may be empty
                                  or the name of the local host for special local 
                                  communication. For local TCP/IP communication this
                                  string should be set to "localhost".
                dbname [IN]     - A pointer to a zero terminated string with the
                                  database name. This string maybe empty. If a 
                                  database ist spedcified the DBMServer receives this
                                  name as a special argument. The XServer checks the
                                  version of the database and starts the correct
                                  DBMServer.
                dbroot [IN]     - A pointer to a zero terminated string with an 
                                  installation path. The DBMServer at this path will
                                  be used. If there is a dbname specified, this 
                                  parameter will be ignored.
                serverpgm [IN]  - A pointer to a zero terminated string with the 
                                  server program. Instead of the default (dbmsrv.exe)
                                  this program will be started at the server node.
                sessionOUT [OUT]- A pointer to a buffer for the session pointer. 
                                  The returned pointer should be used for any 
                                  subsequent DBM-API call.
                errtext [OUT]   - A pointer to a buffer where a description is
                                  available in case of errors

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4  cn14connect (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    const char*               serverpgm,
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext
    );

externC tsp00_Int4  cn14connect_fixedSizeParameters (
    const tsp00_NodeIdc       VAR_ARRAY_REF servernode,
    const tsp00_DbNamec       VAR_ARRAY_REF dbname,
    const tsp00_VFilenamec    VAR_ARRAY_REF dbroot,
    const tsp00_VFilenamec    VAR_ARRAY_REF serverpgm,
    void                   ** sessionOut,
    tsp00_ErrTextc            VAR_ARRAY_REF errtext
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14release

  -----------------------------------------------------------------------------

  description:  disconnect from server
  
                This function disconnects the client application from the server 
                program an terminateds the server program.

  arguments:    session [INOUT] - A pointer to a buffer with the session pointer. 
                                  The function frees the memory and sets the 
                                  pointer to null.
  -----------------------------------------------------------------------------
*/
externC void WINAPI_CN14 cn14release (
    void ** session);

/*!
    @brief calculate a DBM URI

    @param servernode [IN] host name (may end with ":<port>" to request a special port number). Must not be NULL. Pass string with length 0 to indicated that this should not be used.
    @param dbname [IN] database name. Must not be NULL. Pass string with length 0 to indicated that this should not be used.
    @param useOutURI [IN] passing 1: copy calculated URI to location that outURI points to; passing 0: do not copy the calculated URI
    @param outURI [OUT] pointer to calculated URI or NULL
    @param outErr [OUT] error text in case URI calculation failed. Caller must provide enough memory (at least for a tsp00_ErrTextc).
    @return required size in characters (including 0-terminator for non UCS2 callers) for URI string or a negative number in case an error occurred.
            In case of UCS2 callers and useOutURI == 0, an upper bound for the required space is returned. If useOutURI == 1 or caller is not UCS2,
            the exact number of created characters is returned (including 0-terminator for non UCS2 callers).
*/
externC tsp00_Int4 WINAPI_CN14 cn14buildDBMURIWinAPI(
    const char*               servernode,
    const char*               dbname,
    const int                 useOutURI,
    char*                     outURI,
    char*                     outErr);

/*!
    @brief calculate a DBM URI

    @param servernode [IN] host name (may end with ":<port>" to request a special port number) or NULL
    @param dbname [IN] database name or NULL
    @param useOutURI [IN] passing 1: copy calculated URI to location that outURI points to; passing 0: do not copy the calculated URI
    @param outURI [OUT] pointer to calculated URI or NULL
    @param outErr [OUT] error text in case URI calculation failed. Caller must provide enough memory (at least for a tsp00_ErrTextc).
    @return required size in characters (including 0-terminator for non UCS2 callers) for URI string or a negative number in case an error occurred.
            In case of UCS2 callers and useOutURI == 0, an upper bound for the required space is returned. If useOutURI == 1 or caller is not UCS2,
            the exact number of created characters is returned (including 0-terminator for non UCS2 callers).
*/
externC tsp00_Int4 cn14buildDBMURI(
    const char*               servernode,
    const char*               dbname,
    const int                 useOutURI,
    char*                     outURI,
    char*                     outErr);

/*!  
  EndChapter: Connecting and disconnecting
 */

/*!  
  Chapter: Sending and receiving data
 */

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14cmdRequest

  -----------------------------------------------------------------------------

  description:  write and send a packet
  
                This function writes the specified data in the packet and
                sends the paket to server.

                This function is UCS2 enabled for the usage as dbmapi.dll with 
                the DBMGui. Therefore it is forbidden to use this function
                in UCS2 mode with DBMServer-Commands dealing with binary data. 

  arguments:    session  [IN]   - A session pointer received by cn14connect.
                pData    [IN]   - A pointer to the buffer with the command
                                  to send.
                nLen     [IN]   - The length of the command to send.
                errtext  [OUT]  - A pointer to a buffer where a description is
                                  available in case of errors

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 WINAPI_CN14 cn14cmdRequest (
    void           * sessionParm,
    const void     * pData,
    tsp00_Int4       nLen,
    char*            errtext
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14replyReceive

  -----------------------------------------------------------------------------

  description:  receive and read a packet
  
                This function receives the answer from the server and reads 
                the data from the packet to the provided buffer.

                This function is UCS2 enabled for the usage as dbmapi.dll with 
                the DBMGui. Therefore it is forbidden to use this function
                in UCS2 mode with DBMServer-Commands dealing with binary data. 

  arguments:    session  [IN]    - A session pointer received by cn14connect.
                pData    [OUT]   - A pointer to the buffer to store data.
                                   If this is a NULL pointer the function
                                   only receives the answer and holds the
                                   packet. To get information about the length
                                   of the packet or the data in the packet 
                                   use cn14packetstats or cn14bytesAvailable.
                nLen     [INOUT] - IN:  the length of the buffer
                                   OUT: the length of data placed in the buffer
                errtext  [OUT]   - A pointer to a buffer where a description is
                                   available in case of errors

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 WINAPI_CN14 cn14replyReceive (
    void           * sessionParm,
    void           * pData,
    tsp00_Int4     * nLen,
    char*            errtext
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14cmdExecute

  -----------------------------------------------------------------------------

  description:  write, send, receive and read a packet
  
                This function writes the specified data in the packet and
                sends the paket to server and receives the answer from the 
                server and reads the data from the packet to the provided 
                buffer.

                This function is UCS2 enabled for the usage as dbmapi.dll with 
                the DBMGui. Therefore it is forbidden to use this function
                in UCS2 mode with DBMServer-Commands dealing with binary data. 

  arguments:    session  [IN]   - A session pointer received by cn14connect.
                pInData  [IN]   - A pointer to the buffer with the command
                                  to send.
                nInLen   [IN]   - The length of the command to send.
                pOutData [OUT]  - A pointer to the buffer to store data.
                                  If this is a NULL pointer the function
                                  only receives the answer and holds the
                                  packet. To get information about the length
                                  of the packet or the data in the packet 
                                  use cn14packetstats or cn14bytesAvailable.
                nOutLen [INOUT] - IN:  the length of the buffer
                                  OUT: the length of data placed in the buffer
                errtext  [OUT]  - A pointer to a buffer where a description is
                                  available in case of errors

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 WINAPI_CN14 cn14cmdExecute (
    void           * sessionParm,
    const void     * pInData,
    tsp00_Int4       nInLen,
    void           * pOutData,
    tsp00_Int4     * nOutLen,
    char*            errtext
    );


/*!
  -----------------------------------------------------------------------------
 
  function:     cn14ExecuteLoaderCmd

  -----------------------------------------------------------------------------

  description:  write, send, receive and read a packet
  
                This function writes the specified data in the packet and
                sends the paket to server and receives the answer from the 
                server and reads the data from the packet to the provided 
                buffer.
                This function handles commands that exceed the message
                buffer length between server and client by sending command
                parts to the server.

  arguments:    session  [IN]   - A session pointer received by cn14connect.
                pInData  [IN]   - A pointer to the buffer with the command
                                  to send.
                nInLen   [IN]   - The length of the command to send.
                pOutData [OUT]  - A pointer to the buffer to store data.
                                  If this is a NULL pointer the function
                                  only receives the answer and holds the
                                  packet. To get information about the length
                                  of the packet or the data in the packet 
                                  use cn14packetstats or cn14bytesAvailable.
                nOutLen [INOUT] - IN:  the length of the buffer
                                  OUT: the length of data placed in the buffer
                errtext  [OUT]  - A pointer to a buffer where a description is
                                  available in case of errors

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 WINAPI_CN14 cn14ExecuteLoaderCmd (
    void           * sessionParm,
    const void     * pInData,
    tsp00_Int4       nInLen,
    void           * pOutData,
    tsp00_Int4     * nOutLen,
    char*            errtext
    );


/*!
  -----------------------------------------------------------------------------
 
  function:     cn14replyRead

  -----------------------------------------------------------------------------

  description:  read from packet
  
                This function reads remaining data from a packet to the 
                provided buffer. The packet must received before by 
                cn14cmdExecute or cn14replyReceive.

  arguments:    session [IN]    - A session pointer received by cn14connect.
                pData [OUT]     - A pointer to the buffer to store data.
                nOutLen [INOUT] - IN:  the length of the buffer
                                  OUT: the length of data placed in the buffer

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14replyRead (
    void     * sessionParm,
    void     * pData,
    tsp00_Int4 * nOutLen );

/*!  
  EndChapter: Sending and receiving data
 */

/*!  
  Chapter: Asynchron work
 */

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14replyAvailable

  -----------------------------------------------------------------------------

  description:  look for an answer
  
                This function checks if there is an answer available for a
                pending request.

                This function is UCS2 enabled for the usage as dbmapi.dll with 
                the DBMGui.

  arguments:    session [IN]    - A session pointer received by cn14connect.
                errtext [OUT]   - A pointer to a buffer where a description is
                                  available in case of errors

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 WINAPI_CN14 cn14replyAvailable (
    void           * sessionParm,
    char*            errtext
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14cmdCancel

  -----------------------------------------------------------------------------

  description:  cancel a request
  
                This function cancels a pending request

  arguments:    session [IN]    - A session pointer received by cn14connect.
  -----------------------------------------------------------------------------
*/
externC void WINAPI_CN14 cn14cmdCancel (
    void* sessionParm);

/*!  
  EndChapter: Asynchron work
 */

/*!  
  Chapter: Packet information
 */

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14bytesAvailable

  -----------------------------------------------------------------------------

  description:  returns the number of bytes between the current position
                and the end of the packet
  
  arguments:    session [IN]    - A session pointer received by cn14connect.
  
  returns:      * on reading: the number of bytes between the current
                  position and the and of the received data
                * on writing: the maximum number of bytes available for
                  the command
  -----------------------------------------------------------------------------
*/
externC int cn14bytesAvailable (
    void * sessionParm );
 
/*!
  -----------------------------------------------------------------------------
 
  function:     cn14rawReadData

  -----------------------------------------------------------------------------

  description:  returns a pointer to the actual reading position
  
  arguments:    session [IN]    - A session pointer received by cn14connect.
                rc [OUT]        - A pointer to a buffer for the functions return
                                  code (see <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>)

  returns:      the pointer to the actual reading position
  
  -----------------------------------------------------------------------------
*/
externC const char * cn14rawReadData (
    void     * sessionParm,
    tsp00_Int4 * rc);


/*!
  -----------------------------------------------------------------------------
 
  function:     cn14packetstats

  -----------------------------------------------------------------------------

  description:  packet status
  
                Returns some information about the current packet.

  arguments:    session [IN]    - A session pointer received by cn14connect.
                packetSize [OUT]- size of the packet
                packetLen [OUT] - len of data in the packet
                packetPos [OUT] - current read/write position in the packet.
  -----------------------------------------------------------------------------
*/
externC void WINAPI_CN14 cn14packetstats (
    void     * session,
    tsp00_Int4 * packetSize,
    tsp00_Int4 * packetLen,
    tsp00_Int4 * packetPos);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14analyzeDbmAnswer

  -----------------------------------------------------------------------------

  description:  analyze DBMServer answer from the session struture
  
                checks the answer data from the DBMServer, computes
                the DBM errorcode, if exist and deliver an pointer
                to the "payload" data.
                "Payload" means the data after the OK-Line or in case of error
                the data after the ERR-Line + Errorcode-Line.

  arguments:    session [IN]      - A session pointer received by cn14connect.
                pPayLoad  [OUT]   - for returning the position
                nPayLoad  [OUT]   - the lenght of payload
                pErrorCode [OUT]  - errorcode of DBMServer
                errtext   [OUT]   - in case of error the DBMServer errortext

  
  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
                Is there a DBMServer error answer ("ERR"), this function will
                return DBMAPI_NOT_OK_CN14. (In case of a "OK" answer DBMAPI_OK_CN14)
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14analyzeDbmAnswer (
    void        * session,
    const void ** pPayLoad,
    tsp00_Int4    * nPayLoad,
    tsp00_Int4    * pErrorCode,
    tsp00_ErrTextc  VAR_ARRAY_REF errtext
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14analyzeDbmData

  -----------------------------------------------------------------------------

  description:  analyze DBMServer answer from the data paket
  
                checks the answer data from the DBMServer, computes
                the DBM errorcode, if exist and deliver an pointer
                to the "payload" data.
                "Payload" means the data after the OK-Line or in case of error
                the data after the ERR-Line + Errorcode-Line.

  arguments:    pData      [IN]    - the received data buffer
                nDataLen   [IN]    - the received data len
                pPayLoad   [OUT]   - for returning the position
                nPayLoad   [OUT]   - the lenght of payload
                pErrorCode [OUT]   - errorcode of DBMServer
                errtext    [OUT]   - in case of error the DBMServer errortext
  
  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
                Is there a DBMServer error answer ("ERR"), this function will
                return DBMAPI_NOT_OK_CN14. (In case of a "OK" answer DBMAPI_OK_CN14)
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14analyzeDbmData (
    const void   * pData,
    tsp00_Int4       nDataLen,
    const void  ** pPayLoad,
    tsp00_Int4     * nPayLoad,
    tsp00_Int4     * pErrorCode,
    tsp00_ErrTextc   VAR_ARRAY_REF errtext
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14analyzeRpmAnswer

  -----------------------------------------------------------------------------

  description:  analyze Replication Server answer
  
                checks the answer data from the Replication Server, extracts the
                Replication Server error code, if exists and delivers a pointer
                to the error text.
                In case an sql error is given back by the server this function
                extracts the SQL error code, if exists and delivers a pointer
                to the sql error text.

  arguments:    pRPMSession    [IN]   - A session pointer received by cn14connect.
                plErrCode      [OUT]  - pointer, returns the replication server error
                                        code; is set to 0 if no error signaled
                ppszErrText    [OUT]  - pointer to pointer, error text of the
                                        replication server; is set to empty string
                                        if no error or an sql error is encountered
                plErrLen       [OUT]  - pointer, returns the length of the error text
                plSqlCode      [OUT]  - pointer, returns the sql error code;
                                        is set to 0 if no sql error signaled
                ppszSqlErrText [OUT]  - pointer to pointer, sql error text;
                                        is set to empty string if no sql error or
                                        an replication server error is encountered
                plSqlErrLen    [OUT]  - pointer, returns the length of the sql error text

  
  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
                DBMAPI_OK_CN14        - No error from the replication server ("OK "
                DBMAPI_NOT_OK_CN14    - Error signaled by the Replication Server ("ERR"),
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14analyzeRpmAnswer
                    (void         *pRPMSession,
                     tsp00_Int4     *plErrCode,
                     const _TCHAR **ppszErrText,
                     tsp00_Int4     *plErrLen,
                     tsp00_Int4     *plSqlCode,
                     const _TCHAR **ppszSqlErrText,
                     tsp00_Int4     *plSqlErrLen);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14IsUnicode

  -----------------------------------------------------------------------------

  description:  returns the unicode flag
  
  arguments:    session [IN]      - A session pointer received by cn14connect.
  
  returnvalue:  true if unicode, false if not
  -----------------------------------------------------------------------------
*/
externC bool cn14IsUnicode (
    void        * session );

/*!  
  EndChapter: Packet information
*/

/*!  
  Chapter: User management
 */

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14saveUser

  -----------------------------------------------------------------------------

  description:  save user
  
                saves a user in the client environment for authorization
                against DBMServer

  arguments:    szDBName [IN]     - name of database
                szServerNode [IN] - name of server
                szUser [IN]       - name and clear or applcrypted password
                                    of the user "name,pwd"
  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14saveUser (
    const char*           szServerNode,
    const char*           szDBName,
    const _TCHAR *        szUser);

externC tsp00_Int4 cn14saveUser_fixedSizeParameters (
    const tsp00_NodeIdc   VAR_ARRAY_REF szServerNode,
    const tsp00_DbNamec   VAR_ARRAY_REF szDBName,
    const _TCHAR *        szUser);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14saveUserByKey

  -----------------------------------------------------------------------------

  description:  save user
  
                saves a user in the client environment for authorization
                against DBMServer

  arguments:    szUserKey [IN]    - xuser key
                szUser [IN]       - name and clear or applcrypted password
                                    of the user "name,pwd"
                szParams [IN]     - XUser parameter CACHELIMIT,SQLMODE,TIMEOUT,
                                    ISOLATION,DBLOCALE

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14saveUserByKey (
    const _TCHAR *        szKey,
    const char*           szServerNode,
    const char*           szDBName,
    const _TCHAR *        szUser,
    const _TCHAR *        szParams);

externC tsp00_Int4 cn14saveUserByKey_fixedSizeParameters (
    const _TCHAR *        szKey,
    const tsp00_NodeIdc   VAR_ARRAY_REF szServerNode,
    const tsp00_DbNamec   VAR_ARRAY_REF szDBName,
    const _TCHAR *        szUser,
    const _TCHAR *        szParams);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14checkUser

  -----------------------------------------------------------------------------

  description:  check user
  
                checks a user in the client environment against the 
                current value

  arguments:    szDBName [IN]     - name of database
                szServerNode [IN] - name of server
                szUser [IN]       - name and clear or applcrypted password
                                    of the user "name,pwd"
  
  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14checkUser (
    const char*           szServerNode,
    const char*           szDBName,
    const _TCHAR *        szUser);

externC tsp00_Int4 cn14checkUser_fixedSizeParameters (
    const tsp00_NodeIdc   VAR_ARRAY_REF szServerNode,
    const tsp00_DbNamec   VAR_ARRAY_REF szDBName,
    const _TCHAR *        szUser);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14checkUserByKey

  -----------------------------------------------------------------------------

  description:  check user
  
                checks a user in the client environment against the 
                current value

  arguments:    szUserKey [IN]    - xuser key
                szUser [IN]       - name and clear or applcrypted password
                                    of the user "name,pwd"
  
  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14checkUserByKey (
    const _TCHAR *        szKey,
    const _TCHAR *        szUser);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14checkDefaultUser

  -----------------------------------------------------------------------------

  description:  check default user
  
                checks a user in the client environment against the 
                current value.
                If the user in the environment not named to SAPR3 the
                user will set to SAPR3/SAP

  arguments:    szUser [IN]       - name and clear or applcrypted password
                                    of the user "name,pwd"
  
  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14checkDefaultUser (
    const _TCHAR *        szUser);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14deleteUser

  -----------------------------------------------------------------------------

  description:  delete user
  
                deletes a user in the client environment

  arguments:    szDBName [IN]     - name of database
                szServerNode [IN] - name of server
  
  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14deleteUser (
    const char*           szServerNode,
    const char*           szDBName
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14deleteUserByKey

  -----------------------------------------------------------------------------

  description:  delete user
  
                deletes a user in the client environment

  arguments:    szUserKey [IN]    - xuser key
  
  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
externC tsp00_Int4 cn14deleteUserByKey (
    const _TCHAR *        szKey);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14listUsers

  -----------------------------------------------------------------------------

  description:  list users
  
                list all users from the client environment in a buffer

  arguments:    pBuffer [OUT]    - buffer for user data
                nBuffer [IN]     - length of buffer
  
  returnvalue:  number of users found
  -----------------------------------------------------------------------------
*/
externC long cn14listUsers (
    _TCHAR                * pBuffer,
    long                    nBuffer
    );

/*!
  -----------------------------------------------------------------------------
 
  function:     cn14GetTPUser

  -----------------------------------------------------------------------------

  description:  read the tp User

  arguments:    pMode   [IN]    - user mode
                sysid   [IN]    - A zero terminated string with the R/3 system id
                conn    [IN]    - A zero terminated string with the R/3 connection id
                profile [IN]    - A zero terminated string with the R/3 tp profile
                user    [OUT]   - the user
                errtext [OUT]   - A pointer to a buffer where a description is
                                  available in case of errors

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>

  -----------------------------------------------------------------------------
*/
#define TP_USERMODE_DBM_CN14   "DBM"
#define TP_USERMODE_SAP_CN14   "SAP"

externC tsp00_Int4  cn14GetTPUser (
    const char              * pMode,
    const char*               sysid,     
    const char*               conn,     
    const char*               profile,
    tsp00_C64c                VAR_ARRAY_REF user,
    tsp00_ErrTextc          VAR_ARRAY_REF errtext     
    );

/*!  
  EndChapter: User management
 */

#endif /* _HCN14_H_ */
