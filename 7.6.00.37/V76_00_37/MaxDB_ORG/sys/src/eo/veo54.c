/*!
  @file           veo54.c
  @author         JoergM
  @special area   Kernel I/O
  @brief          I/O Convertion Routines and Page0Handling
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

#include "geo54.h"

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

externC tsp00_PageNo eo54SavablePageNo(tsp00_PageNo pageno)
{
    unsigned long input = pageno;
    tsp00_PageNo output = (tsp00_PageNo)0;
    int ii;

    for ( ii = 0; ii < sizeof(tsp00_PageNo); ii++ )
    {
        ((unsigned char *)&output)[ii] = (unsigned char)(input & 0x0FF);
        input >>= 8;
    }
    return output;
}

externC tsp00_Int4 eo54SavableInt4(tsp00_Int4 number)
{
    unsigned long input = number;
    tsp00_Int4 output = (tsp00_Int4)0;
    int ii;

    for ( ii = 0; ii < sizeof(tsp00_Int4); ii++ )
    {
        ((unsigned char *)&output)[ii] = (unsigned char)(input & 0x0FF);
        input >>= 8;
    }
    return output;
}

externC tsp00_PageNo eo54RestoredPageNo(tsp00_PageNo pageno)
{
    tsp00_PageNo input = pageno;
    unsigned long output = 0;
    int ii;

    for ( ii = sizeof(tsp00_PageNo); --ii >= 0; )
    {
        output <<= 8;
        output |= ((unsigned char *)&input)[ii];
    }
    return (tsp00_PageNo)output;
}

externC tsp00_Int4 eo54RestoredInt4(tsp00_Int4 number)
{
    tsp00_Int4 input = number;
    unsigned long output = 0;
    int ii;

    for ( ii = sizeof(tsp00_Int4); --ii >= 0; )
    {
        output <<= 8;
        output |= ((unsigned char *)&input)[ii];
    }
    return (tsp00_Int4)output;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
