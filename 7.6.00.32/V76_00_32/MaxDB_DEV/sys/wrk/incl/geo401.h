/*!
  @file           geo401.h
  @author         StefanP
  @special area   NI helper functions (need no UNICODE support)
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



#ifndef GEO401_H
#define GEO401_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "geo003.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC int   
eo40NiExtractDBNodeFromSaprouterSring ( char                        *SaprouterString,
                                        tsp00_NodeIdc VAR_ARRAY_REF  ServerNode  ) ;

externC int   
eo40NiExtractServiceFromSaprouterSring ( char                        *SaprouterString,
                                         unsigned short              *Service ) ;

externC int   
eo40NiIsSaprouterSring   (  char                   *StrToCheck );


#endif  /* GEO401_H */
