/*!
  @file           KSQL_Connection.cpp
  @author         ThomasA
  @brief          Implements class for a virtuell database connection.

\if EMIT_LICENCE

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



\endif
*/

#include "SQLManager/KernelSQL/KSQL_SQLDBC.hpp"
#include "SQLManager/KernelSQL/KSQL_Connection.hpp"
#include "SQLManager/KernelSQL/KSQL_Runtime.hpp"
#include "SQLManager/KernelSQL/KSQL_Messages.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SQLManager/SQLMan_Context.hpp"


using namespace SQLDBC;

//--------------------------------------------------------------------------------------------

KSQL_Connection::KSQL_Connection()
: m_dateTimeFormat(0)
, m_connection(0)
, m_context(*SQLMan_Context::GetContext())
, m_environment(&m_context.GetSQLDBCRuntime())
{
    m_dateTimeFormat = m_context.SetDateTimeFormat(dtf_normal);
    this->Connect();
}

//--------------------------------------------------------------------------------------------

KSQL_Connection::~KSQL_Connection()
{
    if (m_connection)
    {
        m_connection->disconnect();                      
        m_environment.releaseConnection(m_connection);
    }
    m_context.SetDateTimeFormat(m_dateTimeFormat);
}

//--------------------------------------------------------------------------------------------

bool KSQL_Connection::Connect()
{
    if (0 == m_connection)
    {
        m_context.InitInterfaceTaskTraceContext();
        SQLDBC_Connection* pConnection = m_environment.createConnection(m_context.GetAllocator()); 
        SQLDBC_ConnectProperties properties(m_context.GetAllocator());
        properties.setProperty ("APPLICATION", "KNL");
        properties.setProperty ("STATEMENTCACHESIZE", "0");
        properties.setProperty ("SQLMODE", "INTERNAL");
        // Parse Ids shall be dropped immediately when the statement
        // is released.
        properties.setProperty ("DROPPARSEIDDIRECT", "1"); 
        if (SQLDBC_NOT_OK == pConnection->connect(properties))
        {
            SQLDBC_ErrorHndl e = pConnection->error();
            SAPDB_ToStringClass errCodeStr = ToStr(e.getErrorCode());
            SAPDBErr_MessageList msg ("KSQL", __CONTEXT__, 
                SAPDBErr_MessageList::Error, KSQL_ERR_INTERFACE_RUNTIME_ERROR_ID,
                "Interface runtime error", 2, errCodeStr, e.getErrorText());
            SAPDBErr_MessageList& errList = m_context.GetErrorList();
            errList.AppendNewMessage(msg);
            m_environment.releaseConnection(pConnection);
        }
        else
        {
            m_connection = pConnection; 
        }
    }
    return (0 != m_connection);
}

//--------------------------------------------------------------------------------------------

KSQL_PreparedStatement KSQL_Connection::createPreparedStatement()
{
    return KSQL_PreparedStatement (m_context.GetAllocator(), *m_connection, m_connection->createPreparedStatement());
}

//--------------------------------------------------------------------------------------------

KSQL_Statement KSQL_Connection::createStatement()
{
    return KSQL_Statement (m_context.GetAllocator(), *m_connection, m_connection->createStatement());
}

//--------------------------------------------------------------------------------------------

const KSQL_Connection::KSQL_Error& KSQL_Connection::getError() const
{
    return m_connection->error();
}

//--------------------------------------------------------------------------------------------

bool KSQL_Connection::isOk() const
{
    return (m_connection && (0 == this->getError().getErrorCode()));
}

//--------------------------------------------------------------------------------------------

bool KSQL_Connection::isConnected() const
{
    return (0 != m_connection);
}

//--------------------------------------------------------------------------------------------

