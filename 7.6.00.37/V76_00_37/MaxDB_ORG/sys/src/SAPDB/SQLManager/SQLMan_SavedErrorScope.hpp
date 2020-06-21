/*****************************************************************************/
/*!
  @file         SQLMan_SavedErrorScope.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SQLMan_SavedErrorScope.

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

#ifndef SQLMAN_SAVEDERRORSCOPE_HPP
#define SQLMAN_SAVEDERRORSCOPE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Messages/Msg_List.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class SQLMan_Context;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class  SQLMan_SavedErrorScope
 * @author MartinKi
 *
 * This class saves the error code and related information such as the
 * current context's error message list and restores this information
 * on destruction.
 */
class SQLMan_SavedErrorScope
{
public:
    /*!
     * @brief Saves the current error state and related
     * information. The context itself will not be modified in this
     * constructor!
     *
     * The information saved will be restored upon destruction of the
     * object.
     *
     * Currently, the following information is saved:
     *
     *   - context return code
     *   - context message list
     *   - context error position
     */
    SQLMan_SavedErrorScope(SQLMan_Context& context);

    /*!
     * @brief Restores the saved context information
     *
     * - the old return code and related information will only be
     * restored if the current context's state is Ok
     * - the current context's message list will be overruled by the
     * saved message list
     */
    ~SQLMan_SavedErrorScope();

private:
    //no copy-construction or assignment
    SQLMan_SavedErrorScope(const SQLMan_SavedErrorScope&);
    SQLMan_SavedErrorScope operator=(SQLMan_SavedErrorScope&);

    SQLMan_Context& m_context;
    SAPDB_Int       m_returnCode;
    SAPDB_Int       m_errorPos;
    Msg_List        m_errorList;
};

#endif // SQLMAN_SAVEDERRORSCOPE_HPP
