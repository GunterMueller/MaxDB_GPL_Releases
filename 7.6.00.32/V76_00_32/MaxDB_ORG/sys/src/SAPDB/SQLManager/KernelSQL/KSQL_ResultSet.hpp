/*!
  @file           KSQL_ResultSet.hpp
  @author         ThomasA
  @brief          Implements a class for result set handling

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

#ifndef KSQL_RESULTSET_HPP
#define KSQL_RESULTSET_HPP

#include "SQLManager/SQLMan_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SQLManager/KernelSQL/KSQL_SQLDBC.hpp"

/*!
   @brief a class for result set handling 
 */

class KSQL_ResultSet
{
public :
    /*!
       @brief closes the result set
     */
    int close();
    /*!
       @brief reads an ascii from the current result row
     */
    int getAscii(int Index, void* p, int length);
    /*!
       @brief reads a bool value from the current result row
     */
    int getBool (int Index, bool& b);
    /*!
       @brief reads a pascal bool value from the current result row
     */
    int getBool (int Index, pasbool& b);
    /*!
       @brief reads a byte array from the current result row
     */
    int getBytes(int Index, void* p, int length);
    /*!
       @brief reads a 2 byte integer value from the current result row
     */
    int getInt2 (int Index, SAPDB_Int2& i2);
    /*!
       @brief reads a 4 byte integer value from the current result row
     */
    int getInt4 (int Index, SAPDB_Int4& i4);
    /*!
       @brief reads 8 byte surrogate from the current result row
     */
    int getSurrogate (int Index, SQLMan_Surrogate& surrogate);
    /*!
       @brief reads an identifier from the current result row
     */
    int getIdentifier (int Index, SQLMan_Identifier& identifier);
    /*!
       @brief moves to the next result row
     */
    int next();
    /*!
       @brief returns true, if no error has been occured
     */
    inline bool isOK() const;
private :
    int errorSQLCode(SAPDB_Char const * const fileName, SAPDB_UInt4 lineNumber); //!< transforms error object into sql code
    KSQL_ResultSet(SQLDBC::SQLDBC_ResultSet*);
    int                       m_error; 
    SQLDBC::SQLDBC_ResultSet* m_resultSet;
    friend class KSQL_Statement;
};

inline bool KSQL_ResultSet::isOK() const
{
    return (0 == m_error);
}

#endif
