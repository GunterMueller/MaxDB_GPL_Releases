/*!================================================================

  module:       vpr03Packet.h

  responsible:  BurkhardD

  special area: Auftragsinterface


  description:  Header fuer Verwaltung des Auftragsinterfaces

  see also:

  change history:

        2001-05-17 D025086 Added pr03PacketReqRec and pr03PacketHandleComError
 -------------------------------------------------------------------





    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




 ===================================================================*/
#ifndef __VPR03_PACKET_H__
#define __VPR03_PACKET_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr01.h"
#include "vpr01Con.h"
#include "gpr03.h"


/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#define PR03PACKET_UNKNOWN_VERSION "00000"

typedef  char tpr03_KernelVersionC[sizeof(PR03PACKET_UNKNOWN_VERSION)];

typedef enum tpr03_KernelParam_Enum {
UnicodeCatalog_epr03,
KernelVersionString_epr03,
KnlSessionID_epr03,
KernelFeatures_epr03
} tpr03_KernelParam_Enum;

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

externC BOOLEAN pr03PacketGetKernelParameters( tsp1_segment *segm_ptr,
					       tpr03_KernelParam_Enum fOption,
					       void *pKernelParam );

externC BOOLEAN pr03PacketSetEncoding( tsp1_packet    *pck, tpr05_StringEncoding encodingType );
externC BOOLEAN pr03PacketSetSwapKind(tsp1_packet *pck, tsp00_SwapKind_Enum swapKind);
externC tsp00_SwapKind_Enum pr03PacketGetSwapKind( tsp1_packet *pck );

externC tpr05_StringEncoding pr03PacketGetEncoding( tsp1_packet    *pck);

externC tsp1_packet* pr03PacketInit( tpr01_ConDesc *ConDesc,
                                     tsp1_cmd_mess_type MessType);

/*!
  Function:     pr03PacketNewPacket

  see also:

  Description:  Allocates and intits a pseudopacket with a cbSize Bytes size.
  This packet can't used to comunicate with the DB kernel.

  Arguments:
    cbSize   [in] Number of Bytes for the packet.

  return value: tsp1_packet* pointer to the new allocated packet.
 */

externC tsp1_packet *pr03PacketNewPacket(tsp00_Int4 cbSize);

/*!
  Function:     pr03PacketDeletePacket

  see also:

  Description:  Destroy a pseudopacket.

  Arguments:
    pPacket  [in] pointer to the pseudopacket.

  return value: none.
 */

externC void pr03PacketDeletePacket(tsp1_packet *pPacket);
externC void pr03PacketReqRec(tpr01_ConDesc *ConDesc, struct SQLERROR *sqlemp);
externC void pr03PacketHandleComError(tpr01_ConDesc *ConDesc);

#endif
