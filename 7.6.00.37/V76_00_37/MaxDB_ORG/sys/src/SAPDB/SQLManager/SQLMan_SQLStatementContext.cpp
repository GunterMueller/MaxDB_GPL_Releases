/*!***************************************************************************

  module      : SQLMan_SQLStatementContext.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : internal call of SQL statements.

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

//#if defined (SAPDB_SLOW)
#include "hak10.h"
//#endif
#include "hak93.h"
#include "hak101.h"

#include "SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_TempFileCollection.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h"

//-------------------------------------------------------------------------

SQLMan_SQLStatementContext::SQLMan_SQLStatementContext(SQLMan_Context& context)
: m_statementId(-1)
, m_maxResId(context.a_max_res_id)
, m_context(context)
, m_pMessBlock(0)
, m_pStatementContext(0)
, m_pParameterInfo(0)
, m_pColumnInfo(0)
, m_warnings(context.TransContext().trWarning_gg00)
, m_localCacheUsageOnly(0 != context.a_localCacheUsageOnly)
, m_rollbackFile(context.a_sysdir.ci_cmd_rollback_tree)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_SQLStatementContext::SQLMan_SQLStatementContext", SQLMan_Trace, 1);
    m_statementId = context.RegisterCurrentStatement();
    if (m_statementId < 0)
    {
        context.SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_SQLMAN_NO_MEMORY/*, Msg_Arg("MEMREQUEST",?)*/));
        return;
    }

    m_pStatementContext = context.GetAllocator().Allocate(context.GetStatementPartSize());
    if (!m_pStatementContext)
    {
        context.SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_SQLMAN_NO_MEMORY, Msg_OptArg("MEMREQUEST",context.GetStatementPartSize())));
        return;
    }
    // save statement status of running sql statement
    SAPDB_MemCopyNoCheck (m_pStatementContext, context.GetStatementPartPtr(), context.GetStatementPartSize());
    // save current mess block
    const bool cDeepCopy = true;
    if (context.a_mblock.mb_type() != m_invalid)
    {
        m_pMessBlock = context.SaveMessBlock(!cDeepCopy);
    }
    // save parameter and column info in case of a select ... into
    if (context.IsOk() && context.IsSelectIntoStatement())
    {
        Catalog_ISessionCache& sessionCache = m_context.GetSessionCatalogCache();
        m_pParameterInfo = sessionCache.KeepIntoParameterInfo();
        m_pColumnInfo    = sessionCache.KeepColumnInfo();
    }
#if defined (SAPDB_SLOW)
    if (context.a_mblock.mb_data_len() > 0)
    memset (&context.a_mblock.mb_data()->mbp_buf()[context.a_mblock.mb_data_len()], 0xFF, 
    context.a_mblock.mb_data_size() - context.a_mblock.mb_data_len());
    a10DeallocAll (context);
#endif
    if (context.IsOk())
    {
        bool isDDL = (no_ddl != context.a_is_ddl);
        SQLMan_SqlMode sqlMode = context.a_sqlmode;
        memset (context.GetStatementPartPtr(), 0, context.GetStatementPartSize());
        if (isDDL)
        {
            // don't use shared cache in case of ddl execution
            context.a_localCacheUsageOnly = true;
        }
        m_context.SetSharedSQL();
        context.a_sqlmode.becomes(sqlMode);
        ++m_context.a_dbproc_level;
        // create initial syntax tree for new context
        context.NewSyntaxTree(256);
        // require new statement rollback file
        context.a_sysdir.ci_cmd_rollback_tree.fileRoot_gg00() = NIL_PAGE_NO_GG00;
        // TODO set variable input property and save input map !!
    }
    m_context.GetTempFileCollection().IncrementProcLevel();
}

/*-------------------------------------------------------------------*/

SQLMan_SQLStatementContext::~SQLMan_SQLStatementContext()
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_SQLStatementContext::~SQLMan_SQLStatementContext", SQLMan_Trace, 1);
    m_context.UnregisterStatement(m_statementId);
    m_context.DeleteSyntaxTree();
    if (m_pStatementContext)
    {
        // restore statement state of running sql statement
        SAPDB_MemCopyNoCheck (m_context.GetStatementPartPtr(), m_pStatementContext, m_context.GetStatementPartSize());
        m_context.GetAllocator().Deallocate(m_pStatementContext);
    }
    m_context.a_max_res_id = m_maxResId; // restore maximal result id
    if (m_pMessBlock)
    {
        // restore context mess block
        m_context.RestoreMessBlock(m_pMessBlock);
    }
    if (m_pParameterInfo || m_pColumnInfo)
    {
        Catalog_ISessionCache& sessionCache = m_context.GetSessionCatalogCache();
        sessionCache.RestoreIntoInfo(m_pParameterInfo);
        sessionCache.RestoreIntoInfo(m_pColumnInfo);
    }
    m_context.TransContext().trWarning_gg00 = m_warnings;
    m_context.a_localCacheUsageOnly         = m_localCacheUsageOnly;
    if (m_context.a_sysdir.ci_cmd_rollback_tree.fileRoot_gg00() != NIL_PAGE_NO_GG00)
    {
        // TODO use c++ interface when available
        a101_DestroyGroupedTempFile(m_context.TransContext(), m_context.a_sysdir.ci_cmd_rollback_tree);
#if defined (SAPDB_SLOW)
        if (e_ok != m_context.TransContext().trError_gg00)
        {
            m_context.ThrowError (m_context.TransContext().trError_gg00);
        }
#endif
    }
    m_context.a_sysdir.ci_cmd_rollback_tree = m_rollbackFile;
    m_context.GetTempFileCollection().DecrementProcLevel();
}


