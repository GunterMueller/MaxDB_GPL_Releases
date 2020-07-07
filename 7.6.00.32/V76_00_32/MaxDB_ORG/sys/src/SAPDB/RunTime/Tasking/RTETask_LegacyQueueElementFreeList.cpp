/*!========================================================================

  @file         RTETask_LegacyQueueElementFreeList.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Access to the dobly linked queue element free list for 
                legacy code

  @since        2003-10-01  9:36
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_KGS.hpp"
#include    "geo50_0.h"
#include    "RunTime/Tasking/RTETask_CommonQueue.hpp"
#include    "RunTime/Tasking/RTETask_CommonQueueElement.hpp"
#include    "RunTime/Tasking/RTETask_LegacyQueueElementFreeList.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

externC DLQ_ELEMENT* GetElementFromFreeList()
{
    static RTETask_ICommonQueue* pFreeList = 0;  // shortcut to free list

    if ( 0 == pFreeList )
        pFreeList = RTE_KGS::Instance().GlobalFreeList();

    return *pFreeList->Dequeue();
}


/*---------------------------------------------------------------------------*/

externC void PutElementToFreeList( DLQ_ELEMENT* pQueueElement )
{
    // it's easy to release an element
    pQueueElement->pQueueElementObject->Release();
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/