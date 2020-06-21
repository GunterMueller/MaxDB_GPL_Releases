/*!================================================================

  module:       vpr03Packet.c

 -------------------------------------------------------------------

  responsible:  BurkhardD

  special area: Auftragsinterface

  description:

  see also:

  change history:

        2001-05-17 D025086 Added pr03PacketReqRec and pr03PacketHandleComError
        2000-08-10   9:57    Marco Paskamp   unicode enhancement
  -------------------------------------------------------------------------





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

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vpr03Packet.h"
#include "vpr01Env.h"
#include "vpr01Con.h"
#include "vpr01Trace.h"
#include "hsp26.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#define PR03PACKET_KERNELVERSION_POS (2200)
#define PR03PACKET_KERNELVERSION_LEN (sizeof(PR03PACKET_UNKNOWN_VERSION)-1)

#define PR03PACKET_KnlSessionID_POS (1)
#define PR03PACKET_KnlSessionID_LEN (4)
/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

externC BOOLEAN
pr03PacketGetKernelParameters( tsp1_segment *segm_ptr,
			       tpr03_KernelParam_Enum fOption,
			       void         *pKernelParam )
{
    tsp1_part_ptr part_ptr;
    if (segm_ptr) {
        s26find_part (segm_ptr, (char) sp1pk_session_info_returned, &part_ptr);
        if (part_ptr && pKernelParam) {
            switch(fOption) {
              case UnicodeCatalog_epr03 :{
                *((char *) pKernelParam) = (char) *part_ptr->sp1p_buf;
		        return (TRUE);
                break;
              }
              case KernelVersionString_epr03:{
	            if ( part_ptr->variant.C_1.sp1p_part_header_F.sp1p_buf_len >=
		            (PR03PACKET_KERNELVERSION_POS+PR03PACKET_KERNELVERSION_LEN)) {
		          memcpy( pKernelParam, (part_ptr->variant.C_1.sp1p_buf_F+PR03PACKET_KERNELVERSION_POS), PR03PACKET_KERNELVERSION_LEN );
		          ((char*) pKernelParam)[PR03PACKET_KERNELVERSION_LEN] = '\0';
		          return (TRUE);
	            }
	            break;
              }
              case KnlSessionID_epr03 :{
                memcpy( pKernelParam, &part_ptr->sp1p_buf[PR03PACKET_KnlSessionID_POS], PR03PACKET_KnlSessionID_LEN );
		        return (TRUE);
                break;
              }
            } /* switch */
        } /* if */
        if (fOption == KernelFeatures_epr03) {
          s26find_part (segm_ptr, (char) sp1pk_feature, &part_ptr);
          if (part_ptr && pKernelParam) {
            int argcount = part_ptr->variant.C_1.sp1p_part_header_F.sp1p_arg_count;
            tpr01Con_Feature *knl = (tpr01Con_Feature *)part_ptr->sp1p_buf;
            tpr01Con_Feature *my = pKernelParam;
            for (;argcount > 0;) {
              argcount--;
              if (knl[argcount].feature < sizeof(tpr01Con_FeatureSet)/2) {
                my[knl[argcount].feature] = knl[argcount];
              }
            }
            return(TRUE);
          }
          else 
            return(FALSE);
        } /* if */
    } /* if */
    return(FALSE);
} /* pr03PacketGetKernelParameters */

/*!
  Function:     pr03PacketInit

  see also:

  Description:  Initializes a order packet

  Arguments:
    sqlratype     [in] pointer to a sqlra
    sqlgaentry    [in] pointer to a gaentry
    tpr01_SQLDesc [in] pointer to a SQL descriptor

  return value: tsp1_packet* pointer to the packet
 */

externC tsp1_packet* pr03PacketInit( tpr01_ConDesc *ConDesc,
                                     tsp1_cmd_mess_type MessType)
{
  tsp1_packet *pck=NULL;
  M90TRACE(M90_TR_ENTRY, "pr03PacketInit", 0);
  if (ConDesc) {
    sqlratype *sqlra = ConDesc->sqlca->sqlrap;
    pck = p03cpacketinit (ConDesc->sqlxa->xaSQLDesc, sqlra, ConDesc->ga, MessType);
  }
  M90TRACE(M90_TR_EXIT, "pr03PacketInit", 0);
  M90TRACE(M90_TR_PTR, "pck", &pck);
  return(pck);
} /* pr03PacketInit */

/*!
  Function:     pr03PacketSetSwapKind

  see also:

  Description:  Puts the given local machine swap kind into packet header

  Arguments:
    pck   [in] pointer to a packet
    swapKind   Swaptype of the local machine

  return value: BOOLEAN
    <UL>
        <LI>TRUE: processing succeded
        <LI>FALSE: Error
    </UL>
 */
externC BOOLEAN
pr03PacketSetSwapKind( tsp1_packet *pck, tsp00_SwapKind_Enum swapKind )
{
  if (pck) {
    pck->sp1_header.sp1h_mess_swap = swapKind;
    return (TRUE);
  }
  return(FALSE);
}

/*!
  Function:     pr03PacketGetSwapKind

  see also:

  Description:  Gets the given local machine swap kind from the packet header

  Arguments:
    pck   [in] pointer to a packet

  return value: Swaptype of the local machine
  usually one of these values
  sw_dummy, sw_normal, sw_full_swapped, sw_part_swapped


 */

externC tsp00_SwapKind_Enum
pr03PacketGetSwapKind( tsp1_packet *pck )
{
  if (pck) {
    return pck->sp1_header.sp1h_mess_swap;
  }
  return(sw_dummy);
}


/*!
  Function:     pr03PacketSetEncoding

  see also:

  Description:  Puts the given encoding type to packet header

  Arguments:
    tsp1_packet          [in] pointer to a packet
    tpr05_StringEncoding [in] encoding type which would be set in packet header

  return value: BOOLEAN
    <UL>
        <LI>TRUE: processing succeded
        <LI>FALSE: Error
    </UL>
 */

externC BOOLEAN
pr03PacketSetEncoding( tsp1_packet    *pck,
		       tpr05_StringEncoding encoding )
{
  if (pck) {
    pck->sp1_header.sp1h_mess_code = encoding->EncodingType;
    return (TRUE);
  }
  return(FALSE);
}

/* pr03PacketSetEncoding */

/*!
  Function:     pr03PacketGetEncoding

  see also:

  Description:  Gets the encoding type which was seted in packet header

  Arguments:
    tsp1_packet   [in] pointer to a packet

  return value: tpr05_StringEncoding encoding type
 */
externC tpr05_StringEncoding
pr03PacketGetEncoding( tsp1_packet    *pck)
{
    if (pck)
    {
        if(pck->sp1_header.sp1h_mess_code == (char) csp_ascii)
        {
           return sp77encodingAscii;
        }
        if(pck->sp1_header.sp1h_mess_code == (char) csp_unicode)
        {
           return sp77encodingUCS2;
        }
        if(pck->sp1_header.sp1h_mess_code == (char) csp_unicode_swap)
        {
           return sp77encodingUCS2Swapped;
        }
    }
    return(NULL);
} /* pr03PacketGetEncoding */

externC tsp1_packet *pr03PacketNewPacket(tsp00_Int4 cbSize)
{
  tsp1_packet *pPacket;
  pPacket = pr03mAllocat(cbSize, "pPacket");
  if (pPacket)
    pPacket->sp1_header.sp1h_varpart_size = cbSize - sizeof (tsp1_packet_header);
  return pPacket;
}

externC void pr03PacketDeletePacket(tsp1_packet *pPacket)
{
  pr03mFree(pPacket, "pPacket");
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr03PacketReqRec

  see also:

  Description:    Sends and recieves a orderpacket to the kernel by the 
  given ConDesc.

  This function only also handles a comunication error wich destroys the 
  packet shared with the kernel by calling a connectiondown function

  Arguments:
    *ConDesc   [in] Descriptor of the connection.

  return value: void
 */

externC void pr03PacketReqRec(tpr01_ConDesc *ConDesc, struct SQLERROR *sqlemp)
{
  tpr01_ConContainer *Connection = ConDesc->Connection;
  sqlcatype *sqlca = Connection->sqlca;
  
  if (ConDesc->ga->gareqptr != NULL) {
    ConDesc->ga->gastatus = cpr_co_ok;
    if (pr03PacketGetEncoding(ConDesc->ga->gareqptr) != sp77encodingAscii)
      if (sqlca->sqlgap->gacmpkind != CPR_LA_ODBC) { 
        pr01TracePrintf(sqlca->sqlrap, "Encoding: UCS2");
      }
    p03creqrecpacket(sqlca->sqlrap, ConDesc->ga, sqlemp);
  }
  else {
    p03cseterror (sqlemp, cpr_reflex_notok);
  }
  pr03PacketHandleComError(ConDesc);  
  p03csqlemptosqlca (sqlca, sqlemp);
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr03PacketHandleComError

  see also:

  Description:    Checks whether a cominication error has occured and destroys
  some information from the ConDesc.

  Arguments:
    *ConDesc   [in] Descriptor of the connection.

  return value: void
 */
externC void pr03PacketHandleComError(tpr01_ConDesc *ConDesc)
{
  if (ConDesc->ga->gareference == 0) {
    tpr01_ConContainer *Connection = ConDesc->Connection;
    Connection->SegmentDelete(ConDesc);
  }
}
