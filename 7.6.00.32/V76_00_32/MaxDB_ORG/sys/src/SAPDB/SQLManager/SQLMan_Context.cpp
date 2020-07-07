/*!***************************************************************************

  module      : SQLMan_Context.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : defines SQL manager context.

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

#include "hak06.h"
#include "hak07.h"
#include "hak10.h"
#include "hak101.h"
#include "hak262.h"
#include "heo58.h"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IncrementalMemorySequence.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "SQLManager/SQLMan_TempFileCollection.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "Transaction/Trans_Context.hpp"
#include "KernelCommon/Kernel_Initialization.hpp"

#include "Messages/Msg_List.hpp"
#include "Messages/Msg_Arg.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h"


SQLMan_Context::SQLMan_Context(SQLMan_TransContext& transContext)
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_Context::SQLMan_Context", SQLMan_Trace, 5);
    memset                        ( this, 0, sizeof(*this));
    Kernel_InitSqlContext         (*this, transContext);
    a10_init_cache                (*this);
    Kernel_AllocateSessionBuffers (*this);
}

/*----------------------------------------------------------------*/

SQLMan_Context::~SQLMan_Context()
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_Context::~SQLMan_Context", SQLMan_Trace, 5);
    a10free_cache (*this);
    Kernel_DeallocateSessionBuffers(*this, false);
}

/*----------------------------------------------------------------*/

void SQLMan_Context::DeleteSyntaxTree()
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_Context::DeleteSyntaxTree", SQLMan_Trace, 5);
    if (    (a_ap_tree!=a_session_ap_tree) && 
            (a_ap_tree!=0) ) // enable DeleteSyntaxTree to be called twice (D.T.)
    {
        GetAllocator().Deallocate(a_ap_tree);
        a_ap_tree = 0;
    }
}

/*----------------------------------------------------------------*/

void SQLMan_Context::DestroyCppMembers()
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_Context::DestroyCppMembers", SQLMan_Trace, 5);
    SQLMan_ContextSessionMembers* p = REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members);
    if (p)
    {
        destroy(p, this->GetAllocator());
    }
    a_cpp_session_members = NULL;
}

/*----------------------------------------------------------------*/

SQLMan_Context* SQLMan_Context::GetContext()
{
    return REINTERPRET_CAST(SQLMan_Context*, vGetAcvPtrFromCurrentTask());
}

/*----------------------------------------------------------------*/

SQLMan_SessionInfo& SQLMan_Context::GetSessionInfo()
{
    return CppMembers().m_sessionInfo;
}

/*----------------------------------------------------------------*/
SQLMan_ContextSessionMembers& SQLMan_Context::CppMembers()
{
    return *REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members);
}

const SQLMan_ContextSessionMembers& SQLMan_Context::CppMembersConst() const
{
    return *REINTERPRET_CAST(const SQLMan_ContextSessionMembers*, a_cpp_session_members);
}

bool SQLMan_Context::IsStatementActive(SAPDB_Int statementId) const
{
    return CppMembersConst().m_activeStatementDirectory.IsActive(statementId);
}

KSQL_Runtime& SQLMan_Context::GetSQLDBCRuntime()
{
    return CppMembers().m_sqlRuntime;
}

void SQLMan_Context::InitInterfaceTaskTraceContext()
{
    CppMembers().m_sqlRuntime.InitTraceContext();
}

SAPDB_Int4 SQLMan_Context::RegisterCurrentStatement()
{
    if (CppMembers().m_activeStatementDirectory.RegisterStatement(a_command_id))
    {
      return a_command_id;
    }
    else
    {
        return -1;
    }
}

void SQLMan_Context::UnregisterStatement(SAPDB_Int4 statementId)
{
    CppMembers().m_activeStatementDirectory.UnregisterStatement(statementId);
}

/*----------------------------------------------------------------*/

DBProc_Debugger* SQLMan_Context::GetDBProcDebugger() const
{
    return  REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members)->m_dbprocDebugger;
}

/*----------------------------------------------------------------*/

Msg_List& SQLMan_Context::GetErrorList() const
{
    return *reinterpret_cast<Msg_List*>(a_transinf.tri_trans.trErrorList_gg00); // UH 2004-11-18 changed to trErrorList_gg00
}

void SQLMan_Context::ResetErrorList()
{
    reinterpret_cast<Msg_List*>(a_transinf.tri_trans.trErrorList_gg00)->ClearMessageList(); // UH 2004-11-18 changed to trErrorList_gg00
}

bool SQLMan_Context::IsErrorListEmpty() const
{
    return reinterpret_cast<Msg_List*>(a_transinf.tri_trans.trErrorList_gg00)->IsEmpty(); // UH 2004-11-18 changed to trErrorList_gg00
}
/*----------------------------------------------------------------*/

SysView_PseudoLongHandler& SQLMan_Context::GetPseudoLongHandler() const
{
    return REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members)-> m_pseudoLongHandler;
}

/*----------------------------------------------------------------*/

Catalog_ISessionCache& SQLMan_Context::GetSessionCatalogCache()
{
    return REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members)->m_sessionCache;
}

/*----------------------------------------------------------------*/

SharedSQL_SQLCache& SQLMan_Context::GetSharedSQL_SQLCache()
{
    return REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members)->m_sharedSQL_SQLCache;
}

/*----------------------------------------------------------------*/

SQLMan_TempFileCollection& SQLMan_Context::GetTempFileCollection()
{
    return REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members)->m_tempFileCollection;
}

/*----------------------------------------------------------------*/

Proc_SessionList * SQLMan_Context::GetUDESessions()
{
    return REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members)->UDESessions ();
}

/*----------------------------------------------------------------*/

void SQLMan_Context::GetNlsParams(SQLMan_NlsParams& nlsParams) const
{
    nlsParams = a_nls_params;
}

/*----------------------------------------------------------------*/

void SQLMan_Context::DestroySchemaMap()
{
    reinterpret_cast<SQLMan_ContextSessionMembers*>(a_cpp_session_members)->DestroySchemaMap();
}

/*----------------------------------------------------------------*/

SQLMan_SchemaMap* SQLMan_Context::GetSchemaMap()
{
    return reinterpret_cast<SQLMan_ContextSessionMembers*>(a_cpp_session_members)->GetSchemaMap();
}

/*----------------------------------------------------------------*/

void SQLMan_Context::InitCppMembers()
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_Context::InitCppMembers", SQLMan_Trace, 5);
    a_cpp_session_members = new(this->GetAllocator()) SQLMan_ContextSessionMembers(*this);
}

/*----------------------------------------------------------------*/

void SQLMan_Context::IncrementSyntaxTree()
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_Context::IncrementSyntaxTree", SQLMan_Trace, 5);
    if ( a_ap_tree != a_session_ap_tree ) 
        if ( a_ap_max_node < cak_max_node )
        {

            //// grow by 128 nodes
            //const int supplement = 128;
            //int newSize      = (a_ap_max_node + 1 + supplement) * sizeof(tak_ap_node);

            // grow by 128 nodes
            int supplement   = (cak_max_node - a_ap_max_node);
            supplement       = (supplement > 128 ? 128 : supplement); 
            int newSize      = (a_ap_max_node + 1 + supplement) * sizeof(tak_ap_node);

            void* pNewTree = GetAllocator().Reallocate(newSize, a_ap_tree);
            if (pNewTree)
            {
                a_ap_tree      = REINTERPRET_CAST(tak_ap_max_tree_ptr, pNewTree);
                a_ap_max_node += supplement;
            }
            else
            {
                this->SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_SHAREDSQL_NO_MEMORY, Msg_Arg("MEMREQUEST",newSize)));
            }
        }
        else
        {
            a07_b_put_error (*this, e_stack_overflow, 1);
        }
}

/*----------------------------------------------------------------*/

void SQLMan_Context::NewSyntaxTree(int size)
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_Context::NewSyntaxTree", SQLMan_Trace, 5);
    if ( a_ap_tree == a_session_ap_tree || a_ap_tree == 0 )
    {
        a_ap_tree = REINTERPRET_CAST(tak_ap_max_tree_ptr, GetAllocator().Allocate(size * sizeof(tak_ap_node)));
        if (a_ap_tree)
        {
            a_ap_max_node = size - 1;
            a_scv_index   = 0;
            a_maxnode     = 0;
        }
        else
        {
            this->SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_SHAREDSQL_NO_MEMORY, Msg_Arg("MEMREQUEST",size * sizeof(tak_ap_node))));
        }
    }
}

/*----------------------------------------------------------------*/

void  SQLMan_Context::RestoreMessBlock(void* pSavedMessBlock)
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_Context::RestoreMessBlock", SQLMan_Trace, 5);
    SQLMan_MessBlock* pMessBlock = REINTERPRET_CAST(SQLMan_MessBlock*, pSavedMessBlock);
    a_mblock.mb_data_len() = 0;
    tgg00_BasisError e = e_ok;
    a06cpy_mblock (*this, *pMessBlock, a_mblock, false, e);
    if (e_ok != 0)
    {
        ThrowError(e, 1);
    }
    for ( SAPDB_UInt fieldlistcnt = 0; fieldlistcnt <= cgg_idx_max_valuefieldlist; ++fieldlistcnt )
    {
        SAPDBFields_MemorySequence* pSequence = reinterpret_cast<SAPDBFields_MemorySequence*>(a_mblock.mb_fieldlists()[fieldlistcnt]);
        if (pSequence)
        {
            SAPDBFields_MemorySequence::DropReference(pSequence);
        }
    }
    pMessBlock->DestroyParts(this->GetAllocator());
    this->GetAllocator().Deallocate(pMessBlock);
}

/*----------------------------------------------------------------*/

void* SQLMan_Context::SaveMessBlock(bool DeepCopy)
{
    SAPDBTRACE_METHOD_DEBUG ("SQLMan_Context::SaveMessBlock", SQLMan_Trace, 5);
    SAPDB_UInt4 dummy;
    SQLMan_MessBlock* pMessBlock = REINTERPRET_CAST(SQLMan_MessBlock*, &a_mblock);
    return SQLMan_MessBlock::CreateCopy(*pMessBlock, this->GetAllocator(), 0, DeepCopy, dummy);
}

/*----------------------------------------------------------------*/

void SQLMan_Context::SetDBProcDebugger(DBProc_Debugger* p)
{
    REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members)->m_dbprocDebugger = p;
}

/*----------------------------------------------------------------*/

void SQLMan_Context::SetNoMoreMemoryError(const Msg_List& MsgList)
{
    this->GetErrorList().AppendNewMessage (MsgList);
    a07_b_put_error (*this, e_no_more_memory, 1);
}

/*----------------------------------------------------------------*/

void SQLMan_Context::SetSharedSQL()
{
    a_createSharedSQL = a101_SharedSQL_GetStatusParameter();
}

/*----------------------------------------------------------------*/

void SQLMan_Context::SetStopMsg (SAPDB_Int2 rc,
                                 const void* msg,
                                 int         msgLength)
{
    const bool isFunctionStopMsg = true;
    const bool isUserDefinedMsg  = true;
    a262stop (this, rc, msgLength, REINTERPRET_CAST(tsp00_MoveObjPtr, CONST_CAST(void*, msg)), !isFunctionStopMsg, !isUserDefinedMsg);
}

/*----------------------------------------------------------------*/

void SQLMan_Context::CopyResultPart (const PIn_Part & part)
{
    a262copy_result_part (this, *part.GetRawPart ());
}

/*----------------------------------------------------------------*/

Trans_Context& SQLMan_Context::GetTransContext()
{
    return Trans_Context::GetContext (a_transinf.tri_trans);
}

/*----------------------------------------------------------------*/

void SQLMan_Context::ThrowError (SAPDB_Int2 error, int errorPos, int keyWord)
{
    if (keyWord > 0)
    {
        a07_kw_put_error (*this, error, errorPos, keyWord);
    }
    else
    {
        a07_b_put_error (*this, error, errorPos);
    }
}

/*----------------------------------------------------------------*/

 void SQLMan_Context::ThrowError (SAPDB_Int2 error, int errorPos, const SQLMan_Identifier& Identifier)
 {
     SQLMan_Identifier id = Identifier;
     a07_nb_put_error (*this, error, errorPos, id);
 }
