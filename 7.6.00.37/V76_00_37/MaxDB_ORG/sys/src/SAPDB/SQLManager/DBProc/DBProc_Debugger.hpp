/*!
  @defgroup       DBProc DB-Procedures and DB-Functions
  @file           DBProc_Debugger.hpp
  @author         ThomasA
  @ingroup        DBProc
  @brief          Debugging of PL db-procedures

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

#ifndef DBPROC_DEBUGGER_HPP
#define DBPROC_DEBUGGER_HPP

#include "SQLManager/SQLMan_Types.hpp"
#include "Container/Container_AVLTree.hpp"
#include "Container/Container_Stack.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

class SQLMan_Context;

/*!
   @brief implements debugging of db-procedures, triggers and db-functions
*/
class DBProc_Debugger
{
public :
    /*!
      @brief sets or removes a breakpoint. 
     */
    void addRemoveBreakPoint (const SQLMan_Surrogate& procId, int procKind, int type, int pos, bool doRemove);
    /*!
      @brief checks, if a breakpoint has been reached. If true, the listener is waked up and the
             current task is suspended
    */
    tgg00_BasisError checkDebugBreak(const SQLMan_Surrogate& procId,
        int              procKind,
        int              type,
        int              currStackEntry,
        tsp00_MoveObjPtr pTriggerColumns,
        tsp00_MoveObjPtr pVariables,
        int              codePos, 
        int              codeLength); 
    /*!
       @brief creates a debugger instance. This is shared by 3 tasks (debugged task, debugging task and listener task)
     */
    static void createDebugger(SQLMan_Context& debuggingSession, SQLMan_Context& debuggedSession, SQLMan_Context& listenerSession);
    /*!
       @brief returns info store by storeExecuteInfo
    */
    static void getExecuteInfo (void* pInfo, int size);
    /*!
       @brief returns a pointer to the required function parameters and its length
     */
    void getFunctionParameter (tgg00_StackEntry& funcParameter, tsp00_MoveObjPtr& pParam, int& length);
    /*!
       @brief called whenever a db-procedure/trigger/function is left. Pops it from the call stack
     */
    void popCall();
    /*!
       @brief called whenever a db-procedure/trigger/function is called. Pushed this information onto the stack 
     */
    void pushCall (const SQLMan_Surrogate& procId,
        int              procKind,
        int              linkage,
        void*            pContext);
    /*!
       @brief releases a debugger instance. This is called by each task attached to the instance. When the
              last task releases, the instance is destructed
     */
    static void releaseDebugger(SQLMan_Context* pContext);
    /*!
       @brief returns the method identification and type, the trigger values and the variables of the current db-procedure
     */
    void getInfo(SQLMan_Surrogate& methodId, int& methodKind, int& linkage, tsp00_MoveObjPtr& pTriggerColumns, tsp00_MoveObjPtr& pVariables);
    /*!
       @brief waits for the next breakpoint. Returns the method identification and the
              source code position and length of the breakpoint
     */
    void listen(SQLMan_Surrogate& methodId, int& procKind, int& linkage, int& pos, int& length);
    /*!
      @brief back trace iterator
    */
    int nextBackTrace (int prevLevel, SQLMan_Surrogate& methodId, int& methodKind, int& linkage) const; 
    /*!
       @brief steps forward count steps
     */
    void step(int count);
    /*!
       @ stores information between parse and execute
    */
    static void storeExecuteInfo (void* pInfo, int size);
private :
    DBProc_Debugger(SQLMan_Context& debuggingSession, SQLMan_Context& debuggedSession, SQLMan_Context& listenerSession);
    void clearBreakPoints();
    struct BreakPoint
    {
        SQLMan_Surrogate m_methodId;
        int              m_type;
        int              m_pos;
        BreakPoint(const SQLMan_Surrogate& methodId, int type, int pos)
            : m_methodId(methodId)
            , m_type(type)
            , m_pos(pos)
        {
        }
        bool operator< (const BreakPoint& other) const
        {
            return (0 > memcmp (this, &other, sizeof(other)));
        }
    };
    struct CallStackEntry 
    {
        CallStackEntry()
        {
        }
        CallStackEntry(const SQLMan_Surrogate methodId, int type, int linkage, void* pContext)
            : m_methodId(methodId)
            , m_type(type)
            , m_linkage(linkage)
            , m_pContext(pContext)
        {
        }
        SQLMan_Surrogate m_methodId;
        int              m_type;
        int              m_linkage;
        void*            m_pContext;
    };

    typedef Container_AVLNodeComparator<BreakPoint> BreakPointComparator;
    typedef Container_AVLContentNode<BreakPoint, int, BreakPointComparator> BreakPointNode;
    typedef Container_AVLContentTree<BreakPointNode, BreakPoint, int, BreakPointComparator> BreakPointDirectory;
  
    static BreakPointComparator m_breakpointComparator;
    RTESync_Spinlock                m_lock;
    BreakPointDirectory             m_breakpoints;
    SQLMan_Context*                 m_debuggingSession; 
    SQLMan_Context*                 m_debuggedSession;
    SQLMan_Context*                 m_listenerSession;
    tsp00_MoveObjPtr                m_variables;
    tsp00_MoveObjPtr                m_triggerColumns;
    bool                            m_debuggerWaiting;
    bool                            m_debuggedWaiting;
    SQLMan_Surrogate                m_breakedId;
    int                             m_procKind;
    int                             m_linkage;
    int                             m_step;
    int                             m_breakCodePos;
    int                             m_breakCodeLength;
    void*                           m_pInfo;
    Container_Stack<CallStackEntry> m_callStack;
    int                             m_stepLevel;
};

#endif