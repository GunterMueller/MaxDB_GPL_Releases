/****************************************************************************

  module      : vgg11.cpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: logging ...
  description : description ...


  last changed: 1999-12-13  12:09
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
#include "hgg11.h"
#include <stdio.h>

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

externPascal void g11kernel_version (
    tsp00_Version         VAR_ARRAY_REF  vers)
{
  tsp00_Versionc        VersionString;

#if defined(OMSTST)
  sp100_GetVersionString ( COMP_NAME_OMSTSTKNL_SP100, s100buildnumber, VersionString );
#else
#if COMPILEMODE_MEO00 == SLOW_MEO00 
  sp100_GetVersionString ( COMP_NAME_SLOWKNL_SP100, s100buildnumber, VersionString );
#else

#if COMPILEMODE_MEO00 == QUICK_MEO00 
  sp100_GetVersionString ( COMP_NAME_QUICKKNL_SP100, s100buildnumber, VersionString );
#else
  sp100_GetVersionString ( COMP_NAME_KERNEL_SP100, s100buildnumber, VersionString );
#endif
#endif
#endif
 
  vers.c2p( VersionString );
}

/*--------------------------------------------------------------------------*/

externPascal void g11ParseVersion (
    tsp00_Version         VAR_ARRAY_REF  vers,
    integer               VAR_VALUE_REF  majorRelease,
    integer               VAR_VALUE_REF  minorRelease,
    integer               VAR_VALUE_REF  correctionLevel,
    integer               VAR_VALUE_REF  buildNumber)
{
    sscanf( (char*)&vers[BEG_OF_1ST_VERS_SUBSTR_SP100], "%d.%d.%d", 
        &majorRelease, &minorRelease, &correctionLevel );
    sscanf( (char*)&vers[BEG_OF_2CND_VERS_SUBSTR_SP100], "%d-", &buildNumber);
}

/*--------------------------------------------------------------------------*/

externPascal void g11ParseExtraNumber (
    tsp00_Version         VAR_ARRAY_REF  vers,
    integer               VAR_VALUE_REF  extraNumber)
{
    integer buildNumber;
    sscanf( (char*)&vers[BEG_OF_2CND_VERS_SUBSTR_SP100], "%d-%d-", &buildNumber, &extraNumber);
}
