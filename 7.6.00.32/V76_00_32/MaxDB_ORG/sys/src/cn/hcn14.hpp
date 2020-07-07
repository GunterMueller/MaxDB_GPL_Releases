/*
  -----------------------------------------------------------------------------
  module: hcn14.hpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: Database Manager-API
  description:  API for Database Manager Client Applications


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

#ifndef _HCN14_HPP_
#define _HCN14_HPP_

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */
#include "hcn14.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

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
                oErrtext [OUT]  - A reference to a object to store the full message received from the 
                                  DBMServer during the connect

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14connectDBM (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    void                   ** sessionOut,
    Tools_DynamicUTF8String & oErrtext 
    );


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
                oErrtext [OUT]  - A reference to a object to store the full message received from the 
                                  DBMServer during the connect

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14connectDBMUsr (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    const char*               user,
    void                   ** sessionOut,
    Tools_DynamicUTF8String & oErrtext 
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
                oErrtext [OUT]  - A reference to a object to store the full message received from the 
                                  DBMServer during the connect

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14connectDBMTp (
    const char*               servernode,
    const char*               dbname,
    const char*               dbroot,
    const char*               sysid,
    const char*               conn,
    const char*               profile,
    void                   ** sessionOut,
    Tools_DynamicUTF8String & oErrtext 
    );

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
                oErrtext   [OUT]   - A reference to a object to store the full message received from the 
                                     DBMServer during the connect

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
                Is there a DBMServer error answer ("ERR"), this function will
                return DBMAPI_NOT_OK_CN14. (In case of a "OK" answer DBMAPI_OK_CN14)
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14analyzeDbmAnswer (
    void                    * session,
    const void             ** pPayLoad,
    tsp00_Int4              * nPayLoad,
    tsp00_Int4              * pErrorCode,
    Tools_DynamicUTF8String & oErrtext 
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
                oErrtext   [OUT]   - A reference to a object to store the full message received from the 
                                     DBMServer during the connect

  returnvalue:  <A HREF="#DBMAPI-Constants" >DBMAPI-Constants</A>
                Is there a DBMServer error answer ("ERR"), this function will
                return DBMAPI_NOT_OK_CN14. (In case of a "OK" answer DBMAPI_OK_CN14)
  -----------------------------------------------------------------------------
*/
tsp00_Int4 cn14analyzeDbmData (
    const void              * pData,
    tsp00_Int4                nDataLen,
    const void             ** pPayLoad,
    tsp00_Int4              * nPayLoad,
    tsp00_Int4              * pErrorCode,
    Tools_DynamicUTF8String & oErrtext 
    );

#endif /* _HCN14_HPP_ */
