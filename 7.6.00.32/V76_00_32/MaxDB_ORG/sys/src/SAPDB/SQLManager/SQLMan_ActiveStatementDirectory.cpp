/*!***************************************************************************

  module      : SQLMan_ActiveStatementDirectory.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : directory of currently active sql statements in session.

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

#include "SQLManager/SQLMan_ActiveStatementDirectory.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"

SQLMan_ActiveStatementDirectory::Comparator SQLMan_ActiveStatementDirectory::m_comparator;

//---------------------------------------------------------------------------------------------

SQLMan_ActiveStatementDirectory::SQLMan_ActiveStatementDirectory(SAPDBMem_IRawAllocator& alloc)
: m_activeStatements(&m_comparator, &alloc)
{
}

//---------------------------------------------------------------------------------------------
    
SQLMan_ActiveStatementDirectory::~SQLMan_ActiveStatementDirectory()
{
}

//---------------------------------------------------------------------------------------------
    
bool SQLMan_ActiveStatementDirectory::IsActive(StatementId statementId) const
{
    return (NULL != m_activeStatements.Find( statementId ));
}

//---------------------------------------------------------------------------------------------
    
bool SQLMan_ActiveStatementDirectory::RegisterStatement(StatementId statementId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_ActiveStatementDirectory::RegisterStatement", SQLMan_Trace, 1);
    SAPDBTRACE_WRITELN( SQLMan_Trace, 5, "statementId " << statementId);
    Container_AVLTreeError rc;
    m_activeStatements.Insert(statementId, rc);
    return (rc != AVLTree_OutOfMemory);  // duplicates are ok
}

//---------------------------------------------------------------------------------------------
    
void SQLMan_ActiveStatementDirectory::UnregisterStatement(StatementId statementId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_ActiveStatementDirectory::UnregisterStatement", SQLMan_Trace, 1);
    SAPDBTRACE_WRITELN( SQLMan_Trace, 5, "statementId " << statementId);
    m_activeStatements.Delete(statementId);
}

//---------------------------------------------------------------------------------------------

void SQLMan_ActiveStatementDirectory::UnregisterAll()
{
    m_activeStatements.DeleteAll();
}

