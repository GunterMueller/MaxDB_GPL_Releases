/*!**********************************************************************

  module: hwd07dbc.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Özgen

  special area: WebAgent

  description:  Pooled ODBC connections; C++ interface

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1999-2005 SAP AG



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

#ifndef HWD07DBC_H
#define HWD07DBC_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hwd06waerr.h"
#include "hwd08dbc.h"  // C-API

/*!**********************************************************************

  Chapter: Overview

  description: Pooled ODBC connections

  Because building up a database connection is very expensive we provide
  a pool for ODBC connections with individual connection parameters
  (user,serverdb,...). The MS Driver Manager already supports such a
  pooling but it is only available under Windows and we also want to
  store user data together with the pooled connection which is not
  possible with the driver manager pooling.

  The following example demonstrates the usage of the classes sapdbwa_DBC and
  sapdbwa_DBCPool to get a pooled connection.

  code:
  {
      // create pool with timeout of 60 seconds
      sapdbwa_DBCPool dbcPool( 60 );
      sapdbwa_DBC     dbc();

      // set autocommit off
      dbc.SetAttr( SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF );
      // get connection from pool
      if (dbcPool.Connect( dbc,
                           "WebAgent", "",
                           "p29744",   "db72",
                           "WebAgent", "WebAgent" )) {
          // connect ok
          HDBC = dbc.HDBC();
          // use ODBC to do some SQL ...
          dbcPool.Disconnect( dbc );
      } else {
          // connect failed
      }; // else
  };
*/
/*!
  EndChapter: Overview
************************************************************************/

/*!**********************************************************************

  Class: sapdbwa_DBC

  description:
  A sapdbwa_DBC is the connection handle used to connect through the sapdbwa_DBCPool.
  It represents an ODBC connection with own HENV and HDBC handles,
  connection parameters and attributes.

************************************************************************/

class sapdbwa_DBC {
public:
    /*!**********************************************************************
      Function: sapdbwa_DBC

      description: Constructor for sapdbwa_DBC

      arguments:

      return value: void
    */
    sapdbwa_DBC();

    ~sapdbwa_DBC();

    /*!**********************************************************************
      Function: CheckDBC

      description: Check if connection is still opened and try to reconnect
                   if necessary.

      arguments:
        nativeErr      [in]  The native error code as returned by SQLError,
                             SQLGetDiagRec or SQLGetDiagField.
        sqlState       [in]  The ODBC SQL-State as returned by SQLError.
        connectionDown [out] Set to true if connection was down.

      return value: true if connection is ok or connection was down and
                    could be reconnected. false if connection was down and
                    the reconnect failed.
    */
    sapdbwa_Bool CheckDBC( sapdbwa_Int4       nativeErr,
                     const char  *sqlState,
                     sapdbwa_Bool      *connectionDown );


    /*!**********************************************************************
      Function: HDBC

      description: Get ODBC connection handle.

      arguments:

      return value: The ODBC connection handle.
    */
    SQLHDBC HDBC();


    /*!**********************************************************************
      Function: HENV

      description: Get ODBC environment handle.

      arguments:

      return value: The ODBC environment handle.
    */
    SQLHENV HENV();


    /*!**********************************************************************
      Function: TableContainer

      description: Get name of table container.

      arguments:

      return value: The name of table container.
    */
    const char* TableContainer();
    const char* TableInode();
    sapdbwa_Bool SchemaInfo();


    /*!**********************************************************************
      Function: SetAttr

      description: Set connection attribute.

      This method can only be called before Connect is called.

      arguments:
        attr  [in]  The ODBC connection attribute. The following attributes
                    are supported:
                    <UL>
                    <LI> SQL_SQLMODE
                    <LI> SQL_ATTR_AUTOCOMMIT
                    <LI> SQL_TXN_ISOLATION
                    <LI> SQL_ATTR_SESSION_TIMEOUT
                    </UL>
        value [in]  The attribute value. See ODBC reference for valid values.

      return value: true for success false for failure.
    */
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
    sapdbwa_Bool SetAttr( SQLINTEGER attr, SQLPOINTER value );


    /*!**********************************************************************
      Function: GetAttr

      description: Get connection attribute.

      arguments:
        attr  [in]  The ODBC connection attribute. The following attributes
                    are supported:
                    <UL>
                    <LI> SQL_SQLMODE
                    <LI> SQL_ATTR_AUTOCOMMIT
                    <LI> SQL_TXN_ISOLATION
                    </UL>
        value [in]  Buffer for the attribute value.

      return value: true for success false for failure.
    */
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
    sapdbwa_Bool GetAttr( SQLINTEGER attr, SQLPOINTER *pvalue );

    
	/*!**********************************************************************
	  Function: AddAttr

	  description:	Adds a connection attribute to the extended attribute
					list. If the attribute already exists, its value
					will be updated.
					This attributes will only be used if they have been set
					before connect is called.

	  arguments:
	  attr  [in]  The ODBC connection attribute.
	  value [in]  The attribute value. See ODBC reference for valid values.

	  return value: true for success false for failure.
	*/
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
	sapdbwa_Bool AddAttr( SQLINTEGER attr,
						  SQLPOINTER value,
                          SQLINTEGER stringLength );

    
	/*!**********************************************************************
	  Function: SelectAttr

	  description:	Selects a connection attribute from the extended
					attribute list.

	  arguments:
	  attr  [in]        The ODBC connection attribute you are looking for.
	  pvalue[out]       Attribute value.
	  stringLength[out] String length.

	  return value: true for success false for failure.
	*/
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
	sapdbwa_Bool SelectAttr( SQLINTEGER   attr,
							 SQLPOINTER  *pvalue,
                             SQLINTEGER  *stringLength );

    
	/*!**********************************************************************
	  Function: ClearAttrList

	  description:	Clears the list of connection attributes.

	  arguments:

	  return value: true for success false for failure.
	*/
	sapdbwa_Bool ClearAttrList();

    
    /*!**********************************************************************
      Function: GetErr

      description: Get error information from dbc.

      arguments:

      return value:
      Reference to error object of the dbc. This can be used to retrieve
      error information if a previous operation on the dbc failed.
      See <A HREF="hwd06waerr.html">hwd06waerr.html</A> for a description
      of the error class.
    */
    const sapdbwa_Err& GetErr() const;


    /*!**********************************************************************
      Function: GetUser

      description: Get username of connection.

      arguments:
        buf     [in/out]  Buffer for username string.
        bufLen  [in]      Length of buf.
        
      return value: True if dbc is connected and buf was big enough else
                    false. When false is returned the associated errId
                    is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */
    sapdbwa_Bool GetUser( unsigned char *buf, sapdbwa_UInt4 bufLen );


    /*!**********************************************************************
      Function: GetPassword

      description: Get password of connection.

      arguments:
        buf     [in/out]  Buffer for password string.
        bufLen  [in]      Length of buf.
        
      return value: True if dbc is connected and buf was big enough else
                    false. When false is returned the associated errId
                    is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */
    sapdbwa_Bool GetPassword( unsigned char *buf, sapdbwa_UInt4 bufLen );


    /*!**********************************************************************
      Function: GetServerDb

      description: Get serverdb of connection.

      arguments:
        buf     [in/out]  Buffer for serverdb string.
        bufLen  [in]      Length of buf.
        
      return value: True if dbc is connected and buf was big enough else
                    false. When false is returned the associated errId
                    is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */
    sapdbwa_Bool GetServerDb( unsigned char *buf, sapdbwa_UInt4 bufLen  );

    
    /*!**********************************************************************
      Function: GetServerNode

      description: Get servernode of connection.

      arguments:
        buf     [in/out]  Buffer for servernode string.
        bufLen  [in]      Length of buf.
        
      return value: True if dbc is connected and buf was big enough else
                    false. When false is returned the associated errId
                    is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */    
    sapdbwa_Bool GetServerNode( unsigned char *buf,
                          sapdbwa_UInt4        bufLen  );

    
    /*!**********************************************************************
      Function: GetDatasource

      description: Get datasource of connection.

      arguments:
        buf     [in/out]  Buffer for datasource string.
        bufLen  [in]      Length of buf.
        
      return value: True if dbc is connected and buf was big enough else
                    false. When false is returned the associated errId
                    is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */    
    sapdbwa_Bool GetDatasource( unsigned char *buf,
                          sapdbwa_UInt4        bufLen  );

    
    /*!**********************************************************************
      Function: GetDriver

      description: Get driver of connection.

      arguments:
        buf     [in/out]  Buffer for ddriver string.
        bufLen  [in]      Length of buf.
        
      return value: True if dbc is connected and buf was big enough else
                    false. When false is returned the associated errId
                    is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */    
    sapdbwa_Bool GetDriver( unsigned char *buf,
                      sapdbwa_UInt4        bufLen  );

    
    /*!**********************************************************************
      Function: GetSqlTrace

      description: Get sqltrace filename of connection.

      arguments:
        buf     [in/out]  Buffer for sqltrace string.
        bufLen  [in]      Length of buf.
        
      return value: True if dbc is connected and buf was big enough else
                    false. When false is returned the associated errId
                    is ERR_BUFFER_TOO_SMALL_WD26 or ERR_DBC_NOT_ACTIVE_WD26.
    */    
    sapdbwa_Bool GetSqlTrace( unsigned char *buf,
                        sapdbwa_UInt4        bufLen  );

    
    /*!**********************************************************************
      Function: GetUserData
      
      description: Get user data pointer from dbc handle that was previously
                   set with sapdbwa_DBCSetUserData.
      
      arguments:
      
      return value: The user data pointer or NULL if no pointer was set with
                    sapdbwa_DBCSetUserData.
    */    
    void* GetUserData();


    /*!**********************************************************************
      Function: SetUserData
  
      description: Set user data pointer for the dbc handle.

      arguments:
      data           [in]  User data pointer.
      destructor     [in]  Destructor function that is called when the
                           connection is released.
                       
      return value: The user data pointer or NULL if no pointer was set with
                    sapdbwa_DBCSetUserData.
    */    
    sapdbwa_Bool SetUserData( void             *data,
                        sapdbwa_Destructor     destructor );
    
    sapdbwa_DBCP GetDBC();
private:
    sapdbwa_DBCP dbc;
    sapdbwa_Err  err;
}; /* class sapdbwa_DBC */

/*!**********************************************************************

  EndClass: sapdbwa_DBC

************************************************************************/

/*!**********************************************************************

  Class: sapdbwa_DBCPool

  description:
  A sapdbwa_DBCPool provides functions to connect and disconnect sapdbwa_DBC handles.
  The underlying ODBC connections are pooled and reused, if connect
  parameters match.
  
************************************************************************/

class sapdbwa_DBCPool {
public:
    /*!**********************************************************************
      Function: sapdbwa_DBCPool

      description: Constructor for sapdbwa_DBCPool.

      arguments:
        timeOut  [in]  Timeout in seconds for connections in this pool.
                       If a connection is not used for this time it is
                       closed implicitely and removed from the pool.

      return value: void
    */
    sapdbwa_DBCPool( sapdbwa_Int4 timeOut = -1 );

    ~sapdbwa_DBCPool();

    /*!**********************************************************************
      Function: Connect

      description: Get a connection from the pool.

      arguments:
        dbc         [in]  The connect handle.
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
                          </UL>hwd08dbc.h
                          
                          
      return value: true for success, false if connect failed.

      note: If you use SQL_DRIVER_PROMPT, SQL_DRIVER_COMPLETE or
            SQL_DRIVER_COMPLETE_REQUIRED and the connect box of the ODBC
            driver pops up a connection from the pool cannot be reused.
    */
    sapdbwa_Bool Connect( sapdbwa_DBC         &dbc,
                        const char     *datasource,
                        const char     *driver,
                        const char     *serverNode,
                        const char     *serverDb,
                        const char     *user,
                        const char     *password,
                        const char     *sqltrace = NULL,
                        sapdbwa_UInt2     driverCompletion = 0,
                        SQLHWND         windowHandle = 0,
                        sapdbwa_DBCExclType  exclusive = sapdbwa_DBC_EXCL );

	sapdbwa_Bool ConnectWithServerDB( sapdbwa_DBC         &dbc,
                                      const char          *datasource,
                                      const char          *driver,
                                      const char          *serverNode,
                                      const char          *serverDb,
                                      const char          *user,
                                      const char          *password,
                                      const char          *sqltrace,
                                      sapdbwa_UInt2        driverCompletion,
                                      SQLHWND              windowHandle,
                                      sapdbwa_DBCExclType  exclusive );

    /*!**********************************************************************
      Function: Disconnect

      description: Release connection.

      arguments:
        dbc         [in]  The connection handle.

      return value: true for success, false if the dbc was not connected or
      an ODBC error occured.
    */
    sapdbwa_Bool Disconnect( sapdbwa_DBC &dbc );

    sapdbwa_DBCPoolP GetPool() { return this->pool; };
private:
    sapdbwa_DBCPoolP pool;
}; /* class sapdbwa_DBCPool */

/*!**********************************************************************

  EndClass: sapdbwa_DBCPool

************************************************************************/

/************************************************************************

  inline methods

************************************************************************/

/////////////////
// sapdbwa_DBC
/////////////////

inline sapdbwa_DBC::sapdbwa_DBC()
    : dbc(NULL), err(NULL)
{
    this->dbc = sapdbwa_CreateDBC();
    this->err.SetErr( sapdbwa_DBCGetErr( this->dbc) );
}; // sapdbwa_DBC


inline sapdbwa_DBC::~sapdbwa_DBC()
{
    sapdbwa_DestroyDBC( this->dbc );
}; // ~sapdbwa_DBC


inline sapdbwa_Bool sapdbwa_DBC::CheckDBC( sapdbwa_Int4       nativeErr,
                                    const char      *sqlState,
                                    sapdbwa_Bool      *connectionDown )
{
    return sapdbwa_CheckDBC( this->dbc, nativeErr, sqlState, connectionDown );
}; // CheckDBC


inline SQLHDBC sapdbwa_DBC::HDBC()
{
    return sapdbwa_DBCHandle( this->dbc );
}; // DBCHandle


inline SQLHENV sapdbwa_DBC::HENV()
{
    return sapdbwa_DBCEnv( this->dbc );
}; // DBCHandle


inline const char* sapdbwa_DBC::TableContainer()
{
    return sapdbwa_GetTableContainer(this->dbc);
};

inline const char* sapdbwa_DBC::TableInode()
{
    return sapdbwa_GetTableInode(this->dbc);
};

inline sapdbwa_Bool sapdbwa_DBC::SchemaInfo()
{
    return sapdbwa_GetSchemaInfo(this->dbc);
};

    
/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
inline sapdbwa_Bool sapdbwa_DBC::SetAttr( SQLINTEGER attr, SQLPOINTER value )
{
    return sapdbwa_DBCSetAttr( this->dbc, attr, value );
}; // SetAttr


/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
inline sapdbwa_Bool sapdbwa_DBC::GetAttr( SQLINTEGER attr, SQLPOINTER *pvalue )
{
    return sapdbwa_DBCGetAttr( this->dbc, attr, pvalue );
}; // SetAttr

    
/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
inline sapdbwa_Bool sapdbwa_DBC::AddAttr( SQLINTEGER attr, SQLPOINTER value, SQLINTEGER stringLength )
{
    return sapdbwa_DBCAddAttr( this->dbc, attr, value, stringLength );
}; // AddAttr

    
/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844 */
inline sapdbwa_Bool sapdbwa_DBC::SelectAttr( SQLINTEGER attr, SQLPOINTER *pvalue, SQLINTEGER *stringLength )
{
    return sapdbwa_DBCSelectAttr( this->dbc, attr, pvalue, stringLength );
}; // SelectAttr

    
inline sapdbwa_Bool sapdbwa_DBC::ClearAttrList()
{
    return sapdbwa_DBCClearAttrList( this->dbc );
}; // ClearAttrList


inline const sapdbwa_Err& sapdbwa_DBC::GetErr() const
{
    return this->err;
}; // GetErr


inline sapdbwa_DBCP sapdbwa_DBC::GetDBC()
{
    return this->dbc;
}; // GetDBC


inline void* sapdbwa_DBC::GetUserData()
{
    return sapdbwa_DBCGetUserData( this->dbc );
}; // GetUserData


inline sapdbwa_Bool sapdbwa_DBC::SetUserData( void             *data,
                                   sapdbwa_Destructor     destructor )
{
    return sapdbwa_DBCSetUserData( this->dbc, data, destructor );
}; // SetUserData


inline sapdbwa_Bool sapdbwa_DBC::GetUser( unsigned char *buf,
                               sapdbwa_UInt4    bufLen  )
{
    return sapdbwa_DBCGetUser( this->dbc, buf, bufLen );
}; // GetUser


inline sapdbwa_Bool sapdbwa_DBC::GetPassword( unsigned char *buf,
                                   sapdbwa_UInt4    bufLen  )
{
    return sapdbwa_DBCGetPassword( this->dbc, buf, bufLen );
}; // GetPassword

inline sapdbwa_Bool sapdbwa_DBC::GetDriver( unsigned char *buf,
                                   sapdbwa_UInt4    bufLen  )
{
    return sapdbwa_DBCGetDriver( this->dbc, buf, bufLen );
}; // GetDriver

inline sapdbwa_Bool sapdbwa_DBC::GetDatasource( unsigned char *buf,
                                   sapdbwa_UInt4    bufLen  )
{
    return sapdbwa_DBCGetDatasource( this->dbc, buf, bufLen );
}; // GetDatasource


inline sapdbwa_Bool sapdbwa_DBC::GetServerNode( unsigned char *buf,
                                     sapdbwa_UInt4    bufLen  )
{
    return sapdbwa_DBCGetServerNode( this->dbc, buf, bufLen );
}; // GetServerNode


inline sapdbwa_Bool sapdbwa_DBC::GetServerDb( unsigned char  *buf,
                                   sapdbwa_UInt4     bufLen  )
{
    return sapdbwa_DBCGetServerDb( this->dbc, buf, bufLen );
}; // GetServerDb

/////////////////
// sapdbwa_DBCPool
/////////////////

inline sapdbwa_DBCPool::sapdbwa_DBCPool( sapdbwa_Int4 timeOut )
    : pool(NULL)
{
    pool = sapdbwa_CreateDBCPool( timeOut);
}; // sapdbwa_DBCPool

/*===========================================================================*/

inline sapdbwa_Bool sapdbwa_DBCPool::Connect( sapdbwa_DBC          &dbc,
                                   const char     *datasource,
                                   const char     *driver,
                                   const char     *serverNode,
                                   const char     *serverDb,
                                   const char     *user,
                                   const char     *password,
                                   const char     *sqltrace,
                                   sapdbwa_UInt2     driverCompletion,
                                   SQLHWND         windowHandle,
                                   sapdbwa_DBCExclType  exclusive )
{
    return sapdbwa_DBCConnect( this->pool,
                          dbc.GetDBC(),
                          datasource, driver,
                          serverNode, serverDb,
                          user, password,
                          sqltrace,
                          driverCompletion,
                          windowHandle,
                          exclusive );
}; // Connect

/*===========================================================================*/

inline sapdbwa_Bool sapdbwa_DBCPool::ConnectWithServerDB( sapdbwa_DBC         &dbc,
                                                          const char          *datasource,
                                                          const char          *driver,
                                                          const char          *serverNode,
                                                          const char          *serverDb,
                                                          const char          *user,
                                                          const char          *password,
                                                          const char          *sqltrace,
                                                          sapdbwa_UInt2        driverCompletion,
                                                          SQLHWND              windowHandle,
                                                          sapdbwa_DBCExclType  exclusive )
{
    return sapdbwa_DBCConnectWithServerDB( this->pool, dbc.GetDBC(),
                                           datasource, driver,
                                           serverNode, serverDb, user, password,
                                           sqltrace, driverCompletion, windowHandle, exclusive );
};

/*===========================================================================*/

inline sapdbwa_Bool sapdbwa_DBCPool::Disconnect( sapdbwa_DBC &dbc )
{
    return sapdbwa_DBCDisconnect( this->pool, dbc.GetDBC() );
}; // Disconnect


inline sapdbwa_DBCPool::~sapdbwa_DBCPool()
{
    if (this->pool != NULL) {
        sapdbwa_DestroyDBCPool( this->pool );
        this->pool = NULL;
    }; // if
}; // ~sapdbwa_DBCPool

#endif
