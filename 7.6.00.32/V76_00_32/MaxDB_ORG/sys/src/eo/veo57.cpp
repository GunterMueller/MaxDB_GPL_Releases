/*!
  @file           veo57.cpp
  @author         JoergM
  @special area   Kernel Memory Allocation Interface
  @brief          
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

#include "heo00.h"
#include "heo57.h"
#include "geo00_0.h"
#include "geo50_0.h"
#include "geo00_1.h"

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

//PTS 1106794
/*==========================================================================*/

// vGetAvailableDynPool() returns the available KB in "_SHAREDDYNPOOL"

externC tsp00_Int4 vGetAvailableKBDynPool()
{
#if defined(_WIN32)

  return ( tsp00_Int4( ( (tsp00_Longuint) kgs.pSharedDynPoolTop - (tsp00_Longuint) kgs.pSharedDynPoolPtr) / 1024) ) ;
  
#else

  return ( tsp00_Int4( ((tsp00_Longuint) KGS->dynpool_top - (tsp00_Longuint) KGS->dynpool_ptr) / 1024) ) ; 

#endif

}

//PTS 1106794
/*==========================================================================*/

// vGetAvailableDynPool() returns the available Pages in "_SHAREDDYNDATA"

externC tsp00_Int4 vGetAvailablePagesDynData()
{
#if defined(_WIN32)

  return( (tsp00_Int4)BYTES_TO_PAGES(kgs.pSharedDynDataTop - kgs.pSharedDynDataPtr) ) ;

#else

  return( 0 ) ; /* unix follows*/

#endif
}



/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/