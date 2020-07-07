/*!
  @file     Trace_SlotPool.hpp
  @author   TorstenS
  @ingroup  Trace
  @brief    Implementation of Kernel VTrace. This class is used
            to handle all UKT specific trace slots.
  @see            

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



#ifndef TRACE_SLOT_POOL_HPP
#define TRACE_SLOT_POOL_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"  // PASCAL: SP_basic_constants_and_types

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Trace/Trace_Slot.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
  @class    Trace_SlotPool
  @brief    This class is used to handle all UKT specific trace slots.
 */
/* -----------------------------------------------------------------------*/

class Trace_SlotPool
{
    /// Declare the Trace_SlotPoolIterator to a friend

    friend class Trace_SlotPoolIterator;

public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Constructor. Initialize member data.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    Trace_SlotPool()
    :
    m_pSlot( NULL ),
    m_FirstSlot (0 ),
    m_LastSlot( 0 )
    {
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to create all trace slot handler.
       @param   NumUKTs [in] number of UKT specific trace slots
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool Initialize( const SAPDB_Int4 NumUKTs);

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get a trace slot identified by the UKT id
       @param   UKTId [in] id of the wanted UKT trace slot.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    Trace_Slot & Get( SAPDB_Int4 UKTId ) const
    {
        return( m_pSlot[ UKTId - 1 ] );
    }

private:

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Pointer to the list of trace slot handler
     */
    /* -----------------------------------------------------------------------*/

    Trace_Slot  *m_pSlot;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Logical id of the first trace slot
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Int4  m_FirstSlot;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Logical id of the last trace slot
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Int4  m_LastSlot;
};

/* -----------------------------------------------------------------------*/
/*!
  @class    Trace_SlotPoolIterator
  @brief    This class is used to iterate over all trace slot handler
 */
/* -----------------------------------------------------------------------*/

class Trace_SlotPoolIterator
{
public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Constructor. Initialize member data.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    Trace_SlotPoolIterator( Trace_SlotPool &SlotPool )
    :
    m_CurrentSlot( 1 ),
    m_SlotPool ( SlotPool )
    {}

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to set the iterator to the next slot of the
                trace slot pool.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void operator ++()
    {
        ++m_CurrentSlot;
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to get access to the trace slot handler
                pointed by the iterator.
       @return  Trace_Slot
     */
    /* -----------------------------------------------------------------------*/

    Trace_Slot & operator *() const
    {
        return m_SlotPool.Get( m_CurrentSlot );
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to set the iterator to the first trace slot
                of the pool.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    void Begin()
    {
        m_CurrentSlot = m_SlotPool.m_FirstSlot;
    }

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to detect if the iterator has reached the 
                end of the trace slot handler collection.
       @return  SAPDB_Bool true is returned if the end has reached
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Bool End() const
    {
        return( m_CurrentSlot > m_SlotPool.m_LastSlot );
    }

private:

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Defines the current trace slot handler pointed by the iterator
     */
    /* -----------------------------------------------------------------------*/

    SAPDB_Int4      m_CurrentSlot;

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Is the handler for all trace slots.
     */
    /* -----------------------------------------------------------------------*/

    Trace_SlotPool  &m_SlotPool;
};


#endif  /* TRACE_SLOT_POOL_HPP */

/*===========================================================================*
 *  IMPLEMENTATION OF METHODS                                                *
 *===========================================================================*/

inline
SAPDB_Bool
Trace_SlotPool::Initialize( const SAPDB_Int4 NumUKTs)
{
    // create trace_slots for given UserKernelThreads via
    // placement new and call default constructor.

    newarray (m_pSlot, NumUKTs, RTEMem_Allocator::Instance());

    if ( NULL == m_pSlot )
        return( SAPDB_FALSE );

    m_FirstSlot = 1;
    m_LastSlot  = NumUKTs;

    // Initialize all trace_slots
    SAPDB_Int4              iUKT;
    Trace_SlotPoolIterator  SlotIter( *this );

    for( SlotIter.Begin(), iUKT = 1; !SlotIter.End(); ++SlotIter, ++iUKT )
    {
        if (! ( *SlotIter ).Initialize( iUKT ) )
            return( SAPDB_FALSE );
    }
    return( SAPDB_TRUE );
}

