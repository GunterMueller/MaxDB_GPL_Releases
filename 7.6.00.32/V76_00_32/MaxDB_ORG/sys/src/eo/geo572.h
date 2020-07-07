/*!
  @file           geo572.h
  @author         RaymondR
  @special area   user storage management
  @brief          user storage management
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



#ifndef GEO572_H
#define GEO572_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if !defined(externC) 
# if defined (__cplusplus)
#  define  externC      extern "C"
# else
#  define  externC
# endif
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


externC  void eo572_CreateAllocPool   ();
externC  void eo572_DestroyAllocPool  ();

externC  void sqlallocat              ( tsp00_Int4        Size,
                                        tsp00_Uint1**   Buffer,
                                        tsp00_BoolAddr  Ok );
externC  void sqlfree                 ( tsp00_Uint1*    Buffer );

externC  void sqlreallocat            ( tsp00_Int4        Size,
                                        tsp00_Uint1**   Buffer,
                                        tsp00_BoolAddr  Ok );
externC  void sqlvalidatealloc        ( tsp00_Uint1*    Buffer,
                                        tsp00_BoolAddr  Ok );

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO572_H */
