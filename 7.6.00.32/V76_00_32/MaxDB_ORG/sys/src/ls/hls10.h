/* 
  -----------------------------------------------------------------------------
  module: hls10.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  dispatcher
                
  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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
#ifndef HLS10
#define HLS10

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------

  function:     ls10FreeTransformation

  -----------------------------------------------------------------------------

  description:  Pseudo destructor.
                Destroys load command structure built using the info in the
                command.

  arguments:    Type    [IN]   - type of command struct to destroy
                Struct  [IN]   - struct to destroy

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls10FreeTransformation(tls00_ScanResult Type, char * &Struct);

void
ls10FreeTransformation( tls00_TransformationCmd* pTransformationCmd );

/*!
  -----------------------------------------------------------------------------
 
  function:     ls10ExecChangeUser

  -----------------------------------------------------------------------------

  description:  Disconnects connected user and connects new user to db. In case
                there was a connected user and the new connection fails the function
                restores the old connection.
                Connection means - build a db connection and then logon user.
                Handles connects to ASCII and UNICODE db's.
                
  arguments:    DBInfo   [IN]    - structure with info on the existing session
                                    and the db connected (to connect) to
                Session  [IN]    - structure with info on the new session
                Errtext  [OUT]   - error text

  returnvalue:  errOK_els00                    - in case of NO errors
                errPreviousUserRestored_els98  - no connection for new user, old
                                                  connection restored
                errSQLConnectUser_els98        - no connection
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls10ExecChangeUser(tls00_DBInfo *DBInfo, tls00_SessionInfo *Session, tsp00_Addr ErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls10TransformationPackageId

  -----------------------------------------------------------------------------

  description:  generates guid for Transformation
                

  arguments:    pLoaderCmd   [IN]    - command to process
                pDBInfo              [IN]    - structure with info on 
                                               the existing session
                                               and the db connected to

  returnvalue:  errOK_els00      - no errors
                errors of called load functions
  -----------------------------------------------------------------------------
*/
void
ls10TransformationPackageId     (tls00_Command *&pLoaderCmd, tls00_DBInfo *pDBInfo);

/*!
  -----------------------------------------------------------------------------
 
  function:     ExecuteTransformationPackage 

  -----------------------------------------------------------------------------

  description:  Load dispatcher (starts different load functions)
                Logs command to process and start time to log file

  arguments:    pLoaderCmd   [IN]    - command to process
                pDBInfo   [IN]               - structure with info on 
                                               the existing session
                                               and the db connected to

  returnvalue:  errOK_els00      - no errors
                errors of called load functions
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ExecuteTransformationPackage(tls00_Command *pLoaderCmd, tls00_DBInfo *pDBInfo);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls10ExecSQLStmt

  -----------------------------------------------------------------------------

  description:  Starts execution of external sql command

  arguments:    SvrCmd   [IN]    - command to process
                DBInfo   [IN]    - structure with info on the existing session
                                    and the db connected to

  returnvalue:  errOK_els00                - no errors
                errConnectNoServerDB_els98 - no connection to db
                sql error of executed command
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls10ExecSQLStmt(tls00_Command *SvrCmd, tls00_DBInfo *DBInfo);

/*!
  -----------------------------------------------------------------------------
  function:     ls10ExecRemark
  -----------------------------------------------------------------------------
  description:  Writes remark into protocol file

  arguments:    SvrCmd   [IN]    - structure that contains the remark
                DBInfo   [IN]    - structure with info on the existing session
                                    and the db connected to

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void
ls10ExecRemark(tls00_Command* pLoaderCmd, tls00_DBInfo* pDBInfo); //!!LogMessage


#endif /*HLS10*/
