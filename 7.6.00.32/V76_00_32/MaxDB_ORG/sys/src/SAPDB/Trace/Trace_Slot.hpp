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



#ifndef TRACE_SLOT_HPP
#define TRACE_SLOT_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "Trace/Trace_DevSpace.hpp"
#include "Trace/Trace_Page.hpp"
#include "Trace/Trace_Types.hpp"

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
  @class    Trace_Slot
  @brief    This class is used to  handle the UKT specific kernel trace. 
            Note that only one task of the UKT is able to write into the UKT 
            specific trace slot and therefore no synchronization is needed, 
            except the trace slot flush!
 */

class Trace_Slot
{
    /// Declare the Trace_SlotPoolIterator to a friend

    friend class Trace_SlotAccess;

public:

    /*!
       @brief   Constructor. Initialize member data.
       @return  none
     */

    Trace_Slot()
    :
    m_bSlotWillBeFlushed( SAPDB_FALSE ),
    m_UKTId( 0 ),
    m_NumPages( 0 ),
    m_UsableSize( 0 ),
    m_PageListOffset( 0 ),
    m_PageNo( TRACE_FIRST_PNO ),
    m_pPageList( NULL )
    {}

    /*!
       @brief   Initialize
       @param   UKTId [in] logical id of the UKT. This id is stable as long as
                           the task is within the Trace_Slot context.
       @return  none
     */

    SAPDB_Bool Initialize( const SAPDB_Int4 UKTId );

    /*!
       @brief   This method is used to flush the trace slot into the kernel 
                trace file.
       @param   TaskId   [in] current task id
       @param   DevSpace [in] trace volume
       @return  none
     */

    void Flush(
        const tsp00_TaskId  TaskId,
        Trace_DevSpace      &DevSpace );

    /*!
       @brief   This method is used to write a trace entry smaller than 132
                bytes into the trace slot. Trace entries smaller than 0 and
                and greater than 132 won't be inserted. No errors are reported.
       @param   trans          [in] transaction context
       @param   Entry          [in] trace entry handler for plain entries
       @param   SeqNoGenerator [in] number generator to get unique trace slot 
                                    spanned id's for the trace entries.
       @return  none
     */

    void Insert(
        Trans_Context                           &trans,
        Trace_IBaseEntry                        &Entry,
        RTESync_InterlockedCounter<Trace_SeqNo> &SeqNoGenerator );

    /*!
       @brief   This method is used to write a large trace entry into the trace 
                slot. Trace entries smaller than 0 won't be inserted. No errors 
                are reported. The trace entry will be splitted into separate parts.
                Each part has the layout of an ordinary trace entry, but inside
                the info part it contains a internal header. This internal header
                is used by the diagnose tool the combine the splitted parts to one
                trace entry.
       @param   trans          [in] transaction context
       @param   Entry          [in] trace entry handler for plain entries
       @param   SeqNoGenerator [in] number generator to get unique trace slot 
                                    spanned id's for the trace entries.
       @return  none
     */

    void InsertLarge(
        Trans_Context                           &trans,
        Trace_ILargeEntry                       &Entry,
        RTESync_InterlockedCounter<Trace_SeqNo> &SeqNoGenerator );

    /*!
       @brief   This method is used to re-initialze the trace slot, i.e
                the trace slot will be empty.
       @param   taskId [in] task identification
       @return  none
     */

    void ReInitialization( const tsp00_TaskId    taskId );

private:

    /*!
       @brief   This method is used to write a trace entry into the trace slot.
       @param   FullEntry   [in] trace entry contains user given trace entry
                                 and persistent header information of trace entry
                                 including the unique id.
       @return  none
     */

    void InsertIntoTracePage( Trace_IFullEntry &FullEntry );

private:

    /*!
        @brief  This flag is used to indicate if the trace slot will be flush
                at this moment. Note that the flush command could be executed
                by a task from another UKT! 
     */

    volatile SAPDB_Bool  m_bSlotWillBeFlushed;

    /*!
        @brief  UKT id to which the current trace slot belongs.
     */

    SAPDB_Int4  m_UKTId;

    /*!
        @brief  Number of trace frames belonging to the trace slot.
     */

    SAPDB_Int4  m_NumPages;

    /*!
        @brief  Size in bytes of all trace frames of the  trace slot
     */

    SAPDB_Int4  m_UsableSize;

    /*!
        @brief  Index within m_pPageList to detect the current trace page frame.
     */

    SAPDB_Int4  m_PageListOffset;

    /*!
        @brief  Current logical trace page number.
     */

    Trace_PageNo    m_PageNo;

    /*!
        @brief  Pointer to the list of trace page frames.
     */

    tsp00_PageAddr  m_pPageList;

    /*!
        @brief  Trace page handler to be used for the next trace entry
     */

    Trace_Page  m_Page;
};


/*===========================================================================*
 *  IMPLEMENTATION OF METHODS                                                *
 *===========================================================================*/

#endif  /* TRACE_SLOT_HPP */
