/*!
  @file           veo41.cpp
  @author         RaymondR
  @special area   logging ...
  @brief          description ...
  @see            example.html ...

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

#include "geo200.h"
#include "geo007_1.h"
#include "gsp00.h"
#include "geo41.h"


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
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*---------------------------- class teo41_Swap -----------------------------*/
/*---------------------------------------------------------------------------*/


teo00_Uint1       teo41_Swap::LocalSwapType_eo41 = SWAP_TYPE_UDEFINED_EO41;

/*---------------------------------------------------------------------------*/

teo00_Uint1 const teo41_Swap::eo41_GetLocalSwapType ()
{ 
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_GetLocalSwapType"));

  if ( teo41_Swap::LocalSwapType_eo41 == SWAP_TYPE_UDEFINED_EO41 ) 
    eo41_BuildLocalSwapType ();

  return teo41_Swap::LocalSwapType_eo41; 
}

/*---------------------------------------------------------------------------*/
//
// - old communication only, 6.2 - 7.2
//
teo00_Uint1 teo41_Swap::eo41_LocalSwapTypeToOld ()
{ 
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_LocalSwapTypeToOld"));

  if ( teo41_Swap::LocalSwapType_eo41 == SWAP_TYPE_UDEFINED_EO41 ) 
    eo41_BuildLocalSwapType ();

  if ( LocalSwapType_eo41 == 0 ) 
    return 4;
  if ( LocalSwapType_eo41 == 7 ) 
    return 3;

  return LocalSwapType_eo41 - 2;
}

/*---------------------------------------------------------------------------*/
//
// - old communication only, 6.2 - 7.2
//
teo00_Uint1 teo41_Swap::eo41_OldSwapTypeToNew ( tsp00_Uint1 const  OldSwapType )
{ 
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_OldSwapTypeToNew"));

  return OldSwapType + 2;
}

/*---------------------------------------------------------------------------*/

teo41_Swap::teo41_Swap ()
           : PeerSwapType_eo41 ( SWAP_TYPE_UDEFINED_EO41 )
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::teo41_Swap"));

  if ( teo41_Swap::LocalSwapType_eo41 == SWAP_TYPE_UDEFINED_EO41 ) 
    eo41_BuildLocalSwapType ();
}

/*---------------------------------------------------------------------------*/

teo41_Swap::teo41_Swap ( tsp00_Uint1 const SwapType )
           : PeerSwapType_eo41 ( SwapType )
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::teo41_Swap"));

  if ( teo41_Swap::LocalSwapType_eo41 == SWAP_TYPE_UDEFINED_EO41 ) 
    eo41_BuildLocalSwapType ();
}

/*---------------------------------------------------------------------------*/

bool  teo41_Swap::eo41_SetPeerSwapType   ( tsp00_Uint1 const             SwapType,
                                           teo200_EventList * const      pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::teo41_Swap"));

  if (( SwapType < 7 ) && ( SwapType != 1 )  && ( SwapType != 6 ))
    PeerSwapType_eo41 = SwapType;
  else
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_ILL_SWAP_TYPE, PeerSwapType_eo41 );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

void  teo41_Swap::eo41_SwapInt2 ( unsigned char const *         pUC,
                                  tsp00_Int2                    &Ret ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapInt2"));
  
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
  switch ( PeerSwapType_eo41 )
  {
  case 3:
  case 5:
  case 7:
    // --- no swap: hi-lo
    Ret = ( (tsp00_Int2)(pUC[ 0 ]) << 8 ) | 
          ( (tsp00_Int2)(pUC[ 1 ]) << 0 );
    break;
  case 0:
  case 2:
  case 4:
    // --- full swap: lo-hi
    Ret = ( (tsp00_Int2)(pUC[ 0 ]) << 0 ) | 
          ( (tsp00_Int2)(pUC[ 1 ]) << 8 );
    break;
  default:
    throw teo200_EventList( FUNCTION_NAME_MEO60, ERR_ILL_SWAP_TYPE, PeerSwapType_eo41 );
  }
  
  return;
}

/*---------------------------------------------------------------------------*/

void  teo41_Swap::eo41_SwapInt4 ( unsigned char const *         pUC,
                                  tsp00_Int4                    &Ret ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapInt4"));

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
  switch ( PeerSwapType_eo41 )
  {
  case 3:
  case 7:
    // --- no swap
    Ret = ( (tsp00_Int4)(pUC[ 0 ]) << 24 ) |
          ( (tsp00_Int4)(pUC[ 1 ]) << 16 ) |
          ( (tsp00_Int4)(pUC[ 2 ]) <<  8 ) |
          ( (tsp00_Int4)(pUC[ 3 ]) <<  0 );
    break;
  case 0:
  case 4:
    // --- full swap
    Ret = ( (tsp00_Int4)(pUC[ 0 ]) <<  0 ) |
          ( (tsp00_Int4)(pUC[ 1 ]) <<  8 ) |
          ( (tsp00_Int4)(pUC[ 2 ]) << 16 ) |
          ( (tsp00_Int4)(pUC[ 3 ]) << 24 );
    break;
  case 5:
    Ret = ( (tsp00_Int4)(pUC[ 0 ]) <<  8 ) |
          ( (tsp00_Int4)(pUC[ 1 ]) <<  0 ) |
          ( (tsp00_Int4)(pUC[ 2 ]) << 24 ) |
          ( (tsp00_Int4)(pUC[ 3 ]) << 16 );
    break;

  case 2:
    Ret = ( (tsp00_Int4)(pUC[ 0 ]) << 16 ) |
          ( (tsp00_Int4)(pUC[ 1 ]) << 24 ) |
          ( (tsp00_Int4)(pUC[ 2 ]) <<  0 ) |
          ( (tsp00_Int4)(pUC[ 3 ]) <<  8 );
    break;

  default:
    throw teo200_EventList( FUNCTION_NAME_MEO60, ERR_ILL_SWAP_TYPE, PeerSwapType_eo41 );
  }
  
  return;
}

/*------------------------------*/

void  teo41_Swap::eo41_SwapInt8 ( unsigned char const *         pUC,
                                  tsp00_Int8                    &Ret ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapInt8"));
  
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
  switch ( PeerSwapType_eo41 )
  {
  case 7:
    Ret = ( (tsp00_Int8)(pUC[ 0 ]) << 24 ) |
          ( (tsp00_Int8)(pUC[ 1 ]) << 16 ) |
          ( (tsp00_Int8)(pUC[ 2 ]) <<  8 ) |
          ( (tsp00_Int8)(pUC[ 3 ]) <<  0 ) |
          ( (tsp00_Int8)(pUC[ 4 ]) << 56 ) |
          ( (tsp00_Int8)(pUC[ 5 ]) << 48 ) |
          ( (tsp00_Int8)(pUC[ 6 ]) << 40 ) |
          ( (tsp00_Int8)(pUC[ 7 ]) << 32 );
    break;

  case 5:
    Ret = ( (tsp00_Int8)(pUC[ 0 ]) <<  8 ) |
          ( (tsp00_Int8)(pUC[ 1 ]) <<  0 ) |
          ( (tsp00_Int8)(pUC[ 2 ]) << 24 ) |
          ( (tsp00_Int8)(pUC[ 3 ]) << 16 ) |
          ( (tsp00_Int8)(pUC[ 4 ]) << 40 ) |
          ( (tsp00_Int8)(pUC[ 5 ]) << 32 ) |
          ( (tsp00_Int8)(pUC[ 6 ]) << 56 ) |
          ( (tsp00_Int8)(pUC[ 7 ]) << 48 );
    break;

  case 4:
    // --- full swap
    Ret = ( (tsp00_Int8)(pUC[ 0 ]) <<  0 ) |
          ( (tsp00_Int8)(pUC[ 1 ]) <<  8 ) |
          ( (tsp00_Int8)(pUC[ 2 ]) << 16 ) |
          ( (tsp00_Int8)(pUC[ 3 ]) << 24 ) |
          ( (tsp00_Int8)(pUC[ 4 ]) << 32 ) |
          ( (tsp00_Int8)(pUC[ 5 ]) << 40 ) |
          ( (tsp00_Int8)(pUC[ 6 ]) << 48 ) |
          ( (tsp00_Int8)(pUC[ 7 ]) << 56 );
    break;

  case 3:
    // --- no swap
    Ret = ( (tsp00_Int8)(pUC[ 0 ]) << 56 ) |
          ( (tsp00_Int8)(pUC[ 1 ]) << 48 ) |
          ( (tsp00_Int8)(pUC[ 2 ]) << 40 ) |
          ( (tsp00_Int8)(pUC[ 3 ]) << 32 ) |
          ( (tsp00_Int8)(pUC[ 4 ]) << 24 ) |
          ( (tsp00_Int8)(pUC[ 5 ]) << 16 ) |
          ( (tsp00_Int8)(pUC[ 6 ]) <<  8 ) |
          ( (tsp00_Int8)(pUC[ 7 ]) <<  0 );
    break;

  case 2:
    Ret = ( (tsp00_Int8)(pUC[ 0 ]) << 48 ) |
          ( (tsp00_Int8)(pUC[ 1 ]) << 56 ) |
          ( (tsp00_Int8)(pUC[ 2 ]) << 32 ) |
          ( (tsp00_Int8)(pUC[ 3 ]) << 40 ) |
          ( (tsp00_Int8)(pUC[ 4 ]) << 16 ) |
          ( (tsp00_Int8)(pUC[ 5 ]) << 24 ) |
          ( (tsp00_Int8)(pUC[ 6 ]) <<  0 ) |
          ( (tsp00_Int8)(pUC[ 7 ]) <<  8 );
    break;

  case 0:
    Ret = ( (tsp00_Int8)(pUC[ 0 ]) << 32 ) |
          ( (tsp00_Int8)(pUC[ 1 ]) << 40 ) |
          ( (tsp00_Int8)(pUC[ 2 ]) << 48 ) |
          ( (tsp00_Int8)(pUC[ 3 ]) << 56 ) |
          ( (tsp00_Int8)(pUC[ 4 ]) <<  0 ) |
          ( (tsp00_Int8)(pUC[ 5 ]) <<  8 ) |
          ( (tsp00_Int8)(pUC[ 6 ]) << 16 ) |
          ( (tsp00_Int8)(pUC[ 7 ]) << 24 );
    break;

  default:
    throw teo200_EventList( FUNCTION_NAME_MEO60, ERR_ILL_SWAP_TYPE, PeerSwapType_eo41 );
  }

  return;
}


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/


void teo41_Swap::eo41_BuildLocalSwapType ()
{ 
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_LocalSwapType"));

  union { tsp00_Int8         int8;
          tsp00_C8           c8; }  SwapCheck;
          tsp00_Uint1               SwapType;
  
  // --- set int8 to 0x100000000
  SwapCheck.int8   = 0x01;
  SwapCheck.int8 <<= 32;
  
  for ( SwapType = 0;
        ( SwapType < 8 ) && ( SwapCheck.c8[ SwapType ] != 1 ); 
        SwapType++ ) { ; }
  
  teo41_Swap::LocalSwapType_eo41 = SwapType;

  return;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/