/*****************************************************************************/
/*!
  @file         KSQL_PreparedStatementScope.hpp
  @author       MartinKi

  @brief        Header file for class KSQL_PreparedStatementScope.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/

#ifndef KSQL_PREPAREDSTATEMENTSCOPE_HPP
#define KSQL_PREPAREDSTATEMENTSCOPE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Msg_List;
class KSQL_Connection;
class KSQL_PreparedStatement;
class SAPDBMem_IRawAllocator;
class SQLMan_Context;
class SQLMan_SQLStatementContext;

/*!
 * @brief Helper class to intialize and clean up an internal prepared
 * SQL statement within a scope.
 *
 * @author  MartinKi
 * @ingroup KernelSQL
 */
class KSQL_PreparedStatementScope
{
private:
    SAPDBMem_IRawAllocator&     m_alloc;
    SQLMan_SQLStatementContext* m_pStatementContext;
    KSQL_Connection*            m_pConnection;
    KSQL_PreparedStatement*     m_pStatement;

    //no copy-construction or assignment
    KSQL_PreparedStatementScope(const KSQL_PreparedStatementScope&);
    KSQL_PreparedStatementScope operator=(KSQL_PreparedStatementScope&);
public:

    /*!
     * @brief Constructor, Initialize must be called before the object
     * can be actually used.
     */
    KSQL_PreparedStatementScope(SAPDBMem_IRawAllocator& alloc);

    /*!
     * @brief Destructor, restores the original context.
     */
    ~KSQL_PreparedStatementScope();


    /*!
     * @brief Possible return values of the Initialize method.
     */
    enum InitRc {
        initRcOk,
        initRcOutOfMemory,
        initRcSystemError
    };

    /*!
     * @brief Creates and initializes the new context and prepared statement.
     *
     * This method must only be called once!
     *
     * @param context [in/out] the original context that is to be saved
     * @param statementText [in] the text of the internal SQL statement to
     *                           be prepared
     * @param errMsg [out]     possible error messages
     */
    InitRc Initialize(
        SQLMan_Context& context,
        char const*     statementText,
        Msg_List&       errMsg);


    /*!
     * @brief Returns the prepared statement. Can only be called after
     * a successful call of the Initialize method.
     */
    KSQL_PreparedStatement& GetStatement();
};

/*****************************************************************************/

inline
KSQL_PreparedStatement& KSQL_PreparedStatementScope::GetStatement()
{
    return *m_pStatement;
}

/*****************************************************************************/

#endif // KSQL_PREPAREDSTATEMENTSCOPE_HPP
