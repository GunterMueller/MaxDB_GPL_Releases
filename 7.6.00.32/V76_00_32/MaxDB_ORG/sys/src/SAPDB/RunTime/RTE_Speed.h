/*!***************************************************************************

  module      : RTE_Speed.h

  -------------------------------------------------------------------------

  responsible : StefanP

  special area: RunTime Typedefinitions
  description : Contains all common RunTime Type definitions

  last changed: 2000-06-15  14:25
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


#ifndef RTE_SPEED_H
#define RTE_SPEED_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

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
/*!
  Function:     RTE_GetSpeed
  description:  This function returns the version of the module compilation.
                In that way the determination of the Runtime Enviroment version
                takes place.
                
  arguments:

  return value: "SLOW ", "QUICK " or "FAST" (for fast and everything else)
*/
externC     const SAPDB_Char     *RTE_GetSpeed();


#endif  /* RTE_SPEED_H */
