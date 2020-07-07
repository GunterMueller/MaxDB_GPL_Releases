/*!
  @file           veo44.cpp
  @author         RaymondR
  @special area   communication
  @brief          This module contains the communication request and 
              response classes
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "geo42.h"
#include "geo44.h"
#include "geo47.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


// --- VARPART Values ---
//
//  4                             5
//  1 2 3 4 5 6 7 8 9 A B C D E F 0 1 2 3 4 5 6 7 8 9 A
//  A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
//
// - Lower case characters are reserved for application specific
//   varpart values!
#define VARPART_VAL_MAX_TRANSFER_BUFFER_SIZE_EO44  0x4D   /* = M */
#define VARPART_VAL_TRANSFER_BUFFER_SIZE_EO44      0x54   /* = T */
#define VARPART_VAL_MIN_RESPONSE_PACKET_SIZE_EO44  0x55   /* = U */



// --- Use the following define for nested private structs or unions declared in classes.
//     It's a workaround for the AIX scope error "private member ... cannot be accessed."
#if defined (_WIN32)    
# define  PRIVATE_NESTED_STRUCTS private
#else
# define  PRIVATE_NESTED_STRUCTS public
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

 


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                        class teo44_QueryTransferBufferSizeReq                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


bool teo44_QueryTransferBufferSizeReq::eo42_FillVarPart( teo42_RTEReqRespVarPart   &VarPart,
                                                         teo200_EventList * const  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo44_QueryTransferBufferSizeReq::eo42_FillVarPart"));

  bool Ok = true;

  if ( m_Params.MaxTransferBufferSize_eo44 != (tsp00_Uint4) -1 )
  {
    Ok = VarPart.eo42_PutValue( VARPART_VAL_MAX_TRANSFER_BUFFER_SIZE_EO44, 
                                m_Params.MaxTransferBufferSize_eo44,
                                pEventList );
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool  teo44_QueryTransferBufferSizeReq::eo42_ExtractVarPart( teo42_RTEReqRespVarPart const  &VarPart,
                                                             teo200_EventList * const       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo44_QueryTransferBufferSizeReq::eo42_ExtractVarPart"));
  
  return VarPart.eo42_GetValue( VARPART_VAL_MAX_TRANSFER_BUFFER_SIZE_EO44, 
                                m_Params.MaxTransferBufferSize_eo44,
                                true, pEventList );
}


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*              class teo44_QueryTransferBufferSizeResp                      */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

bool teo44_QueryTransferBufferSizeResp::eo42_FillVarPart( teo42_RTEReqRespVarPart   &VarPart,
                                                          teo200_EventList * const  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo44_QueryTransferBufferSizeResp::eo42_FillVarPart"));

  bool Ok = VarPart.eo42_PutValue( VARPART_VAL_TRANSFER_BUFFER_SIZE_EO44, 
                                   m_Params.TransferBufferSize_eo44,
                                   pEventList );

  if ( ( Ok ) && ( m_Params.MinResponsePacketSize_eo44 != 0 ))
  {
    Ok = VarPart.eo42_PutValue( VARPART_VAL_MIN_RESPONSE_PACKET_SIZE_EO44, 
                                m_Params.MinResponsePacketSize_eo44,
                                pEventList );
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool  teo44_QueryTransferBufferSizeResp::eo42_ExtractVarPart( teo42_RTEReqRespVarPart const  &VarPart,
                                                              teo200_EventList * const       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo44_QueryTransferBufferSizeResp::eo42_ExtractVarPart"));
  
  bool      Ok = VarPart.eo42_GetValue( VARPART_VAL_TRANSFER_BUFFER_SIZE_EO44,  
                                        m_Params.TransferBufferSize_eo44,
                                        false, pEventList );
  if ( Ok ) Ok = VarPart.eo42_GetValue( VARPART_VAL_MIN_RESPONSE_PACKET_SIZE_EO44, 
                                        m_Params.MinResponsePacketSize_eo44,
                                        true, pEventList );
  return Ok;
}


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                   class teo44_SetTransferBufferSizeReq                    */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


bool teo44_SetTransferBufferSizeReq::eo42_FillVarPart( teo42_RTEReqRespVarPart  &VarPart,
                                                       teo200_EventList * const pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo44_SetTransferBufferSizeReq::eo42_FillVarPart"));

  bool Ok = VarPart.eo42_PutValue( VARPART_VAL_TRANSFER_BUFFER_SIZE_EO44, 
                                   m_Params.TransferBufferSize_eo44,
                                   pEventList );
  return Ok;
}

/*---------------------------------------------------------------------------*/

bool  teo44_SetTransferBufferSizeReq::eo42_ExtractVarPart( teo42_RTEReqRespVarPart const  &VarPart,
                                                           teo200_EventList * const       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo44_SetTransferBufferSizeReq::eo42_ExtractVarPart"));
  
  bool Ok = VarPart.eo42_GetValue( VARPART_VAL_TRANSFER_BUFFER_SIZE_EO44, 
                                   m_Params.TransferBufferSize_eo44,
                                   false, pEventList );
  return Ok;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/