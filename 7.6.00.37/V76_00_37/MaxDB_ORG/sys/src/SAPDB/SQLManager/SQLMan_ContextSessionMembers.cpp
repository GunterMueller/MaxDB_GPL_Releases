/*!***************************************************************************

  module      : SQLMan_ContextSessionMembers.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : defines c++ session members SQL manager context.

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/SQLMan_TempFileCollection.hpp"
#include "SQLManager/DBProc/DBProc_Debugger.hpp"
#include "SQLManager/Catalog/Catalog_Interface.hpp"
#include "DBProc/Proc_SessionListContextInterface.hpp"
#include "SQLManager/SQLMan_SchemaMap.hpp"
#include "hak101.h"

SQLMan_ContextSessionMembers::SQLMan_ContextSessionMembers(SQLMan_Context& context)
: m_sessionCache(context)
, m_context(context)
, m_sharedSQL_SQLCache(context.GetAllocator())
, m_activeStatementDirectory(context.GetAllocator())
, m_dbprocDebugger(NULL)
, m_pseudoLongHandler(context)
, m_sqlRuntime(context)
, m_tempFileCollection(context.GetAllocator())
, m_udeSessions(NULL)
, m_schemaMap(NULL)
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_ContextSessionMembers::SQLMan_ContextSessionMembers", SQLMan_Trace, 5);

    m_tempFileCollection.Initialize();
}

//--------------------------------------------------------------------

SQLMan_ContextSessionMembers::~SQLMan_ContextSessionMembers()
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_ContextSessionMembers::~SQLMan_ContextSessionMembers", SQLMan_Trace, 5);
    DBProc_Debugger::releaseDebugger(&m_context);
    Proc_SessionListContextInterface::releaseList (&m_context.GetAllocator (), &m_udeSessions);
    m_context.GetAllocator().Deallocate(m_schemaMap);
}

//--------------------------------------------------------------------

void SQLMan_ContextSessionMembers::DestroySchemaMap()
{
    SQLMan_SchemaMap::Destroy (m_schemaMap);
}

//--------------------------------------------------------------------

SQLMan_SchemaMap* SQLMan_ContextSessionMembers::GetSchemaMap()
{
    if (NULL == m_schemaMap)
    {
        m_schemaMap = SQLMan_SchemaMap::Create(m_context);
    }
    return m_schemaMap;
}

//--------------------------------------------------------------------

Proc_SessionList * SQLMan_ContextSessionMembers::UDESessions ()
{
    if (m_udeSessions == NULL) {
        m_udeSessions = Proc_SessionListContextInterface::allocList (&m_context.GetAllocator ());
    }
    return m_udeSessions;
}

//--------------------------------------------------------------------

