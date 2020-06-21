/*!
  \file    Loader_Connection.cpp
  \author  SteffenS
  \brief   Implements the Loader connection class

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

\endif
*/
#include "Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "Loader/Loader_ErrorCode.hpp"
#include "Loader/Loader_ErrorMsgHandler.hpp"
#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_Loader.hpp"
#include "Interfaces/SQLDBC/SQLDBC.h"


#include "SAPDBCommon/SAPDB_string.h"       // SAPDB_memcpy



/*!
    @brief constructor
*/
Loader_Connection::Loader_Connection(SQLDBC_Environment*    SQLDBC_Environment,
                                     SQLDBC_IRuntime*       LoaderRuntime):
m_pError(NULL),
m_UseEstablishedConnection(SAPDB_FALSE)
{
    m_pSQLDBCEnv     = SQLDBC_Environment;
    m_pLoaderRuntime = LoaderRuntime;

    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, Loader_Connection, m_pLoaderRuntime->m_pClientTrace);

    initializeMembers();

    m_pError = new Loader_ErrorMsgHandler;
    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Error handler created" << endl;
}
// Loader_Connection()


/*!
    @brief constructor
*/
Loader_Connection::Loader_Connection(SQLDBC_Environment*  SQLDBC_Environment,
                                     SQLDBC_IRuntime*     LoaderRuntime,
                                     SQLDBC_Connection*   pSQLDBCConnection):
m_pError(NULL),
m_UseEstablishedConnection(SAPDB_FALSE)
{
    m_pSQLDBCEnv     = SQLDBC_Environment;
    m_pLoaderRuntime = LoaderRuntime;

    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, Loader_Connection, m_pLoaderRuntime->m_pClientTrace);

    initializeMembers();    

    this->m_pSQLDBCConnection = pSQLDBCConnection;

    this->setSessionInformation();

    m_pError = new Loader_ErrorMsgHandler;
    m_UseEstablishedConnection = SAPDB_TRUE;
    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Error handler created" << endl;
}
// Loader_Connection()


/*!
    @brief destructor
*/
Loader_Connection::~Loader_Connection()
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, ~Loader_Connection, m_pLoaderRuntime->m_pClientTrace);

    close();

    if ( (SAPDB_FALSE == m_UseEstablishedConnection) && (NULL != m_pSQLDBCConnection) )   // release SQLDBC connection object
    {
        m_pSQLDBCEnv->releaseConnection(m_pSQLDBCConnection);
    }

    if (NULL != m_pError)
    {
        delete m_pError;
        m_pError = NULL;
    }
}
// ~Loader_Connection()


/*!
    @brief Connects to a given Database
*/
LOADER_Retcode
Loader_Connection::connect(const LOADER_Char *servernode,
                           const LOADER_Char *serverdb,
                           const LOADER_Char *username,
                           const LOADER_Char *password)
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, connect, m_pLoaderRuntime->m_pClientTrace);

    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "connection parameters: "
    //                                                            << "db computer = \"" << servernode << "\""
    //                                                            << "db name = \"" << serverdb << "\""
    //                                                            << "user name = \"" << username << "\""
    //                                                            << "pw = \"" << password << "\""
    //                                                            << endl;

    LOADER_Retcode rcLOA = LOADER_OK;

    // prevent 2 connections in 1 single connection object
    if (0 != m_ConnectionID)
    {
        m_pError->setLoaderError(LOA_ERR_CONNECTION_ALREADY_OPEN);
        return LOADER_NOT_OK;
    }

    //*
    //* Create a new connection object and open a session to the database.
    //*
    m_pSQLDBCConnection = m_pSQLDBCEnv->createConnection();
    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "SQLDBC connection object created" << endl;

    SQLDBC_Retcode rc;

    // Set some special Loader connect attributes additionally to the attributes set by using the overloaded connect function
    m_ConnectProperties.setProperty("APPLICATION", "LOA");

    //*
    //*     Open connection over SQLDBC
    //*
    rc = m_pSQLDBCConnection->connect(servernode, serverdb, username, password, m_ConnectProperties);
    if(SQLDBC_OK != rc)
    {
        m_pError->setLoaderError(LOA_ERR_SQLDBC_CONNECT_FAILED, m_pSQLDBCConnection->error().getErrorText());
        //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "Error connecting via SQLDBC. " << m_pSQLDBCConnection->error().getErrorText() << endl;
        rcLOA = LOADER_NOT_OK;
    }
    else
    {
        //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "SQLDBC connection established" << endl;

        LOADER_UInt4 _len = 0;


        // Get some parameters (mainly user name for later used default values) - depending on the kind of connect: XUSER or not
        const LOADER_Char* pszValue = m_ConnectProperties.getProperty("KEY");
        if (NULL != pszValue)
        {
            LOADER_Bool rcBOOL = LOADER_FALSE;
            
            LOADER_Int4 hostnamelen = 256;
            LOADER_Int4 dbnamelen = 256;
            LOADER_Int4 usernamelen = 256;
            m_pHostName = new LOADER_Char[hostnamelen];
            m_pDBName = new LOADER_Char[dbnamelen];
            m_UserName = new LOADER_Char[usernamelen];
            LOADER_Int4 isolationlevel;
            LOADER_Int4 cachelimit;
            LOADER_Int4 sqlmode;
            LOADER_Int4 timeout;
            SQLDBC_StringEncoding usernameEncoding = SQLDBC_StringEncodingAscii;

            SQLDBC_IRuntime::Error rtError;

            rcBOOL = this->m_pLoaderRuntime->checkKeyOption(pszValue,
                                                            m_pHostName, hostnamelen, 
                                                            m_pDBName, dbnamelen,
                                                            isolationlevel,
                                                            cachelimit,
                                                            sqlmode,
                                                            timeout,
                                                            m_UserName, usernamelen,
                                                            usernameEncoding,
                                                            rtError);

            if (LOADER_FALSE == rcBOOL)
            {
                m_pError->setLoaderError(LOA_ERR_GET_USERNAME_FAILED, rtError.errortext);
                rcLOA = LOADER_NOT_OK;
            }
        }
        else
        {
            _len = strlen(servernode);
            m_pHostName = new LOADER_Char[_len + sizeof(char)];
            SAPDB_memcpy(m_pHostName, servernode, _len);
            m_pHostName[_len] = 0;

            _len = strlen(serverdb);
            m_pDBName = new LOADER_Char[_len + sizeof(char)];
            SAPDB_memcpy(m_pDBName, serverdb, _len);
            m_pDBName[_len] = 0;

            _len = strlen(username);
            m_UserName = new LOADER_Char[_len + sizeof(char)];
            SAPDB_memcpy(m_UserName, username, _len);
            m_UserName[_len] = 0;
        }

        if (LOADER_OK == rcLOA)
        {
            rcLOA = setSessionInformation();
//TODOTODO FEHLERBEHANDLUNG
        }
    }

    //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "rc connect: " << rcLOA << endl;
    return rcLOA;
}
// connect()



LOADER_Retcode
Loader_Connection::connect(const LOADER_Char *servernode, 
                           const LOADER_Char *serverdb,
                           const LOADER_Char *username,
                           const LOADER_Char *password,
                           const Loader_ConnectProperties* properties)
{
    // Wenn key property nicht gesetzt ist, einfach connect aufrufen und vorher die properties setzen
    LOADER_Retcode rcLOA = LOADER_OK;

    const LOADER_Char* pszValue = NULL;
    pszValue = properties->m_prop->getProperty("KEY");
    if (NULL != pszValue)
    {
        m_ConnectProperties.setProperty("KEY", pszValue);
    }
    rcLOA = connect(servernode, serverdb, username, password);

    return rcLOA;
}


LOADER_Retcode
Loader_Connection::disconnect()
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, disconnect, m_pLoaderRuntime->m_pClientTrace);

    return this->close();
}
// disconnect()


LOADER_Retcode
Loader_Connection::close()
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, close, m_pLoaderRuntime->m_pClientTrace);

    LOADER_Retcode rc = LOADER_OK;


    // Is there still a FASTLOAD command that must be released?
    if (NULL != m_pFLCmdObj)
    {
        this->releaseFastloadCommand(m_pFLCmdObj);
    }

    this->rollback();

    // Close the open connection
    if ( (SAPDB_FALSE == m_UseEstablishedConnection) &&  (LOADER_TRUE == m_pSQLDBCConnection->isConnected()) )
    {
        m_pSQLDBCConnection->close();
    }

    if (NULL != m_pHostName)
    {
        delete [] m_pHostName;
        m_pHostName = NULL;
    }

    if (NULL != m_pDBName)
    {
        delete [] m_pDBName;
        m_pDBName = NULL;
    }

    if (NULL != m_UserName)
    {
        delete [] m_UserName;
        m_UserName = NULL;
    }

    if (NULL != m_SYSDBADefault)
    {
        delete [] m_SYSDBADefault;
        m_SYSDBADefault = NULL;
    }
    
    if (NULL != m_pszUserGroupDefault)
    {
        delete [] m_pszUserGroupDefault;
        m_pszUserGroupDefault = NULL;
    }

    this->initializeMembers();

    return rc;
}
// close()


LOADER_Retcode
Loader_Connection::rollback()
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, rollback, m_pLoaderRuntime->m_pClientTrace);

    LOADER_Retcode rc = LOADER_OK;

    if (0 != this->m_ConnectionID)
    {
        // Send ROLLBACK command
        SQLDBC_Retcode rcSQLDBC = m_pSQLDBCConnection->rollback();
        if (SQLDBC_OK != rcSQLDBC) {
            m_pError->setLoaderError(LOA_ERR_SQL_CMD_EXECUTION_FAILED, "ROLLBACK WORK RELEASE", m_pSQLDBCConnection->error().getErrorText());
            rc = LOADER_NOT_OK;
        }
    }

    return rc;
}
// rollback()


const LOADER_Char*
Loader_Connection::getErrorText() const
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, getErrorText, m_pLoaderRuntime->m_pClientTrace);

    return m_pError->getErrorText();
}


LOADER_Int4
Loader_Connection::getErrorCode()
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, getErrorCode, m_pLoaderRuntime->m_pClientTrace);

    return m_pError->getErrorCode();
}


LOADER_Int4
Loader_Connection::getKernelVersion() const
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, getKernelVersion, m_pLoaderRuntime->m_pClientTrace);

    return m_KernelVersion;
}
// getKernelVersion()


LOADER_DateTimeFormat
Loader_Connection::getDateTimeFormat() const
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, getDateTimeFormat, m_pLoaderRuntime->m_pClientTrace);

    return (LOADER_DateTimeFormat)(m_pSQLDBCConnection->getDateTimeFormat());
}
// getDateTimeFormat()


void
Loader_Connection::setSQLMode(LOADER_SQLMode sqlmode)
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, setSQLMode, m_pLoaderRuntime->m_pClientTrace);

    m_SQLMode = sqlmode;
}
// setSQLMode()


LOADER_Bool
Loader_Connection::isConnected() const
{
    return this->m_pSQLDBCConnection->isConnected();
}
// isConnected()


void
Loader_Connection::setAutoCommit(LOADER_Bool autocommit)
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, setAutoCommit, m_pLoaderRuntime->m_pClientTrace);

    m_pSQLDBCConnection->setAutoCommit(autocommit);
}
// setAutoCommit()


LOADER_Bool
Loader_Connection::getAutoCommit() const
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, getAutoCommit, m_pLoaderRuntime->m_pClientTrace);

    return m_pSQLDBCConnection->getAutoCommit();
}
// getAutoCommit()


Loader_FastloadCommand*
Loader_Connection::createFastloadCommand()
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, createFastloadCommand, m_pLoaderRuntime->m_pClientTrace);

    if (NULL != m_pFLCmdObj)
    {
        m_pError->setLoaderError(LOA_ERR_TOO_MANY_FLCMD_OBJECTS);
        return NULL;
    }
    
    m_pFLCmdObj = new Loader_FastloadCommand(m_pLoaderRuntime, this);

    return m_pFLCmdObj;
}
// createFastloadCommand()


void
Loader_Connection::releaseFastloadCommand(Loader_FastloadCommand* pFastloadCommand)
{
    //DBUG_APPL_CONTEXT_METHOD_ENTER(Loader_Connection, releaseFastloadCommand, m_pLoaderRuntime->m_pClientTrace);

    pFastloadCommand->deleteFastloadCommand();

    // pFastloadCommand and m_pFLCmdObj should be the same
    m_pFLCmdObj = NULL;
}
// releaseFastloadCommand()


void
Loader_Connection::initializeMembers()
{
    m_pSQLDBCConnection     = NULL;
    m_pHostName             = NULL;
    m_pDBName               = NULL;
    m_UserName              = NULL;
    m_ConnectionID          = 0;
    m_PacketCount           = 0;
    m_PacketSize            = 0;
    m_PageSize              = 8192;
    m_SQLMode               = LOADER_SQLMODE_INTERNAL;
    m_DBIsUnicode           = LOADER_FALSE;
    m_PacketList[0]         = NULL;
    m_PacketList[1]         = NULL;
    m_pFLCmdObj             = NULL;
}
// initializeMembers()


LOADER_Retcode
Loader_Connection::setSessionInformation()
{
    LOADER_Retcode rc = LOADER_OK;


    LOADER_Bool rcBOOL = LOADER_TRUE;
    
    LOADER_Char             pszTmpUserGroupDefault[LOADER_KNL_ID_SIZE + sizeof(LOADER_Char)];
    LOADER_Char             pszTmpSYSDBADefault[LOADER_KNL_ID_SIZE + sizeof(LOADER_Char)];
    pszTmpUserGroupDefault[0] = 0;
    pszTmpSYSDBADefault[0]    = 0;

    LOADER_UInt4            _len = 0;
    LOADER_Int4             _packetListSize = 0;
    SQLDBC_StringEncoding   _userEncoding;
    SAPDB_Int8              _connectionID = 0;

    rcBOOL = m_pLoaderRuntime->getSessionInformation(m_pSQLDBCConnection,
                                                     _connectionID,
                                                     m_PacketSize,
                                                     m_PageSize,
                                                     _packetListSize,
                                                     m_PacketList,
                                                     pszTmpUserGroupDefault,
                                                     pszTmpSYSDBADefault,
                                                     _userEncoding);
//TODOTODO FEHLERBEHANDLUNG
    if (LOADER_TRUE == rcBOOL)
    {
        m_ConnectionID = _connectionID;

        if (0 != pszTmpUserGroupDefault[0])
        {
            _len = strlen(pszTmpUserGroupDefault);
            m_pszUserGroupDefault = new LOADER_Char[_len + sizeof(LOADER_Char)];
            SAPDB_memcpy(m_pszUserGroupDefault, pszTmpUserGroupDefault, _len);
            m_pszUserGroupDefault[_len] = 0;
        }
        else
        {
            _len = strlen(m_UserName);
            m_pszUserGroupDefault = new LOADER_Char[_len + sizeof(LOADER_Char)];
            SAPDB_memcpy(m_pszUserGroupDefault, m_UserName, _len);
            m_pszUserGroupDefault[_len] = 0;
        }

        _len = strlen(pszTmpSYSDBADefault);
        m_SYSDBADefault = new LOADER_Char[_len + sizeof(LOADER_Char)];
        SAPDB_memcpy(m_SYSDBADefault, pszTmpSYSDBADefault, _len);
        m_SYSDBADefault[_len] = 0;

        m_PacketCount = 2;
        //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "packet count: " << m_PacketCount << endl;

        m_DBIsUnicode = m_pSQLDBCConnection->isUnicodeDatabase();
        if (LOADER_TRUE == m_DBIsUnicode)
        {
            //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "unicode db" << endl;
        }
        else
        {
            //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "non unicode db" << endl;
        }

        m_KernelVersion = m_pSQLDBCConnection->getKernelVersion();
        //SQLDBC_APPL_LEVEL_2_TRACE(m_pLoaderRuntime->m_pClientTrace) << "kernel version: " << m_KernelVersion << endl;

        //m_DateTimeFormat = reinterpret_cast<LOADER_DateTimeFormat>(m_pSQLDBCConnection->getDateTimeFormat());
        m_DateTimeFormat = (LOADER_DateTimeFormat)(m_pSQLDBCConnection->getDateTimeFormat());
    }   // end if (LOADER_TRUE == rcBOOL)

    return rc;
}
// setSessionInformation()