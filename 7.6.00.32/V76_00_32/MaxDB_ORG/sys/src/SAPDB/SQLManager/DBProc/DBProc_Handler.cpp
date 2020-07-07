/*!
  @file           DBProc_Handler.cpp
  @author         ThomasA
  @ingroup        DBProc
  @brief          methods for db-procedure and db-function execution

\if EMIT_LICENCE

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



\endif
*/

#include "SAPDB/SQLManager/SQLMan_Types.hpp"
#include "SAPDB/SQLManager/SQLMan_Context.hpp"
#include "SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SAPDB/SQLManager/SQLMan_MessBlock.hpp"
#include "SAPDB/SQLManager/SQLMan_Tracing.hpp"
#include "SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_MessBlockObject.hpp"
#include "SAPDB/SQLManager/DBProc/DBProc_DBProcDescriptor.hpp"
#include "SAPDB/SQLManager/DBProc/DBProc_Handler.hpp"
#include "SAPDB/SQLManager/DBProc/DBProc_Messages.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Object.hpp"
#include "SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp"

#include "SAPDB/DBProc/Proc_ServerSessionFactory.hpp"
#include "SAPDB/DBProc/Proc_IServerControl.hpp"
#include "SAPDB/DBProc/Proc_ISession.hpp"
#include "SAPDB/DBProc/Proc_SessionList.hpp"
#include "SAPDB/DBProc/Proc_FactoryDirectory.hpp"
#include "SAPDB/DBProc/Proc_ParameterCopy.hpp"
#include "SAPDB/DBProc/Proc_SQL.hpp"
#include "SAPDB/DBProc/Proc_IFactory.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_AutoPtr.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp"
#include "SAPDB/SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_DBProc.h"

#include "SAPDB/RunTime/ProcServer/ProcRTE_Runtime.hpp"

#include "hak07.h"
#include "hak071.h"
#include "hak06.h"
#include "hak10.h"
#include "hak260.h"
#include "hak262.h"
#include "ggg01.h"
#include "hgg01_3.h"
#include "hgg04.h"
#include "hkb71.h"
#include "hak663.h"
#include "hak651.h"
#include "hsp51.h"
#include "hsp77.h"
#include "hsp81.h"

#ifdef SAPDB_SLOW
#define TRACE_CALL_STACK(MSG,CONTEXT) traceCallStack(MSG,CONTEXT);
#else
#define TRACE_CALL_STACK(MSG,CONTEXT)
#endif

//----------------------------------------------------------------------------------

// checks if a given address lies in a buffer

inline static bool inBuffer (void* p, void* pBuffer, int bufferLen)
{
    return (REINTERPRET_CAST(char*, p) >= REINTERPRET_CAST(char*, pBuffer)) &&
           (REINTERPRET_CAST(char*, p) <  REINTERPRET_CAST(char*, pBuffer) + bufferLen);

}

//----------------------------------------------------------------------------------

struct SQLMan_ContextPushInterpreterGlobals
{
    SQLMan_Context&    m_context;
    tgg00_StackListPtr m_workStAddr;
    tsp00_Int4         m_maxStack;
    tsp00_MoveObjPtr   m_workBufAddr;
    tsp00_Int4         m_workBufSize;

    SQLMan_ContextPushInterpreterGlobals(SQLMan_Context& context, tgg00_SelectFieldsParam& interpreterContext, tgg00_SelectFieldsParam& newInterpreterContext)
        : m_context(context)
        , m_workStAddr(context.a_work_st_addr)
        , m_maxStack(context.a_work_st_max)
        , m_workBufAddr(context.a_work_buf_addr)
        , m_workBufSize(context.a_work_buf_size)
    {
        if (inBuffer(interpreterContext.sfp_work_st_addr(), m_workStAddr, m_maxStack * sizeof(tgg00_StackEntry)))
        {
            SAPDBTRACE_WRITELN(DBProc_Trace, 5, "old a_work_st_addr " << ToStr(context.a_work_st_addr));
            SAPDBTRACE_WRITELN(DBProc_Trace, 5, "old a_work_st_max  " << context.a_work_st_max);
            context.a_work_st_addr = newInterpreterContext.sfp_work_st_addr();
            context.a_work_st_max  = newInterpreterContext.sfp_work_st_max();
            SAPDBTRACE_WRITELN(DBProc_Trace, 5, "new a_work_st_addr " << ToStr(context.a_work_st_addr));
            SAPDBTRACE_WRITELN(DBProc_Trace, 5, "new a_work_st_max  " << context.a_work_st_max);
        }
        if (inBuffer(interpreterContext.sfp_workbuf_addr(), m_workBufAddr, m_workBufSize))
        {
            SAPDBTRACE_WRITELN(DBProc_Trace, 5, "old a_work_buf_addr " << ToStr(context.a_work_buf_addr));
            SAPDBTRACE_WRITELN(DBProc_Trace, 5, "old a_work_buf_size  " << context.a_work_buf_size);
            context.a_work_buf_addr = interpreterContext.sfp_workbuf_addr() + interpreterContext.sfp_workbuf_len();
            context.a_work_buf_size = interpreterContext.sfp_workbuf_size() - interpreterContext.sfp_workbuf_len();
            SAPDBTRACE_WRITELN(DBProc_Trace, 5, "new a_work_buf_addr " << ToStr(context.a_work_buf_addr));
            SAPDBTRACE_WRITELN(DBProc_Trace, 5, "new a_work_buf_size  " << context.a_work_buf_size);
        }
    }
    ~SQLMan_ContextPushInterpreterGlobals()
    {
        m_context.a_work_st_addr  = m_workStAddr;
        m_context.a_work_st_max   = m_maxStack;
        m_context.a_work_buf_addr = m_workBufAddr;
        m_context.a_work_buf_size = m_workBufSize;
    }
};

#ifdef SAPDB_SLOW

static void traceCallStack (const char* msg, tgg00_SelectFieldsParam& interpreterContext)
{
    SAPDBTRACE_WRITELN( DBProc_Trace, 1, msg);
    tgg00_StackEntry* pCurrSt = reinterpret_cast<tgg00_StackEntry*>(interpreterContext.sfp_work_st_addr());
    int ix = 0;
    do
    {
        ++ix;
        SQLMAN_TRACE_STACK_ENTRY (DBProc_Trace, 5, "call code", *pCurrSt, ix);
        ++pCurrSt;
    }
    while (pCurrSt <= interpreterContext.sfp_work_st_top());
    SQLMAN_TRACE_BUFFER (DBProc_Trace, 5, "work buffer", interpreterContext.sfp_workbuf_addr(), interpreterContext.sfp_workbuf_len());
}

#endif

// reserves the required space for the function paramters. Up to 1000 bytes are hold
// on the stack, if more is required the space is required from the heap
// all variable are initialize
class DBProc_VariablesSpace
{
public :
    DBProc_VariablesSpace(SQLMan_Context& context, tgg00_DataPart& data, tgg00_SelectFieldsParam& interpreterContext, tgg00_BasisError e)
        : m_pBuf(&m_buffer[0])
        , m_context(context)
    {
        e = e_ok;
        int requiredSize = data.mbp_rec().recLen_gg00();
        if (requiredSize > sizeof(m_buffer))
        {
            m_pBuf = context.GetAllocator().Allocate(requiredSize);
            if (0 == m_pBuf)
            {
                e = e_no_more_memory;
            }
        }
        SAPDB_MemCopyNoCheck (m_pBuf, &data, requiredSize);
        SQLMAN_TRACE_BUFFER (DBProc_Trace, 5, "variables", m_pBuf, requiredSize);
        interpreterContext.sfp_oldrec_addr() = reinterpret_cast<tsp00_MoveObjPtr>(m_pBuf);
        interpreterContext.sfp_oldrec_pos()  = 1;
        interpreterContext.sfp_oldrec_len()  = requiredSize;
        interpreterContext.sfp_oldkey_len()  = 0;
    }
    ~DBProc_VariablesSpace()
    {
        if (m_pBuf != &m_buffer[0])
        {
             m_context.GetAllocator().Deallocate(m_pBuf);
        }
    }
private :
    SAPDB_Byte      m_buffer[1000];
    void*           m_pBuf;
    SQLMan_Context& m_context;
};

//----------------------------------------------------------------------------------

inline static void callFunction (
            SQLMan_Context&          context,
            tgg00_SelectFieldsParam& interpreterContext,
            const tgg00_Surrogate&   functionId,
            int                      paramCount,
            tgg00_BasisError&        e)
{
    SAPDBTRACE_ROUTINE_DEBUG ("callFunction", DBProc_Trace, 1);
    tak_sysbufferaddress p;
    a262LoadCode (context, functionId, *reinterpret_cast<const tsp00_C2*> (cak_init_linkage), p);
    if (p)
    {
        a10Lock (context, p);
        SQLMan_MessBlock& mblock = *reinterpret_cast<SQLMan_MessBlock*> (&p->smessblock().mbr_mess_block);
        tsp00_MoveObjPtr pData = &mblock.mb_data()->mbp_buf();
        mblock.mb_trns()       = &context.TransContext();

        // create a new interpreter context
        tgg00_SelectFieldsParam newInterpreterContext = interpreterContext;
        newInterpreterContext.sfp_work_st_frame() = interpreterContext.sfp_work_st_top() - paramCount;
        if (newInterpreterContext.sfp_work_st_frame() < newInterpreterContext.sfp_work_st_bottom())
        {
            e =  e_stack_type_illegal;
            return;
        }
        int usedStack = 1 + (interpreterContext.sfp_work_st_top() - REINTERPRET_CAST(tgg00_StEntryAddr, interpreterContext.sfp_work_st_addr()));
        newInterpreterContext.sfp_work_st_addr()    = REINTERPRET_CAST(tgg00_StackListPtr, interpreterContext.sfp_work_st_top() + 1);
        newInterpreterContext.sfp_work_st_max()    -= usedStack;
        newInterpreterContext.sfp_work_st_size()    = newInterpreterContext.sfp_work_st_max() * sizeof(tgg00_StackEntry);
        SAPDBTRACE_WRITELN(DBProc_Trace, 5, "sfp_work_st_top " << ToStr(interpreterContext.sfp_work_st_top()));
        SAPDBTRACE_WRITELN(DBProc_Trace, 5, "sfp_work_st_top " << ToStr(interpreterContext.sfp_work_st_addr()));
        SAPDBTRACE_WRITELN(DBProc_Trace, 5, "usedStack " << usedStack);
        newInterpreterContext.sfp_workbuf_addr()   += interpreterContext.sfp_workbuf_len();
        newInterpreterContext.sfp_workbuf_size()   -= interpreterContext.sfp_workbuf_len();
        newInterpreterContext.sfp_pl_ptr()          = reinterpret_cast<tsp00_BufAddr>(p);
        newInterpreterContext.sfp_bd_mess_type()    = mblock.MessType();
        newInterpreterContext.sfp_bd_mess2_type()   = mblock.MessType2();
        newInterpreterContext.sfp_result_wanted()   = false;
        newInterpreterContext.sfp_m_result_addr()   = 0;
        newInterpreterContext.sfp_m_result_size()   = 0;
        newInterpreterContext.sfp_first_qual()      = true;
        newInterpreterContext.sfp_filler2()         = false;
        newInterpreterContext.sfp_sqlmode()         = mblock.SqlMode();
        newInterpreterContext.sfp_dateTimeFormat()  = mblock.DateTimeFormat();
        newInterpreterContext.sfp_result_length()   = 0;
        newInterpreterContext.sfp_bd_inv_only().becomes(primary_only);
        newInterpreterContext.sfp_rec_addr()        = reinterpret_cast<tsp00_BufAddr>(mblock.DataPtr());
        newInterpreterContext.sfp_rec_len()         = mblock.mb_data()->mbp_rec().len();
        newInterpreterContext.sfp_rec_key_len()     = mblock.mb_data()->mbp_rec().keylen();
        // create buffer for function variables
        DBProc_VariablesSpace variablesSpace(context, *mblock.mb_data(), newInterpreterContext, e);
        if (e_ok != e) return;
        newInterpreterContext.sfp_acv_addr()       = reinterpret_cast<tsp00_Addr>(&context); // still needed ??
        newInterpreterContext.sfp_param_buf_addr() = interpreterContext.sfp_workbuf_addr();
        SQLMan_ContextPushInterpreterGlobals pushedGlobalsScope(context, interpreterContext, newInterpreterContext);
        newInterpreterContext.sfp_data_addr() = mblock.DataPtr();
        newInterpreterContext.sfp_data_size() = mblock.DataSize();
        if (e_ok == e)
        {
            const bool c_with_view = true;
            const bool c_check_new_record = true;
            pasbool dummyBool = false;
            tgg00_StackEntry* errorStackEntry = 0;
            k71qual_handling (context.TransContext(), newInterpreterContext, !c_with_view, !c_check_new_record,
                mblock.mb_qual()->mstack_desc(), errorStackEntry, dummyBool);
            if (e_ok == mblock.mb_trns()->trError_gg00)
            {
                newInterpreterContext.sfp_work_st_top()->epos() += interpreterContext.sfp_workbuf_len();
                interpreterContext.sfp_workbuf_len() += newInterpreterContext.sfp_workbuf_len();
                interpreterContext.sfp_work_st_top()  = newInterpreterContext.sfp_work_st_frame() + 1;
                *interpreterContext.sfp_work_st_top() = *newInterpreterContext.sfp_work_st_top();

            }
            else
            {
                e = mblock.mb_trns()->trError_gg00;
            }
        }
        a10Unlock (context, p);
    }
    else
    {
        if (context.ReturnCode() == SQLManErr_Interface::GetInstance().GetReturnCode(e_sysinfo_not_found, context.GetSessionSqlMode()))
        {
            context.ResetReturnCode();
            e = e_old_fileversion;
        }
        else
        {
            e = SQLManErr_Interface::GetInstance().GetBaseError (context.ReturnCode());
        }
    }
}

//----------------------------------------------------------------------------------

void DBProc_Handler::CalculateFunctionResult (tgg00_TransContext                   &transContext,
                                              const tgg00_StackEntry               &functionId,
                                              const SAPDBFields_FieldMap           &inParameters,
                                              const ParameterDescriptors           &parameterDescriptors,
                                              const tgg00_Rec*                     &pResult,
                                              int                                  &resultLen)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DBProc_Handler::CalculateFunctionResult", DBProc_Trace, 1);
    SQLMan_Context* pContext = REINTERPRET_CAST(SQLMan_Context*, transContext.trAcvPtr_gg00);
    if (NULL == pContext)
    {
        pContext = new(*REINTERPRET_CAST(SAPDBMem_IRawAllocator*, transContext.trAllocator_gg00)) SQLMan_Context(transContext);
    }
    tak_sysbufferaddress p;
    a262LoadCode (*pContext, *reinterpret_cast<const SQLMan_Surrogate*>(&functionId),
        *reinterpret_cast<const tsp00_C2*> (cak_init_linkage), p);
    if (p)
    {
        SQLMan_MessBlock* pMblock = reinterpret_cast<SQLMan_MessBlock*> (&p->smessblock().mbr_mess_block);
        tgg00_SelectFieldsParam sel;
        g04init_select_fields (sel, &pMblock->mb_data()->mbp_buf(), pMblock->mb_data_size(),
            pContext->a_work_st_addr, pContext->a_work_st_max,
            pContext->a_work_buf_addr, pContext->a_work_buf_size,
            pContext->a_sqlmode, &pMblock->mb_fieldlists());
        sel.sfp_bd_mess_type()    = pMblock->MessType();
        sel.sfp_bd_mess2_type()   = pMblock->MessType2();
        sel.sfp_result_wanted()   = false;
        sel.sfp_m_result_addr()   = NULL;
        sel.sfp_m_result_size()   = 0;
        sel.sfp_first_qual()      = true;
        sel.sfp_filler2()         = false;
        sel.sfp_sqlmode()         = pMblock->SqlMode();
        sel.sfp_dateTimeFormat()  = pMblock->DateTimeFormat();
        // create buffer for dbproc variables
        DBProc_VariablesSpace variablesSpace(*pContext, *pMblock->mb_data(), sel, transContext.trError_gg00);
        if (e_ok != transContext.trError_gg00) return;
        sel.sfp_acv_addr()      = reinterpret_cast<tsp00_Addr>(pContext);
        sel.sfp_data_addr()     = &pMblock->mb_data()->mbp_buf();
        sel.sfp_data_size()     = pMblock->mb_data_size();
        sel.sfp_workbuf_len()   = 0;
        sel.sfp_workbuf_top()   = 0;
        sel.sfp_work_st_top()   = reinterpret_cast<tgg00_StackEntry*>(sel.sfp_work_st_addr());
        sel.sfp_work_st_frame() = sel.sfp_work_st_top();
        sel.sfp_param_buf_addr()= sel.sfp_workbuf_addr();
        // push all parameters
        SAPDBFields_FieldMap::IndexType fieldCnt = inParameters.GetFieldCount();
        for (SAPDBFields_FieldMap::IndexType fieldIdx = 0; fieldIdx < fieldCnt; ++fieldIdx)
        {
            SAPDBFields_AttributedField field;
            if(inParameters.GetField(fieldIdx, field, pContext->GetErrorList())!=SAPDBFields_IFieldSequence::fieldFound)
            {
                pMblock->mb_trns()->trError_gg00 = e_stack_overflow;
                return;
            }
            // push field to interpreter stack
            int paramLength = field.GetLength();
            if (sel.sfp_workbuf_len() + parameterDescriptors[fieldIdx].m_fixedLength > sel.sfp_workbuf_size())
            {
                pMblock->mb_trns()->trError_gg00 = e_stack_overflow;
                return;
            }
            SAPDB_Byte defByte;
            if (parameterDescriptors[fieldIdx].m_toUnicode) {
                unsigned int destOut;
                defByte = (unsigned char) csp_unicode_def_byte;
                (*sel.sfp_workbuf_addr())[sel.sfp_workbuf_top()] = defByte;
                sp81ASCIItoUCS2(REINTERPRET_CAST(tsp81_UCS2Char*, (sel.sfp_workbuf_addr() + sel.sfp_workbuf_top() + 1)),
                    sel.sfp_workbuf_size() - sel.sfp_workbuf_len(), false, &destOut,
                    field.GetPointer(1), field.GetLength() - 1);
                paramLength = 1 + (paramLength - 1) * 2;
            }
            else {
                defByte = *field.GetPointer(0);
                SAPDB_MemCopyNoCheck(sel.sfp_workbuf_addr()[sel.sfp_workbuf_top()], field.GetPointer(0), field.GetLength());
            }
            if (paramLength < parameterDescriptors[fieldIdx].m_fixedLength)
            {
                switch (defByte) {
                case csp_undef_byte :
                    break;
                case csp_unicode_def_byte :
                    SAPDB_RangeUnicodeFill (__FILE__, 1, parameterDescriptors[fieldIdx].m_fixedLength,
                        &sel.sfp_workbuf_addr()[sel.sfp_workbuf_top()], paramLength + 1,
                        parameterDescriptors[fieldIdx].m_fixedLength - paramLength, csp_unicode_blank,
                        pMblock->mb_trns()->trError_gg00);
                    break;
                default :
                    memset (&sel.sfp_workbuf_addr()[sel.sfp_workbuf_top() + paramLength],
                        defByte, parameterDescriptors[fieldIdx].m_fixedLength - paramLength);
                }
                paramLength = parameterDescriptors[fieldIdx].m_fixedLength;
            }
            ++sel.sfp_work_st_top();
            sel.sfp_work_st_top()->etype().becomes(st_result);
            sel.sfp_work_st_top()->epos()     = sel.sfp_workbuf_top() + 1;
            sel.sfp_work_st_top()->elen_var() = paramLength;
            sel.sfp_workbuf_top() += field.GetLength();
            sel.sfp_workbuf_len() += field.GetLength();
        }
        int usedStack = 1 + (sel.sfp_work_st_top() - REINTERPRET_CAST(tgg00_StEntryAddr, sel.sfp_work_st_addr()));
        sel.sfp_work_st_addr()    = REINTERPRET_CAST(tgg00_StackListPtr, sel.sfp_work_st_top() + 1);
        sel.sfp_work_st_max()    -= usedStack;
        sel.sfp_work_st_size()    = sel.sfp_work_st_max() * sizeof(tgg00_StackEntry);
        sel.sfp_workbuf_addr()   += sel.sfp_workbuf_len();
        sel.sfp_workbuf_size()   -= sel.sfp_workbuf_len();
        k71sel_qualification_test (*pMblock, sel, false, pMblock->mb_data()->mbp_rec());
        if (e_ok == pMblock->mb_trns()->trError_gg00)
        {
            tsp00_MoveObjPtr pOperandAddr;
            k71get_operand (sel, false, pOperandAddr, resultLen, pMblock->mb_trns()->trError_gg00);
            sel.sfp_workbuf_top() += resultLen;
            pResult = REINTERPRET_CAST(tgg00_Rec*, pOperandAddr);
        }
        else
        {
            SQLMan_Context* pContext = SQLMan_Context::GetContext();
            if (pContext)
            {
                int rc = a071_return_code (pMblock->mb_trns()->trError_gg00, pContext->a_sqlmode);
                pContext->GetErrorList().AppendNewMessage (
                    Msg_List(Msg_List::Error,SDBMSG_DBPROC_FUNCTION_CALL_FAILED,Msg_Arg("RETURNCODE",rc)
                ));
                    //SAPDBErr_MessageList (
                    //DBPROC_FUNCTION_CALL_FAILED, ToStr(rc)));
            }
        }
     }
     else
     {
         transContext.trError_gg00 = e_dbfunc_code_not_found;
     }
}

//----------------------------------------------------------------------------------

tgg00_BasisError DBProc_Handler::ExecuteSqlStatement (
                                     SQLMan_Context*          pContext,
                                     tgg00_SelectFieldsParam& interpreterContext,
                                     tsp00_MoveObjPtr         pReturnCode,
                                     tgg00_StackEntry*        pCode,
                                     tsp00_Int4&              currQualIndex)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DBProc_Handler::ExecuteSqlStatement", DBProc_Trace, 1);
    tgg00_BasisError e;
    void* pOperandStack;
    int operandStackSize = sizeof(tgg00_StackEntry) *
        (interpreterContext.sfp_work_st_top() - REINTERPRET_CAST(tgg00_StEntryAddr, interpreterContext.sfp_work_st_addr()));
    if (operandStackSize > 0)
    {
         pOperandStack = pContext->GetAllocator().Allocate(operandStackSize);
         if (pOperandStack)
         {
            SAPDB_MemCopyNoCheck (pOperandStack, interpreterContext.sfp_work_st_addr(), operandStackSize);
         }
         else
         {
            pContext->GetErrorList().AppendNewMessage (Msg_List(Msg_List::Error, SDBMSG_DBPROC_NO_MEMORY, Msg_OptArg("MEMREQUEST",operandStackSize)));
             //pContext->GetErrorList().AppendNewMessage (SAPDBErr_MessageList (
             //   DBPROC_OUT_OF_MEMORY, ToStr(operandStackSize)));
             return e_no_more_memory;
         }
    }
    void* pWorkBuf;
    int workBufLength = interpreterContext.sfp_workbuf_len();
    if (workBufLength > 0)
    {
        pWorkBuf = pContext->GetAllocator().Allocate(workBufLength);
        if (pWorkBuf)
        {
            SAPDB_MemCopyNoCheck (pWorkBuf, interpreterContext.sfp_workbuf_addr(), workBufLength);
        }
        else
        {
            pContext->GetErrorList().AppendNewMessage (Msg_List(Msg_List::Error, SDBMSG_DBPROC_NO_MEMORY, Msg_OptArg("MEMREQUEST",workBufLength)));
            //pContext->GetErrorList().AppendNewMessage (SAPDBErr_MessageList (
            //    DBPROC_OUT_OF_MEMORY, ToStr(workBufLength)));
            return e_no_more_memory;
        }
    }

    a262execute (pContext, interpreterContext, pReturnCode, *REINTERPRET_CAST(tgg00_StackList*, pCode), currQualIndex, e);
    if (operandStackSize > 0)
    {
        SAPDB_MemCopyNoCheck (interpreterContext.sfp_work_st_addr(), pOperandStack, operandStackSize);
        pContext->GetAllocator().Deallocate(pOperandStack);
    }
    if (workBufLength > 0)
    {
        SAPDB_MemCopyNoCheck (interpreterContext.sfp_workbuf_addr(), pWorkBuf, workBufLength);
        pContext->GetAllocator().Deallocate(pWorkBuf);
    }
    return e;
}

//----------------------------------------------------------------------------------

void DBProc_Handler::UserDefinedFunctionCall  (
            tgg00_TransContext&      transContext,
            tgg00_SelectFieldsParam& interpreterContext,
            const tgg00_StackEntry&  functionCode,
            const tgg00_StackEntry&  functionId,
            short&                   error)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DBProc_Handler::UserDefinedFunctionCall", DBProc_Trace, 1);
    error = e_ok;
    SQLMan_Context* pContext = REINTERPRET_CAST(SQLMan_Context*, transContext.trAcvPtr_gg00);
    TRACE_CALL_STACK ("call stack (in)", interpreterContext);
    if (functionCode.ecol_tab()[1])
    {
        // function contains sql, i.e. a new statement context is required
        int       rc = 0;
        int       paramCnt;
        tsp00_C80 params;
        {
            int maxResId = pContext->a_max_res_id;
            SQLMan_SQLStatementContext savedContext(*pContext);
            callFunction (*pContext, interpreterContext, functionId.esurrogate(), functionCode.epos(), error);
            if (e_dbfunc_stop_msg == error)
            {
                rc       = pContext->ReturnCode();
                params   = pContext->a_error_params;
                paramCnt = pContext->a_err_parm_cnt;
            }
            SAPDBTRACE_WRITELN( DBProc_Trace, 5, "init max res id: " << maxResId << " new max res id: " << pContext->a_max_res_id);
            for (int resId = maxResId + 1; resId <= pContext->a_max_res_id; ++resId)
            {
                // close all cursors created inside function
                a663DelResult (*pContext, resId);
            }
        }
        if (0 != rc)
        {
            pContext->SetReturnCode(rc);
            pContext->a_error_params = params;
            pContext->a_err_parm_cnt = paramCnt;
        }
    }
    else
    {
        // function contains no sql, i.e. no new statement context is required
        callFunction (*pContext, interpreterContext, functionId.esurrogate(), functionCode.epos(), error);
    }
    TRACE_CALL_STACK ("call stack (out)", interpreterContext);
    if (e_ok == error)
    {
        SQLMAN_TRACE_STACK_ENTRY (DBProc_Trace, 5, "func result", *interpreterContext.sfp_work_st_top(), 0);
        SQLMAN_TRACE_BUFFER (DBProc_Trace, 5, "result value",
            &interpreterContext.sfp_workbuf_addr()[interpreterContext.sfp_work_st_top()->epos() - 1],
            interpreterContext.sfp_work_st_top()->elen_var());
    }
    else
    {
        if (e_dbfunc_stop_msg == error)
        {
            pContext->a_ak_b_err = e_dbfunc_stop_msg;
        }
    }
}

//----------------------------------------------------------------------------------

inline void DBProc_EvalExpression (SQLMan_Context&    context,
                                       tgg00_StackEntry&  current,
                                       int                startIdx,
                                       int                endIdx)
{
    SQLMan_StackEntry aux = current;
    current.epos()        = current.elen_var();
    current.elen_var()    = (dfloat == aux.eparamdatatype()) ?  csp_float_frac : aux.efraction();
    a651value_calculate (context, startIdx + 1, endIdx + 1, false, false, current.epos(), 1);
    current = aux;
}

//----------------------------------------------------------------------------------

inline void DBProc_GetInputParameter (SQLMan_Context&   context,
                                                           SQLMan_MessBlock&  callCode,
                                                           tgg00_StackEntry&  paramDesc,
                                                           tgg00_StackEntry&  valueDesc,
                                                           SAPDBFields_Field& param)

{
    tsp00_MoveObjPtr pParam;
    if (1 == valueDesc.ecol_tab()[0]) // value from sql packet
    {
        pParam = &context.a_data_ptr[valueDesc.epos()-1];
        if ((SAPDB_Byte) csp_defined_byte == (*pParam)[0])
        {
            switch (paramDesc.eparamdatatype())
            {
            case dcha :
            case ddate:
            case dtime :
            case dtimestamp :
                (*pParam)[0] = csp_ascii_blank;
                break;
            case dunicode :
                (*pParam)[0] = csp_unicode_def_byte;
                break;
            default :
                (*pParam)[0] = 0;
            }
        }
        param.Assign(pParam, valueDesc.elen_var());
    }
    else
    { // value from call mess block
        pParam          = (tsp00_MoveObjPtr) &callCode.DataBuf()[valueDesc.epos()-1];
        int valueLength = valueDesc.elen_var();
        if (valueLength > paramDesc.elen_var())
        { // calculated expression does not fit into parameter
            if ((dfloat == paramDesc.eparamdatatype()) ||
                (dfixed == paramDesc.eparamdatatype()))
            {
                // round number to required length
                int fraction = (dfloat == paramDesc.eparamdatatype()) ? csp_float_frac : paramDesc.efraction();
                tsp00_NumError numError;
                s51kroun (*pParam, 2, valueLength-1, *pParam, 2,
                    paramDesc.elen_var(), fraction, valueLength, numError);
                if (num_ok != numError)
                {
                    param.Deassign();
                    tgg00_BasisError e;
                    k71num_err_to_b_err (numError, e);
                    a07_b_put_error (context, e, 1);
                    return;
                }
            }
            else
            {
                param.Deassign();
                a07_b_put_error (context, e_column_trunc, 1);
                return;
            }
        }
        param.Assign(pParam, valueLength);
    }
}

//----------------------------------------------------------------------------------

void DBProc_Handler::ProcedureCall (SQLMan_Context&   context,
                                    int&              outParamCnt)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DBProc_Handler::ProcedureCall", DBProc_Trace, 1);
    SQLMan_MessBlock& callCode = context.GetMessBlock();
    /* load the code of the procedure first */
    Catalog_ISessionCache& sessionCache = context.GetSessionCatalogCache();
    Catalog_MessBlockObject* pCode = sessionCache.LoadCode(callCode.GetTableId());
    if (pCode)
    {
       const bool isPL = !callCode.GetQualBool();
       SQLMan_MessBlock& dbprocCode = pCode->MessBlock();
       SAPDB_AutoPtr<SAPDBFields_FieldMap>    pFieldMap(context.GetAllocator());
       SAPDB_AutoPtr<DBProc_DBProcDescriptor> pDBProcDescriptor(context.GetAllocator());
       if (!isPL)
       {   // external procedure call (java, ...), provide procedure description an parameter field map
           pDBProcDescriptor.SetPtr(new(context.GetAllocator()) DBProc_DBProcDescriptor(sessionCache, dbprocCode.GetTableId()));
           if (pDBProcDescriptor.IsAssigned())
           {
               if (!pDBProcDescriptor->IsValid())
               {
                   context.ThrowError(pDBProcDescriptor->GetLastError());
                   return;
               }
               pFieldMap.SetPtr(new(context.GetAllocator()) SAPDBFields_FieldMap (context.GetAllocator(), pDBProcDescriptor->ParamCount()));
               if (!pFieldMap.IsAssigned())
               {
                   context.GetErrorList().AppendNewMessage (Msg_List(Msg_List::Error, SDBMSG_DBPROC_NO_MEMORY/*, Msg_Arg("MEMREQUEST",?)*/));
//                   context.SetNoMoreMemoryError(__CONTEXT__);
                   return;
               }
           }
           else
           {
               context.GetErrorList().AppendNewMessage (Msg_List(Msg_List::Error, SDBMSG_DBPROC_NO_MEMORY/*, Msg_Arg("MEMREQUEST",?)*/));
//               context.SetNoMoreMemoryError(__CONTEXT__);
               return;
           }
       }
       outParamCnt     = 0;
       int st_ix       = callCode.GetQualPos() - 1;
       int end_ix      = st_ix + callCode.GetQualCount();
       int valueCnt    = 0;
       int currParam   = -1;
       int firstIdx    = 0;
       /* Push all IN/INOUT parameters from callCode into dbprocCode */
       SQLMan_StackEntry* pValue;
       while (st_ix < end_ix)
       {
           SQLMan_StackEntry* pCurrSt = callCode.StackEntryPtr(st_ix);
           SQLMAN_TRACE_STACK_ENTRY (DBProc_Trace, 5, "call code", *pCurrSt, st_ix + 1);
           switch (pCurrSt->etype())
           {
           case st_jump_output :
               {
                   st_ix = pCurrSt->epos();
                   continue;
               }
           case st_jump_absolute :
               {
                   st_ix += pCurrSt->epos() + 1;
                   continue;
               }
           case st_surrogate :
               {
                   // user defined function
                   ++st_ix;
                   valueCnt += 2; // force DBProc_EvalExpression
                   continue;
               }
           case st_value :
               {   // one parameter value
                   if (0 == valueCnt)
                   {
                       firstIdx = st_ix;
                       pValue   = pCurrSt;
                   }
                   ++valueCnt;
                   break;
               }
           case st_dbproc_param :
               {   // push current parameter
                   ++currParam;
                   if (valueCnt > 1)
                   {
                       // parameter expression has not been evaluated yet, do it now
                       DBProc_EvalExpression (context, *pCurrSt, firstIdx, st_ix - 1);
                   }
                   valueCnt = 0;
                   if (pCurrSt->eop() != op_not_in)
                   { // push input/inout parameter
                       SAPDBFields_AttributedField param;
                       DBProc_GetInputParameter (context, callCode, *pCurrSt, *pValue, param);
                       if (param.IsAssigned())
                       {
                           SQLMAN_TRACE_BUFFER (DBProc_Trace, 5, "Push Parameter", param.GetDataPtr(),  param.GetLength());
                           SAPDB_RangeMove (__FILE__,   2,
                               param.GetLength(), dbprocCode.DataSize(),
                               param.GetDataPtr(), 1,
                               dbprocCode.DataBuf(), pCurrSt->ecol_pos() + 1, param.GetLength(),
                               context.a_returncode);
                           if (pFieldMap.IsAssigned())
                           {
                               pFieldMap->Fill (currParam, currParam, param);
                           }
                       }
                   }
                   else
                   { // output parameter, initialize to NULL value
                       SAPDBTRACE_WRITELN( DBProc_Trace, 1, "out param at " << pCurrSt->ecol_pos() << " length " << pCurrSt->elen_var());
                       dbprocCode.DataBuf()[pCurrSt->ecol_pos()] = (SAPDB_Byte) csp_undef_byte;
                   }
                   if (pCurrSt->eop() != op_in)
                   {  // output parameter
                       ++outParamCnt;
                       if (pFieldMap.IsAssigned())
                       {
                           SAPDBFields_AttributedField outParam(&dbprocCode.DataBuf()[pCurrSt->ecol_pos()], pCurrSt->elen_var());
                           pFieldMap->Fill (currParam, currParam, outParam);
                       }
                   }
                   break;
               }
           }
           ++st_ix;
       }
       if (isPL)
       {
           // call pl sql procedure
           DBProc_Handler::PLDBProcCall (context, pCode, dbprocCode);
       }
       else
       {
           DBProc_Handler::ExternalProcCall (context, *pDBProcDescriptor, *pFieldMap);
       }
    }
    else
    {
        // could not load code
    }
}

//----------------------------------------------------------------------------------

void DBProc_Handler::PLDBProcCall (SQLMan_Context&          context,
                                   Catalog_MessBlockObject* pCode,
                                   SQLMan_MessBlock&        dbprocCode)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DBProc_Handler::PLDBProcCall", DBProc_Trace, 1);
    if (dbprocCode.StringCount() > 0)
    {   // initialize local variables with NULL
        SQLMan_StackEntry& varDescriptor = dbprocCode.StackEntry(dbprocCode.StringPos()-1);
        SAPDB_RangeFill (__FILE__,   3,
            dbprocCode.DataSize(), dbprocCode.DataPtr(),
            cgg_rec_key_offset + varDescriptor.epos(), varDescriptor.elen_var(), (SAPDB_Byte) csp_undef_byte,
            context.a_returncode);
    }
    if (context.IsOk())
    {
       a262Call (context, REINTERPRET_CAST(tak_sysbufferaddress, pCode), dbprocCode.DataPtr());
    }
}

/*----------------------------------------*/

static Proc_IFactory *
createFactory (
    SQLMan_Context           * context,
    DBProc_DBProcDescriptor  * DBProcDescriptor,
//    SAPDBErr_MessageList     * errInfo)
    Msg_List                 * errInfo)
{
    Proc_IFactory * result = NULL;
    char            serverKind;
    char            commandLine [256];
    char            extraInfo [256];
    const char *    languageString = DBProcDescriptor->GetLanguageString ();
    int             sessionLimit;

    if (!Proc_SQL::selectUdeServerConfig (context, languageString,
            &serverKind, commandLine, extraInfo, &sessionLimit, errInfo)) {
        return NULL;
    }
    //result = new (ProcRTE_Runtime::allocator ())
    //    Proc_ServerSessionFactory(commandLine);
    result = reinterpret_cast<Proc_IFactory *> (
        ProcRTE_Runtime::alloc (sizeof (Proc_ServerSessionFactory)));
    if (result == NULL)
    {
        *errInfo = Msg_List(Msg_List::Error, SDBMSG_DBPROC_NO_MEMORY, Msg_OptArg("MEMREQUEST",sizeof(Proc_ServerSessionFactory)));
        //*errInfo = SAPDBErr_MessageList (
        //        DBPROC_OUT_OF_MEMORY, ToStr (sizeof (Proc_ServerSessionFactory)));
        return NULL;
    }
    new (reinterpret_cast<SAPDB_Byte*> (result)) Proc_ServerSessionFactory(
        languageString, serverKind, commandLine, sessionLimit);
    Proc_FactoryDirectory::addFactory (DBProcDescriptor->GetLanguage (),
            languageString, result);
    return result;
}

/*----------------------------------------*/

static Proc_ISession *
getProcServerSession (
    SQLMan_Context           & context,
    DBProc_DBProcDescriptor  & DBProcDescriptor,
//    SAPDBErr_MessageList     & errInfo)
    Msg_List                 & errInfo)
{
    SAPDBTRACE_ROUTINE_DEBUG ("getProcServerSession", DBProc_Trace, 1);
    /*
     * try to find an open session for languageString
     */
    Proc_SessionList * sessionList = context.GetUDESessions ();
    Proc_ISession * result = sessionList->findSession (
        DBProcDescriptor.GetLanguage (), DBProcDescriptor.GetLanguageString ());
    if (result != NULL) {
        return result;
    }
    /*
     * find or create the proc server session factory
     *   depending on the dbproc language
     */
    Proc_IFactory * factory = Proc_FactoryDirectory::findFactory (
        DBProcDescriptor.GetLanguage (), DBProcDescriptor.GetLanguageString ());
    if (factory == NULL) {
        /*
         * create a new factory
         */
        factory = createFactory (&context, &DBProcDescriptor, &errInfo);
#if defined (SAPDB_SLOW)
        Proc_FactoryDirectory::dump (5);
#endif
    }
    if (factory == NULL) {
        return NULL;
    }
    /*
     * create a new session object
     */
    result = factory->createSession (&errInfo);
    if (result == NULL) {
        return NULL;
    }
    SAPDBTRACE_WRITELN (DBProc_Trace, 2, "created session "
        << ToStr (result) << " from factory " << ToStr (factory));
    /*
     * store the newly created session object
     *  in the session context
     */
    sessionList->addSession (DBProcDescriptor.GetLanguage (),
        DBProcDescriptor.GetLanguageString (), result);
    /*
     * return result
     */
    return result;
}

/*----------------------------------------*/

static void
signalError (
    SQLMan_Context          & context,
//    SAPDBErr_MessageList    & errInfo,
    Msg_List                & errInfo,
    const char              * defaultMessage)
{
    SAPDB_Char msgbuf [72];
    int msglen;
    int udeprocFailed = -4711;

    if (errInfo.IsEmpty ()) {
        msglen = sp77sprintf (msgbuf, sizeof (msgbuf), " %.70s", defaultMessage);
    }
    else {
        if (errInfo.SystemRC () != 0) {
            udeprocFailed = errInfo.SystemRC ();
        }
        SAPDB_Char const * component = errInfo.Component ();
        SAPDB_Char const * msgtext = errInfo.Message ();
        msglen = sp77sprintf (msgbuf, sizeof (msgbuf), " [%s] %.*s",
            component, 70 - strlen (component), msgtext);
    }
    context.SetStopMsg (udeprocFailed, msgbuf, msglen);
}

/*----------------------------------------*/

DBProc_Handler::CallResult DBProc_Handler::ExternalProcCall (
    SQLMan_Context          & context,
    DBProc_DBProcDescriptor & DBProcDescriptor,
    SAPDBFields_FieldMap    & parameterMap)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DBProc_Handler::ExternalProcCall", DBProc_Trace, 1);
    Proc::Error err;

//    SAPDBErr_MessageList errInfo;
    Msg_List errInfo;
    SAPDBTRACE_WRITELN( DBProc_Trace, 1, "Name        : " << DBProcDescriptor.GetExternalInfo());
    SAPDBTRACE_WRITELN( DBProc_Trace, 1, "DBProc Name : " << DBProcDescriptor.GetName());
    SAPDBTRACE_WRITELN( DBProc_Trace, 1, "Date/Time   : " << DBProcDescriptor.GetCreateDateTime());
    SAPDBTRACE_WRITELN( DBProc_Trace, 1, "Language    : " << DBProcDescriptor.GetLanguageString());
    Proc_ParameterCopy paramCopy (&context.GetAllocator (), &parameterMap);
    /*
     * get the proc server session
     */
    Proc_ISession * session;
    session = getProcServerSession (context, DBProcDescriptor, errInfo);
    if (session == NULL) {
        signalError (context, errInfo, "UDE prepare: unexpected error");
        return call_failed;
    }
    err = session->callDBProc (&context, &DBProcDescriptor, &paramCopy);
    /*
     * end of external procedure call
     */
    DBProc_Handler::CallResult result = call_ok;
    switch (err) {
        case Proc::udeReturnedError:
            // already handled in lower level
            break;
        default:
            signalError (context, errInfo, "UDE call: unexpected error");
            result = call_failed;
            break;
        case Proc::ok:
            // return the output parameters, temporary solution only !!
            SAPDB_Int2 e = e_ok;
            a06reset_retpart (context);
            // a262ReturnParam (&context, dunknown, NULL, 0, e);
            DBProc_DBProcDescriptor::ParameterIterator outParamIter = DBProcDescriptor.FirstParameter();
            while (outParamIter)
            {
                if (outParamIter.IsOutputParameter())
                {
                    SAPDBFields_AttributedField field;
//                    SAPDBErr_MessageList        errlist;
                    Msg_List errlist;
                    if(paramCopy.GetField(outParamIter.ParameterNo(), field, errlist) != SAPDBFields_IFieldSequence::fieldFound)
                        RTE_Crash(errlist);
                    a262ReturnParam (&context, outParamIter.ParamDataType() ,
                        REINTERPRET_CAST(tsp00_MoveObjPtr, field.GetDataPtr() - 1), field.GetLength() + 1, e);
                    if (e_ok != e)
                    {
                        context.ThrowError(e);
                    }
                }
                ++outParamIter;
            }
            /*
             * set cursor name
             */
            const tsp00_Char * cursorName;
            int                cursorNameLen;
            if (paramCopy.GetCursorName (cursorName, cursorNameLen)) {
                --cursorName;  // a262ReturnCursor expects defined byte
                a262ReturnCursor (&context,
                    (tsp00_MoveObjPtr) cursorName, cursorNameLen, e);
            }
            break;
    }

    /*
     *
     */
    return result;
}

/*----------------------------------------*/

bool DBProc_Handler::CheckExternalProc (
    SQLMan_Context          & context,
    DBProc_DBProcDescriptor & DBProcDescriptor)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DBProc_Handler::CheckExternalProc", DBProc_Trace, 1);
    return true;
}

/*----------------------------------------*/

