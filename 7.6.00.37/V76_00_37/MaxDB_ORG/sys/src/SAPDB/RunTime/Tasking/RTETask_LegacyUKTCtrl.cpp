/*!========================================================================

  @file         RTETask_LegacyUKTCtrl.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Access old UKT control

                The following class provides methodes to access 
                old UKT control.

  @since        2003-09-19  11:19
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

#include    "geo50_0.h"
#include    "RunTime/Tasking/RTETask_LegacyUKTCtrl.h"
#include    "RunTime/Tasking/RTETask_LegacyUKTCtrl.hpp"
#include    "RunTime/Tasking/RTETask_TaskScheduler.hpp"

#if defined (_WIN32)
#else
# include    "gen88.h"  /* no check */
#endif


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

RTETask_LegacyUKTCtrl*  RTETask_LegacyUKTCtrl::Self()
{
    UKT_CTRL   pUKT; 

#   if defined (_WIN32)
     pUKT = THIS_UKT_CTRL;
#   else
     pUKT = en88GetThisUkt();
#   endif

    return  (0 == pUKT) ? 0 : pUKT->pLegacyUKTCtrlObject;
}


/*---------------------------------------------------------------------------*/

externC void  UpdateCurrentTaskInTaskScheduler( UKT_CTRL   pUKT )
{
    pUKT->pTaskSchedulerObject->LoadCurrentTask();
}

/*---------------------------------------------------------------------------*/

externC SAPDB_UInt4 NumberOfCommunicationQueueElements( UKT_CTRL   pUKT )
{
    return pUKT->pTaskSchedulerObject->CommQueue().NumberOfQueueElements();
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/