/*!========================================================================

  @file         RTECKC_KernelCommunicationSegment.cpp
  @ingroup      Client Kernel Communication
  @author       StefanP

  @brief        Communication Segment for the Client Kernel Communication

  @since        2003-10-30  12:40
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

#include    "RunTime/ClientKernelCommunication/RTECKC_KernelCommunicationSegment.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"


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

void           RTECKC_KernelCommunicationSegment::SetActualTaskSchedulerSemaphoreIdx    (SAPDB_UInt4    idx)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_KernelCommunicationSegment::SetActualTaskSchedulerSemaphoreIdx", CKC_Trace, 9);

    GetLoadBalancingOption ()->SetTaskSchedulerSemaphoreIdx (idx);
}
