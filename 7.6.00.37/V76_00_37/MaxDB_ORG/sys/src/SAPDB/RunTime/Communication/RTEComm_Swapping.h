/*!**************************************************************************

  module      : RTEComm_Swapping.h

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: byte swapping 
  description : byte swapping 


  last changed: 2002-11-15  13:55
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


*****************************************************************************/


#ifndef RTECOMM_SWAPPING_H
#define RTECOMM_SWAPPING_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_MessageList.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define SWAP_TYPE_UDEFINED        0xFF

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


# if defined (__cplusplus)
#include "RunTime/Communication/RTEComm_Swapping.hpp"
#endif

#endif  /* RTECOMM_SWAPPING_H */
