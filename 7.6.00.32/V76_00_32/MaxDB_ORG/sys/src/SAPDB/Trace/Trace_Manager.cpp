/*!
  @file     Trace_Manager.hpp
  @author   TorstenS
  @ingroup  Trace
  @brief    This class defines the singleton component trace manager and 
            offers the possibility to insert trace entries into the UKT 
            specific memory trace buffers. Furthermore it's possible to
            write the materialized trace entries into the kernel trace file.
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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo55k.h"   // RTE   : Vbegexcl/Vendexcl 
#include "heo56.h"    // RTE   : Vsuspend
#include "hgg01.h"    // PASCAL: Configuration
#include "hgg01_3.h"  // PASCAL: Configuration
#include "hgg08.h"    // PASCAL: RegionIds

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Tasking/RTETask_ITaskScheduler.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

#include "Trace/Trace_Manager.hpp"
#include "Trace/Trace_Writer.hpp"

#include "KernelCommon/Kernel_Task.hpp"

#include "Transaction/Trans_Context.hpp"

#include "SQLManager/SQLMan_Context.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

Trace_Manager*  Trace_Manager::m_Instance = NULL;

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
   @class   Trace_Lock
   @brief   This class is used to synchronize concurrent access to the shared
            data of the Trace_Manager. To synchronize the access the g08trace
            region is used.
 */
/* -----------------------------------------------------------------------*/

class Trace_Lock
{
public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   The constructor is used to get the lock.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    Trace_Lock( const tsp00_TaskId TaskId ):m_Owner( TaskId )
    {
        vbegexcl( m_Owner, g08trace );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   The destructor will release the lock.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    ~Trace_Lock()
    {
        vendexcl( m_Owner, g08trace );
    }

private:

    /* -----------------------------------------------------------------------*/
    /*!
    @brief: Contains the taskId of the lock holder.
    */
    /* -----------------------------------------------------------------------*/

    tsp00_TaskId    m_Owner;
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Singleton interface                                                       */
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Trace_Manager::CreateInstance( const tsp00_TaskId TaskId )
{
    if ( NULL == m_Instance )
    {
        SAPDB_Int4  NumUKTs = g01NumberOfUKTs();

        m_Instance = new( RTEMem_Allocator::Instance() ) Trace_Manager( NumUKTs );

        if (NULL == m_Instance ) return( SAPDB_FALSE );

        if (! m_Instance->Initialize( TaskId )) return( SAPDB_FALSE );
    }
    return( true );
};

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Trace_Manager::Initialize( const tsp00_TaskId TaskId )
{

    m_DevSpace.Initialize( m_DumpFile );

    if (! m_DevSpace.Open( TaskId )) return( SAPDB_FALSE );

    if (! m_DevSpace.WriteInfoPage( TaskId )) return( SAPDB_FALSE );

    return( m_SlotPool.Initialize( m_NumUKTs ) );
};

/*---------------------------------------------------------------------------*/

void
Trace_Manager::Flush( const tsp00_TaskId TaskId )
{

    if ( ! IsTraceWriterAlife() )
        return;

    Trace_SlotPoolIterator  SlotIter( m_SlotPool );

    // Flush memory trace into knltrace file
    for( SlotIter.Begin(); !SlotIter.End(); ++SlotIter )
    {
        if( SAPDB_TRUE ==  m_bDoClearTrace )
            ( *SlotIter ).ReInitialization( TaskId );

        ( *SlotIter ).Flush( TaskId, m_DevSpace );
    }
    {
        Trace_Lock  Lock( TaskId );

        // Resume user task(s) waiting for trace flush
        m_UserWaitFlushQueue.ResumeAll(); // UH 2003-02-26
        // One requested trace flush is executed
        m_bFlushTraceActive = SAPDB_FALSE; // PTS 1112933 TS 2001-12-04
        m_bDoClearTrace     = SAPDB_FALSE;
    }
};

/*---------------------------------------------------------------------------*/

void
Trace_Manager::Insert( Trace_IBaseEntry &Entry )
{
    Trans_Context* pTrans;
    
    if( ! GetTransContext( pTrans ))
        return;

    if (! WriteEntryPossible( *pTrans, Entry.GetSize() ))
        return;

    // This method is not able to materialize trace entries greater than 132 bytes
    if ( Entry.MaximumSize() < Entry.GetSize() )
        return;

    // Get the RTE internal number of the UKT in which the current task is running.
    const RTE_TaskSchedulerId   ownUKTId = pTrans->Task().ITaskScheduler().ID();

    m_SlotPool.Get( ownUKTId ).Insert( *pTrans, Entry, m_SeqNoGenerator );
};

/*---------------------------------------------------------------------------*/

void
Trace_Manager::InsertLarge( Trace_ILargeEntry   &Entry )
{
    Trans_Context* pTrans;
    
    if( ! GetTransContext( pTrans ))
        return;

    if (! WriteEntryPossible( *pTrans, Entry.GetSize() ))
        return;

    // Get the RTE internal number of the UKT in which the current task is running.
    const RTE_TaskSchedulerId   ownUKTId = pTrans->Task().ITaskScheduler().ID();

    m_SlotPool.Get( ownUKTId ).InsertLarge( *pTrans, Entry, m_SeqNoGenerator );
};

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Trace_Manager::TriggerFlush( const tsp00_TaskId taskId )
{
    // Only called by the trace writer task
    SAPDBERR_ASSERT_STATE( taskId == m_TraceWriterTaskId );

    {
        Trace_Lock  lock( taskId );

        if( SAPDB_TRUE == m_bFlushTraceActive )
            return( SAPDB_FALSE ); // flush trace is already triggered
        else
        {
            m_bFlushTraceActive = SAPDB_TRUE;
            return( SAPDB_TRUE ); // wake up trace writer to flush trace
        }
    }
}

/*---------------------------------------------------------------------------*/

void
Trace_Manager::PrepareAndWaitForFlush(
    const tsp00_TaskId TaskId,
    const SAPDB_Bool   bReInitialization )
{
    tgg00_WaitContext waitcontext;

    if ( ! IsTraceWriterAlife() )
        return;
    {
        Trace_Lock  Lock( TaskId );

        // Resume the trace writer to flush UKT specific trace slots
        if ( SAPDB_FALSE == m_bFlushTraceActive ) // PTS 1112933 TS 2001-12-04
        {
            m_bFlushTraceActive = SAPDB_TRUE;

            if( SAPDB_TRUE == bReInitialization )
            {
                m_bDoClearTrace = SAPDB_TRUE;
                // Reset the sequence number generater for the trace entries
                m_SeqNoGenerator.SetValue( TRACE_FIRST_SEQNO );
            }

            Trace_Writer::GetInstance().Resume();
        }
        // Store user taskid to enable wake up after the
        // trace writer has flushed the complete trace.
        m_UserWaitFlushQueue.InsertAsFirst( TaskId, // PTS 1112933 TS 2001-12-04
                                            waitcontext ); // UH 2003-02-26
    }
    vsuspend (TaskId, 51);
};

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Trace_Manager::WriteEntryPossible(
    Trans_Context       &Trans,
    const SAPDB_Int4    EntrySize ) const
{
    if(( ! IsTraceWriterAlife()) || ( SAPDB_TRUE == m_bDoClearTrace ))
        return( SAPDB_FALSE );

    // if session specific trace is enabled than only the specified session
    // has the right to write into the kernel trace.
    if (
        (! g01vtrace.vtrSession_gg00.gg90IsNil() )
        &&
        ( g01vtrace.vtrSession_gg00 != Trans.SessionId())
        &&
        (! Trans.IsNilSession())
    )
        return( SAPDB_FALSE );

    if  ( 0 >= EntrySize )
        return( SAPDB_FALSE );

    return( SAPDB_TRUE );
};

/*---------------------------------------------------------------------------*/

bool
Trace_Manager::GetTransContext( Trans_Context* &pTrans ) const
{
    RTETask_ITask& task           = *RTETask_ITask::Self();
    SQLMan_Context *pSqlContext   = Kernel_Task::Instance().GetSQLManContext( task );

    if ( 0 != pSqlContext ){
        pTrans = &(Trans_Context::GetContext( pSqlContext->TransContext()));
        return true;
    }

    Trans_Context *pTransContext = Kernel_Task::Instance().GetTransContext( task );

    if ( 0 != pTransContext ){
        pTrans = pTransContext;
        return true;
    }

    // SQLMan_Context or Trans_Context must be given to the
    // kernel local storage in Kernel_Task
    // But during restart CreateTransContext allocates memory. At this time
    // no transcontext is given, but a trace_allocator would try to write
    // a trace-entry. This ends in an error, which causes the writing of a trace_entry
    // ....... - so allow also the case that there is no transaction given
    // SAPDBERR_ASSERT_STATE( 0 != pTransContext );
    return false; // do not write trace!
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

externC SAPDB_Bool
Trace_Flush( const tsp00_TaskId taskId )
{
    return( Trace_Manager::GetInstance().TriggerFlush( taskId ));
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
