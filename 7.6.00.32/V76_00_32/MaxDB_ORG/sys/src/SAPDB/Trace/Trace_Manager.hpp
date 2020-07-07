/*!
  @file     Trace_Manager.hpp
  @author   TorstenS
  @ingroup  Trace
  @brief    This class defines the singleton component trace manager and 
            offers the possibility to insert trace entries into the UKT 
            specific memory trace buffers. Furthermore it's possible to
            write the materialized trace entries into the kernel trace file
            The writeable trace entries are derived from the abstract
            classes Trace_IBaseEntry and Trace_ILargeEntry found in 
            Trace_Types.hpp. The concretely implementations of the trace
            entries are in Trace_[A-Z*]Entry.hpp, e.g.Trace_PacketEntry.hpp.

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



#ifndef TRACE__MANAGER_HPP
#define TRACE__MANAGER_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types

#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_TaskWaitList.hpp"

#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"

#include "Trace/Trace_DevSpace.hpp"
#include "Trace/Trace_Types.hpp"
#include "Trace/Trace_SlotPool.hpp"

class RTETask_ITask;
class Trans_Context;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class    Trace_Manager
  @brief    This class is the kernel trace interface.
 */

class Trace_Manager
{
public:

    /*!
       @brief   This method creates the singleton trace manager.It is assumed 
                that this method will be callled only one time by the timeout 
                task and therefore no synchonization mechanism is used.
       @param   TaskId [in] current task id
       @return  true means instance could be created; else false
     */

    static SAPDB_Bool CreateInstance( const tsp00_TaskId TaskId );

    /*!
       @brief   This method returns the access to the trace manager. It is 
                assumed that the trace manager singleton is available! No
                CreateInstance call is will be done.
       @param   TaskId [in] current task id
       @return  Access to the trace manager singleton
     */

    static Trace_Manager& GetInstance(){return( *m_Instance );};

    /*!
       @brief   This method is used to flush the memory trace buffers into
                the kernel trace file. The flush will be done by the trace 
                writer task, triggered by the diagnose vtrace flush command,
                the diagnose clear vtrace command or a kernel abort.
       @param   TaskId [in] current task id
       @return  none
     */

    void Flush( const tsp00_TaskId TaskId );

    /*!
       @brief   This method provides the name of the kernel dump file. It is
                used by the trace writer to write the memory structures, e.g.
                data cache into the kernel dump file. During the flush of an
                UKT specific trace buffer, it is locked via a spinlock, so that
                no other task within the same UKT is able to write trace entries
                into the trace buffer.
       @return  Name of the kernel dump file
     */

    tsp00_VFilename GetDumpFile() const
    {
        return( m_DumpFile );
    }

    /*!
       @brief   This method is used to write a trace entry smaller than 132
                bytes into the kernel trace. Trace entries smaller than 0 and
                and greater than 132 won't be inserted. No errors are reported.
       @param   Entry [in] trace entry handler for plain entries
       @return  none
     */

    void Insert( Trace_IBaseEntry &Entry );

    /*!
       @brief   This method is used to write a large trace entry into the kernel 
                trace. Trace entries smaller than 0 won't be inserted. No errors 
                are reported. The trace entry will be splitted into separate parts.
                Each part has the layout of an ordinary trace entry, but inside
                the info part it contains a internal header. This internal header
                is used by the diagnose tool the combine the splitted parts to one
                trace entry.
       @param   Entry [in] trace entry handler for huge entries.
       @return  none
     */

    void InsertLarge( Trace_ILargeEntry   &Entry );

    /*!
       @brief   This method is used to force the flushing of the memory trace 
                buffers into the kernel trace file. It's assumed and tested by
                an assertion that the calling task is the trace writer!
                The flush order is locked via the trace region to avoid parallel 
                access. 
       @param   TaskId [in] current task id 
       @return  (SAPDB_Bool) true, if no flush request is already active
     */

    SAPDB_Bool TriggerFlush( const tsp00_TaskId taskId ); // PTS 1120283 TS 2003-01-27

    /*!
       @brief   This method is used to force the flushing of the memory trace 
                buffers into the kernel trace file. The calling user task will
                be suspended as long as the trace is not flushed by the trace 
                writer task. The flush order is locked via the trace region to
                avoid parallel access. After the complete flush the waiting tasks
                will be resumed by the trace writer task. This method furthermore
                offers the possibility to reinitialize the kernel trace file. This
                means that all information will be lost!
       @param   TaskId [in] current task id
       @param   bReInitialization [in] true means to reinitialize the trace file.
       @return  none
     */

    void PrepareAndWaitForFlush(
        const tsp00_TaskId    TaskId,
        const SAPDB_Bool      bReInitialization = false );

    /*!
       @brief   This method is used one time by the trace writer task to register 
                themself by the trace manager component after the own creation.
       @param   TaskId [in] current task id
       @return  none
     */

    void TraceWriterRegistration( const tsp00_TaskId taskId )
    {
        m_TraceWriterTaskId = taskId;
    }

private:

    /*!
       @brief   constructor
       @param   NumUKTs [in] number of UKTs
       @return  none
     */

    Trace_Manager ( const SAPDB_Int4  numUKTs )
    :
    m_TraceWriterTaskId( cgg_nil_pid ),
    m_NumUKTs( numUKTs ),
    m_bDoClearTrace( SAPDB_FALSE ),
    m_bFlushTraceActive( SAPDB_FALSE ),
    m_SeqNoGenerator( TRACE_FIRST_SEQNO )
    {}

    /*!
       @brief   This method is used to initialize the trace component.
       @param   TaskId [in] current task id
       @return  (SAPDB_Bool) true if successfull
     */

    SAPDB_Bool Initialize( const tsp00_TaskId TaskId );

    /*!
       @brief   This method is used to check if the kernel trace is writable
       @param   Trans     [in] Transaction Context
       @param   EntrySize [in] Length of given trace entry
       @return  (SAPDB_Bool) true if writable
     */

    SAPDB_Bool WriteEntryPossible(
        Trans_Context       &Trans,
        const SAPDB_Int4    EntrySize ) const;

    /*!
       @brief   This method is used to determine if the trace writer is alife.
       @return  (SAPDB_Bool) true if registered
     */

    SAPDB_Bool IsTraceWriterAlife() const
    {
        return( cgg_nil_pid != m_TraceWriterTaskId );
    }

    /*!
       @brief   The assignment operator is not supported to avoid not allowed
                duplicates.
       @param   traceManager [in] traceManager 
       @return  Trace_Manager&
     */

    Trace_Manager& operator=( const Trace_Manager &traceManager );

    /*!
       @brief   The copy operator is not supported to avoid not allowed
                duplicates.
       @param   traceManager [in] traceManager 
       @return  Trace_Manager&
     */

    Trace_Manager( const Trace_Manager &traceManager );

    /*!
       @brief   returns current trans context
       @param   pTrans [out] own trans context
       @return  (bool) trans context is available
     */

    bool GetTransContext( Trans_Context* &pTrans ) const;

private:

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  TraceManager Instance pointer
     */
    /* -----------------------------------------------------------------------*/

    static  Trace_Manager   *m_Instance;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  contains the id of the trace writer, if the trace writer is
                alife.
     */
    /* -----------------------------------------------------------------------*/

    tsp00_TaskId    m_TraceWriterTaskId;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Is the number of UKTs using the kernel trace
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Int4  m_NumUKTs;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Indicates if the kernel trace will be cleared at this moment
     */
    /* -----------------------------------------------------------------------*/

    volatile SAPDB_Bool m_bDoClearTrace;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Indicates if the kernel trace will be flushed at this moment
     */
    /* -----------------------------------------------------------------------*/

    volatile SAPDB_Bool m_bFlushTraceActive; // PTS 1112933 TS 2001-12-04

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Queue for the tasks waiting for a kernel trace flush
     */
    /* -----------------------------------------------------------------------*/

    Kernel_TaskWaitList    m_UserWaitFlushQueue;  // use: Kernel_TaskWaitList UH 2003-02-26 // PTS 1112933 TS 2001-12-04

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Collection of UKT specific trace areas. Each slot will be
                used only by the tasks of one UKT and therefore no syncronization
                is needed.
     */
    /* -----------------------------------------------------------------------*/

    Trace_SlotPool  m_SlotPool;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Trace Volume handler  
     */
    /* -----------------------------------------------------------------------*/

    Trace_DevSpace  m_DevSpace;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Number generator for the kernel trace entries to guarantee
                the uniqueness of the entries.
     */
    /* -----------------------------------------------------------------------*/

    RTESync_InterlockedCounter<Trace_SeqNo> m_SeqNoGenerator;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Name of the kernel dump file
     */
    /* -----------------------------------------------------------------------*/

    tsp00_VFilename m_DumpFile;
};


#endif  /* TRACE__MANAGER_HPP */
