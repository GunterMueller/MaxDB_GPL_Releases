/*!
  @file           KSQL_Statement.cpp
  @author         ThomasA
  @brief          Implements a class for sql processing without parameters

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

#include "SQLManager/KernelSQL/KSQL_Statement.hpp"
#include "SQLManager/KernelSQL/KSQL_Messages.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

using namespace SQLDBC;

struct KSQL_StatementBody
{
  
  KSQL_StatementBody(SAPDBMem_IRawAllocator& alloc, SQLDBC_Connection& con, SQLDBC_Statement* pStatement)
    : m_allocator(alloc) 
    , m_refCount(1)
    , m_connection(con)
    , m_pStatement(pStatement)
    , m_pLastError(0)
  {
  }
  ~KSQL_StatementBody()
  {
      this->release();
      destroy (m_pLastError, m_allocator);
  }
  void  incRefCount() { 
    ++m_refCount;
  }
  int   decRefCount() { 
    return --m_refCount;
  } 
  void release()
  {
      if (m_pStatement)
      {
          m_connection.releaseStatement(m_pStatement);
          m_pStatement = 0;
      }
  }
  SAPDBMem_IRawAllocator& m_allocator;
  int                     m_refCount;
  SQLDBC_Connection&      m_connection;
  SQLDBC_Statement*       m_pStatement;
  SQLDBC_ErrorHndl*       m_pLastError;
};

//-----------------------------------------------------------------------

KSQL_Statement::KSQL_Statement(SAPDBMem_IRawAllocator& alloc, SQLDBC_Connection& con, SQLDBC_Statement* pStatement)
: m_body(new(alloc) KSQL_StatementBody (alloc, con, pStatement))
{
}

//-----------------------------------------------------------------------

KSQL_Statement::KSQL_Statement(const KSQL_Statement& source)
: m_body(source.m_body)
{
  m_body->incRefCount();
}

//-----------------------------------------------------------------------

void KSQL_Statement::operator=(const KSQL_Statement& source)
{
  this->~KSQL_Statement();
  new((void*) this) KSQL_Statement(source);
}

//-----------------------------------------------------------------------

KSQL_Statement::~KSQL_Statement()
{
    if (m_body && (0 == m_body->decRefCount()))
    {
        SAPDBMem_IRawAllocator& allocator = m_body->m_allocator; 
        destroy(m_body, allocator);
    }
}

//-----------------------------------------------------------------------

KSQL_Statement::SQLCode KSQL_Statement::asciiExecute (const char* pStatement)
{
    if (this->isValid())
    {
        SQLDBC_Retcode retcode = m_body->m_pStatement->execute (pStatement, SQLDBC_StringEncodingAscii);
        if (SQLDBC_OK == retcode) 
        {
            return 0;
        }
        else
        {
            this->storeLastError();
            const char* pDummy1;
            const char* pDummy2;
            return this->describeLastError(pDummy1, pDummy2);
        }
    }
    else
    {
        SQLMan_Context* pContext = SQLMan_Context::GetContext();
        SAPDBErr_MessageList msg ("KSQL", __CONTEXT__, KSQL_ERR_UNINITIALIZED_STATEMENT);  
        SAPDBErr_MessageList& errList = pContext->GetErrorList();
        errList.AppendNewMessage(msg);
        return RUN_TIME_ERROR;
    }
}

//-----------------------------------------------------------------------

KSQL_Statement::SQLCode KSQL_Statement::describeLastError(const char* &pSQLState, const char* &pMsgText) const
{
    if (m_body->m_pLastError)
    {
        pSQLState = m_body->m_pLastError->getSQLState();
        pMsgText  = m_body->m_pLastError->getErrorText();
        return m_body->m_pLastError->getErrorCode();
    }
    else
    {
        static const char* undefinedMsg = "UNDEFINED";
        pSQLState = undefinedMsg;
        pMsgText  = undefinedMsg;
        return 0;
    }
}

//-----------------------------------------------------------------------

SAPDBMem_IRawAllocator& KSQL_Statement::GetAllocator()
{
    return m_body->m_allocator;
}

//-----------------------------------------------------------------------

KSQL_ResultSet KSQL_Statement::getResultSet()
{
    return KSQL_ResultSet(m_body->m_pStatement->getResultSet());
}

//-----------------------------------------------------------------------

int KSQL_Statement::getRowsAffected()
{
    return m_body->m_pStatement->getRowsAffected();
}

//-----------------------------------------------------------------------

KSQL_Statement::SQLCode KSQL_Statement::unicodeExecute (const char* pStatement)
{
    SQLDBC_Retcode retcode = m_body->m_pStatement->execute (pStatement, SQLDBC_StringEncodingAscii);
    return retcode;
}

//-----------------------------------------------------------------------

bool KSQL_Statement::isValid() const
{
    return ((0 != m_body) && (0 != m_body->m_pStatement));
}

//-----------------------------------------------------------------------

SQLDBC_Statement* KSQL_Statement::getStatementPointer() const
{
    return m_body->m_pStatement;
}

//-----------------------------------------------------------------------

KSQL_Statement::SQLCode KSQL_Statement::setInterfaceError()
{
    this->storeLastError();
    SQLDBC_ErrorHndl e = getStatementPointer()->error();
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    SAPDB_ToStringClass errCodeStr = ToStr(e.getErrorCode());
    SAPDBErr_MessageList msg ("KSQL", __CONTEXT__, 
            SAPDBErr_MessageList::Error, KSQL_ERR_INTERFACE_RUNTIME_ERROR_ID,
            "Interface runtime error", 2, errCodeStr, e.getErrorText());
    SAPDBErr_MessageList& errList = pContext->GetErrorList();
    errList.AppendNewMessage(msg);
    return e.getErrorCode();
}

//-----------------------------------------------------------------------

void KSQL_Statement::storeLastError()
{
    if (!m_body->m_pLastError)
    {
        m_body->m_pLastError = new(m_body->m_allocator) SQLDBC_ErrorHndl();
    }
    if (m_body->m_pLastError)
    {
        *m_body->m_pLastError = m_body->m_pStatement->error();
    }
}

//-----------------------------------------------------------------------

void KSQL_Statement::release()
{
    if (this->isValid())
    {
        m_body->release();
    }
}

//-----------------------------------------------------------------------

void KSQL_Statement::setCursorName(const char* name)
{
    SQLDBC_StringEncoding encoding = SQLDBC_StringEncodingAscii;
    m_body->m_pStatement->setCursorName (name, SQLDBC_NTS, encoding);
}
