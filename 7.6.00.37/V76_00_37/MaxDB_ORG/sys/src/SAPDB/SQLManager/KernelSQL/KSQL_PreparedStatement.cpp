/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"
#include "SQLManager/Catalog/Catalog_Interface.hpp"
#include "SQLManager/KernelSQL/KSQL_SQLDBC.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/System/RTESys_Time.h"

using namespace SQLDBC;

#define KSQL_MAX_CHUNK_ENTRIES 32

struct KSQL_LengthChunk
{
    KSQL_LengthChunk()
        : m_next(0)
    {
    }
    SQLDBC_Length     m_lengthArray[KSQL_MAX_CHUNK_ENTRIES];
    KSQL_LengthChunk* m_next;
};

struct KSQL_LengthBuffer
{
    int                     m_lastIndex;
    KSQL_LengthChunk*       m_currChunk;
    KSQL_LengthChunk        m_firstChunk;
    SAPDBMem_IRawAllocator& m_allocator;
    KSQL_LengthBuffer(SAPDBMem_IRawAllocator& Allocator)
        : m_lastIndex(0)
        , m_currChunk(&m_firstChunk)
        , m_firstChunk()
        , m_allocator(Allocator)
    {
    }
    ~KSQL_LengthBuffer()
    {
        KSQL_LengthChunk* p = m_firstChunk.m_next;
        while (p)
        {
            KSQL_LengthChunk* pFree = p;
            p = p->m_next;
            m_allocator.Deallocate(pFree);
        }
    }
    SQLDBC_Length* GetLengthBuffer()
    {
        if (m_lastIndex >= KSQL_MAX_CHUNK_ENTRIES)
        {
            m_lastIndex = 0;
            if (0 == m_currChunk->m_next)
            {
                m_currChunk->m_next = new (m_allocator) KSQL_LengthChunk;
            }
            if (m_currChunk->m_next)
            {
                m_currChunk = m_currChunk->m_next;
            }
            else
            {
                return 0;
            }
        }
        return &m_currChunk->m_lengthArray[m_lastIndex++];
    }
    void Reset ()
    {
         m_lastIndex = 0;
         m_currChunk = &m_firstChunk;
    }
};

static const bool c_null_terminated = true;

inline SQLDBC::SQLDBC_PreparedStatement* KSQL_PreparedStatement::getPreparedStatementPointer()const
{
    return REINTERPRET_CAST (SQLDBC::SQLDBC_PreparedStatement*, getStatementPointer());
}

//---------------------------------------------------------------------------------

KSQL_PreparedStatement::~KSQL_PreparedStatement()
{
    if (m_pLengthBuffer)
    {
        destroy (m_pLengthBuffer, GetAllocator());
    }
}

//---------------------------------------------------------------------------------

KSQL_PreparedStatement::KSQL_PreparedStatement(SAPDBMem_IRawAllocator&    alloc,
                                               SQLDBC::SQLDBC_Connection& con,
                                               SQLDBC::SQLDBC_Statement*  pStatement)
: KSQL_Statement(alloc, con, pStatement)
, m_error(0)
, m_pLengthBuffer(0)
{
    if (!this->isValid())
    {
        m_error = -1; // TODO
    }
}

//---------------------------------------------------------------------------------

SQLDBC_Length* KSQL_PreparedStatement::AllocLength(SQLDBC_Length length)
{
    // TODO overflow
    if (0 == m_pLengthBuffer)
    {
         m_pLengthBuffer = new (this->GetAllocator()) KSQL_LengthBuffer(this->GetAllocator());
    }
    if (0 == m_pLengthBuffer)
    {
        // TODO error handling
        return 0;
    }
    SQLDBC_Length* pLength = m_pLengthBuffer->GetLengthBuffer();
    if (!pLength)
    {
        return 0;
    }
    *pLength = length;
    return pLength;
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bind (int parameterIdx, bool& b)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx, SQLDBC_HOSTTYPE_UINT1 , &b, NULL, sizeof(b)));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bind (int parameterIdx, SAPDB_UInt1& ui1)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx, SQLDBC_HOSTTYPE_UINT1 , &ui1, NULL, sizeof(ui1)));
    }
    return (0 == m_error);

}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bind (int parameterIdx, SAPDB_Int2& i2)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx, SQLDBC_HOSTTYPE_INT2 , &i2, NULL, sizeof(i2)));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bind (int parameterIdx, SAPDB_Int4& i4)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx, SQLDBC_HOSTTYPE_INT4 , &i4, NULL, sizeof(i4)));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bind (int parameterIdx, SAPDB_UInt4& ui4)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx, SQLDBC_HOSTTYPE_UINT4 , &ui4, NULL, sizeof(ui4)));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bind (int parameterIdx, SAPDB_UInt8& ui8)
{
     if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx, SQLDBC_HOSTTYPE_UINT8 , &ui8, NULL, sizeof(ui8)));
    }
    return (0 == m_error);
}

bool KSQL_PreparedStatement::bind (int parameterIdx, SQLMan_Surrogate& surrogate)
{

    SQLDBC_Length* pLength = this->AllocLength(sizeof(surrogate));
    if (0 == m_error)
    {
       m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx,
           SQLDBC_HOSTTYPE_BINARY, &surrogate, pLength, sizeof(surrogate), !c_null_terminated));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bind (int parameterIdx, const SQLMan_Surrogate& surrogate)
{

    SQLDBC_Length* pLength = this->AllocLength(sizeof(surrogate));
    if (0 == m_error)
    {
       m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx,
           SQLDBC_HOSTTYPE_BINARY, *CONST_CAST(SQLMan_Surrogate*, &surrogate), pLength, sizeof(surrogate), !c_null_terminated));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bind (int parameterIdx, SQLMan_Identifier& identifier)
{
	bool unicode_catalog = Catalog_Interface::GetInstance().IsUnicodeCatalog();

    SQLDBC_Length* pLength = this->AllocLength(unicode_catalog ? sizeof(identifier) : (sizeof(identifier) / 2 ));
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx,
            unicode_catalog ? SQLDBC_HOSTTYPE_UCS2 : SQLDBC_HOSTTYPE_ASCII, 
            &identifier, pLength, 
            (unicode_catalog ? sizeof(identifier) : (sizeof(identifier) / 2 )), 
            !c_null_terminated));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bind (int parameterIdx, void* pAddr, int length)
{
    SQLDBC_Length* pLength = this->AllocLength(length);
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx,
            SQLDBC_HOSTTYPE_BINARY, pAddr, pLength, length, !c_null_terminated));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bindNullValue (int parameterIdx)
{
    SQLDBC_Length* pLength = this->AllocLength(SQLDBC_NULL_DATA);
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx,
            SQLDBC_HOSTTYPE_BINARY, NULL, pLength, 0));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bindInput (int parameterIdx, const void* pAddr, int length)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx, SQLDBC_HOSTTYPE_BINARY,
            const_cast<void *>(pAddr), NULL, length, !c_null_terminated));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bindAsciiInput (int parameterIdx, const char * pAddr, int length, bool terminate)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (parameterIdx,
            SQLDBC_HOSTTYPE_ASCII, const_cast<char *>(pAddr), NULL, length, terminate));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bindAsciiOutput (int parameterIdx, char * pAddr, IFR_Length * lengthInOut, bool terminate)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (
            parameterIdx, SQLDBC_HOSTTYPE_ASCII, pAddr, lengthInOut, *lengthInOut, terminate));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bindInt4Output (int parameterIdx, SAPDB_Int4& i4, IFR_Length * outlen)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (
            parameterIdx, SQLDBC_HOSTTYPE_INT4 , &i4, outlen, sizeof(i4)));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bindInt4Input (int parameterIdx, const SAPDB_Int4& i4)
{
    if (0 == m_error)
    {
        m_error = this->result (getPreparedStatementPointer()->bindParameter (
            parameterIdx, SQLDBC_HOSTTYPE_INT4 , 
            &const_cast<SAPDB_Int4&>(i4), NULL, sizeof(i4)));
    }
    return (0 == m_error);
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bindIdentifier4Input (
    int parameterIdx, const void* identifier)
{
    return bind (
        parameterIdx,
        *reinterpret_cast<SQLMan_Identifier*>(
            const_cast<void*>(identifier)
            ));
}

//---------------------------------------------------------------------------------

bool KSQL_PreparedStatement::bindTimestampInput (
    int parameterIdx, const void * timestamp)
{
    return bindAsciiInput (
        parameterIdx,
        reinterpret_cast<const char*>(timestamp),
        sizeof (RTESys_SQLTimestamp));
}

//---------------------------------------------------------------------------------

KSQL_PreparedStatement::SQLCode KSQL_PreparedStatement::result(int retCode, bool intoMessageList)
{
    if (SQLDBC_OK == retCode)
    {
        return 0;
    }
    if (SQLDBC_NO_DATA_FOUND == retCode)
    {
        return 100;
    }
    if (intoMessageList)
    {
        return this->setInterfaceError();
    }
    else
    {
        this->storeLastError();
        SQLDBC_ErrorHndl e = getStatementPointer()->error();
        return e.getErrorCode();
    }
}

//---------------------------------------------------------------------------------

KSQL_PreparedStatement::SQLCode KSQL_PreparedStatement::execute()
{
    if (0 == m_error)
    {
        if (m_pLengthBuffer)
        {
            m_pLengthBuffer->Reset();
        }
        return this->result (getPreparedStatementPointer()->execute(), false);
    }
    return m_error;
}

//---------------------------------------------------------------------------------

KSQL_PreparedStatement::SQLCode KSQL_PreparedStatement::prepare(const char* pStatement)
{
    if (0 == m_error)
    {
        m_error = this->result (this->getPreparedStatementPointer()->prepare (pStatement, SQLDBC_StringEncodingAscii), false);
    }
    return m_error;
}

//---------------------------------------------------------------------------------

void KSQL_PreparedStatement::resetError()
{
    m_error = 0;
}

