/*****************************************************************************/
/*!
  @file         SQLMan_ChangedUserScope.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SQLMan_ChangedUserScope.

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

#ifndef SQLMAN_CHANGEDUSERSCOPE_HPP
#define SQLMAN_CHANGEDUSERSCOPE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class SQLMan_Context;
//class SQLMan_Identifier;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class  SQLMan_ChangedUserScope
 * @author MartinKi
 *
 * This class allows one to switch the user in the SQL context within
 * the lifetime of an object of this class.
 */
class SQLMan_ChangedUserScope
{
public:
    /*!
     * @brief Constructor.
     *
     * The current user in the context is switched to new user until
     * the destructor of the object is called. Check the context's
     * return code to verify that the switch was successful.
     *
     * @param context [in] the context in which the user should be changed
     * @param newUser [in] user name of the user to switch to
     */
    SQLMan_ChangedUserScope(
        SQLMan_Context&          context,
        const SQLMan_Identifier& newUser);

    /*!
     * @brief Constructor.
     *
     * The current user in the context is switched to new user until
     * the destructor of the object is called. Check the context's
     * return code to verify that the switch was successful.
     *
     * @param context       [in] the context in which the user should be changed
     * @param newUser       [in] user name of the user to switch to
     * @param newCurrSchema [in] current schema of the user to switch to
     */
    SQLMan_ChangedUserScope(
        SQLMan_Context&          context,
        const SQLMan_Surrogate&  newUser,
		const SQLMan_Surrogate&  newCurrSchema);

    /*!
     * @brief Destructor.
     *
     * Switches back the context to the original user if the switch in
     * the constructor has been successful.
     */
    ~SQLMan_ChangedUserScope();

private:
    SQLMan_Context& m_context;
    void*           m_pSavedContext;

    //no copy-construction or assignment
    SQLMan_ChangedUserScope(const SQLMan_ChangedUserScope&);
    SQLMan_ChangedUserScope operator=(SQLMan_ChangedUserScope&);
};

#endif // SQLMAN_CHANGEDUSERSCOPE_HPP
