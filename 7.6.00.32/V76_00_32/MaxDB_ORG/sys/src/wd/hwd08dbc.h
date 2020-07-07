/*!**********************************************************************

  module: hwd08dbc.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Özgen

  special area: WebAgent

  description:  Persistent ODBC connections; C interface

  see also:

  -----------------------------------------------------------------------

						Copyright (c) 1999-2005 SAP AG



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

#ifndef HWD08DBC_H
#define HWD08DBC_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>

#include "hwd01wadef.h"
#include "hwd06waerr.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: sapdbwa_DBCP

  description:
  A sapdbwa_DBCP is the connection handle used to connect through the sapdbwa_DBCPoolP.
  It represents an ODBC connection with own HENV and HDBC handles,
  connection parameters and attributes.
  
************************************************************************/

typedef struct st_wa_dbc *sapdbwa_DBCP;

/*!**********************************************************************
  Function: sapdbwa_CreateDBC

  description: Create a sapdbwa_DBCP connection handle.

  arguments:

  return value: void
*/
sapdbwa_DBCP WINAPI sapdbwa_CreateDBC();

    
/*!**********************************************************************
  Function: sapdbwa_DestroyDBC

  description: Free all resources for a sapdbwa_DBCP connection handle.

  arguments:
    dbc  [in]  The DBC handle to destroy.
    
  return value: void
*/    
void WINAPI sapdbwa_DestroyDBC( sapdbwa_DBCP dbc );

    
/*!**********************************************************************
  Function: sapdbwa_DBCHandle

  description: Get ODBC connection handle.

  arguments:
    dbc  [in]  The DBC handle.
    
  return value: The ODBC connection handle.
*/
    
SQLHDBC WINAPI sapdbwa_DBCHandle( sapdbwa_DBCP dbc );

    
/*!**********************************************************************
  Function: sapdbwa_DBCEnv

  description: Get ODBC environment handle.

  arguments:
    dbc  [in]  The DBC handle.

  return value: The ODBC environment handle.
*/
    
SQLHENV WINAPI sapdbwa_DBCEnv( sapdbwa_DBCP dbc );

    
/*!**********************************************************************
  Function: sapdbwa_GetTableContainer

  description: Get name of table container.

  arguments:
    dbc  [in]  The DBC handle.

  return value: The table name.
*/
    
const char* WINAPI sapdbwa_GetTableContainer(sapdbwa_DBCP dbc);
const char* WINAPI sapdbwa_GetTableInode(sapdbwa_DBCP dbc);
sapdbwa_Bool WINAPI sapdbwa_GetSchemaInfo(sapdbwa_DBCP dbc);


/*!**********************************************************************
  Function: sapdbwa_DBCGetDriver

  description: Get driver of connection.

  arguments:
    dbc     [in]  The DBC handle.
    buf     [in/out]  Buffer for driver string.
    bufLen  [in]      Length of buf.
        
  return value: True if dbc is connected and buf was big enough else
                false. When false is returned the associated errId
                is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */
sapdbwa_Bool WINAPI sapdbwa_DBCGetDriver( sapdbwa_DBCP   dbc,
                                          unsigned char *buf,
                                          sapdbwa_UInt4  bufLen );

/*!**********************************************************************
  Function: sapdbwa_DBCGetDatasource

  description: Get datasource of connection.

  arguments:
    dbc     [in]  The DBC handle.
    buf     [in/out]  Buffer for datasource string.
    bufLen  [in]      Length of buf.
        
  return value: True if dbc is connected and buf was big enough else
                false. When false is returned the associated errId
                is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */
sapdbwa_Bool WINAPI sapdbwa_DBCGetDatasource( sapdbwa_DBCP    dbc,
                                              unsigned char  *buf,
                                              sapdbwa_UInt4   bufLen );

/*!**********************************************************************
  Function: sapdbwa_DBCSetAttr

  description: Set connection attribute.

  This method can only be called before Connect is called.

  arguments:
  dbc   [in]  The DBC handle.
  attr  [in]  The ODBC connection attribute. The following attributes
  are supported:
  <UL>
  <LI> SQL_SQLMODE
  <LI> SQL_ATTR_AUTOCOMMIT
  <LI> SQL_TXN_ISOLATION
  </UL>
  value [in]  The attribute value. See ODBC reference for valid values.

  return value: true for success false for failure.
*/
/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool WINAPI sapdbwa_DBCSetAttr( sapdbwa_DBCP   dbc,
                                        SQLINTEGER     attr,
                                        SQLPOINTER     value );


/*!**********************************************************************
  Function: sapdbwa_DBCGetAttr

  description:	Gets a connection attribute. If the searched attribute is
				non of the standard attributes (see sapdbwa_DBCSetAttr)
				the method will fail.

  arguments:
	dbc   [in]  The DBC handle.
    attr  [in]  The ODBC connection attribute wich is being searched.
    pvalue[in]  Buffer for the attribute value.

  return value: true for success false for failure.
*/
/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool WINAPI sapdbwa_DBCGetAttr( sapdbwa_DBCP   dbc,
										SQLINTEGER     attr,
										SQLPOINTER    *pvalue );
    
/*!**********************************************************************
  Function: sapdbwa_DBCAddAttr

  description:	Adds a connection attribute to the extended attribute
				list. If the attribute already exists, its value
				will be updated.
				This attributes will only be used if they have been set
				before connect is called.

  arguments:
  dbc   [in]  The DBC handle.
  attr  [in]  The ODBC connection attribute.
  value [in]  The attribute value. See ODBC reference for valid values.

  return value: true for success false for failure.
*/
/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool WINAPI sapdbwa_DBCAddAttr( sapdbwa_DBCP  dbc,
										SQLINTEGER    attr,
										SQLPOINTER    value,
                                        SQLINTEGER    stringLength );

    
/*!**********************************************************************
  Function: sapdbwa_DBCSelectAttr

  description:	Selects a connection attribute from the extended
				attribute list.

  arguments:
  dbc   [in]  The DBC handle.
  attr  [in]  The ODBC connection attribute you are looking for.
  pvalue[in]  Buffer for the attribute value.

  return value: true for success false for failure.
*/
/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
sapdbwa_Bool WINAPI sapdbwa_DBCSelectAttr( sapdbwa_DBCP    dbc,
										   SQLINTEGER      attr,
										   SQLPOINTER     *pvalue,
                                           SQLINTEGER     *stringLength );

    
/*!**********************************************************************
  Function: sapdbwa_DBCClearAttrList

  description:	Clears the list of connection attributes.

  arguments:
  dbc   [in]  The DBC handle.

  return value: true for success false for failure.
*/
sapdbwa_Bool WINAPI sapdbwa_DBCClearAttrList( sapdbwa_DBCP  dbc );

    
/*!**********************************************************************
  Function: sapdbwa_DBCGetErr

  description: Get error information from dbc.

  arguments:
  dbc   [in]  The DBC handle.

  return value:
  Reference to error object of the dbc. This can be used to retrieve
  error information if a previous operation on the dbc failed.
  See <A HREF="hwd06waerr.html">hwd06waerr.html</A> for a description
  of the error class.
*/
sapdbwa_ErrP WINAPI sapdbwa_DBCGetErr( sapdbwa_DBCP dbc );

    
/*!**********************************************************************
  Function: sapdbwa_DBCGetUser

  description: Get username of connection.

  arguments:
  dbc     [in]  The DBC handle.
  buf     [in/out]  Buffer for username string.
  bufLen  [in]      Length of buf.
        
  return value: True if dbc is connected and buf was big enough else
  false. When false is returned the associated errId
  is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
*/
sapdbwa_Bool WINAPI sapdbwa_DBCGetUser( sapdbwa_DBCP         dbc,
                                 unsigned char  *buf,
                                 sapdbwa_UInt4     bufLen );



/*!**********************************************************************
  Function: sapdbwa_DBCGetPassword

  description: Get password of connection.

  arguments:
    dbc     [in]  The DBC handle.
    buf     [in/out]  Buffer for password string.
    bufLen  [in]      Length of buf.
        
  return value: True if dbc is connected and buf was big enough else
                false. When false is returned the associated errId
                is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */
sapdbwa_Bool WINAPI sapdbwa_DBCGetPassword( sapdbwa_DBCP        dbc,
                                     unsigned char *buf,
                                     sapdbwa_UInt4    bufLen );
    
/*!**********************************************************************
  Function: sapdbwa_DBCGetServerDb

  description: Get serverdb of connection.

  arguments:
  dbc     [in]      The DBC handle.
  buf     [in/out]  Buffer for serverdb string.
  bufLen  [in]      Length of buf.
        
  return value: True if dbc is connected and buf was big enough else
  false. When false is returned the associated errId
  is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
*/
sapdbwa_Bool WINAPI sapdbwa_DBCGetServerDb( sapdbwa_DBCP         dbc,
                                     unsigned char  *buf,
                                     sapdbwa_UInt4     bufLen  );

    
/*!**********************************************************************
  Function: sapdbwa_DBCGetServerNode

  description: Get servernode of connection.

  arguments:
  dbc     [in]      The DBC handle.
  buf     [in/out]  Buffer for servernode string.
  bufLen  [in]      Length of buf.
        
  return value: True if dbc is connected and buf was big enough else
  false. When false is returned the associated errId
  is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
*/    
sapdbwa_Bool WINAPI sapdbwa_DBCGetServerNode( sapdbwa_DBCP        dbc,
                                       unsigned char *buf,
                                       sapdbwa_UInt4    bufLen  );

    
/*!**********************************************************************
  Function: sapdbwa_CheckDBC
  
  description: Check if connection is still opened and try to reconnect
      if necessary.

  arguments:
  dbc            [in]  The DBC handle.
  nativeErr      [in]  The native error code as returned by SQLError,
                       SQLGetDiagRec or SQLGetDiagField.
  sqlState       [in]  The ODBC SQL-State as returned by SQLError.
  connectionDown [out] Set to true if connection was down.
  
  return value: true if connection is ok or connection was down and
  could be reconnected. false if connection was down and
  the reconnect failed.
*/
sapdbwa_Bool WINAPI sapdbwa_CheckDBC( sapdbwa_DBCP      dbc,
                               sapdbwa_Int4   nativeErr,
                               const char  *sqlState,
                               sapdbwa_Bool  *connectionDown );

/*!**********************************************************************
  Function: sapdbwa_DBCGetUserData
  
  description: Get user data pointer from dbc handle that was previously
               set with sapdbwa_DBCSetUserData.

  arguments:
  dbc            [in]  The DBC handle.
  
  return value: The user data pointer or NULL if no pointer was set with
                sapdbwa_DBCSetUserData.
*/    
void* WINAPI sapdbwa_DBCGetUserData( sapdbwa_DBCP dbc );


/*!**********************************************************************
  Function: sapdbwa_DBCSetUserData
  
  description: Set user data pointer for the dbc handle.

  arguments:
  dbc            [in]  The DBC handle.
  data           [in]  User data pointer.
  destructor     [in]  Destructor function that is called when the
                       connection is released.
                       
  return value: The user data pointer or NULL if no pointer was set with
                sapdbwa_DBCSetUserData.
*/    
sapdbwa_Bool WINAPI sapdbwa_DBCSetUserData( sapdbwa_DBCP           dbc,
                                 void             *data,
                                 sapdbwa_Destructor     destructor );
    
/*!**********************************************************************

  EndChapter: sapdbwa_DBCP

************************************************************************/

/*!**********************************************************************

  Chapter: sapdbwa_DBCPoolP

  description:
  A sapdbwa_DBCPoolP provides functions to connect and disconnect sapdbwa_DBC handles.
  The underlying ODBC connections are pooled and reused, if connect
  parameters match.

  The time an unused connection remains in the pool may be limited by a
  timeout.
  
************************************************************************/

typedef struct st_wa_dbc_pool *sapdbwa_DBCPoolP;

typedef enum
{
    sapdbwa_DBC_EXCL,
    sapdbwa_DBC_EXCL_AUTOCOMMIT_OFF,
    sapdbwa_DBC_SHARED
} sapdbwa_DBCExclType;

#define sapdbwa_DBC_NO_TIMEOUT -1
    
/*!**********************************************************************
  Function: sapdbwa_CreateDBCPool

  description: Create a new sapdbwa_DBCPoolP.

  arguments:
  timeOut  [in]  Timeout in seconds for connections in this pool.
                 If a connection is not used for this time it is
                 closed implicitely and removed from the pool. Use
                 sapdbwa_DBC_NO_TIMEOUT for infinit timeout.

  return value: The new sapdbwa_DBCPoolP.
*/
sapdbwa_DBCPoolP WINAPI sapdbwa_CreateDBCPool( sapdbwa_Int4  timeOut );

    
/*!**********************************************************************
  Function: sapdbwa_DestroyDBCPool

  description: Destroy all resources for a sapdbwa_DBCPool.

  arguments:
    dbcPool [in] The pool to destroy.
    
  return value: void
*/    
void WINAPI sapdbwa_DestroyDBCPool( sapdbwa_DBCPoolP  dbcPool );
    

/*!**********************************************************************
  Function: sapdbwa_DBCConnect

  description: Get a connection from the pool.

  arguments:
    dbcPool     [in]  The DBC pool.
    dbc         [in]  The dbc handle.
    datasource  [in]  The ODBC datasource to use.
    driver      [in]  The ODBC driver name.
    serverNode  [in]  The host of the database.
    user        [in]  The username.
    password    [in]  Valid password for user.
    sqltrace    [in]  Prefix for SQL-trace file (may include absolute
                      path). An id and the extension '.pct' is appended
                      automatically. So every connection has its own
                      trace file.
    driverCompletion [in] Argument for SQLDriverConnect.
                          SQL_DRIVER_NOPROMPT, SQL_DRIVER_COMPLETE,
                          SQL_DRIVER_COMPLETE_REQUIRED or SQL_DRIVER_PROMPT.
                          See ODBC refernce for details.
    windowHandle [in] Handle of parent window. May be NULL if
                      driverCompletion is SQL_DRIVER_NOPROMPT.
    exclusive   [in]  Specifies how the connection may be shared:
                          <UL>
                          <LI> sapdbwa_DBC_EXCL: The returned connection is
                               currently unused and won't be returned
                               for another Connect until Disconnect is called.
                          <LI> sapdbwa_DBC_EXCL_AUTOCOMMIT_OFF: Only autocommit
                               off connections are exclusiv. Autocommit on
                               connections are shared.
                          <LI> sapdbwa_DBC_SHARED: The connection may be returned
                               by several Connects. Only if
                               #Connects-#Disconntecs is 0 the connection can be
                               used exclusive.
                          </UL>
                          
  return value: true for success, false if connect failed.

  note: If you use SQL_DRIVER_PROMPT, SQL_DRIVER_COMPLETE or
        SQL_DRIVER_COMPLETE_REQUIRED and the connect box of the ODBC
        driver pops up a connection from the pool cannot be reused.
  
*/
sapdbwa_Bool WINAPI sapdbwa_DBCConnect( sapdbwa_DBCPoolP       dbcPool,
                                        sapdbwa_DBCP           dbc,
                                        const char            *datasource,
                                        const char            *driver,
                                        const char            *serverNode,
                                        const char            *serverDb,
                                        const char            *user,
                                        const char            *password,
                                        const char            *sqltrace,
                                        sapdbwa_UInt2          driverCompletion,
                                        SQLHWND                windowHandle,
                                        sapdbwa_DBCExclType    exclusive );
    
sapdbwa_Bool WINAPI sapdbwa_DBCConnectWithServerDB( sapdbwa_DBCPoolP       dbcPool,
                                                    sapdbwa_DBCP           dbc,
                                                    const char            *datasource,
                                                    const char            *driver,
                                                    const char            *serverNode,
                                                    const char            *serverDb,
                                                    const char            *user,
                                                    const char            *password,
                                                    const char            *sqltrace,
                                                    sapdbwa_UInt2          driverCompletion,
                                                    SQLHWND                windowHandle,
                                                    sapdbwa_DBCExclType    exclusive );
    
    
/*!**********************************************************************
  Function: sapdbwa_DBCDisconnect

  description: Release connection. 

  arguments:
    dbcPool     [in]  The DBC pool.
    dbc         [in]  The connection handle.

    return value: true for success, false if the dbc was not connected
                  (ERR_DBC_NOT_ACTIVE_WD26) or an ODBC error occured.
*/
sapdbwa_Bool WINAPI sapdbwa_DBCDisconnect( sapdbwa_DBCPoolP  dbcPool,
                                    sapdbwa_DBCP      dbc );

/*!**********************************************************************

  EndChapter: sapdbwa_DBCPoolP

************************************************************************/
#if defined(__cplusplus)
}
#endif

#endif
