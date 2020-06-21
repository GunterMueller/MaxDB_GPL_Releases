/*!**************************************************************************

  module      : RTEComm_Swapping.cpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: 
  description : 


  last changed: 2002-11-15  13:40
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Comm_Messages.hpp"
#include "RunTime/Communication/RTEComm_Swapping.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

extern SAPDBTrace_Topic Runtime_Trace; ///< global runtime trace object
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

/*---------------------------------------------------------------------------*/
/*------------------------- class RTEComm_Swapping --------------------------*/
/*---------------------------------------------------------------------------*/


SAPDB_UInt1       RTEComm_Swapping::LocalSwapType = SWAP_TYPE_UDEFINED;

/*---------------------------------------------------------------------------*/

SAPDB_UInt1 const RTEComm_Swapping::GetLocalSwapType ()
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::GetLocalSwapType", Runtime_Trace, 5);

  if ( RTEComm_Swapping::LocalSwapType == SWAP_TYPE_UDEFINED ) 
    BuildLocalSwapType ();

  return RTEComm_Swapping::LocalSwapType; 
}

/*---------------------------------------------------------------------------*/
//
// - old communication only, 6.2 - 7.2
//
SAPDB_UInt1 RTEComm_Swapping::LocalSwapTypeToOld ()
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::LocalSwapTypeToOld", Runtime_Trace, 5);

  if ( RTEComm_Swapping::LocalSwapType == SWAP_TYPE_UDEFINED ) 
    BuildLocalSwapType ();

  if ( LocalSwapType == 0 ) 
    return 4;
  if ( LocalSwapType == 7 ) 
    return 3;

  return LocalSwapType - 2;
}

/*---------------------------------------------------------------------------*/
//
// - old communication only, 6.2 - 7.2
//
SAPDB_UInt1 RTEComm_Swapping::OldSwapTypeToNew ( SAPDB_UInt1 const  OldSwapType )
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::OldSwapTypeToNew", Runtime_Trace, 5);

  return OldSwapType + 2;
}

/*---------------------------------------------------------------------------*/

RTEComm_Swapping::RTEComm_Swapping ()
                 : PeerSwapType ( SWAP_TYPE_UDEFINED )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::RTEComm_Swapping", Runtime_Trace, 5);

  if ( RTEComm_Swapping::LocalSwapType == SWAP_TYPE_UDEFINED ) 
    BuildLocalSwapType ();
}

/*---------------------------------------------------------------------------*/

RTEComm_Swapping::RTEComm_Swapping ( SAPDB_UInt1 const SwapType )
                 : PeerSwapType ( SwapType )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::RTEComm_Swapping", Runtime_Trace, 5);

  if ( RTEComm_Swapping::LocalSwapType == SWAP_TYPE_UDEFINED ) 
    BuildLocalSwapType ();
}

/*---------------------------------------------------------------------------*/

bool  RTEComm_Swapping::SetPeerSwapType ( SAPDB_UInt1 const      SwapType,
                                          SAPDBErr_MessageList   &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::RTEComm_Swapping", Runtime_Trace, 5);

  if (( SwapType < 7 ) && ( SwapType != 1 )  && ( SwapType != 6 ))
    PeerSwapType = SwapType;
  else
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_ILL_SWAP_TYPE, SAPDB_ToString(PeerSwapType) );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool  RTEComm_Swapping::SwapInt2 ( unsigned char const *         pUC,
                                   SAPDB_Int2                    &Ret,
                                   SAPDBErr_MessageList          &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapInt2", Runtime_Trace, 5);
  
  // --- allowed swap types
  //     
  // MemPos: 0 1  2 3   4 5  6 7
  //     
  //         4 3  2 1   8 7  6 5    = 0
  //         2 1  4 3   6 5  8 7    = 2
  //         1 2  3 4   5 6  7 8    = 3    // - no swap 
  //         8 7  6 5   4 3  2 1    = 4    // - full swap
  //         7 8  5 6   3 4  1 2    = 5
  //         5 6  7 8   1 2  3 4    = 7
  //
  //
  switch ( PeerSwapType )
  {
  case 3:
  case 5:
  case 7:
    // --- no swap: hi-lo
    Ret = ( (SAPDB_Int2)(pUC[ 0 ]) << 8 ) | 
          ( (SAPDB_Int2)(pUC[ 1 ]) << 0 );
    break;
  case 0:
  case 2:
  case 4:
    // --- full swap: lo-hi
    Ret = ( (SAPDB_Int2)(pUC[ 0 ]) << 0 ) | 
          ( (SAPDB_Int2)(pUC[ 1 ]) << 8 );
    break;
  default:
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_ILL_SWAP_TYPE, SAPDB_ToString(PeerSwapType) );
    return false;
  }
  
  return true;
}

/*---------------------------------------------------------------------------*/

bool  RTEComm_Swapping::SwapInt4 ( unsigned char const *         pUC,
                                   SAPDB_Int4                    &Ret,
                                   SAPDBErr_MessageList          &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapInt4", Runtime_Trace, 5);

  // --- allowed swap types
  //     
  // MemPos: 0 1  2 3   4 5  6 7
  //     
  //         4 3  2 1   8 7  6 5    = 0
  //         2 1  4 3   6 5  8 7    = 2
  //         1 2  3 4   5 6  7 8    = 3    // - no swap 
  //         8 7  6 5   4 3  2 1    = 4    // - full swap
  //         7 8  5 6   3 4  1 2    = 5
  //         5 6  7 8   1 2  3 4    = 7
  //
  //
  switch ( PeerSwapType )
  {
  case 3:
  case 7:
    // --- no swap
    Ret = ( (SAPDB_Int4)(pUC[ 0 ]) << 24 ) |
          ( (SAPDB_Int4)(pUC[ 1 ]) << 16 ) |
          ( (SAPDB_Int4)(pUC[ 2 ]) <<  8 ) |
          ( (SAPDB_Int4)(pUC[ 3 ]) <<  0 );
    break;
  case 0:
  case 4:
    // --- full swap
    Ret = ( (SAPDB_Int4)(pUC[ 0 ]) <<  0 ) |
          ( (SAPDB_Int4)(pUC[ 1 ]) <<  8 ) |
          ( (SAPDB_Int4)(pUC[ 2 ]) << 16 ) |
          ( (SAPDB_Int4)(pUC[ 3 ]) << 24 );
    break;
  case 5:
    Ret = ( (SAPDB_Int4)(pUC[ 0 ]) <<  8 ) |
          ( (SAPDB_Int4)(pUC[ 1 ]) <<  0 ) |
          ( (SAPDB_Int4)(pUC[ 2 ]) << 24 ) |
          ( (SAPDB_Int4)(pUC[ 3 ]) << 16 );
    break;

  case 2:
    Ret = ( (SAPDB_Int4)(pUC[ 0 ]) << 16 ) |
          ( (SAPDB_Int4)(pUC[ 1 ]) << 24 ) |
          ( (SAPDB_Int4)(pUC[ 2 ]) <<  0 ) |
          ( (SAPDB_Int4)(pUC[ 3 ]) <<  8 );
    break;

  default:
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_ILL_SWAP_TYPE, SAPDB_ToString(PeerSwapType) );
    return false;
  }
  
  return true;
}

/*------------------------------*/

bool  RTEComm_Swapping::SwapInt8 ( unsigned char const *         pUC,
                                   SAPDB_Int8                    &Ret,
                                   SAPDBErr_MessageList          &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapInt8", Runtime_Trace, 5);
  
  // --- allowed swap types
  // 
  // MemPos: 0 1  2 3   4 5  6 7
  //     
  //         4 3  2 1   8 7  6 5    = 0
  //         2 1  4 3   6 5  8 7    = 2
  //         1 2  3 4   5 6  7 8    = 3    // - no swap 
  //         8 7  6 5   4 3  2 1    = 4    // - full swap
  //         7 8  5 6   3 4  1 2    = 5
  //         5 6  7 8   1 2  3 4    = 7
  //
  //
  switch ( PeerSwapType )
  {
  case 7:
    Ret = ( (SAPDB_Int8)(pUC[ 0 ]) << 24 ) |
          ( (SAPDB_Int8)(pUC[ 1 ]) << 16 ) |
          ( (SAPDB_Int8)(pUC[ 2 ]) <<  8 ) |
          ( (SAPDB_Int8)(pUC[ 3 ]) <<  0 ) |
          ( (SAPDB_Int8)(pUC[ 4 ]) << 56 ) |
          ( (SAPDB_Int8)(pUC[ 5 ]) << 48 ) |
          ( (SAPDB_Int8)(pUC[ 6 ]) << 40 ) |
          ( (SAPDB_Int8)(pUC[ 7 ]) << 32 );
    break;

  case 5:
    Ret = ( (SAPDB_Int8)(pUC[ 0 ]) <<  8 ) |
          ( (SAPDB_Int8)(pUC[ 1 ]) <<  0 ) |
          ( (SAPDB_Int8)(pUC[ 2 ]) << 24 ) |
          ( (SAPDB_Int8)(pUC[ 3 ]) << 16 ) |
          ( (SAPDB_Int8)(pUC[ 4 ]) << 40 ) |
          ( (SAPDB_Int8)(pUC[ 5 ]) << 32 ) |
          ( (SAPDB_Int8)(pUC[ 6 ]) << 56 ) |
          ( (SAPDB_Int8)(pUC[ 7 ]) << 48 );
    break;

  case 4:
    // --- full swap
    Ret = ( (SAPDB_Int8)(pUC[ 0 ]) <<  0 ) |
          ( (SAPDB_Int8)(pUC[ 1 ]) <<  8 ) |
          ( (SAPDB_Int8)(pUC[ 2 ]) << 16 ) |
          ( (SAPDB_Int8)(pUC[ 3 ]) << 24 ) |
          ( (SAPDB_Int8)(pUC[ 4 ]) << 32 ) |
          ( (SAPDB_Int8)(pUC[ 5 ]) << 40 ) |
          ( (SAPDB_Int8)(pUC[ 6 ]) << 48 ) |
          ( (SAPDB_Int8)(pUC[ 7 ]) << 56 );
    break;

  case 3:
    // --- no swap
    Ret = ( (SAPDB_Int8)(pUC[ 0 ]) << 56 ) |
          ( (SAPDB_Int8)(pUC[ 1 ]) << 48 ) |
          ( (SAPDB_Int8)(pUC[ 2 ]) << 40 ) |
          ( (SAPDB_Int8)(pUC[ 3 ]) << 32 ) |
          ( (SAPDB_Int8)(pUC[ 4 ]) << 24 ) |
          ( (SAPDB_Int8)(pUC[ 5 ]) << 16 ) |
          ( (SAPDB_Int8)(pUC[ 6 ]) <<  8 ) |
          ( (SAPDB_Int8)(pUC[ 7 ]) <<  0 );
    break;

  case 2:
    Ret = ( (SAPDB_Int8)(pUC[ 0 ]) << 48 ) |
          ( (SAPDB_Int8)(pUC[ 1 ]) << 56 ) |
          ( (SAPDB_Int8)(pUC[ 2 ]) << 32 ) |
          ( (SAPDB_Int8)(pUC[ 3 ]) << 40 ) |
          ( (SAPDB_Int8)(pUC[ 4 ]) << 16 ) |
          ( (SAPDB_Int8)(pUC[ 5 ]) << 24 ) |
          ( (SAPDB_Int8)(pUC[ 6 ]) <<  0 ) |
          ( (SAPDB_Int8)(pUC[ 7 ]) <<  8 );
    break;

  case 0:
    Ret = ( (SAPDB_Int8)(pUC[ 0 ]) << 32 ) |
          ( (SAPDB_Int8)(pUC[ 1 ]) << 40 ) |
          ( (SAPDB_Int8)(pUC[ 2 ]) << 48 ) |
          ( (SAPDB_Int8)(pUC[ 3 ]) << 56 ) |
          ( (SAPDB_Int8)(pUC[ 4 ]) <<  0 ) |
          ( (SAPDB_Int8)(pUC[ 5 ]) <<  8 ) |
          ( (SAPDB_Int8)(pUC[ 6 ]) << 16 ) |
          ( (SAPDB_Int8)(pUC[ 7 ]) << 24 );
    break;

  default:
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_ILL_SWAP_TYPE, SAPDB_ToString(PeerSwapType) );
    return false;
  }

  return true;
}


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/


void RTEComm_Swapping::BuildLocalSwapType ()
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::LocalSwapType", Runtime_Trace, 5);

  union { SAPDB_Int8         int8;
          char               c8[8]; }  SwapCheck;
          SAPDB_UInt1                  SwapType;
  
  // --- set int8 to 0x100000000
  SwapCheck.int8   = 0x01;
  SwapCheck.int8 <<= 32;
  
  for ( SwapType = 0;
        ( SwapType < 8 ) && ( SwapCheck.c8[ SwapType ] != 1 ); 
        SwapType++ ) { ; }
  
  RTEComm_Swapping::LocalSwapType = SwapType;

  return;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/