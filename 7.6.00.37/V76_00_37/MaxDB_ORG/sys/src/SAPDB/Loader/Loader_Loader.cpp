/*!
  \file    Loader_Loader.cpp
  \author  SteffenS
  \brief   Implements the main Loader class

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
#include "Loader/Loader_ErrorCode.hpp"
#include "Loader/Loader_Common.hpp"
#include "Loader/Loader_ErrorMsgHandler.hpp"
#include "Interfaces/SQLDBC/SQLDBC.h"
#include "Loader/Loader_Loader.hpp"


/*!
    @brief constructor
*/
Loader::Loader():
m_pLoaderRuntime(NULL),
m_pSQLDBCEnvironment(NULL)
{
    LOADER_Int4 errorTextSize = 2000;
    LOADER_Char errorText[2000];
    errorText[0] = 0;

//    m_pLoaderRuntime     = new Loader_ClientRuntime(errorText, 255);
    m_pLoaderRuntime     = SQLDBC::GetSingleThreadClientRuntime(errorText, 255);
    m_pSQLDBCEnvironment = new SQLDBC_Environment(m_pLoaderRuntime);

#ifdef LOADER_TRACING
    if (NULL != m_pLoaderRuntime)
    {
//        m_pLoaderRuntime->setLoaderTraceLevel(LOADER_TRACE_LEVEL_2);
    }
#endif

}
// Loader()

/*!
    @brief destructor
*/
Loader::~Loader()
{
    if (NULL != m_pSQLDBCEnvironment)
    {
        delete m_pSQLDBCEnvironment;
        m_pSQLDBCEnvironment = NULL;
    }
}


static const char* pszLoaderVersion =
  "SDBLoader <MAJOR_VERSION>.<MINOR_VERSION>.<CORRECTION_LEVEL>    BUILD <BUILD_NUMBER_EX_STRING>";


/*!
    @brief version of the Loader used
*/
const char *Loader::getLoaderVersion()
{
    return pszLoaderVersion;
}


/*!
    @brief creates a connection object
*/
Loader_Connection*
Loader::createConnection() const
{
    Loader_Connection* pConn = new Loader_Connection(m_pSQLDBCEnvironment, m_pLoaderRuntime);
    return pConn;
}
//createConnection()


/*!
    @brief creates a connection object; gets an existing SQLDBC connection object
*/
Loader_Connection*
Loader::createConnection(SQLDBC_Connection* pSQLDBCConnection) const
{
    //Loader_Connection* pConn = new Loader_Connection(m_pSQLDBCEnvironment, m_pLoaderRuntime);
    Loader_Connection* pConn = new Loader_Connection(m_pSQLDBCEnvironment, m_pLoaderRuntime, pSQLDBCConnection);
    return pConn;
}
//createConnection()


/*!
    @brief releases a connection object
*/
void
Loader::releaseConnection(Loader_Connection* pLoaderConnection) const
{
    delete pLoaderConnection;
}
// releaseConnection()


/*!
    @brief sets the trace level

void
Loader::setTraceLevel(LOADER_TraceLevel level)
{
    if (NULL != m_pLoaderRuntime)
    {
        m_pLoaderRuntime->setLoaderTraceLevel(level);
    }
}
*/