/*!**********************************************************************

  module:       sapdbwa_session.hpp (hwd04wases.h)

  -----------------------------------------------------------------------

  responsible:	Holger Bischoff
				Markus Özgen

  special area: SAPDB WebAgent Session Pool C++ Interface

  description:  Public description of WebAgent session pool C++ interface

  see also:     

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 2000-2005 SAP AG



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

#ifndef SAPDBWASESSION_CPP_H
#define SAPDBWASESSION_CPP_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <sapdbwa_session.h>	// C-API sessionpool
#include <sapdbwa_error.h>		// Error

/*!**********************************************************************

  Chapter: Overview

  description:
  Opening and releasing database connection are expensive operations.
  The usage of a database connection in web applications is typically
  limited to one request. All transactions are closed when the handling
  of one request is finished. Because opening and closing a database
  connection for every HTTTP request would be slow we provide a pool
  with open database connections that can be reused by further requests.

  This pooling is not limited to web applications. It may also be useful
  for other applications.
*/
/*!
  EndChapter: Overview
************************************************************************/

/*!**********************************************************************

  Class: sapdbwa_DbConnection

  description: A sapdbwa_DbConnection is the handle for a pooled connection.

  It may represent a ODBC- or a SQLClass database connection.

  Every connection may contain a user data structure, that contains
  statement handles for this connection or other stuff.
  
************************************************************************/

class sapdbwa_DbConnection {
public:
    sapdbwa_DbConnection() : conn(NULL), err(NULL) { };
    sapdbwa_DbConnection( sapdbwa_DbConnectionP connP );
    // copy constructor
    sapdbwa_DbConnection( const sapdbwa_DbConnection &conn );
    sapdbwa_DbConnection& operator=( const sapdbwa_DbConnection &conn );

    /*!**********************************************************************
      Function: DbDisconnect

      description: Put this connection back to the pool.

      After calling this method further usage of this connection is not
      allowed.

      arguments:

      return value: true for success, false for a fatal error.
    */
    sapdbwa_Bool DbDisconnect();


    /*!**********************************************************************
      Function: OdbcHandle

      description: Return the ODBC environment and connection-handle for a
                   ODBC connection.

      arguments:
        phenv [out] Pointer to environment handle.
        phdbc [out] Pointer to connection handle.

      return value: True for success, false if this is not a ODBC connection
      or if the connection is invalid.
    */
    sapdbwa_Bool OdbcHandle( SQLHENV      *phenv,
                           SQLHDBC      *phdbc ) const;


    /*!**********************************************************************
      Function: SqlSession

      description: Return the SqlSession for a SQLClass connection.

      arguments:
        sqlSession [out] Pointer to sqlSession.

      return value: True for success, false if this is not a SQLClass
      connection or if the connection is invalid.
    */
    sapdbwa_Bool SqlSession( void **sqlSession ) const;


    /*!**********************************************************************
      Function: GetUserData

      description: Return pointer to user data that was set with SetUserData.

      arguments:
      
      return value: A pointer to user data or NULL if no user data was set.
    */
    void* GetUserData() const;


    /*!**********************************************************************
      Function: SetUserData

      description: Store data in a connection.

      An existing user data pointer is overwritten.

      arguments:
        data       [in] A pointer to user data that is associated with the
                        sapdbwa_DbConnection.
        destructor [in] A pointer to the destructor function for data. This
                        destructor is called with data as argument when the
                        connection closed.

      return value: true
    */
    sapdbwa_Bool SetUserData( void          *data,
                            sapdbwa_Destructor  destructor );


    /*!**********************************************************************
      Function: CheckConnection

      description: Check if connection is still opened and try to reconnect
                   if necessary.

      arguments:
        nativeErr      [in]  The native error code as returned by SQLError,
                             SQLGetDiagRec or SQLGetDiagField.
        sqlState       [in]  The SQL-State as returned by SQLError.
        connectionDown [out] Set to true if connection was down.

      return value: true if connection is ok or connection was down and
                    could be reconnected. false if connection was down and
                    the reconnect failed.
    */
    sapdbwa_Bool CheckConnection( sapdbwa_Int4   nativeErr,
                                const char  *sqlState,
                                sapdbwa_Bool  *connectionDown );

    /*!**********************************************************************
      Function: GetErr

      description: Get error information from connection.

      arguments:

      return value:
      Reference to error object of the connection.
      This can be used to retrieve
      error information if a previous operation on the connection failed.
      See <A HREF="hwd06waerr.html">hwd06waerr.html</A> for a description
      of the error class.
    */
    const sapdbwa_Err& GetErr() const;

    sapdbwa_DbConnectionP GetConn() { return this->conn; };
private:
    sapdbwa_DbConnectionP conn;
    sapdbwa_Err           err;
}; /* class sapdbwa_DbConnection */

/*!**********************************************************************

  EndClass: sapdbwa_DbConnection

************************************************************************/

/*!**********************************************************************

  Class: sapdbwa_SessionPool

  description: Pool for open database connections.

  A sessionpool holds a number of identical (same serverdb, servernode,
  user, password, connection parameters) database connections. Connections
  are opened on demand. The total number of used connections may be limited.

  The sessionpool is designed to keep different kinds of connections.
  It may be initialized as an ODBC- or a SQLClass-sessionpool.

************************************************************************/

class sapdbwa_SessionPool {
public:
    /*!**********************************************************************
      Function: sapdbwa_SessionPool

      description: Constructor for sapdbwa_SessionPool. 

      arguments:
        name  [out]  The name of the pool. This is only used for tracing.

      return value: void
    */
    sapdbwa_SessionPool( char *name = NULL );
    sapdbwa_SessionPool( sapdbwa_SessionPoolP pool );
    ~sapdbwa_SessionPool();

    /*!**********************************************************************
      Function: DbConnect

      description: Get a sapdbwa_DbConnection from the pool.

      Before this method is called one of the methods Init or SqlInit
      have to be called to initialize the sessionpool.

      arguments:
        conn  [out]  The open db-connection.

      return value: true for success, false for failure. It may fail because
      of the following reasons:
      <UL>
      <LI> Memory allocation error (sapdbwa_ERR_MOMORY).
      <LI> Opening the connection failed (sapdbwa_ERR_ODBC or sapdbwa_ERR_SQLCLASS).
      <LI> The poolSize limit of open connections is reached and all
           connections are currently used (ERR_CANT_GET_DBCONNECTION_WD26).
      </UL>
    */
    sapdbwa_Bool DbConnect( sapdbwa_DbConnection &conn );


    /*!**********************************************************************
      Function: GetErr

      description: Get error information from pool.

      arguments:

      return value:
      Reference to error object of the pool. This can be used to retrieve
      error information if a previous operation on the pool failed.
      See <A HREF="hwd06waerr.html">hwd06waerr.html</A> for a description
      of the error class.
    */
    const sapdbwa_Err& GetErr() const;

    /*!**********************************************************************
      Function: GetServerNode

      description: Return the configured servernode.

      arguments:

      return value: Pointer to servernode string.
    */
    const char* GetServerNode() const;


    /*!**********************************************************************
      Function: GetServerDb

      description: Return the configured serverdb.

      arguments:

      return value: Pointer to serverdb string.
    */
    const char* GetServerDb() const;


    /*!**********************************************************************
      Function: GetUserName

      description: Return the configured username.

      arguments:

      return value: Pointer to username string.
    */
    const char* GetUserName() const;


    /*!**********************************************************************
      Function: GetPassword

      description: Return the configured password.

      arguments:

      return value: Pointer to password string.
    */
    const char* GetPassword() const;


    /*!**********************************************************************
      Function: Init

      description: Initialize pool for OBDC connections.

      arguments:
        datasource [in]  The ODBC datasource.
        drivername [in]  The name of the ODBC driver (only used if datasource
                         is empty or NULL).
        serverNode [in]  The servernode name (host of the database).
        serverDb   [in]  The name of the serverdb.
        user       [in]  The username.
        password   [in]  The password of user.
        poolSize   [in]  The maximal number of open connections.
        autocommit [in]  true: Autocommit on, false: autocommit off.
        sqltrace   [in]  Name of sqltrace file.

      return value: true for success false for failure. The function may fail
      because of the following reasons:
      <UL>
      <LI> Memory allocation error (sapdbwa_ERR_MOMORY).      
      <LI> One of the parameter strings is too long (sapdbwa_ERR_PARAM_TOO_LONG).
      <LI> The pool size is < 1 or > 128 (sapdbwa_ERR_POOLSIZE).
      </UL>
    */    
    sapdbwa_Bool Init( const char     *datasource,
                     const char     *driver,
                     const char     *serverNode,
                     const char     *serverDb,
                     const char     *user,
                     const char     *password,
                     sapdbwa_UInt4     poolSize,
                     sapdbwa_Bool      autocommit = true,
                     const char     *sqltrace = NULL );
    

    /*!**********************************************************************
      Function: SqlInit

      description: Initialize pool for SQLClass connections.

      arguments:
        serverNode [in]  The servernode name (host of the database).
        serverDb   [in]  The name of the serverdb.
        user       [in]  The username.
        password   [in]  The password of user.
        isolation  [in]  The isolation level (0,1,2,3).

      return value: true for success false for failure. The function may fail
      because of the following reasons:
      <UL>
      <LI> Memory allocation error (sapdbwa_ERR_MOMORY).      
      <LI> One of the parameter strings is too long (sapdbwa_ERR_PARAM_TOO_LONG).
      <LI> The pool size is < 1 or > 128 (sapdbwa_ERR_POOLSIZE).
      </UL>
    */    
    sapdbwa_Bool SqlInit( const char     *serverNode,
                        const char     *serverDb,
                        const char     *user,
                        const char     *password,
                        sapdbwa_Int4      isolation,
                        sapdbwa_UInt4     poolSize );
    
    sapdbwa_SessionPoolP GetPool() { return this->pool; };
private:
    sapdbwa_SessionPoolP pool;
    sapdbwa_Bool         ownPool;
    sapdbwa_Err          err;
}; /* class sapdbwa_SessionPool */

/*!**********************************************************************

  EndClass: sapdbwa_SessionPool

************************************************************************/

/************************************************************************

  Class: sapdbwa_SessionPool, inline methods

************************************************************************/

inline sapdbwa_SessionPool::sapdbwa_SessionPool( char *name )
    : pool(NULL), ownPool(true), err(NULL)
{
    this->pool = sapdbwa_CreateSessionPool( name );
    if (this->pool) {
        this->err.SetErr( sapdbwa_SesGetErr(pool) );
    };
}; // sapdbwa_SessionPool


inline sapdbwa_SessionPool::sapdbwa_SessionPool( sapdbwa_SessionPoolP pool )
    : pool(pool), ownPool(false), err(pool ? sapdbwa_SesGetErr(pool) : NULL)
{
}; // sapdbwa_SessionPool


inline sapdbwa_SessionPool::~sapdbwa_SessionPool()
{
    if (this->ownPool) {
        sapdbwa_DestroySessionPool( this->pool );
    }; // if
}; // ~sapdbwa_SessionPool


inline sapdbwa_Bool sapdbwa_SessionPool::DbConnect( sapdbwa_DbConnection &conn )
{
    sapdbwa_DbConnectionP c = sapdbwa_DbConnect( this->pool );

    conn = sapdbwa_DbConnection( c );
    
    return ( c != NULL );
}; // DbConnect


inline const sapdbwa_Err& sapdbwa_SessionPool::GetErr() const
{
    return this->err;
}; // GetErr


inline const char* sapdbwa_SessionPool::GetServerNode() const
{
    return sapdbwa_GetServerNode( this->pool );
}; // GetServerNode


inline const char* sapdbwa_SessionPool::GetServerDb() const
{
    return sapdbwa_GetServerDb( this->pool );
}; // GetServerDb


inline const char* sapdbwa_SessionPool::GetUserName() const
{
    return sapdbwa_GetUserName( this->pool );
}; // GetUserName


inline const char* sapdbwa_SessionPool::GetPassword() const
{
    return sapdbwa_GetPassword( this->pool );
}; // GetPassword


inline sapdbwa_Bool sapdbwa_SessionPool::Init( const char     *datasource,
                                        const char     *driver,
                                        const char     *serverNode,
                                        const char     *serverDb,
                                        const char     *user,
                                        const char     *password,
                                        sapdbwa_UInt4     poolSize,
                                        sapdbwa_Bool      autocommit,
                                        const char     *sqltrace )
{
    return sapdbwa_InitSessionPool( this->pool,
                               datasource,
                               driver,
                               serverNode,
                               serverDb,
                               user,
                               password,
                               poolSize,
                               autocommit,
                               sqltrace );
}; /* Init */


inline sapdbwa_Bool sapdbwa_SessionPool::SqlInit( const char     *serverNode,
                                           const char     *serverDb,
                                           const char     *user,
                                           const char     *password,
                                           sapdbwa_Int4      isolation,
                                           sapdbwa_UInt4     poolSize )
{
    return sapdbwa_InitSqlSessionPool( this->pool,
                                  serverNode,
                                  serverDb,
                                  user,
                                  password,
                                  isolation,
                                  poolSize );
}; /* SqlInit */

/*!**********************************************************************

  EndClass: sapdbwa_SessionPool

************************************************************************/

/************************************************************************

  Class: sapdbwa_DbConnection, inline methods

************************************************************************/

inline sapdbwa_DbConnection::sapdbwa_DbConnection( sapdbwa_DbConnectionP connP )
    : conn(connP), err(NULL)
{
    if (connP) {
        this->err.SetErr( sapdbwa_ConnGetErr(connP) );
    };
}; // sapdbwa_DbConnection


inline sapdbwa_DbConnection::sapdbwa_DbConnection( const sapdbwa_DbConnection &conn )
    : conn(conn.conn), err(NULL)
{    
    if (this->conn) {
        this->err.SetErr( sapdbwa_ConnGetErr(this->conn) );
    };
}; // sapdbwa_DbConnection


inline sapdbwa_DbConnection& sapdbwa_DbConnection::operator=( const sapdbwa_DbConnection &c )
{
    this->conn = c.conn;
    if (this->conn) {
        this->err.SetErr( sapdbwa_ConnGetErr(this->conn) );
    } else {
        this->err.SetErr( NULL );
    };

    return *this;
}; // operator=


inline sapdbwa_Bool sapdbwa_DbConnection::DbDisconnect()
{
    return (this->conn != NULL) ? sapdbwa_DbDisconnect( this->conn ) : true;
}; // DbDisconnect


inline sapdbwa_Bool sapdbwa_DbConnection::OdbcHandle( SQLHENV      *phenv,
                                               SQLHDBC      *phdbc ) const
{
    return sapdbwa_OdbcHandle( this->conn, phenv, phdbc );
}; // OdbcHandle


inline sapdbwa_Bool sapdbwa_DbConnection::SqlSession( void **sqlSession ) const
{
    return sapdbwa_SqlHandle( this->conn, sqlSession );
}; // SqlSession


inline void* sapdbwa_DbConnection::GetUserData() const
{
    return sapdbwa_GetUserData( this->conn );
}; // GetUserData


inline sapdbwa_Bool sapdbwa_DbConnection::SetUserData( void          *data,
                                                sapdbwa_Destructor  destructor )
{
    return sapdbwa_SetUserData( this->conn, data, destructor );
}; // SetUserData


inline sapdbwa_Bool sapdbwa_DbConnection::CheckConnection( sapdbwa_Int4   nativeErr,
                                                    const char  *sqlState,
                                                    sapdbwa_Bool  *connDown )
{
    return sapdbwa_CheckConnection( this->conn,
                               nativeErr, sqlState, connDown );
}; // CheckConnection


inline const sapdbwa_Err& sapdbwa_DbConnection::GetErr() const
{
    return this->err;
}; // GetErr


/*!**********************************************************************

  EndClass: sapdbwa_DbConnection

************************************************************************/

#endif

