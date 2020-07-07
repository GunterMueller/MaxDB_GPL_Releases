/*!
  @file           KSQL_PreparedStatement.hpp
  @author         ThomasA
  @brief          Implements a class for sql processing with parameters

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

#ifndef KSQL_PREPAREDSTATEMENT_HPP
#define KSQL_PREPAREDSTATEMENT_HPP

#include "SQLManager/KernelSQL/KSQL_Statement.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Interfaces/Runtime/IFR_Types.h"
#include "RunTime/System/RTESys_Time.h"

namespace SQLDBC {
    class  SQLDBC_Connection;
    class  SQLDBC_PreparedStatement;
    class  SQLDBC_Statement;
};

struct KSQL_LengthBuffer;

/*!
   @brief implements a prepared sql statement, i.e. a statement which is able to handle
          sql with parameters.
          All methods set the internal error member, and are noop's if the
          error is <> 0 at method entry.
*/
class KSQL_PreparedStatement : public KSQL_Statement
{
public :
    /*!
       @brief destructor
     */
    virtual ~KSQL_PreparedStatement();
    /*!
       @brief binds a bool by address
    */
    bool bind (int parameterIdx, bool& b);
    /*!
       @brief binds a 1 byte unsigned integer by address
    */
    bool bind (int parameterIdx, SAPDB_UInt1& ui1);
    /*!
       @brief binds a 2 byte integer by address
    */
    bool bind (int parameterIdx, SAPDB_Int2& i2);
    /*!
       @brief binds a 4 byte integer by address
    */
    bool bind (int parameterIdx, SAPDB_Int4& i4);
    /*!
       @brief binds an unsigned 4 byte integer by address
    */
    bool bind (int parameterIdx, SAPDB_UInt4& ui4);
    /*!
       @brief binds an unsigned 8 byte integer by address
    */
    bool bind (int parameterIdx, SAPDB_UInt8& ui8);
    /*!
       @brief binds a surrogate by address
    */
    bool bind (int parameterIdx, SQLMan_Surrogate& surrogate);
    /*!
       @brief binds a const surrogate by address
    */
    bool bind (int parameterIdx, const SQLMan_Surrogate& surrogate);
    /*!
       @brief binds a surrogate by address
    */
    bool bind (int parameterIdx, SQLMan_Identifier& identifier);
    /*!
       @brief binds by address and length
    */
    bool bind (int parameterIdx, void* pAddr, int length);
    /*!
       @brief binds null value
    */
    bool bindNullValue (int parameterIdx);
    /*!
       @brief binds ascii input by address and length
    */
    bool bindInput (int parameterIdx, const void* pAddr, int length);
    /*!
       @brief binds ascii input by address and length
    */
    bool bindAsciiInput (int parameterIdx, const char * pAddr, int length, bool terminate = true);
    /*!
       @brief binds by address and length
    */
    bool bindAsciiOutput (int parameterIdx, char * pAddr, IFR_Length * lengthInOut, bool terminate = true);
    /*!
       @brief binds by address and length
    */
    bool bindInt4Output (int parameterIdx, SAPDB_Int4& i4, IFR_Length * outlen);
    /*!
       @brief binds an int4 as input
    */
    bool bindInt4Input (int parameterIdx, const SAPDB_Int4& i4);
    /*!
       @brief binds an SQLMan_Identifier as input
    */
    bool bindIdentifier4Input(int parameterIdx, const void * identifier);
    /*!
       @brief binds an SQL timestamp as input
    */
    bool bindTimestampInput(int parameterIdx, const void * timestamp);

    /*!
       @brief executes the prepared statement
    */
    SQLCode execute();
    /*!
       @brief prepares then statement
    */
    SQLCode prepare(const char* pStatement);
    /*!
       @brief sets internal error to 0
    */
    void resetError();

private :
    int                m_error;
    KSQL_LengthBuffer* m_pLengthBuffer;
    SQLDBC_Length* AllocLength(SQLDBC_Length length);
    KSQL_PreparedStatement(SAPDBMem_IRawAllocator& alloc, SQLDBC::SQLDBC_Connection& con, SQLDBC::SQLDBC_Statement* pStatement);
    SQLCode result(int retCode, bool intoMessageList = true);
    inline SQLDBC::SQLDBC_PreparedStatement* getPreparedStatementPointer()const;
    friend class KSQL_Connection;
};

#endif
