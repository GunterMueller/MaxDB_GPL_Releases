/****************************************************************************

  module      : vsp101.cpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: Version for Pascal Components
  description : description ...


  last changed: 1999-12-01  14:01
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include "gsp00.h"
#include "gsp100.h"
#include "hsp100.h"

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
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externPascal void sp101_GetVersionStringP  ( tsp00_C9                  &ComponentNameP,
                                             tsp00_Version             &VersionStringP )
{
  ROUTINE_DBG_MEO00 ("sp101_GetVersionStringP");

  tsp00_Versionc        VersionString;
  char                  *pVersion      = VersionString;
  char                  *pVersionP     = VersionStringP;
  short                 SizeOfVersionP = sizeof(VersionStringP);
  tsp100_CompName       ComponentName;



  memcpy ( ComponentName, &ComponentNameP, sizeof(ComponentNameP));
  ComponentName[sizeof(ComponentName)-1] = '\0';

  // -- LinkNumberFunc: e.g. 's98linknumber'
  sp100_GetVersionString ( ComponentName, s100buildnumber, VersionString );
    
  // VersionStringP.c2p( VersionString );
  sp36_c2p(&VersionStringP[0], sizeof(tsp00_Version), VersionString.asCharp());

  return;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
