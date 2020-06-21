/*!
  @file     Trace_Slot.hpp
  @author   TorstenS
  @ingroup  Trace
  @brief    Implementation of Kernel VTrace. This class is used to
            handle the UKT specific kernel trace. Note that only
            one task of the UKT is able to write into the UKT specific
            trace slot and therefore no synchronization is needed, except
            the trace slot flush!

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

#include "ggg90.h"  // CPP   : GG_kernel_templates
#include "hgg01.h"  // PASCAL: Configuration
#include "hsp77.h"

#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

#include "Trace/Trace_Slot.hpp"
#include "Trace/Trace_Entry.hpp"
#include "Trace/Trace_InfoPage.hpp"

#include "Transaction/Trans_Context.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
   @class   Trace_SlotAccess
   @brief   This class is used to prevent trace entry writing if the
            kernel trace will be flushed at this moment. Note that
            no latches are used to avoid loss of performance!
 */
/* -----------------------------------------------------------------------*/

class Trace_SlotAccess
{
public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   The constructor is used to delay trace entry materialization
                if the current trace slot is  flushed by the trace writer.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    Trace_SlotAccess(
        const tsp00_TaskId  taskId,
        const Trace_Slot    &traceSlot )
    {
        // do not materialize trace entries if the trace will be
        // flushed at this moment. This loop should prevent (in a very
        // cheap manner) that trace entries are not written during a
        // trace flush.

        while( SAPDB_TRUE == traceSlot.m_bSlotWillBeFlushed ){
            RTE_ISystem::GiveUpTimeSlice();
        }
    }
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


SAPDB_Bool
Trace_Slot::Initialize( const SAPDB_Int4 UKTId )
{
    if ( !Trace_InfoPage::GetUktTraceBuffer( UKTId, m_pPageList, m_NumPages) )
    {
        return ( SAPDB_FALSE );
    }

    m_UKTId = UKTId;

    if( 0 < m_NumPages )
    {
        m_Page.AssignNewPage( m_pPageList, m_PageNo );
        m_UsableSize = m_NumPages * m_Page.BodySize();
    }
    else
        m_UsableSize = 0;

    return( SAPDB_TRUE );
};

/*---------------------------------------------------------------------------*/

void
Trace_Slot::Flush(
    const tsp00_TaskId    TaskId,
    Trace_DevSpace        &DevSpace )
{
    if( 0 < m_UsableSize ) // PTS 1120309 TS 2003-01-24
    {
        m_bSlotWillBeFlushed = SAPDB_TRUE;

        DevSpace.Write( TaskId, m_UKTId, m_pPageList, m_NumPages );

        m_bSlotWillBeFlushed = SAPDB_FALSE;
    }
};

/*---------------------------------------------------------------------------*/

void
Trace_Slot::Insert(
    Trans_Context                            &trans,
    Trace_IBaseEntry                         &Entry,
    RTESync_InterlockedCounter<Trace_SeqNo>  &SeqNoGenerator )
{
    if ( m_UsableSize < Entry.GetSize() )
        return;

    // Get unique number for the current trace entry

    Trace_SeqNo SequenceNo = 0;
    SeqNoGenerator.Increment( 1, &SequenceNo );

    Trace_SlotAccess slotAccess( trans.Task().ID(), *this );

    Trace_KernelFullEntry   FullEntry( trans, Entry, SequenceNo );

    InsertIntoTracePage( FullEntry );
};

/*---------------------------------------------------------------------------*/

void
Trace_Slot::InsertLarge(
    Trans_Context                            &trans,
    Trace_ILargeEntry                        &Entry,
    RTESync_InterlockedCounter<Trace_SeqNo>  &SeqNoGenerator )
{
    if ( m_UsableSize < Entry.GetSize() )
        return;

    // Get unique number(s) for the current trace entry

    Trace_SeqNo SequenceNo = 0;

    SeqNoGenerator.Increment( Entry.GetNumberOfParts(), &SequenceNo );

    Trace_LargeEntryIterator Iter( Entry );

    Trace_SlotAccess slotAccess( trans.Task().ID(), *this );

    for( Iter.Begin(); !Iter.End(); ++Iter )
    {
        Trace_OverflowEntry OverflowEntry( (tgg00_Debug::fromConst( bd_oflw )),
                                           Entry.GetLayer(), Entry.GetType(), Iter.GetPart(), Iter.GetPartSize(),
                                           Iter.GetPartNo() );

        Trace_KernelFullEntry   FullEntry( trans, OverflowEntry, SequenceNo + Iter.GetPartNo() -1 );

        InsertIntoTracePage( FullEntry );
    }
};

/*---------------------------------------------------------------------------*/

void
Trace_Slot::ReInitialization( const tsp00_TaskId    taskId )
{
    if( 0 >= m_UsableSize ) // PTS 1120309 TS 2003-01-24
        return;

    m_PageListOffset = 0;
    m_PageNo         = TRACE_FIRST_PNO;


    for( SAPDB_Int4 PageCount = 0; PageCount < m_NumPages; ++PageCount)
    {
        m_Page.AssignNewPageAndFormat( (m_pPageList + PageCount), TRACE_NIL_PNO );
    };

    // Set trace page handler onto the first trace page
    m_Page.AssignNewPage( m_pPageList, m_PageNo );
};

/*---------------------------------------------------------------------------*/

void
Trace_Slot::InsertIntoTracePage( Trace_IFullEntry &FullEntry )
{
    if (! m_Page.IsSpaceAvailable( FullEntry.GetSize() ))
    {
        // Prepare next trace page to store current trace entry

        if (m_PageListOffset < ( m_NumPages-1 ))
        {
            m_Page.AssignNewPage(( m_pPageList + ( ++m_PageListOffset )), ++m_PageNo );
        }
        else
        {
            // The end of the trace buffers is reached. Next trace page is the
            // first allocated page
            m_PageListOffset = 0;
            m_Page.AssignNewPage( m_pPageList, ++m_PageNo );
        }
    }
    m_Page.Insert( FullEntry );
};

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
