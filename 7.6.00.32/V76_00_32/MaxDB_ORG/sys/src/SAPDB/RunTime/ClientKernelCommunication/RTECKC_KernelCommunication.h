/*!========================================================================

  @file         RTECKC_KernelCommunication.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Used to reference all global data of the 
                Kernel communication

  @since        2005-09-22  12:21
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end
  \endif
 ============================================================================*/


#ifndef RTECKC_KERNELCOMMUNICATION_H
#define RTECKC_KERNELCOMMUNICATION_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/ClientKernelCommunication/RTECKC_KernelCommunication.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

SAPDB_ULong TimeElapsedSinceLastCommandExecution(bool ignoreMyself)
{
    return RTE_KGS::Instance().KernelCommunication()->TimeElapsedSinceLastCommandExecution( ignoreMyself );
}



#endif  /* RTECKC_KERNELCOMMUNICATION_H */
