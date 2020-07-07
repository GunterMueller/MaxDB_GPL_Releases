/*!***************************************************************************

  module      : RTE_Speed.c

  -------------------------------------------------------------------------

  responsible : StefanP

  special area: RunTime
  description : Determination of the Runtime version (Slow/quick/fast)


  last changed: 2001-05-25  13:53
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "SAPDBCommon/SAPDB_C_Cplusplus.h"
#include "RunTime/RTE_Types.h"



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
  Function:     RTE_GetSpeed
  description:  This function returns the version of the module compilation.
                In that way the determination of the Runtime Enviroment version
                takes place.
                
  arguments:

  return value: "SLOW ", "QUICK " or "FAST" (for fast and everything else)
*/
externC const SAPDB_Char     *RTE_GetSpeed()
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
 

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    #ifdef SAPDB_SLOW
        return (RTE_SLOW);
    #elif SAPDB_QUICK 
        return (RTE_QUICK);
    #elif defined(SAPDB_OMSTST)
        return (RTE_TEST);
    #else
        return (RTE_FAST);
    #endif
}

