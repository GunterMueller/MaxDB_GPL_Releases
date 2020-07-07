/*!
  @file           KSQL_Statement.hpp
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

#ifndef KSQL_STATEMENT_HPP
#define KSQL_STATEMENT_HPP

#include "SQLManager/KernelSQL/KSQL_SQLDBC.hpp"

namespace SQLDBC {
    class  SQLDBC_Connection;
    class  SQLDBC_Statement;
};
class  SAPDBMem_IRawAllocator;
struct KSQL_StatementBody;

#include "SQLManager/KernelSQL/KSQL_ResultSet.hpp"

class KSQL_Statement
{
    public  :
        enum Errors { RUN_TIME_ERROR = -1 };
        typedef int SQLCode;
        /*!
           @brief copy constructor
         */
        KSQL_Statement(const KSQL_Statement& source);
        /*!
           @brief destructor
         */
        ~KSQL_Statement();
        /*!
           @brief executes a sql statement given as an ascii string
         */
        SQLCode asciiExecute   (const char* pStatement);
        /*!
           @brief returns sql state and message text of the last error
         */
        virtual SQLCode describeLastError(const char* &pSQLState, const char* &pMsgText) const;
        /*!
           @brief returns the resultset of the executed statement
         */
        KSQL_ResultSet getResultSet();
        /*!
           @brief returns the numbers of rows affected after the statement execution
         */
        int getRowsAffected();
        /*!
           @brief executes a sql statement given as an unicode string
         */
        SQLCode unicodeExecute (const char* pStatement);
        /*!
           @brief return true, if the current instance is valid; otherwise false
         */
        bool isValid() const;
        /*!
           @brief release the statement and returns resources
         */
        void release();
        /*!
           @brief sets the cursor name
           @param name null terminated ascii string
         */
        void setCursorName(const char* name);
    protected :
        KSQL_Statement(SAPDBMem_IRawAllocator& alloc, SQLDBC::SQLDBC_Connection& con, SQLDBC::SQLDBC_Statement* pStatement);
        void operator=(const KSQL_Statement& source);
        SAPDBMem_IRawAllocator& GetAllocator();
        SQLDBC::SQLDBC_Statement* getStatementPointer() const;
        void incRefCount();
        SQLCode setInterfaceError();
        void storeLastError();
        KSQL_StatementBody* m_body;
        friend class KSQL_Connection;
};

#endif
