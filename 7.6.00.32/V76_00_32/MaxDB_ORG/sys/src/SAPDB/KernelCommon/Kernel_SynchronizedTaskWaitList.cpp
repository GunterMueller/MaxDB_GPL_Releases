/*!
  @file     Kernel_SynchronizedTaskWaitList.cpp
  @author   TorstenS
  @ingroup  KernelCommon
  @brief    This class can be used to collect tasks to suspend or resume them
*/

/*
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "heo56.h"  // vsuspend
#include "heo55k.h" // vbegexcl
#include "hgg08.h"  // g08KnlWaitQueue

#include "KernelCommon/Kernel_SynchronizedTaskWaitList.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  INSTANCE METHODS OF THE INTERFACE CLASSES  OF THE CONVERTER CLASS        *
 *===========================================================================*/

Kernel_SynchronizedTaskWaitList::Kernel_SynchronizedTaskWaitList(
    const SAPDB_Int2                suspendReason,
    const Kernel_IWaitListAction*   pAction )
        :
        m_SuspendReason( suspendReason ),
        m_RegionId( g08KnlWaitQueue ),
        m_pAction( pAction )
{}

/*---------------------------------------------------------------------------*/

void
Kernel_SynchronizedTaskWaitList::ResumeAllWaitingTasks( const RTE_TaskId taskId )
{
    vbegexcl( taskId, m_RegionId );

    if( m_pAction )
        m_pAction->ExecuteBeforeResume();

    m_WaitList.ResumeAll();

    vendexcl( taskId, m_RegionId );
}

/*---------------------------------------------------------------------------*/

void
Kernel_SynchronizedTaskWaitList::InsertTask(
    const RTE_TaskId    taskId,
    WaitContext         &waitContext )
{
    vbegexcl( taskId, m_RegionId );

    if( m_pAction )
        m_pAction->ExecuteBeforeSuspend( this->IsEmpty());

    m_WaitList.InsertAsFirst( taskId,waitContext );

    vendexcl( taskId, m_RegionId );
}

/*---------------------------------------------------------------------------*/

void Kernel_SynchronizedTaskWaitList::Wait( const RTE_TaskId taskId ) const
{
    vsuspend( taskId, m_SuspendReason );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
