/*!
  -----------------------------------------------------------------------------
  module: hls15.h
  -----------------------------------------------------------------------------
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  user management
                
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
#ifndef HLS15
#define HLS15

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------
  function:     ls15SwitchUser
  -----------------------------------------------------------------------------
  description:  Switches the user connected to the given user without changing the
                session and without delivering a password.
  
  arguments:    DBInfo      [IN]  - structure that contains all information on
                                    the running session and the db connected to.
                UserName    [IN]  - user name to switch to, UTF8 encoded
                pszErrText  [OUT] - error text buffer

  returnvalue:  errOK_els00              - everything went fine
                errCurrentUserCmd_els98  - error processing command

                The function logs the error message to log file and return string.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15SwitchUser(tls00_DBInfo            *pDBInfo,
               Tools_DynamicUTF8String  UserName,
               tsp00_Addr               pszErrText);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls15SwitchUserSchema

  -----------------------------------------------------------------------------

  description:  Switches the schema connected to the given schema without changing the
                session and without delivering a password.
  
  arguments:    DBInfo      [IN]  - structure that contains all information on
                                    the running session and the db connected to.
                SchemaName  [IN]  - schema name to switch to, UTF8 encoded
                pszErrText  [OUT] - error text buffer

  returnvalue:  errOK_els00              - everything went fine

                The function logs the error message to log file and return string.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15SwitchUserSchema(tls00_DBInfo            *pDBInfo,
                     Tools_DynamicUTF8String  UserName,
                     Tools_DynamicUTF8String  SchemaName,
                     tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
   function:     ls15SwitchSchema
  -----------------------------------------------------------------------------
  description:  Switches the schema connected to the given schema without changing the
                session and without delivering a password.
  
  arguments:    DBInfo      [IN]  - structure that contains all information on
                                    the running session and the db connected to.
                SchemaName  [IN]  - schema name to switch to, UTF8 encoded
                pszErrText  [OUT] - error text buffer

  returnvalue:  errOK_els00              - everything went fine
                The function logs the error message to log file and return string.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15SwitchSchema(tls00_DBInfo           *pDBInfo,
                 Tools_DynamicUTF8String schemaname,
                 tsp00_Addr              ErrText);



/*!
  -----------------------------------------------------------------------------
   function:    ls15SwitchFormat
  -----------------------------------------------------------------------------
  description:  Switches the format for date, time and timestamp values of the
                kernel.
  
  arguments:    pDBInfo     [IN]  - structure that contains all information on
                                    the running session and the db connected to.
                newFormat   [IN]  - format to switch to
                pszErrText  [OUT] - error text buffer

  returnvalue:  errOK_els00              - everything went fine
                The function logs the error message to log file and return string.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15SwitchFormat(tls00_DBInfo*   pDBInfo,
                 tls00_DateTime  newFormat,
                 tsp00_Addr      pszErrText);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls15SQLGetUser

  -----------------------------------------------------------------------------

  description:  reads user info for a database from xuser file
  
                This function does not write any error message to the log file. 
                In case of error it returns an error code and a description that
                is a null-terminated string.

  arguments:    *Session [IN/OUT]   - Structure that contains all information to open
                                       a session to a database.
                                       It will be filled with user information that is
                                       read from the xuser file using the user key which
                                       itself is a member of this session structure.
                *ErrText [OUT]      - Error text

  returnvalue:  errOK_els00                 - everything went fine
                errSqlConnectUserkey_els98  - error in getting the information using
                                              the user key
  -----------------------------------------------------------------------------
*/
int ls15SQLGetUser(tls00_SessionInfo* Session, tsp00_Addr pszErrText);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls15RSCommit

  -----------------------------------------------------------------------------

  description:  Special commit function for load and extract. Sends 'COMMIT' 
                message to the kernel only if the commit immediately flag is
                set (autocommit = on).
  
  arguments:    DBInfo [IN]     - Structure with all information on the running
                                   session and the db connected to
                ErrText [OUT]   - Error text

  returnvalue:  errOK_els00              - everything went fine
                errFinishPart_els00      - could not finish part to send - fatal error
                error code for sql error - otherwise
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls15RSCommit(tls00_DBInfo *DBInfo, tsp00_Addr ErrText, SAPDB_Int4 lPcktNo = 0);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls15CommitUser

  -----------------------------------------------------------------------------

  description:  closes a connection of a specified user to the database,
                In case there was no connection this is simply reported via 
                error message to the caller.
                Before closing the connection a ROLLBACK message is sent.
  
  arguments:    DBInfo   [IN]   - Structure with all information on the running
                                   session and the db connected to
                ErrText  [OUT]  - Error text

  returnvalue:  errOK_els00                 - everything went fine
                errConnectNoServerDB_els98  - no connection of user to db
                errFinishPart_els00         - could not finish part to send - fatal error
                error code for sql error    - otherwise

  -----------------------------------------------------------------------------
*/
int ls15CommitUser (tls00_DBInfo*  DBInfo, tsp00_Addr  ErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls15Commit

  -----------------------------------------------------------------------------

  description:  enforces an 'COMMIT WORK'
  
  arguments:    DBInfo [IN]     - Structure with all information on the running
                                   session and the db connected to
                ErrText [OUT]   - Error text

  returnvalue:  errOK_els00              - everything went fine
                errFinishPart_els00      - could not finish part to send - fatal error
                error code for sql error - otherwise
  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls15Commit(tls00_DBInfo *DBInfo, tsp00_Addr ErrText, SAPDB_Int4 lPcktNo = 0);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls15Rollback

  -----------------------------------------------------------------------------

  description:  enforces a 'rollback work'; in case a 'rollback work release' is
                requested the default parameter must be set.
  
  arguments:    DBInfo   [IN]   - Structure with all information on the running
                                   session and the db connected to
                ErrText  [OUT]  - Error text
                bRelease [IN]   - bool parameter indicating a release

  returnvalue:  errOK_els00              - everything went fine
                errFinishPart_els00      - could not finish part to send - fatal error
                error code for sql error - otherwise

  -----------------------------------------------------------------------------
*/
tsp00_Int4 ls15Rollback(tls00_DBInfo *DBInfo, tsp00_Addr ErrText, bool bRelease = false);



/*!
  -----------------------------------------------------------------------------

  function: ls15CheckUserMode

  -----------------------------------------------------------------------------

  description:  Checks the usermode of the given user

  arguments:    pDBInfo     [IN]   - structure with all information on the running
                                      session and the db connected to
                UserName    [IN]   - name of user to check mode for, UTF8 encoded
                UserMode    [IN]   - mode to check
                pszErrText  [OUT]  - error text

  returnvalue:  errOK_els00             - user is sysdba
                dberrRowNotFound_els98  - user does not have UserMode
                errInternSQL_els98      - error executing the statement used to
                                          check the status of logged on user

                Function already logs the error messages to log file in case of
                errInternSQL_els98. Otherwise it returns only the error code.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15CheckUserMode(tls00_DBInfo            *pDBInfo,
                  Tools_DynamicUTF8String  UserName,
                  tls00_DBUserMode         UserMode,
                  tsp00_Addr               pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls15Connect
  -----------------------------------------------------------------------------
  description:  Connects user to db:
                 - connection to db without user data
                 - logon user to db
                First the user is logged on to db with ascii encoding. If this
                fails a unicode connect is performed.

                In case the user cannot not be logged on the db connection is
                released, too.
                Gets the db version and fills version structure in DBInfo_ls00
                structure.

  arguments:    pDBInfo    [IN]    - struct that contains all info on the running
                                      session and the db connected to.
                pszErrText [OUT]   - error text

  returnvalue:  errOK_els00              - in case of NO errors
                errSQLConnectUser_els98  - error while processing user logon

                Error messages will be logged to the log file and the return string.
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls15Connect(tls00_DBInfo *pDBInfo, tsp00_Addr pszErrText);

/*!
  -----------------------------------------------------------------------------
  function:     ls15LoadCodePage
  -----------------------------------------------------------------------------
  description:  Gets called if user specified special code page in session command.
                Checks wether a system table SYSDBA.CODEPAGE exists or not and
                if this table contains the code page mapping requested.
                If the table does not have the code page requested the function
                reads mapping table (ASCII code page to UCS2) from a file located
                in env-directory and loads the system table and fills a code page
                structure in the internal session structure.
                If no system table CODEPAGE can be found or the table does not
                contain the right mapping and no file can be found either the
                function fails.

  arguments:    pDBInfo    [IN]    - struct that contains all info on the running
                                      session and the db connected to.
                pszErrText [OUT]   - error text

  returnvalue:  errOK_els00                   - loading successful
                errInvalidHexInCPFile_els98   - invalid hex constant found in code page file
                errInvalidASCIICPValues_els98 - invalid ascii values found in code page file
                errCreate_GrantCPTable_els98  - creating/granting non existing code page
                                                 table failed
                errReadCPTable_els98          - reading code page table failed
                errFileRead_els98             - reading code page file failed
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls15LoadCodePage(tls00_DBInfo *pDBInfo, tsp00_Addr pszErrText);


/*!
  -----------------------------------------------------------------------------
  function:     ls15CheckConnection
  -----------------------------------------------------------------------------
  description:  Checks for timeout of connection and refreshes the connection
                or reconnects in case AUTOCOMMIT is true.

  arguments:    pDBInfo    [IN]    - struct that contains all info on the running
                                      session and the db connected to.
                pszErrText [OUT]   - error text

  returnvalue:  errOK_els00
                connection error in case
  -----------------------------------------------------------------------------
*/
SAPDB_Int4
ls15CheckConnection(tls00_DBInfo *pDBInfo, tsp00_Addr pszErrText);


inline tsp100_VersionResult
ls15CheckKernelVersion(tls00_DBInfo* pDBInfo, char szMajor, char szMinor, char szCorrLevel, char szBuildNumberPrefix)
{
    tsp100_VersionID1 Version76;
    Version76.MajorVersion_sp100 = szMajor;
    Version76.MinorVersion_sp100 = szMinor;
    Version76.CorrLevel_sp100    = szCorrLevel;
    Version76.BuildNumberPrefix_sp100  = szBuildNumberPrefix;
    return sp100_Compare2VersionIDs(VersionIDType1_esp100, &pDBInfo->dbiDBVersion_ls00, &Version76);
}



/* Importierte Funktionen */

externC void s02applencrypt     (...);                      /* aus vsp02c */

inline  void ls15ApplEncrypt    (tsp00_Pw&  ClearPwd, tsp00_CryptPw&  CryptPwd)
{
    s02applencrypt (&ClearPwd, &CryptPwd);
}


#endif /*HLS15*/