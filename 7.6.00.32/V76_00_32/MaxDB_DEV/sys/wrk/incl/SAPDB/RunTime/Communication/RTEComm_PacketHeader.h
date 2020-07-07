/*!**************************************************************************

  module      : RTEComm_PacketHeader.h

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: communication base classes
  description : This module contains the common communication (base) classes.


  last changed: 2002-11-15  16:20
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


#ifndef RTECOMM_PACKETHEADER_H
#define RTECOMM_PACKETHEADER_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define     RTE_COMM_UNDEF_REF  SAPDB_MAX_UINT4

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
    Type need to select protocol in the following interfaces
*/
typedef enum { Prot_Undefined = 0,
               Prot_DBManager,
               Prot_ConsoleSM,
               Prot_ConsoleFIFO,
               Prot_DiagPipe,
               Prot_CommSeg
} RTEComm_ProtocolID;


# if defined (__cplusplus)
#include "RunTime/Communication/RTEComm_PacketHeader.hpp"
# endif

#endif  /* RTECOMM_PACKETHEADER_H */
