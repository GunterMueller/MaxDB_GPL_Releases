/*!
  @file           DBProc_Debugger.cpp
  @author         ThomasA
  @ingroup        DBProc
  @brief          methods for db-procedure debugging

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
#include "SQLManager/DBProc/DBProc_Debugger.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include "heo56.h" // vwait, vsignal
#include "hkb71.h"

#define STEP_INTO     0
#define STEP_CANCEL -10
#define STEP_OUT     -3
#define STEP_STOP    -2
#define UNDEFINED_STEP_LEVEL -1

DBProc_Debugger::BreakPointComparator DBProc_Debugger::m_breakpointComparator;

//-----------------------------------------------------------------------------

void DBProc_Debugger::addRemoveBreakPoint (const SQLMan_Surrogate& procId, int procKind, int type, int pos, bool doRemove)
{
    RTESync_LockedScope sync(m_lock);
    Container_AVLTreeError rc;
    if (doRemove)
    {
        rc = m_breakpoints.Delete(BreakPoint(procId, type, pos));
    }
    else
    {
        m_breakpoints.Insert(BreakPoint(procId, type, pos), procKind, rc);
    }
}

//-----------------------------------------------------------------------------

tgg00_BasisError DBProc_Debugger::checkDebugBreak(const SQLMan_Surrogate& procId,
                                      int              procKind,
                                      int              type,
                                      int              currStackEntry,
                                      tsp00_MoveObjPtr pTriggerColumns,
                                      tsp00_MoveObjPtr pVariables,
                                      int              codePos, 
                                      int              codeLength)
{
    bool doBreak       = false;
    tgg00_BasisError e = e_ok;
	const BreakPointNode* pBreakPoint = 0;
    if (STEP_CANCEL == m_step)
    {
        return e_cancelled;
    }
    m_lock.Lock();
    if (m_debuggerWaiting && (m_step > 0))
    {
        --m_step;
        if (0 == m_step)
        {
            m_procKind  = procKind;
            m_linkage   = type;
            doBreak = (UNDEFINED_STEP_LEVEL == m_stepLevel);
            if (!doBreak)
            {
                doBreak = (m_stepLevel == m_callStack.GetSize());
                if (!doBreak)
                {
					// is this an active breakpoint ?
					pBreakPoint = m_breakpoints.Find(BreakPoint(procId, type, currStackEntry));
					if(!pBreakPoint)
					{
                        // restore step
                        m_step = 1;
					}
                }
            }
        }
    }
    else
    {
        if (m_debuggerWaiting)
		{
			pBreakPoint = m_breakpoints.Find(BreakPoint(procId, type, currStackEntry));
		}
    }
	if (pBreakPoint)
	{
		 m_procKind  = *pBreakPoint->GetContent();
         m_linkage   = type;
         m_step      = 0;
         m_stepLevel = UNDEFINED_STEP_LEVEL;
         doBreak     = true;
	}
    if (doBreak)
    {
        m_debuggerWaiting = false;
        m_breakedId       = procId;         
        m_breakCodePos    = codePos;
        m_breakCodeLength = codeLength;
        m_variables       = pVariables;
        m_triggerColumns  = pTriggerColumns;
        m_debuggerWaiting = false;
        vsignal (m_listenerSession->TransContext().trTaskId_gg00, true);
        tsp00_Bool dummy;
        m_debuggedWaiting = true;
        m_lock.Unlock();
        vwait (m_debuggedSession->TransContext().trTaskId_gg00, dummy);
        m_debuggedWaiting = false;
    }
    else
    {
        m_lock.Unlock();
    }
    return e;
}

//------------------------------------------------------------------------------

DBProc_Debugger::DBProc_Debugger(SQLMan_Context& debuggingSession, SQLMan_Context& debuggedSession, SQLMan_Context& listenerSession)
: m_debuggingSession(&debuggingSession)
, m_debuggedSession(&debuggedSession)
, m_listenerSession(&listenerSession)
, m_lock()
, m_variables(0)
, m_debuggerWaiting(false)
, m_debuggedWaiting(false)
, m_procKind(-1)
, m_linkage(-1)
, m_step(0)
, m_breakCodePos(0)
, m_breakCodeLength(0)
, m_breakpoints(&m_breakpointComparator, &RTEMem_Allocator::Instance())
, m_callStack(RTEMem_Allocator::Instance())
, m_stepLevel(UNDEFINED_STEP_LEVEL)
{
    SAPDBTRACE_WRITELN( DBProc_Trace, 1, "debuggingSession " << debuggingSession.TransContext().trTaskId_gg00);
    SAPDBTRACE_WRITELN( DBProc_Trace, 1, "debuggedSession " << debuggedSession.TransContext().trTaskId_gg00);
    SAPDBTRACE_WRITELN( DBProc_Trace, 1, "listenerSession " << listenerSession.TransContext().trTaskId_gg00);
}

//------------------------------------------------------------------------------

void DBProc_Debugger::clearBreakPoints()
{
    m_breakpoints.DeleteAll();
}

//------------------------------------------------------------------------------

void DBProc_Debugger::createDebugger(SQLMan_Context& debuggingSession, SQLMan_Context& debuggedSession, SQLMan_Context& listenerSession)
{
    DBProc_Debugger* p = debuggedSession.GetDBProcDebugger();
    if (p)
    {
        RTESync_LockedScope sync(p->m_lock);
        if (p->m_debuggingSession || p->m_listenerSession)
        {
            // there's already a debugger attached
            debuggingSession.ThrowError (e_invalid);
            return;
        }
        else
        {
             p->m_debuggingSession = &debuggingSession;
             p->m_listenerSession  = &listenerSession;
        }
    }
    else
    {
        p = new(RTEMem_Allocator::Instance()) DBProc_Debugger(debuggingSession, debuggedSession, listenerSession);
    }
    if (p)
    {
        debuggingSession.SetDBProcDebugger(p);
        debuggedSession.SetDBProcDebugger(p);
        listenerSession.SetDBProcDebugger(p);
    }
}

//------------------------------------------------------------------------------

void DBProc_Debugger::getExecuteInfo (void* pInfo, int size)
{
    SQLMan_Context*  pContext  = SQLMan_Context::GetContext();
    DBProc_Debugger* pDebugger = pContext->GetDBProcDebugger();
    if (pDebugger && (pDebugger->m_pInfo))
    {
        SAPDB_MemCopyNoCheck (pInfo, pDebugger->m_pInfo, size);
        pContext->GetAllocator().Deallocate(pDebugger->m_pInfo);
        pDebugger->m_pInfo = NULL;
    }
}

//------------------------------------------------------------------------------

void DBProc_Debugger::getFunctionParameter (tgg00_StackEntry& funcParameter, tsp00_MoveObjPtr &pParam, int& length)
{
    tgg00_BasisError e;
    CallStackEntry& top = m_callStack.Top();
    tgg00_SelectFieldsParam& interpreterContext = *reinterpret_cast<tgg00_SelectFieldsParam*>(top.m_pContext);
    tgg00_StackEntry* workStTop = interpreterContext.sfp_work_st_top();
    interpreterContext.sfp_work_st_top() = &funcParameter;
    k71get_operand (interpreterContext, false, pParam, length, e);
    interpreterContext.sfp_work_st_top() = workStTop;
    if (e_ok != e)
    {
        pParam = 0;
    }
}

//------------------------------------------------------------------------------

void DBProc_Debugger::getInfo(SQLMan_Surrogate& methodId, 
                              int&              methodKind,
                              int&              linkage,
                              tsp00_MoveObjPtr& pTriggerColumns, 
                              tsp00_MoveObjPtr& pVariables)
{
    RTESync_LockedScope sync(m_lock);
    if (m_debuggedWaiting)
    {
        methodId        = m_breakedId;
        methodKind      = m_procKind;
        linkage         = m_linkage;
        pVariables      = m_variables;
        pTriggerColumns = m_triggerColumns;
    }
    else
    {
        pVariables = NULL;
    }
}

//------------------------------------------------------------------------------

int DBProc_Debugger::nextBackTrace (int prevLevel, SQLMan_Surrogate& methodId, int& methodKind, int& linkage) const
{
    int currLevel = (-1 == prevLevel) ? m_callStack.GetSize() - 1 : prevLevel - 1;
    if (currLevel >= 0 && currLevel < (int) m_callStack.GetSize())
    {
        methodKind = m_callStack[currLevel].m_type;
        methodId   = m_callStack[currLevel].m_methodId;
        linkage    = m_callStack[currLevel].m_linkage;
        return currLevel;
    }
    return -1;
}

//------------------------------------------------------------------------------

void DBProc_Debugger::listen(SQLMan_Surrogate& methodId, int& procKind, int& linkage, int& pos, int& length)
{
    m_lock.Lock();
    m_debuggerWaiting = true;
    tsp00_Bool ok;
    m_lock.Unlock();
    vwait (m_listenerSession->TransContext().trTaskId_gg00, ok);
    RTESync_LockedScope sync(m_lock);
    if (!ok)
    {
        m_listenerSession->ThrowError (e_invalid);
    }
    else
    {
        methodId = m_breakedId;
        procKind = m_procKind;
        linkage  = m_linkage;
        pos      = m_breakCodePos;
        length   = m_breakCodeLength;
    }
}

//------------------------------------------------------------------------------

void DBProc_Debugger::popCall ()
{
    if (m_stepLevel == m_callStack.GetSize())
    {
        --m_stepLevel;
    }
    m_callStack.Pop();
    if (m_callStack.IsEmpty())
    {
        m_step = 0;
    }
}

//------------------------------------------------------------------------------

void DBProc_Debugger::pushCall (const SQLMan_Surrogate& procId,
               int              procKind,
               int              linkage,
               void*            pContext)
{
    CallStackEntry callInfo(procId, procKind, linkage, pContext);
    m_callStack.Push(callInfo);
}

//------------------------------------------------------------------------------

void DBProc_Debugger::releaseDebugger(SQLMan_Context* pContext)
{
    DBProc_Debugger* pDebugger = pContext->GetDBProcDebugger();
    if (pDebugger)
    {
        RTESync_LockedScope sync(pDebugger->m_lock);
        pContext->SetDBProcDebugger(NULL);
        if ((pContext == pDebugger->m_debuggingSession) || (pContext == pDebugger->m_debuggedSession))
        {
            if (pDebugger->m_debuggerWaiting)
            {
                pDebugger->m_debuggerWaiting = false;
                vsignal (pDebugger->m_listenerSession->TransContext().trTaskId_gg00, false);
            }
            if (pContext == pDebugger->m_debuggingSession)
            {
                pDebugger->clearBreakPoints();
                if (pDebugger->m_debuggedWaiting)
                {
                   pDebugger->m_debuggedWaiting = false;
                   vsignal (pDebugger->m_debuggedSession->TransContext().trTaskId_gg00, true);
                }
                pDebugger->m_debuggingSession = NULL;
            }
            else
            {
                pDebugger->m_debuggedSession = NULL;
            }
        }
        if (pContext == pDebugger->m_listenerSession)
        {
            pDebugger->m_listenerSession = NULL;
        }
        if (pDebugger->m_debuggingSession || pDebugger->m_debuggedSession || pDebugger->m_listenerSession)
        {
            return;
        }
        destroy (pDebugger, RTEMem_Allocator::Instance());
    }
}

//------------------------------------------------------------------------------

void DBProc_Debugger::step(int count)
{
    if (STEP_STOP == count)
    {
        // stop debugging
        releaseDebugger(SQLMan_Context::GetContext());
        return;
    }
    RTESync_LockedScope sync(m_lock);
    if (m_debuggedWaiting)
    {
        if (STEP_INTO == count)
        {
            m_stepLevel = UNDEFINED_STEP_LEVEL;
            count = 1;
        }
        else
        {
            if (STEP_OUT == count)
            {
                m_stepLevel = m_callStack.GetSize() - 1;
                count = 1;
            }
            else
            {
                m_stepLevel = m_callStack.GetSize();
            }
        }
        m_step            = count;
        m_debuggedWaiting = false;
        vsignal (m_debuggedSession->TransContext().trTaskId_gg00, true);
    }
}

//------------------------------------------------------------------------------

void DBProc_Debugger::storeExecuteInfo (void* pInfo, int size)
{
    SQLMan_Context*  pContext  = SQLMan_Context::GetContext();
    DBProc_Debugger* pDebugger = pContext->GetDBProcDebugger();
    if (pDebugger)
    {
        pDebugger->m_pInfo = pContext->GetAllocator().Allocate(size);
        if (pDebugger->m_pInfo)
        {
            SAPDB_MemCopyNoCheck (pDebugger->m_pInfo, pInfo, size);
        }
    }
}