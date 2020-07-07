/*

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
#include "SQLManager/KernelSQL/KSQL_ResultSet.hpp"
#include "SQLManager/KernelSQL/KSQL_Messages.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_Interface.hpp"

using namespace SQLDBC;

KSQL_ResultSet::KSQL_ResultSet(SQLDBC::SQLDBC_ResultSet* pResultSet)
: m_error(0)
, m_resultSet(pResultSet)
{
}

//-----------------------------------------------------------

int KSQL_ResultSet::close()
{
    //SQLDBC_Retcode retCode =  
    m_resultSet->close(); 
    //if (SQLDBC_OK != retCode)
    //{
    //    if (0 == m_error) || (100 == m_error)
    //    {
    //        return this->errorSQLCode(__CONTEXT__);
    //    }
    //}
    return m_error;
}

//-----------------------------------------------------------

int KSQL_ResultSet::errorSQLCode(SAPDB_Char const * const fileName, SAPDB_UInt4 lineNumber)
{
    SQLDBC_ErrorHndl e = m_resultSet->error();
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    SAPDBErr_MessageList msg ("KSQL",fileName, lineNumber, 
        SAPDBErr_MessageList::Error, KSQL_ERR_INTERFACE_RUNTIME_ERROR_ID,
        "Interface runtime error", 2, ToStr(e.getErrorCode()), e.getErrorText());
    SAPDBErr_MessageList& errList = pContext->GetErrorList();
    errList.AppendNewMessage(msg);
    m_error = e.getErrorCode();
    return m_error;
}

//-----------------------------------------------------------

int KSQL_ResultSet::next()
{
    if (0 == m_error)
    {
        SQLDBC_Retcode retCode =  m_resultSet->next();
        {
            if (SQLDBC_OK != retCode) 
            {
                if (SQLDBC_NO_DATA_FOUND == retCode)
                {
                    m_error = 100;
                }
                else
                {
                    return this->errorSQLCode(__CONTEXT__);
                }
            }
        }
    }
    return m_error;  
}

//-----------------------------------------------------------

int KSQL_ResultSet::getAscii(int Index, void* p, int length)
{
    if (0 == m_error)
    {
        SQLDBC_Length dummyLength;
        SQLDBC_Retcode retCode = m_resultSet->getObject(Index, SQLDBC_HOSTTYPE_ASCII, p, &dummyLength, length, false); 
        if (SQLDBC_OK != retCode)
        {
            return this->errorSQLCode(__CONTEXT__);
        }
    }
    return m_error;
}

//-----------------------------------------------------------

int KSQL_ResultSet::getBool (int Index, bool& b)
{
    if (0 == m_error)
    {
        SQLDBC_Length dummyLength;
        SQLDBC_Retcode retCode = m_resultSet->getObject(Index, SQLDBC_HOSTTYPE_UINT1 , &b, &dummyLength, sizeof(b), false); 
        if (SQLDBC_OK != retCode)
        {
            return this->errorSQLCode(__CONTEXT__);
        }
    }
    return m_error;
}

//-----------------------------------------------------------

int KSQL_ResultSet::getBool (int Index, pasbool& b)
{
    if (0 == m_error)
    {
        SQLDBC_Length dummyLength;
        SQLDBC_Retcode retCode = m_resultSet->getObject(Index, SQLDBC_HOSTTYPE_UINT1 , &b, &dummyLength, sizeof(b), false); 
        if (SQLDBC_OK != retCode)
        {
            return this->errorSQLCode(__CONTEXT__);
        }
    }
    return m_error;
}

//-----------------------------------------------------------

int KSQL_ResultSet::getBytes(int Index, void* p, int length)
{
    if (0 == m_error)
    {
        SQLDBC_Length dummyLength;
        SQLDBC_Retcode retCode = m_resultSet->getObject(Index, SQLDBC_HOSTTYPE_BINARY  , p, &dummyLength, length, false); 
        if (SQLDBC_OK != retCode)
        {
            return this->errorSQLCode(__CONTEXT__);
        }
    }
    return m_error;
}

//-----------------------------------------------------------

int KSQL_ResultSet::getInt2 (int Index, SAPDB_Int2& i2)
{
    if (0 == m_error)
    {
        SQLDBC_Length dummyLength;
        SQLDBC_Retcode retCode = m_resultSet->getObject(Index, SQLDBC_HOSTTYPE_INT2, &i2, &dummyLength, sizeof(i2), false); 
        if (SQLDBC_OK != retCode)
        {
            return this->errorSQLCode(__CONTEXT__);
        }
    }
    return m_error;
}

//-----------------------------------------------------------

int KSQL_ResultSet::getInt4 (int Index, SAPDB_Int4& i4)
{
    if (0 == m_error)
    {
        SQLDBC_Length dummyLength;
        SQLDBC_Retcode retCode = m_resultSet->getObject(Index, SQLDBC_HOSTTYPE_INT4, &i4, &dummyLength, sizeof(i4), false); 
        if (SQLDBC_OK != retCode)
        {
            return this->errorSQLCode(__CONTEXT__);
        }
    }
    return m_error;
}

//-----------------------------------------------------------

int KSQL_ResultSet::getSurrogate (int Index, SQLMan_Surrogate& surrogate)
{
    if (0 == m_error)
    {
        SQLDBC_Length length = sizeof(surrogate);
        SQLDBC_Retcode retCode = m_resultSet->getObject(Index, SQLDBC_HOSTTYPE_BINARY, &surrogate, &length, sizeof(surrogate), false); 
        if (SQLDBC_OK != retCode)
        {
            return this->errorSQLCode(__CONTEXT__);
        }
    }
    return m_error;
}

//-----------------------------------------------------------

int KSQL_ResultSet::getIdentifier (int Index, SQLMan_Identifier& identifier)
{
    if (0 == m_error)
    {
        SQLDBC_Length length = sizeof(identifier);
        SQLDBC_Retcode retCode = m_resultSet->getObject(Index,
            Catalog_Interface::GetInstance().IsUnicodeCatalog() ? SQLDBC_HOSTTYPE_UCS2 : SQLDBC_HOSTTYPE_ASCII,
            &identifier, &length, sizeof(identifier), false); 
        if (SQLDBC_OK != retCode)
        {
            return this->errorSQLCode(__CONTEXT__);
        }
    }
    return m_error;
}
