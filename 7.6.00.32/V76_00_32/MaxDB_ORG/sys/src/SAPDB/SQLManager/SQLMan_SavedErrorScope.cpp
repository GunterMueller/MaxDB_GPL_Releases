/*****************************************************************************/
/*!
  @file         SQLMan_SavedErrorScope.cpp
  @author       MartinKi
  @ingroup

  @brief        Implementation file for class SQLMan_SavedErrorScope.

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"

#include "SQLManager/SQLMan_SavedErrorScope.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/**************************************************************************/

SQLMan_SavedErrorScope::SQLMan_SavedErrorScope(SQLMan_Context& context)
    : m_context( context ),
      m_returnCode( m_context.ReturnCode() ),
      m_errorList( m_context.GetErrorList() ),
      m_errorPos( m_context.GetErrorPos() )
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_SavedErrorScope::SQLMan_SavedErrorScope", SQLMan_Trace, 1);
}

/**************************************************************************/

SQLMan_SavedErrorScope::~SQLMan_SavedErrorScope()
{
	SAPDBTRACE_METHOD_DEBUG (
        "SQLMan_SavedErrorScope::~SQLMan_SavedErrorScope", SQLMan_Trace, 1);

    if ( m_context.IsOk() ) {
        m_context.SetReturnCode( m_returnCode );
        m_context.SetErrorPos( m_errorPos );
    }

    if ( ! m_errorList.IsEmpty() ) {
        m_errorList.MarkAsSubMessages();
        m_context.GetErrorList().MarkAsSubMessages();
        m_context.GetErrorList().Overrule( m_errorList );
    }
}
