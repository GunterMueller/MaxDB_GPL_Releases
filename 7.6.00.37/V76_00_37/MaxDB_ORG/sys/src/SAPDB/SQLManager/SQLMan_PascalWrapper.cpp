/*****************************************************************************

  @file SQLMan_PascalWrapper.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : defines PASCAL interface of SQL manager.

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

#include "hak01.h"
#include "hak06.h"
#include "hak07.h"
#include "hak071.h"
#include "hak10.h"
#include "hak101.h"
#include "hsp77.h"
#include "hsp80.h"
#include "hsp81.h"
#include "hbd998.h"
#include "hgg01_3.h"
#include "hgg06.h" // gg06SessionGetInt4
#include "gsp00.h"
// next two to be excluded if 'new' messagelist can be used without old one
#include "geo200.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp"

#include "SAPDB/Container/Container_Vector.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"
#include "SQLManager/SQLMan_UpdateStatistics.hpp"
#include "SQLManager/SQLMan_UpdateCounter.hpp"
#include "SQLManager/SQLMan_Connect.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_DiagnoseDropFile.hpp"
#include "SQLManager/SQLMan_SessionInfo.hpp"
#include "SQLManager/SQLMan_SessionList.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Handles.hpp"
#include "SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "SAPDB/SQLManager/SharedSQL/SharedSQL_CommandCache.hpp"
#include "SQLManager/Catalog/Catalog_SessionCache.hpp"
#include "SQLManager/Catalog/Catalog_TableObject.hpp"
#include "SQLManager/Catalog/Catalog_SharedSqlObject.hpp"
#include "SQLManager/Catalog/Catalog_Instance.hpp"
#include "SQLManager/Catalog/Catalog_Interface.hpp"
#include "SQLManager/DBProc/DBProc_Debugger.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Instance.hpp"
#include "SQLManager/SQLMan_SchemaMap.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "KernelCommon/Kernel_Dump.hpp"
#include "RunTime/RTE_MySQLProxy.h"
#include "Join/Join_HashAccessAllocator.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include "hbd998.h"
#include "hbd01.h"

#include "SQLManager/SQLMan_LogicalFileNameGenerator.hpp"
#include "SQLManager/SQLMan_TempFileCollection.hpp"
#include "DataAccess/Data_TempFileTypes.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "hbd998.h"
#include "PacketInterface/PacketCheck/PkgCheck_Checker.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_VarData.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_PropertyList.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SQLManager/Tables/SQLTab_SysCheckTableLog.hpp"
#include "SQLManager/Tables/SQLTab_SysUpgradeHistory.hpp"
#include "SQLManager/Tables/SQLTab_SysUpdStatLog.hpp"
#include "SQLManager/Tables/SQLTab_SysUpdStatWanted.hpp"
#include "SQLManager/Tables/SQLTab_SysUpdateCounterWanted.hpp"
#include "KernelCommon/Kernel_IAdminConfig.hpp"



#include "SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h"

//-------------------------------------------------------------------------

// for test purposes the shared sql allocator can be write protected
// except in the shared sql interface routines. This allows to find
// source code that unauthorized writes into data stored in the
// shared sql component.
// caution : this works in a single user environment only !

// DirkT: To switch on SharedSQL Protection use praeprozesor define in SharedSQL_Types.hpp instead of the following !!!
//#define SHAREDSQL_PROTECT 1

#define SQLMAN_SHARED_SQL_TRACE(X) \
if (SharedSQL_Trace.TracesLevel (3)) {\
    SAPDBTrace_Stream stream (&SharedSQL_Trace.GetTracePrinter ());\
    stream << X << NewLine;\
}

inline SAPDB_OStream & operator<< (SAPDB_OStream & ostream, const SharedSQL_ParseID & parseId)
{
    SAPDBTrace_Hex hex(&parseId, sizeof(parseId));
    return hex.Print(ostream);
}

#if defined (SHAREDSQL_PROTECT)
class SQLMan_AllocatorWriteUnProtector
{
public :
    SQLMan_AllocatorWriteUnProtector(SAPDBMem_RawAllocator* pAllocator)
        : m_pAllocator(pAllocator)
    {
        if (m_pAllocator)
        {
            m_pAllocator->Unprotect();
        }
    };
    ~SQLMan_AllocatorWriteUnProtector()
    {
        if (m_pAllocator)
        {
            m_pAllocator->WriteProtect();
        }
    }
private :
   SAPDBMem_RawAllocator* m_pAllocator;
};

static SAPDBMem_RawAllocator* m_pSharedSQLAllocator = 0;

#define SQLMAN_UNPROTECT_ALLOCATOR(A) if (0 == m_pSharedSQLAllocator) \
                                         m_pSharedSQLAllocator = A;   \
                                      SQLMan_AllocatorWriteUnProtector unprotector(m_pSharedSQLAllocator);
#else
#define SQLMAN_UNPROTECT_ALLOCATOR(A)
#endif

namespace SQLMan_PascalWrapper {

    class SQLMan_SharedSQLInvalidateInfo
    {
    public :
        SQLMan_SharedSQLInvalidateInfo (SAPDBMem_IRawAllocator& Allocator, bool NotifyApplication)
            : m_NotifyApplication(NotifyApplication)
            , m_invalidateAll(false)
            , m_invalidateSchema(false)
            , m_compare()
            , m_tables ()
        {
            m_tables.AdviseAllocator(&Allocator);
            m_tables.AdviseCompare(&m_compare);
        }
        void Store (const tgg00_Surrogate& SchemaId, const tsp00_KnlIdentifier& Tablename, bool ExplicitSchema)
        {
            if (m_invalidateAll)
            {
                return;
            }
            if (Tablename == a01_il_b_identifier)
            {
                m_invalidateSchema = true;
            }
            else
            {
                if (m_invalidateSchema)
                {
                    return;
                }
            }
            SharedSQL_Table table(SchemaId, Tablename, ExplicitSchema);
            Container_AVLTreeError rc;
            m_tables.Insert (table, rc);
            if (AVLTree_OutOfMemory == rc)
            {
                m_invalidateAll = true;
                m_tables.DeleteAll();
            }
        }
        void Invalidate (SQLMan_Context& Context, SharedSQL_SQLCache& SharedSQLCache)
        {
            if (m_invalidateAll)
            {
                SharedSQLCache.Invalidate(m_NotifyApplication);
                return;
            }
            if (m_invalidateSchema)
            {
                SharedSQL_TableTree::Iterator iter = m_tables.First();
                while (iter)
                {
                    SharedSQLCache.Invalidate(Context, iter()->GetKey()->mSchema, m_NotifyApplication);
                    ++iter;
                }
                return;
            }
            else
            {
                SharedSQLCache.Invalidate(Context, &m_tables, m_NotifyApplication);
            }
        }
    private :
		bool                m_NotifyApplication;
        bool                m_invalidateAll;
        bool                m_invalidateSchema;
        SharedSQL_Table     m_compare;
        SharedSQL_TableTree m_tables;
    };

};

using namespace SQLMan_PascalWrapper;

//-------------------------------------------------------------------------

externPascal tgg00_VoidPtr a101_GetErrorMessageList (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    // UH 2004-10-01 new
	SAPDBTRACE_ROUTINE_DEBUG ("a101_GetErrorMessageList", SQLMan_Trace, 1);
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    return & context.GetErrorList();
}

//-------------------------------------------------------------------------

void a101_AnalyzeErrorMessageList (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    Msg_List                            errList;
    SAPDB_UInt4                         savedBytes;
    tsp1_part                          *pError;
    void                               *pMsgText;

	SAPDBTRACE_ROUTINE_DEBUG ("a101_AnalyzeErrorMessageList", SQLMan_Trace, 1);

    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    Msg_List & acvMsgList = context.GetErrorList();

    if (!acvMsgList.IsEmpty())
    {
#ifdef SAPDB_WITH_NEW_MSG_LIST_OUTPUT_CLASSES
        if ( !Msg_IOutput::GetDiagnosticOutputChannel().Output(acvMsgList, errList) )
        {
            Msg_IOutput::GetEmergencyOutputChannel().Output(acvMsgList);
        }
#endif
        RTE_Message (acvMsgList);

        if (NULL == context.GetCurrentReturnPartPtr())
		{
			a06init_curr_retpart (acv);
		}

		pError = context.GetCurrentReturnPartPtr();
		pMsgText = REINTERPRET_CAST(SAPDB_Byte*, pError) + sizeof (pError->sp1p_part_header());

        if ( acvMsgList.Store( context.GetCurrentReturnPartPtr()->sp1p_buf_size()
                             , true
                             , pMsgText
			                 , savedBytes
                             , errList) )
		{
			pError->sp1p_buf_len() = savedBytes;
			a06finish_curr_retpart (acv, sp1pk_message_list, 1);
		}

		acvMsgList.ClearMessageList();
	 }
}

//-------------------------------------------------------------------------

void a101_ResetReturnCode(
    tak_all_command_glob  VAR_VALUE_REF  acv)
{

	SAPDBTRACE_ROUTINE_DEBUG ("a101_ResetReturnCode", SQLMan_Trace, 1);

    SQLMan_Context::AcvToContext(acv).ResetReturnCode();    
}

//-------------------------------------------------------------------------

void a101_AttachColumnBuffer (
    tgg00_VoidPtr                        pTable)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_AttachColumnBuffer", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );
    SQLMAN_UNPROTECT_ALLOCATOR(0);
    REINTERPRET_CAST(Catalog_TableObject*, pTable)->IncColBufferRefCount();
}

//-------------------------------------------------------------------------

pasbool a101_CheckMySqlPassword (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Name            VAR_ARRAY_REF  password,
    pasbool                              isOldVersion,
    tsp00_C8              VAR_ARRAY_REF  receivedPassword)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_CheckMySqlPassword", SQLMan_Trace, 1);
    tsp00_Int4 hash0, hash1;
    RTE_GetMySQLHash(hash0, hash1, password);
    return RTE_CheckMySQLPassword(receivedPassword, isOldVersion, acv.a_challenge, hash0, hash1);
}

//-------------------------------------------------------------------------

void a101_CatalogCollectUpgradeInfo (
    tsp00_C40             VAR_ARRAY_REF  msg)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_CatalogCollectUpgradeInfo", Catalog_Trace, 1);
    SAPDB_Char nullTerminatedMsg[sizeof(msg)+1];
    SAPDB_MemCopyNoCheck (&nullTerminatedMsg[0], &msg[0], sizeof(msg));
    nullTerminatedMsg[sizeof(msg)] = 0;
    Kernel_IAdminConfig::Instance().MigrationHandler().CollectMigrationInfo (Kernel_Migration::catalog, &nullTerminatedMsg[0]);
}

//--------------------------------------------------------------------------

void a101_CollectNewSoftwareVersion ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_CollectNewSoftwareVersion", Catalog_Trace, 1);
    Kernel_IAdminConfig::Instance().MigrationHandler().CollectMigrationInfo (Kernel_Migration::all, "NEW SOFTWARE VERSION");
}


//--------------------------------------------------------------------------

void a101_StoreUpgradeInfo ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_StoreUpgradeInfo", Catalog_Trace, 1);
    SQLTab_SysUpgradeHistory sysUpgradeHistory;
    sysUpgradeHistory.StoreUpgradeHistory();
}

//--------------------------------------------------------------------------

void a101_CopyMessBlockShallow (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_MessBlock       VAR_VALUE_REF  Source,
    tgg00_MessBlock       VAR_VALUE_REF  Destination,
    pasbool                              withoutData,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    SQLMan_MessBlock* pSource      = reinterpret_cast<SQLMan_MessBlock*>(&Source);
    SQLMan_MessBlock* pDestination = reinterpret_cast<SQLMan_MessBlock*>(&Destination);
    e = pSource->Copy (SQLMan_Context::AcvToContext(acv), *pDestination, withoutData != 0, NULL);
}

//-------------------------------------------------------------------------

pasbool a101_DebuggerActive (
    tgg00_TransContext    VAR_VALUE_REF  transContext)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DebuggerActive", SQLMan_Trace, 1);
    tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, transContext.trAcvPtr_gg00);
    return (pAcv && (NULL != SQLMan_Context::AcvToContext(*pAcv).GetDBProcDebugger()));
}

//-------------------------------------------------------------------------

void a101_DebuggerCheckDebugBreak (
    tgg00_TransContext       VAR_VALUE_REF  transContext,
    tgg00_SelectFieldsParam  VAR_VALUE_REF  interpreterContext,
    integer                                 currStackEntry,
    tsp00_MoveObjPtr                        pVariables,
    integer                                 codePos,
    integer                                 codeLength,
    tgg00_BasisError         VAR_VALUE_REF  e)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DebuggerCheckDebugBreak", SQLMan_Trace, 1);
    tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, transContext.trAcvPtr_gg00);
    DBProc_Debugger* pDebugger = SQLMan_Context::AcvToContext(*pAcv).GetDBProcDebugger();
    e = e_ok;
    if (pDebugger)
    {
        tak_sysbufferaddress p  = REINTERPRET_CAST(tak_sysbufferaddress, interpreterContext.sfp_pl_ptr());
        SQLMan_MessBlock* pCode = REINTERPRET_CAST(SQLMan_MessBlock*, &p->smessblock().mbr_mess_block);
        int type                = (mm_trigger == pCode->mb_type2()) ? p->syskey().slinkage()[1] : 1;
        int kind;
        switch (pCode->mb_type2())
        {
        case mm_trigger :
            kind = cak_i_trigger;
            break;
        case mm_ignore :
            kind = cak_i_function;
            break;
        default :
            kind = cak_i_dbproc;
        }
        e = pDebugger->checkDebugBreak(pCode->GetTableId(), kind, type, currStackEntry,
            REINTERPRET_CAST(tsp00_MoveObjPtr, interpreterContext.sfp_rec_addr()), pVariables, codePos, codeLength);
    }
}

//-------------------------------------------------------------------------

void a101_DebuggerCreate (
    tak_all_command_glob  VAR_VALUE_REF  debuggingAcv,
    tak_all_command_glob  VAR_VALUE_REF  debuggedAcv,
    tak_all_command_glob  VAR_VALUE_REF  listenerAcv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DebuggerCreate", SQLMan_Trace, 1);
    DBProc_Debugger::createDebugger(SQLMan_Context::AcvToContext(debuggingAcv),
        SQLMan_Context::AcvToContext(debuggedAcv),
        SQLMan_Context::AcvToContext(listenerAcv));
}

//-------------------------------------------------------------------------

void a101_DebuggerGetExecuteInfo (
    tsp00_MoveObjPtr                     pInfo,
    integer                              size)
{
    DBProc_Debugger::getExecuteInfo (pInfo, size);
}

//-------------------------------------------------------------------------

void a101_DebuggerGetFunctionParameter (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_StackEntry      VAR_VALUE_REF  stackCode,
    tsp00_MoveObjPtr      VAR_VALUE_REF  pParam,
    integer               VAR_VALUE_REF  length)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DebuggerGetFunctionParameter", SQLMan_Trace, 1);
    DBProc_Debugger* pDebugger = SQLMan_Context::AcvToContext(acv).GetDBProcDebugger();
    if (pDebugger)
    {
        pDebugger->getFunctionParameter (stackCode, pParam, length);
    }
}

//-------------------------------------------------------------------------

void a101_DebuggerGetMethodInfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  methodId,
    integer               VAR_VALUE_REF  methodKind,
    integer               VAR_VALUE_REF  linkage,
    tsp00_MoveObjPtr      VAR_VALUE_REF  pTriggerColumns,
    tsp00_MoveObjPtr      VAR_VALUE_REF  pVariables)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DebuggerGetMethodInfo", SQLMan_Trace, 1);
    DBProc_Debugger* pDebugger = SQLMan_Context::AcvToContext(acv).GetDBProcDebugger();
    if (pDebugger)
    {
        pDebugger->getInfo (methodId, methodKind, linkage, pTriggerColumns, pVariables);
    }
}

//-------------------------------------------------------------------------

void a101_DebuggerListen (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  methodId,
    integer               VAR_ARRAY_REF  procKind,
    integer               VAR_ARRAY_REF  linkage,
    integer               VAR_VALUE_REF  pos,
    integer               VAR_VALUE_REF  length)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DebuggerListen", SQLMan_Trace, 1);
    DBProc_Debugger* pDebugger = SQLMan_Context::AcvToContext(acv).GetDBProcDebugger();
    if (pDebugger)
    {
        pDebugger->listen(methodId, procKind, linkage, pos, length);
    }

}

//-------------------------------------------------------------------------

externPascal void a101_DebuggerPopCall (
    tgg00_TransContext       VAR_VALUE_REF  transContext)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DebuggerPopCall", SQLMan_Trace, 1);
    tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, transContext.trAcvPtr_gg00);
    DBProc_Debugger* pDebugger = SQLMan_Context::AcvToContext(*pAcv).GetDBProcDebugger();
    if (pDebugger)
    {
        pDebugger->popCall();
    }
}

//-------------------------------------------------------------------------

externPascal void a101_DebuggerPushCall (
    tgg00_TransContext       VAR_VALUE_REF  transContext,
    tgg00_SelectFieldsParam  VAR_VALUE_REF  interpreterContext)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DebuggerPushCall", SQLMan_Trace, 1);
    tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, transContext.trAcvPtr_gg00);
    DBProc_Debugger* pDebugger = SQLMan_Context::AcvToContext(*pAcv).GetDBProcDebugger();
    if (pDebugger)
    {
        tak_sysbufferaddress p  = REINTERPRET_CAST(tak_sysbufferaddress, interpreterContext.sfp_pl_ptr());
        SQLMan_MessBlock* pCode = REINTERPRET_CAST(SQLMan_MessBlock*, &p->smessblock().mbr_mess_block);
        int linkage             = (mm_trigger == pCode->mb_type2()) ? p->syskey().slinkage()[1] : 1;
        int kind;
        switch (pCode->mb_type2())
        {
        case mm_trigger :
            kind = cak_i_trigger;
            break;
        case mm_ignore :
            kind = cak_i_function;
            break;
        default :
            kind = cak_i_dbproc;
        }
        pDebugger->pushCall (pCode->GetTableId(), kind, linkage, &interpreterContext);
    }
}

//-------------------------------------------------------------------------

void a101_DebuggerStep (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              count)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DebuggerStep", SQLMan_Trace, 1);
    DBProc_Debugger* pDebugger = SQLMan_Context::AcvToContext(acv).GetDBProcDebugger();
    if (pDebugger)
    {
        pDebugger->step(count);
    }
}

//-------------------------------------------------------------------------

void a101_DebuggerStoreExecuteInfo (
    tsp00_MoveObjPtr                     pInfo,
    integer                              size)
{
    DBProc_Debugger::storeExecuteInfo (pInfo, size);
}

//-------------------------------------------------------------------------

void a101_DebuggerAddRemoveBreakPoint (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  procId,
    integer                              procKind,
    integer                              type,
    integer                              pos,
    pasbool                              doRemove)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101DebuggerWaitForBreakPoint", SQLMan_Trace, 1);
    DBProc_Debugger* pDebugger = SQLMan_Context::AcvToContext(acv).GetDBProcDebugger();
    if (pDebugger)
    {
        pDebugger->addRemoveBreakPoint(procId, procKind, type, pos, 0 != doRemove);
    }
}

//-------------------------------------------------------------------------

void a101_DetachColumnBuffer (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_VoidPtr                        pTable)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DetachColumnBuffer", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );
    SQLMAN_UNPROTECT_ALLOCATOR(0);
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    SAPDBMem_IRawAllocator* pAllocator = SharedSQL_CommandCache::Instance(SAPDB_FALSE);
    if (pAllocator)
    {
        REINTERPRET_CAST(Catalog_TableObject*, pTable)->DecColBufferRefCount(*pAllocator);
    }
}

//-------------------------------------------------------------------------

tak_sysbufferaddress a101_DescribeParseId (
    tgg00_VoidPtr                        pExecuteHandle)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DescribeParseId", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );
    SQLMAN_UNPROTECT_ALLOCATOR (0);
    Catalog_SharedSqlObject::Catalog_Key key;
    SharedSQL_ExecuteHandle& executeHandle = *REINTERPRET_CAST (SharedSQL_ExecuteHandle*, pExecuteHandle);
    const void* p = executeHandle.GetPlan(&key);
    if (p)
    {
       const tsp00_C12& describeKey = REINTERPRET_CAST(const Catalog_SharedSqlObject*, p)->GetDescribeId();
       Catalog_Object::Catalog_Key resultKey(describeKey);
       p = executeHandle.GetPlan(&resultKey);
       if (p)
       {
         unsigned char* pChar = REINTERPRET_CAST(unsigned char*, CONST_CAST(void*, p)) + Catalog_SessionCache::ObjectHeader;
         return REINTERPRET_CAST(tak_sysbufferaddress, pChar);
       }
    }
    return NULL;
}

//-------------------------------------------------------------------------

pasbool a101_DropParseId (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_parsid            VAR_VALUE_REF  parsedId)
{
    // drops a parseid. Returns true (ok), if the parseid has been dropped successfully
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DropParseId", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
	SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();

    SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());

    return (false == sharedSQLCache.DropParseID(*REINTERPRET_CAST(const SharedSQL_ParseID*, &parsedId)));
}

//-------------------------------------------------------------------------

void a101_DestroyContextMembers (tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DestroyContextMembers", SQLMan_Trace, 1);
    SQLMAN_UNPROTECT_ALLOCATOR (0);
    SQLMan_Context::AcvToContext(acv).DestroyCppMembers();
}

//-------------------------------------------------------------------------

void a101_GetMySQLScramble (
    tsp00_C8              VAR_ARRAY_REF  scramble)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_GetMySQLScramble", SQLMan_Trace, 1);
    RTE_GetMySQLScramble(scramble);
}

//-------------------------------------------------------------------------

void a101_InitContextMembers (tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_InitContextMembers", SQLMan_Trace, 1);
    SQLMan_Context::AcvToContext(acv).InitCppMembers();
}

//-------------------------------------------------------------------------

externPascal void a101_InitSessionInfo (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_InitSessionInfo", SQLMan_Trace, 1);
    
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    context.GetSessionInfo().Init(context);
}

//-------------------------------------------------------------------------

externPascal void a101_SetSessionInfoIsolationLevel (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Uint1                          isolationLevel)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_SetSessionInfoIsolationLevel", SQLMan_Trace, 1);
    
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    context.GetSessionInfo().SetIsolationLevel(isolationLevel);
}

//-------------------------------------------------------------------------

pasbool a101_IsStatementActive (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int4                           statementId)
{
    return SQLMan_Context::AcvToContext(acv).IsStatementActive(statementId);
}

//-------------------------------------------------------------------------

void* a101_GetExecuteHandle (
     tak_all_command_glob  VAR_VALUE_REF  acv,
	 tak_parsid            VAR_VALUE_REF  parseId,
     pasbool               CountExecuting )
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_GetExecuteHandle", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();

    SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());

    SharedSQL_ParseID& ParseId      = *REINTERPRET_CAST(SharedSQL_ParseID*, &parseId);

    SQLMAN_SHARED_SQL_TRACE ("GetExecuteHandle for ParseID" << ParseId);

    SAPDB_Bool                  Valid, minus9;
    const SharedSQL_ModuleName* pModulename;
    SharedSQL_ExecuteHandle* pExecuteHandle = sharedSQLCache.NewExecuteHandle(context.GetAllocator(), ParseId,
        (CountExecuting?true:false), Valid, minus9, pModulename, context.GetErrorList());
	if (pExecuteHandle)
	{
        if (!Valid)
        {
            SQLMAN_SHARED_SQL_TRACE ("Plan " << ToStr(pExecuteHandle->GetPlanObjectPtr()) << " invalid , -9 = " << ToStr(minus9));
			SQLMAN_SHARED_SQL_TRACE ("a_returncode      = " << acv.a_returncode);
            context.ThrowError (e_old_fileversion);
            return pExecuteHandle;
        }
        if (pModulename && pModulename->mSize > 0)
        {
            int modulnamelen = pModulename->mSize;
            if (modulnamelen > sizeof (acv.a_modul_name))
            {
                modulnamelen = sizeof (acv.a_modul_name);
            }
            SAPDB_MemCopyNoCheck (&acv.a_modul_name[0], pModulename->mName, modulnamelen);
        }
		ParseId = *(pExecuteHandle->GetMasterParseID());
        SQLMAN_SHARED_SQL_TRACE ("Plan " << ToStr(pExecuteHandle->GetPlanObjectPtr()) << " valid, master parseid = " << ParseId << " handle " << ToStr(pExecuteHandle));
		SQLMAN_SHARED_SQL_TRACE ("a_returncode = " << acv.a_returncode);
	}
    else
    {
        SQLMAN_SHARED_SQL_TRACE ("no execute handle found");
    }
    return pExecuteHandle;
}

//-------------------------------------------------------------------------

tgg00_VoidPtr a101_GetPlanElement (
    tgg00_VoidPtr                        pExecuteHandle,
    tgg00_SysInfoKey      VAR_VALUE_REF  key)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_GetPlanElement", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );

    SQLMAN_UNPROTECT_ALLOCATOR (0);
    void* p = CONST_CAST(void*, REINTERPRET_CAST (SharedSQL_ExecuteHandle*, pExecuteHandle)->GetPlan(&key));
	if (p)
	{
	    return REINTERPRET_CAST(unsigned char*, p) + Catalog_SessionCache::ObjectHeader;
	}
	else
	{
        //SQLMAN_SHARED_SQL_TRACE ("Plan element not found");
		return NULL;
	}
}

//-------------------------------------------------------------------------

void* a101_GetPrepareHandle (tak_all_command_glob  VAR_VALUE_REF  acv,
							   pasbool               VAR_VALUE_REF  isPrepared,
							   tak_parsid            VAR_VALUE_REF  masterParsedId,
                               tgg00_VoidPtr         VAR_VALUE_REF  pSegment,
                               integer               VAR_VALUE_REF  segmentLength)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_GetPrepareHandle", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();

    SharedSQL_SQLContext  SQLContext(   0,						// TODO : SharedRepository
		                                acv.a_curr_user_id,     // User
                                        acv.a_curr_schema_id,   // CurrSchema
                                        acv.a_curr_dbprocedure, // embedding dbprocedure
                                        acv.a_sqlmode,          // SQLMode
                                        acv.a_iso_level,        // IsolatioLevel
                                        acv.a_dt_format,        // DataTimeFormat
                                        acv.a_nls_params.date_format,
                                        acv.a_nls_params.df_length,
                                        acv.a_cmd_packet_header.sp1h_mess_code, // CodeType
                                        (acv.a_cmd_segment_header.sp1c_mass_cmd() ? true : false),  // MassCommand
                                        (acv.a_variable_input ? true : false),                      // VariableInput
										(acv.a_info_output ? true : false)							// WithInfo
                                    );

	pSegment      = NULL;
	segmentLength = 0;

    void* Description = 0;
    SAPDB_Int4 DescriptionSize = 0;
    if (acv.a_appl_param_part != NULL)
    {
        Description     = &acv.a_appl_param_part->sp1p_buf()[0];
        DescriptionSize = acv.a_appl_param_part->sp1p_buf_len();
    }
    SharedSQL_SQLStmt     Statement( &acv.a_cmd_part->sp1p_buf()[0], acv.a_cmd_part->sp1p_buf_len(), Description, DescriptionSize );

    SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());
    SAPDB_Bool Internal = acv.a_in_ddl_trigger;
    SAPDB_Bool ParseAgain = acv.a_parsing_again;
    SharedSQL_PrepareHandle* pHandle = sharedSQLCache.NewPrepareHandle(context.GetAllocator(), SQLContext, Statement, Internal, ParseAgain, context.GetErrorList());
    if (pHandle)
    {
        SharedSQL_CommandStatus Status;
        isPrepared = (false == pHandle->MustPrepare(Status));
        // if isPrepared is TRUE the Status is Prepared
        // if isPrepared is FALSE the Status may be New, Invalid (due to DDL) or Dropped (due to out of memory)
        // In case of Dropped it may be possible to reparse without sendig -8/9 to the application
        SQLMAN_SHARED_SQL_TRACE ("Status " << ((Invalid == Status) ? "invalid" :
            (Dropped == Status) ? "dropped" : (New == Status) ? "new" : (Preparing == Status) ? "preparing" : "prepared"));

        SQLMAN_SHARED_SQL_TRACE ("GetPrepareHandle, handle " << ToStr(pHandle));
        if (Status == Invalid || Status == Dropped)
        {
            // DDT: if Status == Dropped the command max be reparsed without notifying the client !!!
			const bool c_isRollback = true;
			a10_cache_delete (acv, !c_isRollback );
            acv.a_parsing_again = (Status == Dropped);
        }
        else
        {
            if (isPrepared)
            {
                masterParsedId = *REINTERPRET_CAST(const tak_parsid*, pHandle->GetMasterParseID());
                SQLMAN_SHARED_SQL_TRACE ("  plan " << ToStr(pHandle->GetPlanObjectPtr()) << " already prepared, master parseid = " << *pHandle->GetMasterParseID());
				Catalog_Object::Catalog_Key key;
				const Catalog_Object* pObject = REINTERPRET_CAST(const Catalog_Object*, pHandle->GetPlan(&key));
				if (pObject)
				{
					pSegment = CONST_CAST(void*, pObject->GetInfo(segmentLength));
				}
            }
        }
    }
    return pHandle;
}

/*----------------------------------------------------------------*/

void a101_GetStatement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_VoidPtr                        pExecuteHandle,
    tsp00_MoveObjPtr      VAR_VALUE_REF  pStatement,
    integer               VAR_VALUE_REF  statementLength,
    tsp00_MoveObjPtr      VAR_VALUE_REF  pParamDescription,
    integer               VAR_VALUE_REF  paramDescLength,
    tsp00_SqlMode         VAR_VALUE_REF  sqlmode,
    pasbool               VAR_VALUE_REF  prepareFlag,
    pasbool               VAR_VALUE_REF  massCmdFlag)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_GetStatement", SQLMan_Trace, 1);
    const SharedSQL_ExecuteHandle* pHandle = REINTERPRET_CAST (const SharedSQL_ExecuteHandle*, pExecuteHandle);
    const SharedSQL_SQLContext* pSharedSQLContext;
    const SharedSQL_SQLStmt statement = pHandle->GetSQLStmt(pSharedSQLContext);
    pStatement        = REINTERPRET_CAST(tsp00_MoveObjPtr, statement.mStmt);
    statementLength   = statement.mStmtSize;
    pParamDescription = REINTERPRET_CAST(tsp00_MoveObjPtr, statement.mDescription);
    paramDescLength   = statement.mDescriptionSize;
    sqlmode.becomes(pSharedSQLContext->SQLMode);
    SAPDB_Bool minus9;
    pHandle->IsPlanValid(minus9);
    prepareFlag = minus9;
    massCmdFlag = pSharedSQLContext->MassCmd;
}

/*----------------------------------------------------------------*/

void a101_IncrementSyntaxTree (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_IncrementSyntaxTree", SQLMan_Trace, 1);
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    context.IncrementSyntaxTree();
}

/*----------------------------------------------------------------*/

void a101_InvalidateListAppend (
                                tak_all_command_glob  VAR_VALUE_REF  acv,
                                tgg00_Surrogate       VAR_ARRAY_REF  SchemaId,
                                tsp00_KnlIdentifier   VAR_ARRAY_REF  Tablename)
{
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    // disable shared sql for the current session. Otherwise plans could be produced for objects that
    // become rolled back at transaction end
    context.a_createSharedSQL = false;
    if (0 == context.a_invalidateList)
    {
        // info does not exist, create now
        context.a_invalidateList = new (context.GetAllocator()) SQLMan_SharedSQLInvalidateInfo (context.GetAllocator(), /*NotifyApplication=*/true);
        if (0 == context.a_invalidateList)
        {
            context.SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_SQLMAN_NO_MEMORY, Msg_OptArg("MEMREQUEST", sizeof(SQLMan_SharedSQLInvalidateInfo))));
            return;
        }
    }
    SQLMan_SharedSQLInvalidateInfo* pInvalidateInfo = reinterpret_cast<SQLMan_SharedSQLInvalidateInfo*>(context.a_invalidateList);
    pInvalidateInfo->Store (SchemaId, Tablename, true);
}

/*----------------------------------------------------------------*/

void a101PacketError (
    tsp00_C50             VAR_ARRAY_REF  fmt,
    tsp00_Int4                           val1,
    tsp00_Int4                           val2,
    tsp00_Int4                           val3)
{
    int len = sizeof(fmt);
    while (' ' == fmt[len-1]) --len;
    char msg[51];
    SAPDB_MemCopyNoCheck (&msg[0], &fmt[0], len);
    msg[len] = 0;
    char opMsg[100];
    sp77sprintf (opMsg, sizeof(opMsg), msg, val1, val2, val3);
    {
        Kernel_OpError (csp3_structure_check, csp3_n_packet) << opMsg;
    }
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    pContext->ThrowError (e_invalid);
}

/*----------------------------------------------------------------*/

pasbool a101_PutParseId (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_VoidPtr                        pPrepareHandle,
    tak_parsid            VAR_VALUE_REF  parseId)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_PutParseId", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );

    SharedSQL_PrepareHandle* cmdHandle  = REINTERPRET_CAST(SharedSQL_PrepareHandle*, pPrepareHandle);
	SQLMan_Context&           context    = SQLMan_Context::AcvToContext(acv);
	SharedSQL_SQLCache&      sharedSQLCache = context.GetSharedSQL_SQLCache();

    SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());

    SharedSQL_ParseID& ParseId = *REINTERPRET_CAST(SharedSQL_ParseID*, &parseId);
    SharedSQL_ModuleName Module;
    if (acv.a_modul_name != a01_il_b_identifier)
    {
        Module.mName = acv.a_modul_name;
        Module.mSize = sizeof (acv.a_modul_name);
    }
    SQLMAN_SHARED_SQL_TRACE ("PutParseId " << ParseId);
    return sharedSQLCache.PutParseID(cmdHandle, ParseId, Module, (0 != acv.a_initial_segment_header.sp1c_prepare()), context.GetErrorList());
}

/*----------------------------------------------------------------*/

void a101_GetModuleName (
    tak_all_command_glob    VAR_VALUE_REF acv,
    tak_parsid              VAR_VALUE_REF parseId,
    tgg00_VoidPtr           VAR_VALUE_REF NamePtr,
    tsp00_Int4              VAR_VALUE_REF Length)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_GetModuleName", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );

    SQLMan_Context& Context = SQLMan_Context::AcvToContext(acv);
    SharedSQL_SQLCache& SharedSQLCache = Context.GetSharedSQL_SQLCache();
    SharedSQL_ParseID& ParseId = *REINTERPRET_CAST(SharedSQL_ParseID*, &parseId);

    NamePtr = NULL;
    Length  = 0;
    const SharedSQL_ModuleName* Module = SharedSQLCache.GetModuleName( ParseId );
    if ( Module )
    {
        NamePtr = Module->mName;
        Length  = Module->mSize;
    }
}

/*----------------------------------------------------------------*/

pasbool a101_SetExecuteStatisticData (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_parsid            VAR_VALUE_REF  parseId,
    tgg00_Surrogate       VAR_VALUE_REF  CmdID,
    tsp00_Int4                           ExecuteTimeSec,
    tsp00_Int4                           ExecuteTimeMicroSec,
    tsp00_Int4                           ReadRowCount,
    tsp00_Int4                           QualifiedRowCount,
    tsp00_Int4                           VirtualReadCount,
    tsp00_Int4                           PhysicalReadCount,
    tsp00_Int4                           FetchRowCount,
    tsp00_Int4                           SuspendCount,
    tsp00_Int4                           WaitCount)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_SetExecuteStatisticData", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );

    SQLMan_Context& Context = SQLMan_Context::AcvToContext(acv);
    SharedSQL_SQLCache& SharedSQLCache = Context.GetSharedSQL_SQLCache();
    SharedSQL_ParseID& ParseId = *REINTERPRET_CAST(SharedSQL_ParseID*, &parseId);

    return SharedSQLCache.SetExecuteStatisticData(
            ParseId,
            (SharedSQL_CommandID&) CmdID, // output parameter
            ExecuteTimeSec, ExecuteTimeMicroSec,
            ReadRowCount, QualifiedRowCount,
            VirtualReadCount, PhysicalReadCount,
            FetchRowCount,
            SuspendCount, WaitCount);
}

/*----------------------------------------------------------------*/

void a101_ReleaseExecuteHandle (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_VoidPtr         VAR_VALUE_REF  planHandle)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_ReleaseExecuteHandle", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );

    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();

    SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());

    SharedSQL_ExecuteHandle* pExecuteHandle = REINTERPRET_CAST(SharedSQL_ExecuteHandle*, planHandle);

    if ((csp_old_fileversion == acv.a_returncode) || ( csp_use_new_pars_info == acv.a_returncode))
    {
        // plan no longer executable
        SharedSQL_ParseID ParseId = *(pExecuteHandle->GetMasterParseID());
		SQLMAN_SHARED_SQL_TRACE ("a_returncode = " << acv.a_returncode);
        SQLMAN_SHARED_SQL_TRACE ("SetPlanInvalid MasterParseID = " << ParseId);
        pExecuteHandle->SetPlanInvalid();
    }
    SQLMAN_SHARED_SQL_TRACE ("ReleaseExecuteHandle " << ToStr(pExecuteHandle));

    pExecuteHandle->ReleaseHandle();
    sharedSQLCache.DestroyExecuteHandle(pExecuteHandle);
    planHandle = NULL;
}

/*----------------------------------------------------------------*/

tsp00_Int4 a101_SetMaxParallelServers (
    tsp00_Int4  max)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_SetMaxParallelServers", SQLMan_Trace, 1);
    if (max > IOMan_IDataInfo::GetInstance().ConfiguredDataVolumes() - 1)
    {
        return IOMan_IDataInfo::GetInstance().ConfiguredDataVolumes() - 1;
    }
    else
    {
        return max > 0 ? max : 0;
    }
}

/*----------------------------------------------------------------*/

pasbool a101_StoreTableInfo (tak_all_command_glob  VAR_VALUE_REF  acv,
                     tgg00_Surrogate        VAR_VALUE_REF   SchemaID,
                     tsp00_KnlIdentifier    VAR_VALUE_REF   TableName,
					 pasbool                                WithSpecifiedSchema,
                     tgg00_VoidPtr                          pHandle)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_StoreTableInfo", SQLMan_Trace, 1);
    
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();

    if (context.IsOk())
    {
        SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());

        SharedSQL_Table SSQLTable(SchemaID, TableName, WithSpecifiedSchema);
        SharedSQL_PrepareHandle* pCmdHandle = REINTERPRET_CAST(SharedSQL_PrepareHandle*, pHandle);
        return pCmdHandle->PutTable(SSQLTable, context.GetErrorList());
    }
    return false;

}

/*----------------------------------------------------------------*/

pasbool a101_StorePlan (tak_all_command_glob  VAR_VALUE_REF  acv,
                     tak_parsid            VAR_VALUE_REF  parseId,
                     pasbool                              storePlan,
                     tgg00_VoidPtr                        pHandle)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_StorePlan", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );

    SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    SharedSQL_PrepareHandle* pCmdHandle = REINTERPRET_CAST(SharedSQL_PrepareHandle*, pHandle);

    bool ok = context.IsOk();
    if (ok && storePlan)
    {
        SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();
		SharedSQL_ParseID& ParseId = *REINTERPRET_CAST(SharedSQL_ParseID*, &parseId);

		SQLMAN_SHARED_SQL_TRACE ("StorePlan ParseID = " << ParseId);
        ok = context.GetSessionCatalogCache().StorePlan(sharedSQLCache, pCmdHandle,
            *REINTERPRET_CAST(SQLMan_ParseId*, &parseId), (0 != acv.a_initial_segment_header.sp1c_prepare()));
    }
    if (!ok || !storePlan)
    {
        if (!ok)
        {
            Msg_List msg(Msg_List::Error, SDBMSG_SQLMAN_STORE_PLAN_FAILED);
            context.GetErrorList().AppendNewMessage(msg);
        }
        SQLMAN_SHARED_SQL_TRACE ("ErrorWhilePreparing");
        pCmdHandle->ErrorWhilePreparing();
    }
    return ok;
}

/*----------------------------------------------------------------*/

#if defined (KERNEL80)
externPascal void a101_StoreTable (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_baserecord        VAR_VALUE_REF  BaseRecord)
{
   REINTERPRET_CAST(Catalog_TableObject*, &BaseRecord)->outcopy(SQLMan_Context::AcvToContext(acv));
}
#endif

/*----------------------------------------------------------------*/


void a101_ReleasePrepareHandle (tak_all_command_glob  VAR_VALUE_REF  acv,
                     tgg00_VoidPtr VAR_VALUE_REF          pHandle,
                     tsp00_Int4                           PrepareTimeSecs,
                     tsp00_Int4                           PrepareTimeMicroSecs)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_ReleasePrepareHandle", SQLMan_Trace, 1);
    //SAPDBERR_ASSERT_STATE( a01SharedSqlEnabled );

    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    SharedSQL_PrepareHandle* pCmdHandle = REINTERPRET_CAST(SharedSQL_PrepareHandle*, pHandle);

    SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();
    SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());

    SQLMAN_SHARED_SQL_TRACE ("ReleasePrepareHandle " << ToStr(pCmdHandle));
    if (pCmdHandle)
    {
        if (!context.IsOk())
        {
            SQLMAN_SHARED_SQL_TRACE ("ErrorWhilePreparing");
            pCmdHandle->ErrorWhilePreparing();
        }
        pCmdHandle->ReleaseHandle(PrepareTimeSecs, PrepareTimeMicroSecs, context.QueryRewriteDone());
        sharedSQLCache.DestroyPrepareHandle(pCmdHandle);
    }
    pHandle = NULL;
}

/*----------------------------------------------------------------*/

tgg00_VoidPtr a101_RTEMemAllocate (tsp00_Int4 Size)
{
    return RTEMem_Allocator::Instance().Allocate(Size);
}

/*----------------------------------------------------------------*/

void a101_RTEMemDeallocate (tgg00_VoidPtr VAR_VALUE_REF  p)
{
    RTEMem_Allocator::Instance().Deallocate(p);
    p = 0;
}

/*----------------------------------------------------------------*/

void a101_CleanUpAll (tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_CleanUpAll", SQLMan_Trace, 1);

    SharedSQL_SQLCache& sharedSQLCache = SQLMan_Context::AcvToContext(acv).GetSharedSQL_SQLCache();
    SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());
    sharedSQLCache.CleanUpAll();
}

/*----------------------------------------------------------------*/

void a101_InvalidateAll (tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_InvalidateAll", SQLMan_Trace, 1);

    if ( a101_SharedSQL_GetStatusParameter() )
    {
        SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
        SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();
        SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());
        sharedSQLCache.Invalidate(true);
    }
}

void a101_SilentReparseAll (tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_SilentReparseAll", SQLMan_Trace, 1);
    if ( a101_SharedSQL_GetStatusParameter() )
    {
        SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
        SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();
        SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());
        sharedSQLCache.Invalidate(false);
    }
}

//-------------------------------------------------------------------------

void a101_InvalidateSelective (tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_InvalidateSelective", SQLMan_Trace, 1);
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    SQLMan_SharedSQLInvalidateInfo* pInfo = reinterpret_cast<SQLMan_SharedSQLInvalidateInfo*>(context.a_invalidateList);
    if ( pInfo )
    {
        if ( a101_SharedSQL_GetStatusParameter() && context.IsOk() )
        {
            SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();
            SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());
            pInfo->Invalidate(context, sharedSQLCache);
        }
        destroy (pInfo, context.GetAllocator());
        context.a_invalidateList = 0;
    }
}

//-------------------------------------------------------------------------

void a101_SilentReparseSelective (tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_SilentReparseSelective", SQLMan_Trace, 1);

    if ( a101_SharedSQL_GetStatusParameter() )
    {
        SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
        SharedSQL_SQLCache& sharedSQLCache = context.GetSharedSQL_SQLCache();
        SQLMAN_UNPROTECT_ALLOCATOR (sharedSQLCache.GetCommandCacheBaseRawAllocator());

        // put the tables involved with this command.
//        Container_Vector<SharedSQL_Table> Tables(allocator);    // allocator = sharedSQLCache !
        // ...
        // DDT: fill the container here
        // ...
        sharedSQLCache.Invalidate(context, /*&Tables*/ 0, false);

		// OR
		// SharedSQL_Schema Schema;
		//sharedSQLCache.SilentReparse(Schema);
    }
}

/*----------------------------------------------------------------*/

tsp00_Int4 a101_RegisterCurrentStatement (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    return SQLMan_Context::AcvToContext(acv).RegisterCurrentStatement();
}

/*----------------------------------------------------------------*/

void a101_UnregisterStatement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int4                           statementId)
{
    SQLMan_Context::AcvToContext(acv).UnregisterStatement(statementId);
}

/*----------------------------------------------------------------*/

tsp00_Int2 a071_return_code (
    tgg00_BasisError         b_err,
    tsp00_SqlMode_Param      sqlmode)
{
    return (SQLManErr_Interface::GetInstance().GetReturnCode(b_err, sqlmode));
}

/*----------------------------------------------------------------*/

void a071_getmessagetext (
    tsp00_Int2                           msgno,
    char*                                msgbuf,
    tsp00_Int2            VAR_VALUE_REF  msglen,
    pasbool               VAR_VALUE_REF  found)
{
    const char *msg = SQLManErr_Interface::GetInstance().GetErrorText(msgno);
    if (NULL == msg)
    {
        found = false;
        msglen = 0;
    }
    else
    {
        found = true;
        msglen = strlen (msg);
        SAPDB_MemCopyNoCheck (msgbuf, msg, msglen);
    }
}

/*----------------------------------------------------------------*/

void a071_nextmessagetext (
    tsp00_Int2            VAR_VALUE_REF  msgno,
    char*                                msgbuf,
    tsp00_Int2            VAR_VALUE_REF  msglen,
    pasbool               VAR_VALUE_REF  found)
{
    const char *msg = SQLManErr_Interface::GetInstance().GetNextText(&msgno);
    if (NULL == msg)
    {
        found = false;
        msglen = 0;
        return;
    }
    else
    {
        found = true;
        msglen = strlen (msg);
        SAPDB_MemCopyNoCheck (msgbuf, msg, msglen);
        return;
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_CreateJoinHashAllocatorSingleton()
{
	SAPDBTRACE_ROUTINE_DEBUG (
        "a101_CreateJoinHashAllocatorSingleton", SQLMan_Trace, 1);

    Join_HashAccessAllocator::Instance();
}

/*----------------------------------------------------------------*/

externPascal SAPDB_Int4 a101_GetMaxJoinHashTableSize()
{
    return Join_HashAccessAllocator::Instance().GetMaxHashTableSize();
}

/*----------------------------------------------------------------*/
externPascal void a101_IncrementProcLevel (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SQLMan_Context::GetContext()->GetTempFileCollection()
        .IncrementProcLevel();
}

/*----------------------------------------------------------------*/

externPascal void a101_DecrementProcLevel (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SQLMan_Context::GetContext()->GetTempFileCollection()
        .DecrementProcLevel();
}

/*----------------------------------------------------------------*/

externPascal tsp00_Int a101_GetProcLevel (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    return SQLMan_Context::GetContext()->GetTempFileCollection()
        .GetProcLevel();
}

/*----------------------------------------------------------------*/

externPascal pasbool a101_SetProcLevel (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const integer                        newProcLevel)
{
    return SQLMan_Context::GetContext()->GetTempFileCollection()
        .SetProcLevel(newProcLevel) == true;
}

/*----------------------------------------------------------------*/

externPascal void a101_CreateGroupedTempFile(
    tgg00_TransContext VAR_VALUE_REF     trans,
    tgg00_FileId       VAR_VALUE_REF     tempFileId,
    tgg00_TfnTemp_Param                  tempFileType)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_CreateGroupedTempFile", SQLMan_Trace, 1);

    trans.trError_gg00 = e_ok;

    SAPDBERR_ASSERT_STATE( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 );

    tgg00_Surrogate logicalFileName;

    if ( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 ) {
        SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );
        // extract part which contains grouping information from filename
        tempFileId.fileTfnTemp_gg00().becomes( tempFileType );
        SAPDB_MemCopyNoCheck( &logicalFileName,
                              &tempFileId.fileTfnTemp_gg00(),
                              sizeof(logicalFileName) );
    } else {
        SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 0 ] == 0x80 ); // FIXME
        SAPDBERR_ASSERT_STATE(
            bd998GetExtendedTempFileType( trans, tempFileId )
            ==
            tempFileType );
        logicalFileName = tempFileId.fileTabId_gg00();
    }

    Msg_List message;

    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    fileCollection.CreateFileFromFileName(
        trans,
        logicalFileName,
        tempFileId,
        message ); // FIXME: there should be some error checking

    if ( trans.trError_gg00 != e_ok ) {
        return;
    }

    // make sure that everything behind fileTabId_gg00 is zero
    memset(
        reinterpret_cast<char*>(&tempFileId.fileTabId_gg00())
        + sizeof(tgg00_Surrogate),
        0,
        sizeof(tgg00_FileId)
        - FILE_ID_HEAD_MXGG00
        - sizeof( tgg00_Tfn )  // fileTfn
        - sizeof( tsp00_C1 )   // fileTfnNo
        - sizeof(tgg00_Surrogate) // fileTabId
        );
    // store file type in last 4 bytes:
    //tempFileId.fileTempCnt_gg00() = tempFileType;
    tempFileId.fileName_gg00()[ 15 ] = tempFileType;
}

/*----------------------------------------------------------------*/

externPascal void a101_ResetGroupedTempFile(
    tgg00_TransContext VAR_VALUE_REF     trans,
    tgg00_FileId       VAR_VALUE_REF     tempFileId,
    tgg00_TfnTemp_Param                  tempFileType)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_ResetGroupedTempFile", SQLMan_Trace, 1);

    trans.trError_gg00 = e_ok;

    if ( tempFileId.fileRootCheck_gg00() != ROOT_CHECK_GG00 ) {
        SAPDBERR_ASSERT_STATE(
            tempFileId.fileTabId_gg00()[ 0 ] == 0x80 ); // FIXME
        // file already exists, just reset it
        b01treset_file( trans, tempFileId );
        return;
    }

    // file is new
    tgg00_Surrogate logicalFileName;

    // extract part which contains grouping information from filename
    tempFileId.fileTfnTemp_gg00().becomes( tempFileType );
    SAPDB_MemCopyNoCheck( &logicalFileName,
                          &tempFileId.fileTfnTemp_gg00(),
                          sizeof(logicalFileName) );

    Msg_List message;
    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    SAPDB_Int  level;
    SAPDB_Int  subLevel;
    SAPDB_Int4 index;
    Data_TempFileTypes::FileType fileType;

    if ( SQLMan_LogicalFileNameGenerator::ParseFileName(
             logicalFileName,
             fileType,
             level,
             subLevel,
             index )
        ) {
        SAPDBERR_ASSERT_STATE( tempFileType == fileType );

        FileDir_FileNo fileNo;
        if ( SQLMan_TempFileOkay != fileCollection.GetFile(
                 fileType, level, subLevel, index,
                 fileNo, message ) ) {
            // file is new
            fileCollection.CreateFileFromFileName(
                trans,
                logicalFileName,
                tempFileId,
                message ); // FIXME: there should be some error checking
        } else {
            // file is already registered, get fileNo and reset it
            SAPDB_MemCopyNoCheck(
                tempFileId.fileTabId_gg00(),
                &fileNo,
                sizeof( tgg00_Surrogate ) );
            b01treset_file( trans, tempFileId );
        }
    } else {
        // this should not happen
        trans.trError_gg00 = e_invalid_tablename;
    }

    if ( trans.trError_gg00 != e_ok ) {
        return;
    }

    // make sure that everything behind fileTabId_gg00 is zero
    memset(
        reinterpret_cast<char*>(&tempFileId.fileTabId_gg00())
        + sizeof(tgg00_Surrogate),
        0,
        sizeof(tgg00_FileId)
        - FILE_ID_HEAD_MXGG00
        - sizeof( tgg00_Tfn )  // fileTfn
        - sizeof( tsp00_C1 )   // fileTfnNo
        - sizeof(tgg00_Surrogate) // fileTabId
        );
    // store file type in last 4 bytes:
    // tempFileId.fileTempCnt_gg00() = tempFileType;
    tempFileId.fileName_gg00()[ 15 ] = tempFileType;
}

/*----------------------------------------------------------------*/

externPascal void a101_DestroyGroupedTempFile(
    tgg00_TransContext VAR_VALUE_REF trans,
    tgg00_FileId       VAR_VALUE_REF tempFileId)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_DestroyGroupedTempFile", SQLMan_Trace, 1);

    trans.trError_gg00 = e_ok;

    Data_TempFileTypes::FileType fileType;
    SAPDB_Int            level;
    SAPDB_Int            subLevel;
    SAPDB_Int4           index;
    Msg_List             message;
    FileDir_FileNo       fileNo;
    SQLMan_TempFileReturnCode rc = SQLMan_TempFileOkay;

    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    if ( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 ) {
        SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

        tgg00_Surrogate logicalFileName;
        // extract part which contains grouping information from filename
        SAPDB_MemCopyNoCheck( &logicalFileName,
                          &tempFileId.fileTfnTemp_gg00(),
                          sizeof(logicalFileName) );

        if ( SQLMan_LogicalFileNameGenerator::ParseFileName(
                 logicalFileName,
                 fileType,
                 level,
                 subLevel,
                 index )
            ) {
            if ( SQLMan_TempFileOkay != fileCollection.GetFile(
                     fileType, level, subLevel, index,
                     fileNo, message ) ) {
                trans.trError_gg00 = e_file_not_found;
                return;
            } else {
                // file is already registered, get fileNo and reset it
                SAPDB_MemCopyNoCheck(
                    tempFileId.fileTabId_gg00(),
                    &fileNo,
                    sizeof( tgg00_Surrogate ) );
            }
        } else {
            trans.trError_gg00 = e_file_not_found;
            return;
        }
    } else {
        SAPDB_MemCopyNoCheck(
            &fileNo, &tempFileId.fileTabId_gg00(), sizeof(fileNo) );

        SQLMan_TempFileReturnCode rc = fileCollection.GetFileInfo(
            fileNo,
            fileType,
            level, subLevel, index,
            message);
    }

    if ( SQLMan_TempFileOkay == rc ) {
        fileCollection.DestroyFileByFileNo(
            trans,
            fileNo,
            message ); // FIXME: there should be some error checking
    }

    if ( (SQLMan_TempFileOkay != rc) || (trans.trError_gg00 != e_ok) ) {
        return;
    }

    // reset fileId to logical state
    SAPDB_MemCopyNoCheck( &tempFileId.fileTfnTemp_gg00(),
                          &fileNo,
                          sizeof(fileNo) );
    tempFileId.fileRootCheck_gg00() = ROOT_CHECK_GG00;
    tempFileId.fileZeroSite_gg00()  = 0;
    tempFileId.fileSession_gg00().gg90SetNil();
    tempFileId.fileTfnTemp_gg00().becomes(
        Data_TempFileTypes::MapFileTypeToTfnTemp( fileType ) );
}

/*----------------------------------------------------------------*/

externPascal void a101_DestroyGroupedTempFiles(
    tgg00_TransContext VAR_VALUE_REF trans,
    const tgg00_TfnTemp_Param              fileType,
    const SAPDB_Int4                       level,
    const SAPDB_Int2                       subLevel,
    tgg00_Filename VAR_ARRAY_REF           fileName)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DestroyGroupedTempFiles", SQLMan_Trace, 1);

    trans.trError_gg00 = e_ok;

    Msg_List message;
    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    fileCollection.DestroyAtLevel(
        trans,
        Data_TempFileTypes::MapTfnTempToFileType(fileType),
        level,
        subLevel,
        message ); // FIXME: there should be some error checking
}

/*----------------------------------------------------------------*/

externPascal void a101_DestroyAllGroupedTempFiles(
    tgg00_TransContext VAR_VALUE_REF trans,
    tgg00_Filename VAR_ARRAY_REF           fileName)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DestroyAllGroupedTempFiles", SQLMan_Trace, 1);

    // FIXME: fileName argument should be removed after switch to new fdir
    trans.trError_gg00 = e_ok;

    Msg_List message;
    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    fileCollection.DestroyAll(
        trans,
        message ); // FIXME: there should be some error checking
}

/*----------------------------------------------------------------*/

externPascal pasbool a101_IsTempFile (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_FileId    VAR_VALUE_REF  fileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_IsTempFile", SQLMan_Trace, 1);

    const tgg00_BasisError auxErr = acv.a_transinf.tri_trans.trError_gg00;

    acv.a_transinf.tri_trans.trError_gg00 = e_ok;
    const pasbool isTempFile =
        bd998IsTempFile( acv.a_transinf.tri_trans, fileId );

    if ( (e_ok != acv.a_transinf.tri_trans.trError_gg00)
         && (e_ok != auxErr) ) {
        a07_b_put_error( acv, acv.a_transinf.tri_trans.trError_gg00, 1);
        return false;
    }

    acv.a_transinf.tri_trans.trError_gg00 = auxErr;

    return isTempFile;
}

/*----------------------------------------------------------------*/

externPascal pasbool a101_IsExtendedTempFile (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_FileId    VAR_VALUE_REF  fileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_IsExtendedTempFile", SQLMan_Trace, 1);

        return
            a101_IsTempFile( acv, fileId )
            && ( tfnTemp_egg00 == fileId.fileTfn_gg00() );
}

/*----------------------------------------------------------------*/

externPascal tgg00_TfnTemp_Param a101_GetExtendedTempFileType (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_FileId    VAR_VALUE_REF  tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_GetExtendedTempFileType", SQLMan_Trace, 1);

    const tgg00_BasisError auxErr = acv.a_transinf.tri_trans.trError_gg00;

    acv.a_transinf.tri_trans.trError_gg00 = e_ok;
    const tgg00_TfnTemp_Param tempFileType =
        bd998GetExtendedTempFileType( acv.a_transinf.tri_trans, tempFileId );

    if ( (e_ok != acv.a_transinf.tri_trans.trError_gg00)
         && (e_ok != auxErr) ) {
        a07_b_put_error( acv, acv.a_transinf.tri_trans.trError_gg00, 1);
        return ttfnNone_egg00;
    }

    acv.a_transinf.tri_trans.trError_gg00 = auxErr;

    return tempFileType;
}

/*----------------------------------------------------------------*/

externPascal void a101_SetTempFileLevel (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_FileId          VAR_VALUE_REF  tempFileId,
    const tsp00_Int2                     level)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_SetTempFileLevel", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_STATE( a101_IsExtendedTempFile( acv, tempFileId ) );

    SAPDBERR_ASSERT_STATE( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 );
    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

    tsp00_IntMapC2 i2c;
    switch ( a101_GetExtendedTempFileType (acv, tempFileId) )
    {
    case ttfnFromSelect_egg00:
    case ttfnComplexSelect_egg00:
    case ttfnComplexView_egg00:
    case ttfnSubquery_egg00:
    case ttfnUnion_egg00:
        tempFileId.fileLevel_gg00()[ 0 ] = static_cast<char>(level);
        break;
    default:
        i2c.mapInt_sp00() = level;
        tempFileId.fileLevel_gg00() = i2c.mapC2_sp00();
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_SetTempFileSublevel (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_FileId          VAR_VALUE_REF  tempFileId,
    const tsp00_Int2                     sublevel)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_SetTempFileSublevel", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_STATE( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 );
    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

    SAPDBERR_ASSERT_STATE( a101_IsExtendedTempFile( acv, tempFileId ) );
    if ( ! a101_IsExtendedTempFile( acv, tempFileId ) ) {
        a07_b_put_error( acv, e_invalid_filetype, 1 );
        return;
    }

    switch ( a101_GetExtendedTempFileType (acv, tempFileId) )
    {
    case ttfnFromSelect_egg00:
    case ttfnComplexSelect_egg00:
    case ttfnComplexView_egg00:
    case ttfnSubquery_egg00:
    case ttfnUnion_egg00:
        break;
    default:
        SAPDBERR_ASSERT_STATE( false );
        a07_b_put_error( acv, e_invalid_filetype, 1 );
        return;
    }

    tempFileId.fileLevel_gg00()[ 1 ] = static_cast<char>(sublevel);
}

/*----------------------------------------------------------------*/

externPascal void a101_SetTempFileIndex (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_FileId          VAR_VALUE_REF  tempFileId,
    const tsp00_Int4                     index)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_SetTempFileIndex", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_STATE( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 );
    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

    SAPDBERR_ASSERT_STATE( a101_IsExtendedTempFile( acv, tempFileId ) );
    if ( ! a101_IsExtendedTempFile( acv, tempFileId ) ) {
        a07_b_put_error( acv, e_invalid_filetype, 1 );
        return;
    }

    tempFileId.fileTempCnt_gg00() = index;
}

/*----------------------------------------------------------------*/

externPascal tsp00_Int2 a101_GetTempFileLevel (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_FileId    VAR_VALUE_REF  tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_GetTempFileLevel", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_STATE( a101_IsExtendedTempFile( acv, tempFileId ) );
    if ( ! a101_IsExtendedTempFile( acv, tempFileId ) ) {
        a07_b_put_error( acv, e_invalid_filetype, 1 );
        return 0;
    }

    tgg00_BasisError auxError = acv.a_transinf.tri_trans.trError_gg00;
    acv.a_transinf.tri_trans.trError_gg00 = e_ok;

    tsp00_Int2 level =
        bd998GetTempFileLevel( acv.a_transinf.tri_trans, tempFileId );

    if ( e_ok != acv.a_transinf.tri_trans.trError_gg00 ) {
        a07_b_put_error( acv, acv.a_transinf.tri_trans.trError_gg00, 1);
        return 0;
    }

    acv.a_transinf.tri_trans.trError_gg00 = auxError;
    return level;
}

/*----------------------------------------------------------------*/

externPascal tsp00_Int2 a101_GetTempFileSublevel (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_FileId    VAR_VALUE_REF  tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_GetTempFileSublevel", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_STATE( a101_IsExtendedTempFile( acv, tempFileId ) );
    if ( ! a101_IsExtendedTempFile( acv, tempFileId ) ) {
        a07_b_put_error( acv, e_invalid_filetype, 1 );
        return 0;
    }

    tgg00_BasisError auxError = acv.a_transinf.tri_trans.trError_gg00;
    acv.a_transinf.tri_trans.trError_gg00 = e_ok;

    tsp00_Int2 subLevel =
        bd998GetTempFileSublevel( acv.a_transinf.tri_trans, tempFileId );

    if ( e_ok != acv.a_transinf.tri_trans.trError_gg00 ) {
        a07_b_put_error( acv, acv.a_transinf.tri_trans.trError_gg00, 1);
        return 0;
    }

    acv.a_transinf.tri_trans.trError_gg00 = auxError;
    return subLevel;
}

/*----------------------------------------------------------------*/

externPascal tsp00_Int4 a101_GetTempFileIndex (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_FileId    VAR_VALUE_REF  tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_GetTempFileIndex", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_STATE( a101_IsExtendedTempFile( acv, tempFileId ) );
    if ( ! a101_IsExtendedTempFile( acv, tempFileId ) ) {
        a07_b_put_error( acv, e_invalid_filetype, 1 );
        return 0;
    }

    tgg00_BasisError auxError = acv.a_transinf.tri_trans.trError_gg00;
    acv.a_transinf.tri_trans.trError_gg00 = e_ok;

    tsp00_Int4 index =
        bd998GetTempFileIndex( acv.a_transinf.tri_trans, tempFileId );

    if ( e_ok != acv.a_transinf.tri_trans.trError_gg00 ) {
        a07_b_put_error( acv, acv.a_transinf.tri_trans.trError_gg00, 1);
        return 0;
    }

    acv.a_transinf.tri_trans.trError_gg00 = auxError;
    return index;
}

/*----------------------------------------------------------------*/

externPascal void a101_GetTempFileInstance(
    tgg00_TransContext VAR_VALUE_REF trans,
    tgg00_FileId    VAR_VALUE_REF    tempFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_GetTempFileInstance", SQLMan_Trace, 1);
    SAPDBERR_ASSERT_STATE( bd998IsExtendedTempFile( trans, tempFileId ) );

    trans.trError_gg00 = e_ok;

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1,
        "checking filename: "
        << SAPDBTrace_Hex(
            &tempFileId.fileName_gg00(),
            sizeof(tgg00_Filename)));

    if ( tempFileId.fileRootCheck_gg00() != ROOT_CHECK_GG00 ) {
        // file id is already that of a temp file instance - nothing to do
        SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] != 0xFF );
        return;
    }

    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

    // file is new
    tgg00_Surrogate logicalFileName;

    // extract part which contains grouping information from filename
    SAPDB_MemCopyNoCheck( &logicalFileName,
                          &tempFileId.fileTfnTemp_gg00(),
                          sizeof(logicalFileName) );

    Msg_List message;
    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    SAPDB_Int  level;
    SAPDB_Int  subLevel;
    SAPDB_Int4 index;
    Data_TempFileTypes::FileType fileType;

    if ( ! SQLMan_LogicalFileNameGenerator::ParseFileName(
             logicalFileName,
             fileType,
             level,
             subLevel,
             index )
        ) {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "invalid logical file id : "
            << SAPDBTrace_Hex(&logicalFileName, sizeof(tgg00_Surrogate))
            << NewLine
            << "filename                : "
            << SAPDBTrace_Hex(&tempFileId.fileName_gg00(),
                              sizeof(tgg00_Filename))
            );
        // this should not happen
        SAPDBERR_ASSERT_STATE( false );
        trans.trError_gg00 = e_invalid_tablename;
    }

    FileDir_FileNo fileNo;
    if ( SQLMan_TempFileOkay == fileCollection.GetFile(
             fileType, level, subLevel, index,
             fileNo, message ) ) {

        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "replaced logical file no : "
            << SAPDBTrace_Hex(&logicalFileName,
                              sizeof(tgg00_Surrogate))
            << NewLine
            << "by instance file no      : " << fileNo );

        // file is already registered, get fileNo
        SAPDB_MemCopyNoCheck(
            tempFileId.fileTabId_gg00(),
            &fileNo,
            sizeof( tgg00_Surrogate ) );

        // we need to get rid of root check, otherwise we won't
        // remember we already have the instance file id
        b01filestate( trans, tempFileId );
//         SAPDBERR_ASSERT_STATE( tempFileId.fileRootCheck_gg00() != ROOT_CHECK_GG00 );
//         trans.trError_gg00 = e_ok; // ignore errors

        // make sure that everything behind fileTabId_gg00 is zero
        memset(
            reinterpret_cast<char*>(&tempFileId.fileTabId_gg00())
            + sizeof(tgg00_Surrogate),
            0,
            sizeof(tgg00_FileId)
            - FILE_ID_HEAD_MXGG00
            - sizeof( tgg00_Tfn )  // fileTfn
            - sizeof( tsp00_C1 )   // fileTfnNo
            - sizeof(tgg00_Surrogate) // fileTabId
            );
        // store file type in last 4 bytes:
        // tempFileId.fileTempCnt_gg00() = fileType;
        tempFileId.fileName_gg00()[ 15 ] = fileType;
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_ExistsGroupedTempFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  tempFileId)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_ExistsGroupedTempFile", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_STATE( bd998IsExtendedTempFile( trans, tempFileId ) );

    trans.trError_gg00 = e_ok;

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 1,
        "checking filename: "
        << SAPDBTrace_Hex(
            &tempFileId.fileName_gg00(),
            sizeof(tgg00_Filename)));

    if ( tempFileId.fileRootCheck_gg00() != ROOT_CHECK_GG00 ) {
        // file id is already that of a temp file instance - check if
        // file exists
        SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] != 0xFF );

        b01filestate( trans, tempFileId );

        return;
    }

    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

    // file is new
    tgg00_Surrogate logicalFileName;

    // extract part which contains grouping information from filename
    SAPDB_MemCopyNoCheck( &logicalFileName,
                          &tempFileId.fileTfnTemp_gg00(),
                          sizeof(logicalFileName) );

    Msg_List message;
    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    SAPDB_Int  level;
    SAPDB_Int  subLevel;
    SAPDB_Int4 index;
    Data_TempFileTypes::FileType fileType;

    if ( ! SQLMan_LogicalFileNameGenerator::ParseFileName(
             logicalFileName,
             fileType,
             level,
             subLevel,
             index )
        ) {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "invalid logical file id : "
            << SAPDBTrace_Hex(&logicalFileName, sizeof(tgg00_Surrogate))
            << NewLine
            << "filename                : "
            << SAPDBTrace_Hex(&tempFileId.fileName_gg00(),
                              sizeof(tgg00_Filename))
            );
        // this should not happen
        SAPDBERR_ASSERT_STATE( false );
        trans.trError_gg00 = e_invalid_tablename;
    }

    FileDir_FileNo fileNo;
    if ( SQLMan_TempFileOkay == fileCollection.GetFile(
             fileType, level, subLevel, index,
             fileNo, message ) ) {

        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "replaced logical file no : "
            << SAPDBTrace_Hex(&logicalFileName,
                              sizeof(tgg00_Surrogate))
            << NewLine
            << "by instance file no      : " << fileNo );

        // file is already registered, get fileNo
        SAPDB_MemCopyNoCheck(
            tempFileId.fileTabId_gg00(),
            &fileNo,
            sizeof( tgg00_Surrogate ) );

        // we need to get rid of root check, otherwise we won't
        // remember we already have the instance file id
        b01filestate( trans, tempFileId );

        // make sure that everything behind fileTabId_gg00 is zero
        memset(
            reinterpret_cast<char*>(&tempFileId.fileTabId_gg00())
            + sizeof(tgg00_Surrogate),
            0,
            sizeof(tgg00_FileId)
            - FILE_ID_HEAD_MXGG00
            - sizeof( tgg00_Tfn )  // fileTfn
            - sizeof( tsp00_C1 )   // fileTfnNo
            - sizeof(tgg00_Surrogate) // fileTabId
            );
        // store file type in last byte:
        tempFileId.fileName_gg00()[ 15 ] = fileType;
    } else {
        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "logical file no not registered: "
            << SAPDBTrace_Hex(&logicalFileName,
                              sizeof(tgg00_Surrogate)) );
        trans.trError_gg00 = e_file_not_found;
        return;
    }
}

/*----------------------------------------------------------------*/

/// adds an entry for logical file name of tempFileId to temp file
/// collection; tempFileId itself is not changed!
externPascal void a101_AddGroupedTempFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  tempFileId,
    tgg00_Surrogate       VAR_ARRAY_REF  instanceId)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_AddGroupedTempFile", SQLMan_Trace, 1);

    trans.trError_gg00 = e_ok;

    SAPDBERR_ASSERT_STATE( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 );
    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );

    tgg00_TfnTemp tempFileType = tempFileId.fileTfnTemp_gg00();

    tgg00_Surrogate logicalFileName;

    // extract part which contains grouping information from filename
    SAPDB_MemCopyNoCheck( &logicalFileName,
                          &tempFileId.fileTfnTemp_gg00(),
                          sizeof(logicalFileName) );

    Msg_List message;
    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    fileCollection.AddFileEntry(
        trans,
        instanceId,
        logicalFileName,
        message ); // FIXME: there should be some error checking

    if ( trans.trError_gg00 != e_ok ) {
        return;
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_GetLogicalFilename (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  tempFileId)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_GetLogicalFilename", SQLMan_Trace, 1);

    trans.trError_gg00 = e_ok;

    if ( tempFileId.fileRootCheck_gg00() == ROOT_CHECK_GG00 ) {
        // file id is logical file name
        SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] == 0xFF );
        return;
    }

    SAPDB_Int  level;
    SAPDB_Int  subLevel;
    SAPDB_Int4 index;
    Data_TempFileTypes::FileType fileType;

    SAPDBTRACE_WRITELN(
        SQLMan_Trace, 5,
        "file no : "
        << SAPDBTrace_Hex(&tempFileId.fileTabId_gg00(), sizeof(tgg00_Surrogate))
        );

    SAPDBERR_ASSERT_STATE( tempFileId.fileTabId_gg00()[ 2 ] != 0xFF );

    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();


    FileDir_FileNo fileNo;
    SAPDB_MemCopyNoCheck(
        &fileNo,
        &tempFileId.fileTabId_gg00(),
        sizeof( fileNo )
        );

    Msg_List message;
    if ( SQLMan_TempFileOkay == fileCollection.GetFileInfo(
             fileNo, fileType, level, subLevel, index, message ) ) {

        // reset everything starting from fileTabId_gg00
        SAPDB_MemFill(
            &tempFileId.fileTabId_gg00(),
            0,
            sizeof(tgg00_FileId)
            - FILE_ID_HEAD_MXGG00
            - sizeof( tgg00_Tfn )  // fileTfn
            - sizeof( tsp00_C1 ),   // fileTfnNo
            "SQLMananger", __FILE__, __LINE__, message
            );

        tempFileId.fileRootCheck_gg00() = ROOT_CHECK_GG00;
        tempFileId.fileRoot_gg00()      = NIL_PAGE_NO_GG00;

        // add logical file name information
        tempFileId.fileTfnTemp_gg00().becomes(
            Data_TempFileTypes::MapFileTypeToTfnTemp( fileType ) );
        SAPDB_MemCopyNoCheck(
            &tempFileId.fileZeroSite_gg00(),
            cgg_zero_c2,
            sizeof( tgg00_ServerdbNo )
            );
        tempFileId.fileSession_gg00().gg90SetNil();

        bd998SetTempFileLevel( trans, tempFileId, level );
        switch ( fileType )
        {
        case Data_TempFileTypes::FromSelect:
        case Data_TempFileTypes::ComplexSelect:
        case Data_TempFileTypes::ComplexView:
        case Data_TempFileTypes::Subquery:
        case Data_TempFileTypes::Union:
            bd998SetTempFileSublevel( trans, tempFileId, subLevel );
        }

        bd998SetTempFileIndex( trans, tempFileId, index );

        SAPDBTRACE_WRITELN(
            SQLMan_Trace, 1,
            "replaced instance file no : " << fileNo << NewLine
            << "by logical file id        : "
            << SAPDBTrace_Hex(&tempFileId.fileTabId_gg00(),
                              sizeof(tgg00_Surrogate))
            );
    } else {
        // this should not happen...
        trans.trError_gg00 = e_system_error;
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_RemoveGroupedTempFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  tempFileId)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_RemoveGroupedTempFile", SQLMan_Trace, 1);

    a101_GetLogicalFilename (trans, tempFileId);

    if ( e_ok != trans.trError_gg00 ) {
        return;
    }

    SAPDB_Int  level = bd998GetTempFileLevel( trans, tempFileId );
    SAPDB_Int  subLevel = bd998GetTempFileSublevel( trans, tempFileId );
    SAPDB_Int4 index = bd998GetTempFileIndex( trans, tempFileId );
    Data_TempFileTypes::FileType fileType =
        Data_TempFileTypes::MapTfnTempToFileType(
            bd998GetExtendedTempFileType( trans, tempFileId ) );

    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    Msg_List message;
    SQLMan_TempFileReturnCode rc =
        fileCollection.RemoveFileEntry(
            fileType, level, subLevel, index, message );

    if ( rc != SQLMan_TempFileOkay ) {
        trans.trError_gg00 = e_file_not_found;
    }
}

/*----------------------------------------------------------------*/

externPascal pasbool a101_CheckRequestPacket (
    tsp1_packet_ptr   packet,
    tsp00_Int4        packetSize)
{
    char errbuf [132];
    return PkgCheck_CheckPacket (packet, packetSize, errbuf);
}

/*----------------------------------------------------------------*/

externPascal pasbool a101_BasicAuthenticationAllowed (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    return SQLMan_Connect::BasicAuthenticationAllowed ();
}

/*----------------------------------------------------------------*/

static bool
convertBufToClientEncoding (
    tak_all_command_glob  & acv,
    const char            * source,
    char                  * target,
    int                     size)
{
    tsp00_CodeType serverCode = g01unicode ? csp_unicode : csp_ascii;
    tsp00_CodeType clientCode = acv.a_in_packet->sp1_header.sp1h_mess_code;
    if (clientCode == csp_unicode_native) {
        if (acv.a_in_packet->sp1_header.sp1h_mess_swap == sw_full_swapped) {
            clientCode = csp_unicode_swap;
        }
        else {
            clientCode = csp_unicode;
        }
    }
    if (serverCode == clientCode) {
        memcpy (target, source, size);
        return true;
    }
    switch (clientCode) {
        case csp_unicode_swap:
            // swap
            {
                for (int i = 0; i < size; i += 2) {
                    target [i] = source [i + 1];
                    target [i + 1] = source [i];
                }
            }
            return true;
            break;
        case csp_unicode:
            // should never happen, handled by == clause,
            // because kernel is always big endian
            return false;
            break;
        case csp_ascii:
            {
                // unicode to ascii
                unsigned int destOut;
                int converted = sp81UCS2toASCII((tsp00_Byte *) target, size / 2, & destOut,
                    (const tsp81_UCS2Char *)  source, size, false);
                if (converted / 2 < size) {
                    memset (target + converted, ' ', size - converted);
                }
                return converted = size / 2;
            }
            break;
        default:
            // should never happen
            return false;
            break;
    }
}

/*----------------------------------------------------------------*/

static bool getUsernameParameter (
    tsp00_KnlIdentifier & userName_init,
    tsp00_MoveObjPtr      dataBuf,
    tsp00_Int4            dataLength,
    int                   inputEncoding,
    int                   fieldIndex)
{
    SAPDBFields_VarData::Reader   reader (dataBuf);
    const void * data;
    int          datalen;
    bool result = false;

    reader.skip (fieldIndex);
    if (reader.next (data, datalen)) {
        if (g01unicode) {
            tsp00_Int4 outlen = sizeof (tsp00_KnlIdentifier);
            tsp8_uni_error transRC;
            tsp00_Int4 errpos;
            tsp8_uni_opt_set emptyset;
            emptyset.clear ();

            s80uni_trans (data, datalen, inputEncoding,
                &userName_init, outlen, csp_unicode,
                emptyset, transRC, errpos);
            result = (transRC == uni_ok) || (transRC == uni_dest_too_short);
            if (result && (outlen < sizeof (tsp00_KnlIdentifier))) {
                void * target = userName_init + outlen;
                unsigned int targetlen = sizeof (tsp00_KnlIdentifier) - outlen;
                sp77encodingUCS2->fillString (&target, &targetlen, targetlen, ' ');
            }
        }
        else {
            if (datalen > sizeof (tsp00_KnlIdentifier)) {
                datalen = sizeof (tsp00_KnlIdentifier);
            }
            SAPDB_memcpy (userName_init, data, datalen);
            int fillsize = sizeof (tsp00_KnlIdentifier) - datalen;
            if (fillsize > 0) {
                SAPDB_memset (userName_init + datalen, ' ', fillsize);
            }
            result = true;
        }
    }
    else {
        result = false;
    }
    return result;
}


/*----------------------------------------------------------------*/

externPascal pasbool a101_InitAuthentication (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_VALUE_REF  userName_init,
    pasbool                              isParameter,
    tsp00_MoveObjPtr                     dataBuf,
    tsp00_Int4                           dataLength)
{
    SQLMan_Context & context = SQLMan_Context::AcvToContext(acv);
    SQLMan_Connect * connect;
    int              fieldsToSkip = 0;

    /*
     * get username as parameter value
     */
    if (isParameter) {
        int inputEncoding = acv.a_cmd_packet_header.sp1h_mess_code;
        if (!getUsernameParameter (userName_init, dataBuf, dataLength,
            inputEncoding, 0))
        {
            return false;
        }
        fieldsToSkip = 1;
    }
    /*
     * convert username to client encoding
     */
    tsp00_KnlIdentifier userName;
    if (!convertBufToClientEncoding (acv, userName_init, userName, sizeof (tsp00_KnlIdentifier))) {
        return false;
    }
    /*
     * create challenge
     */
    SAPDBMem_IRawAllocator & allocator = context.GetAllocator ();
    connect = new (allocator) SQLMan_Connect (context, userName);
    if (connect == NULL) {
        return false;
    }
    const char    * algorithm;
    const void    * challenge;
    int             challengeLen;
    if (!connect->CreateChallenge (dataBuf, dataLength, fieldsToSkip,
            algorithm, challenge, challengeLen))
    {
        destroy (connect, allocator);
        a07_b_put_error(acv,  e_no_matching_authentication, 1);
        return false;
    }
    /*
     * write challenge to return segment
     */
    a06init_curr_retpart (acv);
    tsp1_part * part = acv.a_curr_retpart;
    SAPDBFields_VarData::Writer writer (&part->sp1p_buf (), part->sp1p_buf_size ());
    writer.addField (algorithm, strlen (algorithm));
    writer.addField (challenge, challengeLen);
    /*
     * add properties
     */
    SAPDBFields_VarData::Writer subwriter;
    writer.openSubfields (subwriter);
    SAPDBFields_PropertyList::Writer properties (&subwriter);
    properties.addEntry (csp1_maxpasswordlen_tag, g01unicode ? "9" : "18");
    writer.closeSubfields (subwriter);
    /*
     * close part
     */
    int fieldCount;
    int buflen;
    writer.close (fieldCount, buflen);
    part->sp1p_buf_len () = buflen;
    a06finish_curr_retpart (acv, sp1pk_vardata, fieldCount);
    /*
     * store info in acv
     */
    acv.a_authentication = (tsp00_MoveObjPtr) connect;
    /*
     * return
     */
    return true;
}

/*----------------------------------------------------------------*/

externPascal void
a101_ClearAuthenticationInfo (
    tak_all_command_glob   VAR_VALUE_REF acv)
{
    SQLMan_Connect * connect = (SQLMan_Connect *) acv.a_authentication;
    if (connect != NULL) {
        SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
        SAPDBMem_IRawAllocator & allocator = context.GetAllocator ();
        destroy (connect, allocator);
        acv.a_authentication = NULL;
    }
}

/*----------------------------------------------------------------*/

externPascal pasbool a101_VerifyAuthentication (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  userName,
    tsp00_Name            VAR_ARRAY_REF  pwd_init,
    tsp00_MoveObjPtr                     dataBuf,
    tsp00_Int4                           dataLength)
{
    SQLMan_Connect * connect = (SQLMan_Connect *) acv.a_authentication;
    tsp00_Name               pwd;
    int                      fieldsToSkip = 0;

    /*
     * get username as parameter value
     */
    if (connect->wasUsernameParam ()) {
        int inputEncoding = acv.a_cmd_packet_header.sp1h_mess_code;
        if (!getUsernameParameter (userName, dataBuf, dataLength,
            inputEncoding, 0))
        {
            return false;
        }
        fieldsToSkip = 1;
    }
    /*
     * convert password to client encoding
     */
    if (!convertBufToClientEncoding (acv, pwd_init, pwd, sizeof (tsp00_Name))) {
        return false;
    }
    /*
     * verify
     */
    pasbool result = connect->VerifyResponse (pwd, dataBuf, dataLength, fieldsToSkip);
    return result;
}

/*----------------------------------------------------------------*/

externPascal void a101_GetUsernameHostvar (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  userName,
    integer                              parameterIndex)
{
    int inputEncoding = acv.a_cmd_packet_header.sp1h_mess_code;
    --parameterIndex; // pascal index starts at 1
    bool ok = getUsernameParameter (userName, acv.a_data_ptr,
            acv.a_data_length, inputEncoding, parameterIndex);
    if (!ok) {
        memset (userName, ' ', sizeof (tsp00_KnlIdentifier));
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_MarkUsernameAsParameter (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SQLMan_Connect * connect = (SQLMan_Connect *) acv.a_authentication;
    connect->markUsernameAsParam ();
}

/*----------------------------------------------------------------*/

externPascal void a101_DiagnoseDropFile (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  surrogate)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_DiagnoseDropFile", SQLMan_Trace, 1);

    FileDir_FileNo fileNo( surrogate );
    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);

    if ( ! sqlContext.IsOk() ) {
        return;
    }

    Msg_List errMsg;
    SQLMan_DiagnoseDropFile::ReturnCode retCode =
        SQLMan_DiagnoseDropFile::Execute( sqlContext, fileNo, errMsg );

    if ( retCode != SQLMan_DiagnoseDropFile::DropOk ) {
        switch ( retCode ) {
            case SQLMan_DiagnoseDropFile::FileNotFound:
                a07_b_put_error( acv, e_file_not_found, 1);
                break;
            case SQLMan_DiagnoseDropFile::CatalogObjectsExist:
                a07_b_put_error( acv, e_missing_privilege, 1);
                break;
            default:
                a07_b_put_error( acv, e_missing_privilege, 1);
        }
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_DumpSessionInfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tsp00_VFilename  VAR_ARRAY_REF dmpFileName,
    const tsp00_Int4                     dmpFileNo,
    tsp00_Int4            VAR_VALUE_REF  dmpPageNo,
    const tsp00_BufAddr                  dmpBufAddr,
    const tsp00_Int4                     dmpBufSize)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_DumpSessionInfo", SQLMan_Trace, 1);

    Data_PageFrame pageFrame( dmpBufAddr, dmpBufSize );
    Kernel_Dump dump( dmpFileName, pageFrame, dmpFileNo, dmpPageNo );

    if ( SQLMan_Context::GetContext() == 0 ) {
        return;
    }

    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();
    fileCollection.Dump( dump );

    FileDir_IDirectory::SessionInstance( acv.a_transinf.tri_trans )
        .Dump( acv.a_transinf.tri_trans.trTaskId_gg00, dump );

    // dmpPageNo has not been incremented during dump writing
    dmpPageNo = dump.GetPageNo();
}

/*----------------------------------------------------------------*/

externPascal void a101_GetGroupedFileForFileId (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  fileId,
    tgg00_TfnTemp_Param                  tempFileType,
    tgg00_FileId          VAR_VALUE_REF  logicalFileId)
{
	SAPDBTRACE_ROUTINE_DEBUG (
        "a101_GetLogicalSurrogateForFileId", SQLMan_Trace, 1);

    SAPDBERR_ASSERT_ARGUMENT( ! bd998IsTempFile( trans, fileId ) );

    SQLMan_TempFileCollection& fileCollection =
        SQLMan_Context::GetContext()->GetTempFileCollection();

    FileDir_FileNo fileNoToMap( fileId.fileTabId_gg00() );
    Msg_List       msgList;

    Data_TempFileTypes::FileType fileType =
        Data_TempFileTypes::MapTfnTempToFileType( tempFileType );

    SQLMan_TempFileReturnCode retCode =
        fileCollection.MapFile(
            fileType, fileNoToMap, logicalFileId, msgList );

    if ( retCode != SQLMan_TempFileOkay ) {
        switch ( retCode ) {
            case SQLMan_TempFileOutOfMemory:
                trans.trError_gg00 = e_no_more_memory;
                break;
            default:
                trans.trError_gg00 = e_system_error;
        }
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_CreateUpdStatObject(
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Addr            VAR_VALUE_REF  objAddr,
    tsp00_Int4                           sampleRows,
    tsp00_Int4                           samplePercentage)
{
	SAPDBTRACE_ROUTINE_DEBUG (
        "a101_CreateUpdStatObject", SQLMan_Trace, 1);

    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);

    SQLMan_UpdateStatistics* updStat =
        new (sqlContext.GetAllocator()) SQLMan_UpdateStatistics(
            sqlContext,
            acv.a_acc_termid,
            sampleRows,
            samplePercentage );

    Msg_List errMsg;
    if ( updStat && ( ! updStat->Initialize(errMsg) ) ) {
        sqlContext.GetErrorList() = errMsg;
        a07_b_put_error( acv, e_system_error, 1);
    }

    objAddr = reinterpret_cast<tsp00_Addr>( updStat );
}

/*----------------------------------------------------------------*/

externPascal void a101_DestroyUpdStatObject(
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Addr            VAR_VALUE_REF  updStatObjAddr)
{
	SAPDBTRACE_ROUTINE_DEBUG (
        "a101_DestroyUpdStatObject", SQLMan_Trace, 1);

    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);

    if ( updStatObjAddr != 0 ) {
        SQLMan_UpdateStatistics* updStat =
            reinterpret_cast<SQLMan_UpdateStatistics*>( updStatObjAddr );

        destroy ( updStat, sqlContext.GetAllocator() );
    }

    updStatObjAddr = 0;
}

/*----------------------------------------------------------------*/

externPascal void a101_UpdStatAddTableRequest(
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Addr                      updStatObjAddr,
    tgg00_Surrogate  VAR_ARRAY_REF  tableSurr,
    tsp00_Int4                      noOfPagesToSample)
{
	SAPDBTRACE_ROUTINE_DEBUG (
        "a101_UpdStatAddTableRequest", SQLMan_Trace, 1);

    if ( acv.a_returncode != 0 ) {
        return;
    }

    if ( updStatObjAddr == 0 ) {
        a07_b_put_error( acv, e_system_error, 1);
    }

    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);

    SQLMan_UpdateStatistics& updStat =
        *reinterpret_cast<SQLMan_UpdateStatistics*>( updStatObjAddr );

    Msg_List errMsg;
    SQLMan_UpdateStatistics::ReturnCode retCode;

    retCode = updStat.AddTableStatRequest( tableSurr, noOfPagesToSample, errMsg );

    if ( retCode != SQLMan_UpdateStatistics::Okay ) {
        a07_b_put_error( acv, e_system_error, 1);
    }

    sqlContext.GetErrorList() = errMsg;
}

/*----------------------------------------------------------------*/

externPascal void a101_UpdStatProcessRequests(
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Addr                      updStatObjAddr)
{
	SAPDBTRACE_ROUTINE_DEBUG (
        "a101_UpdStatProcessRequests", SQLMan_Trace, 1);

    if ( acv.a_returncode != 0 ) {
        return;
    }

    if ( updStatObjAddr == 0 ) {
        a07_b_put_error( acv, e_system_error, 1);
    }

    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);

    SQLMan_UpdateStatistics& updStat =
        *reinterpret_cast<SQLMan_UpdateStatistics*>( updStatObjAddr );

    Msg_List errMsg;
    SQLMan_UpdateStatistics::ReturnCode retCode;

    retCode = updStat.ProcessRequests( errMsg );

    if ( retCode != SQLMan_UpdateStatistics::Okay ) {
        switch ( retCode ) {
            case SQLMan_UpdateStatistics::Cancelled:
                a07_b_put_error( acv, e_cancelled, 1);
                break;
            default:
                a07_b_put_error( acv, e_system_error, 1);
        }
    }

    sqlContext.GetErrorList() = errMsg;
}

/*----------------------------------------------------------------*/

externPascal pasbool a101_ParallelUpdStatEnabled()
{
    return SQLMan_UpdateStatistics::IsParallel();
}

/*----------------------------------------------------------------*/

externPascal void a101_InsertIntoSysUpdStatLog (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schemaName,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  tableName,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  columnName,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  indexName,
    tgg00_Surrogate       VAR_ARRAY_REF  tableFileNo,
    tsp00_Timestamp       VAR_ARRAY_REF  executionTimeStamp,
    tsp00_Int4                           samplePercent,
    tsp00_Int4                           sampleRows,
    tsp00_Timestamp       VAR_ARRAY_REF  beginTimeStamp,
    tsp00_Timestamp       VAR_ARRAY_REF  endTimeStamp,
    tgg91_SessionNo       VAR_VALUE_REF  session,
    tsp00_TermId          VAR_ARRAY_REF  termid,
    tsp00_Int4                           seqno)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_InsertIntoSysUpdStatLog", SQLMan_Trace, 1);

    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
    SQLTab_SysUpdStatLog updateStatLog( sqlContext );

    SAPDB_Int4 sessionId = gg06SessionGetInt4(
        sqlContext.TransContext().trSessionId_gg00 );

    SQLTab_SysUpdStatLog::ReturnCode retCode = updateStatLog.InsertRow(
        SQLTab_SysUpdStatLog::UpdateDuplicates,
        SQLTab_SysUpdStatLog::WithCommit,
        schemaName,
        tableName,
        columnName,
        indexName,
        tableFileNo,
        *reinterpret_cast<RTESys_SQLTimestamp*>(&executionTimeStamp),
        samplePercent,
        sampleRows,
        *reinterpret_cast<RTESys_SQLTimestamp*>(&beginTimeStamp),
        *reinterpret_cast<RTESys_SQLTimestamp*>(&endTimeStamp),
        sessionId,
        termid,
        seqno,
        sqlContext.GetErrorList());

    if ( retCode != SQLTab_SysUpdStatLog::Okay ) {
        switch ( retCode ) {
            case SQLTab_SysUpdStatLog::LockRowFailed:
                a07_b_put_error( acv, e_request_timeout, 1);
                break;
            case SQLTab_SysUpdStatLog::Cancelled:
                a07_b_put_error( acv, e_cancelled, 1);
                break;
            case SQLTab_SysUpdStatLog::InsertFailed:
            case SQLTab_SysUpdStatLog::DuplicateKey:
            case SQLTab_SysUpdStatLog::SystemError:
            default:
                a07_b_put_error( acv, e_system_error, 1);
                break;
        }
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_InsertIntoSysUpdStatWanted (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_Surrogate VAR_ARRAY_REF   tableSurrogate)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_InsertIntoSysUpdStatWanted",
                              SQLMan_Trace, 1);

    a101_InsertColumnIntoSysUpdStatWanted(
        acv,
        tableSurrogate,
        SQLMan_Identifier() );
}

/*----------------------------------------------------------------*/

externPascal void a101_InsertColumnIntoSysUpdStatWanted (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_Surrogate VAR_ARRAY_REF   tableSurrogate,
    const tsp00_KnlIdentifier  VAR_ARRAY_REF  columnName)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_InsertColumnIntoSysUpdStatWanted",
                              SQLMan_Trace, 1);

    if ( ! FileDir_FileNo( tableSurrogate ).IsPersistentFileNo() ) {
        return;
    }

    Msg_List errMsg;
    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
    Catalog_ObjectId tableId( tableSurrogate );

    Catalog_TableDescriptor tableDesc( sqlContext, tableId );
    if ( ! tableDesc ) {
        return;
    }

    SQLTab_SysUpdStatWanted updStatWanted( sqlContext );
    const SQLTab_SysUpdStatWanted::InsertRc insertRc =
        updStatWanted.InsertRow(
            tableDesc,
            SQLMan_Identifier( columnName ),
            errMsg );
    if ( insertRc != SQLTab_SysUpdStatWanted::irOkay ) {
        switch ( insertRc ) {
            case SQLTab_SysUpdStatWanted::irFailed:
            case SQLTab_SysUpdStatWanted::irLockRowFailed:
            case SQLTab_SysUpdStatWanted::irNoStatisticsPossible:
                errMsg.ClearMessageList();
                break;
            case SQLTab_SysUpdStatWanted::irCancelled:
                sqlContext.ThrowError( e_cancelled );
                break;
            case SQLTab_SysUpdStatWanted::irSystemError:
            default:
                // write msg to knldiag, but ignore error
                RTE_Message( errMsg );
                errMsg.ClearMessageList();
        }
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_InsertIntoSysCheckTableLog (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schemaName,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  tableName,
    tsp00_Int4                           msgNo)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_InsertIntoSysCheckTableLog",
                              SQLMan_Trace, 1);

    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
    SQLTab_SysCheckTableLog checkTableLog( sqlContext );

    SQLTab_SysCheckTableLog::ReturnCode retCode = checkTableLog.InsertRow(
        schemaName,
        tableName,
        msgNo,
        sqlContext.GetErrorList());

    if ( retCode != SQLTab_SysUpdStatLog::Okay ) {
        switch ( retCode ) {
            case SQLTab_SysCheckTableLog::LockRowFailed:
                a07_b_put_error( acv, e_request_timeout, 1);
                break;
            case SQLTab_SysCheckTableLog::Cancelled:
                a07_b_put_error( acv, e_cancelled, 1);
                break;
            case SQLTab_SysCheckTableLog::InsertFailed:
            case SQLTab_SysCheckTableLog::DuplicateKey:
            case SQLTab_SysCheckTableLog::SystemError:
            default:
                a07_b_put_error( acv, e_system_error, 1);
                break;
        }
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_AddToSessionList (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_AddToSessionList",
                              SQLMan_Trace, 1);

    SQLMan_SessionList& sessionList = SQLMan_SessionList::Instance();
    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);

    SQLMan_SessionList::AddReturnCode rc = sessionList.Add(
        sqlContext, 
        sqlContext.GetErrorList());

    if ( rc != SQLMan_SessionList::arcOk ) {
        if ( rc == SQLMan_SessionList::arcOutOfMemory ) {
            a07_b_put_error (acv, e_no_more_memory, 1);
        }
    }            
}

/*----------------------------------------------------------------*/

externPascal void a101_RemoveFromSessionList (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_RemoveFromSessionList",
                              SQLMan_Trace, 1);

    SQLMan_SessionList& sessionList = SQLMan_SessionList::Instance();
    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);

    sessionList.Remove(
        sqlContext, 
        sqlContext.GetErrorList());
}

/*----------------------------------------------------------------*/

externPascal void a101_UpdateStatisticsColumn (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int4                           samplePercent,
    tsp00_Int4                           sampleRows)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_UpdateStatisticsColumn", SQLMan_Trace, 1);

    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
    SQLMan_UpdateStatistics updateStat(
            sqlContext,
            acv.a_acc_termid,
            sampleRows,
            samplePercent );

    if ( ! updateStat.Initialize( sqlContext.GetErrorList() ) ) {
        a07_b_put_error( acv, e_system_error, 1);
    }

    updateStat.UpdateStatisticsColumn( sqlContext.GetErrorList() );
}

/*----------------------------------------------------------------*/

externPascal void a101_UpdateCounter (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_UpdateCounter", SQLMan_Trace, 1);

    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);

    SQLMan_UpdateCounter updCounter( sqlContext );
    if ( updCounter.Initialize( sqlContext) ) {
        updCounter.Execute( sqlContext );
    }
}

/*----------------------------------------------------------------*/

externPascal void a101_DeleteFromSysUpdateCounterWanted (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_Surrogate  VAR_ARRAY_REF  surr)
{
	SAPDBTRACE_ROUTINE_DEBUG ("a101_DeleteFromSysUpdateCounterWanted",
                              SQLMan_Trace, 1);

    Msg_List errMsg;
    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
    SQLTab_SysUpdateCounterWanted updateCounterWanted( sqlContext );

    SQLTab_SysUpdateCounterWanted::DeleteRc retCode =
        updateCounterWanted.DeleteRow( surr, errMsg );


    if ( retCode != SQLTab_SysUpdateCounterWanted::delRcOk ) {
        switch ( retCode ) {
            case SQLTab_SysUpdateCounterWanted::delRcRowNotFound:
                break;
            case SQLTab_SysUpdateCounterWanted::delRcLockRequestTimeout:
                a07_b_put_error( acv, e_request_timeout, 1);
                break;
            case SQLTab_SysUpdateCounterWanted::delRcCancelled:
                a07_b_put_error( acv, e_cancelled, 1);
                break;
            case SQLTab_SysUpdateCounterWanted::delRcSystemError:
            default:
                a07_b_put_error( acv, e_system_error, 1);
                break;
        }
        sqlContext.GetErrorList() = errMsg;
    }
}

/*----------------------------------------------------------------*/

/* schema mapping */

inline static SQLMan_SchemaMap* ak101GetSchemaMap (tak_all_command_glob  VAR_VALUE_REF  acv)
{
    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
    return sqlContext.GetSchemaMap();
}

/*----------------------------------------------------------------*/

 pasbool a101_AnySchemaMapped (
    tak_all_command_glob  VAR_VALUE_REF  acv)
 {
     SAPDBTRACE_ROUTINE_DEBUG ("a101_AnySchemaMapped", SQLMan_Trace, 1);
     SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
     SQLMan_SchemaMap* pSchemaMap = sqlContext.GetSchemaMap();
     if (pSchemaMap)
     {
         return pSchemaMap->AnySchemaMapped ();
     }
     return false;
 }

 /*----------------------------------------------------------------*/

void a101_DestroySchemaMap (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
     SAPDBTRACE_ROUTINE_DEBUG ("a101_DestroySchemaMap", SQLMan_Trace, 1);
     SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
     sqlContext.DestroySchemaMap();
}

 /*----------------------------------------------------------------*/

void a101_EnableSchemaMap (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool                              doEnable)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_EnableSchemaMap", SQLMan_Trace, 1);
    if (SQLMan_SchemaMap* pSchemaMap = ak101GetSchemaMap(acv))
    {
        pSchemaMap->Enable (doEnable != 0);
    }
}

/*----------------------------------------------------------------*/

 void a101_MapSchemaName (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schemaName,
    integer                              pos)
 {
      SAPDBTRACE_ROUTINE_DEBUG ("a101_MapSchemaName", SQLMan_Trace, 1);
      SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
      SQLMan_SchemaMap* pSchemaMap = sqlContext.GetSchemaMap();
      if (pSchemaMap)
      {
          pSchemaMap->MapSchema (*reinterpret_cast<SQLMan_Identifier*>(&schemaName), pos);
      }
 }

/*----------------------------------------------------------------*/

void a101_ReadSchemaMap (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_C24             VAR_ARRAY_REF  guid)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_ReadSchemaMap", SQLMan_Trace, 1);
    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
    SQLMan_SchemaMap* pSchemaMap = sqlContext.GetSchemaMap();
    if (pSchemaMap)
    {
        pSchemaMap->ReadMap (guid);
    }
    else
    {
        sqlContext.SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_SQLMAN_NO_MEMORY));
    }
}

/*----------------------------------------------------------------*/

 void a101_RenameSchemaNames (
    tak_all_command_glob  VAR_VALUE_REF  acv)
 {
    SAPDBTRACE_ROUTINE_DEBUG ("a101_RenameSchemaNames", SQLMan_Trace, 1);
    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
    SQLMan_SchemaMap* pSchemaMap = sqlContext.GetSchemaMap();
    if (pSchemaMap)
    {
        pSchemaMap->Rename ();
    }
 }

 /*----------------------------------------------------------------*/

void a101_ResetMappingInfo (
    tak_all_command_glob  VAR_VALUE_REF  acv)
{
     SAPDBTRACE_ROUTINE_DEBUG ("a101_SetMappingOffset", SQLMan_Trace, 1);
     if (SQLMan_SchemaMap* pSchemaMap = ak101GetSchemaMap(acv))
     {
         pSchemaMap->Reset ();
     }
 }

/*----------------------------------------------------------------*/

 pasbool a101_SchemaMappingActive (
    tak_all_command_glob  VAR_VALUE_REF  acv)
 {
    SAPDBTRACE_ROUTINE_DEBUG ("a101_SchemaMappingActive", SQLMan_Trace, 1);
    SQLMan_Context& sqlContext = SQLMan_Context::AcvToContext(acv);
    SQLMan_SchemaMap* pSchemaMap = sqlContext.GetSchemaMap();
    if (pSchemaMap)
    {
        return pSchemaMap->SchemaMappingActive ();
    }
    return false;
 }

 /*----------------------------------------------------------------*/

 void a101_SetMappingOffset (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              offset)
 {
     SAPDBTRACE_ROUTINE_DEBUG ("a101_SetMappingOffset", SQLMan_Trace, 1);
     if (SQLMan_SchemaMap* pSchemaMap = ak101GetSchemaMap(acv))
     {
         pSchemaMap->SetOffset (offset);
     }
 }
