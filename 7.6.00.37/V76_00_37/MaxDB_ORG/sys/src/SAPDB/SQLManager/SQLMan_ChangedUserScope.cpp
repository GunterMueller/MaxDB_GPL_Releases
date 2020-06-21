/*****************************************************************************/
/*!
  @file         SQLMan_ChangedUserScope.cpp
  @author       MartinKi
  @ingroup

  @brief        Implementation file for class SQLMan_ChangedUserScope.

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

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Types.hpp"

#include "SQLManager/SQLMan_ChangedUserScope.hpp"

#include "hak51.h"  // a51switch_user, a51RestoreUserContext
#include "hak103.h" // a103GetSchemaName

/*****************************************************************************/

SQLMan_ChangedUserScope::SQLMan_ChangedUserScope(
    SQLMan_Context&          context,
    const SQLMan_Identifier& newUserName)
    : m_context( context ),
      m_pSavedContext( 0 )
{
    tsp00_Addr savedContextAddr;
    a51switch_user(
        m_context,
        const_cast<SQLMan_Identifier&>( newUserName ),
        savedContextAddr );

    m_pSavedContext = savedContextAddr;
}

SQLMan_ChangedUserScope::SQLMan_ChangedUserScope(
    SQLMan_Context&          context,
    const SQLMan_Surrogate&  newUser,
	const SQLMan_Surrogate&  newCurrSchema)
    : m_context( context ),
      m_pSavedContext( 0 )
{
	SQLMan_Identifier newUserName(false);
	SQLMan_Identifier newCurrSchemaName(false);

	a103GetSchemaName(context, newUser, newUserName);
	if ( !context.IsOk() )
	{
		context.ResetReturnCode();
		return;
	}
	if ( newCurrSchema != newUser )
	{
		a103GetSchemaName(context, newCurrSchema, newCurrSchemaName);
		if ( !context.IsOk() )
		{
			context.ResetReturnCode();
			return;
		}
	}

    tsp00_Addr savedContextAddr;
    a51switch_user(
        m_context,
        const_cast<SQLMan_Identifier&>( newUserName ),
        savedContextAddr );

	if ( newCurrSchema != newUser )
	{
        context.a_curr_schema_set   = true;
        context.a_curr_schema       = newCurrSchemaName;
        context.a_curr_schema_id    = newCurrSchema;
	}

    m_pSavedContext = savedContextAddr;
}

/**************************************************************************/

SQLMan_ChangedUserScope::~SQLMan_ChangedUserScope()
{
    tsp00_Addr savedContextAddr =
        reinterpret_cast<tsp00_Addr>( m_pSavedContext );

    a51RestoreUserContext( m_context, savedContextAddr );
}

/**************************************************************************/
