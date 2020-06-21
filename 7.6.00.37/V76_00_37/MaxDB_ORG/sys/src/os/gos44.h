/*!
  @file           gos44.h
  @author         RaymondR
  @brief          Windows file handling
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



#ifndef GOS44_H
#define GOS44_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp01.h"

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

// --- VOS44C.C --------------------------------------------------------------

externC tsp00_Int        
sql44c_get_dev_type_by_filename ( char                 *pszName );

externC tsp00_Bool        
sql44c_subst_log_parts          ( char                 *pszOutName,
                                  char                 *pszInName );

externC tsp00_Int     
sql44c_get_file_info            ( char                 *pszFName,
                                  tsp00_Int            *plAtt,
                                  tsp00_Uint           *pFSizeLow,
                                  tsp00_Uint           *pFSizeHigh);

externC bool     
os44Filecopy                   (  char                 *pszSrcName,
                                  char                 *pszDestName,
                                  tsp01_RteError       *RteError ) ;

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS44_H */
