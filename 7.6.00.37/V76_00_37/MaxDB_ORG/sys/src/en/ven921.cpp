/*!========================================================================

  @file         ven921.cpp
  @ingroup      
  @author       RaymondR

  @brief        cplusplus wrapper


  @since        2005-08-29  16:54
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

#include <ctype.h>
#include <time.h>

#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "RunTime/Tasking/RTETask_TaskScheduler.hpp"
#include    "RunTime/Tasking/RTETask_CommunicationQueue.hpp"
#include    "gen921.h"

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

externC SAPDB_ULong en921GetTaskCommandTimeOut( struct RTETask_Task*  pTaskObject )
{
    // While pTaskObject->ClientConnections() references a member object, no remapping is needed.
    return pTaskObject->ClientConnections().GetSessionTimeOut();
}

externC SAPDB_Char const * const en921GetTaskDisplayName (struct RTETask_Task*  pTaskObject,
                                                          SAPDB_Bool            longName)
{
    return ((SAPDB_Char *)(pTaskObject->TypeAsString (longName)));
}



/*!
  @brief remap pointer

  Remapping only done if pointers inside range. 
  
  This template function was needed to remap the pointers into shared memory (KGS + more) copy for UNIX console. 
  Without remapping the UNIX console is depending on mapping the console shared memory at its original
  position, which is not always possible (i.e. Solaris 64bit has such a problem...).

  @parameter pointer [in/out] pointer to check
  @parameter rangeStartAddress [in] start address of range
  @parameter rangeEndAddress [in]   end address of range
  @parameter copyStartAddress [in]  start address of copy range
  @returns none
 */
template <class targetType> void RTE_RemapPointer( targetType       * &pointer
                                                 , SAPDB_Byte const *  rangeStartAddress
                                                 , SAPDB_Byte const *  rangeEndAddress
                                                 , SAPDB_Byte       *  copyStartAddress )
{
    SAPDB_Byte const *tmpPointer = reinterpret_cast<SAPDB_Byte const *>(pointer);
    
    if ( tmpPointer >= rangeStartAddress
      && tmpPointer <  rangeEndAddress )
    {
        pointer = reinterpret_cast<targetType *>( copyStartAddress + (tmpPointer - rangeStartAddress) );
    }
}

/*==========================================================================*/

externC void en921ShowCommqueueRemapped ( struct RTETask_TaskScheduler*  pTaskSchedulerObject
                                        , SAPDB_Bool        needsRemapping
                                        , SAPDB_Byte const *rangeStartAddress
                                        , SAPDB_Byte const *rangeEndAddress
                                        , SAPDB_Byte       *copyStartAddress )
{
    SAPDB_Bool                   First     = true;
    RTETask_CommunicationQueue  &CommQueue = pTaskSchedulerObject->CommQueue();
    RTETask_CommonQueueElement*  pElement;

    for ( pElement = CommQueue.FirstInQueue(); 
          pElement; 
          pElement = CommQueue.NextInQueue( *pElement ) )
    {
        if ( needsRemapping )
        {
            // Only pointer inside ´C-structures´ have been converted, so it may be needed now
            RTE_RemapPointer( pElement, rangeStartAddress, rangeEndAddress, copyStartAddress);
        }

        if ( First )
        {
            printf ( "    com_queue\n" );
            First = false;
        }

        if ( needsRemapping )
        {
            // Only pointer inside ´C-structures´ have been converted, so it may be needed now
            DLQ_ELEMENT *pQueueElement = reinterpret_cast<DLQ_ELEMENT *>(
                pElement);

            RTE_RemapPointer( pQueueElement->pForward, rangeStartAddress, rangeEndAddress, copyStartAddress);
                     /* now CommQueue.NextInQueue( *pElement ) is save... */

            RTE_RemapPointer( pQueueElement->taskid, rangeStartAddress, rangeEndAddress, copyStartAddress);
                     /* now pElement->Task() is save... */

            // just for completeness...
            RTE_RemapPointer( pQueueElement->pQueueElementObject, rangeStartAddress, rangeEndAddress, copyStartAddress);
            RTE_RemapPointer( pQueueElement->pBackward, rangeStartAddress, rangeEndAddress, copyStartAddress);
            RTE_RemapPointer( pQueueElement->fromtaskid, rangeStartAddress, rangeEndAddress, copyStartAddress);
        }

        if ( !pElement->Task()->LegacyTaskCtrl().PrioFlag() )
        {
            printf ( "        task T%d\n", 
                     pElement->Task()->GetTaskID() );
        }
        else
        {
            printf ( "        task T%d   prio %d\n", 
                     pElement->Task()->GetTaskID(), 
                     pElement->Task()->LegacyTaskCtrl().PrioFlag() );
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/