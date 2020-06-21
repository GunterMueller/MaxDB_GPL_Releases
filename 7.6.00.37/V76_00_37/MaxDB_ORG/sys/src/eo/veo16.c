/*!
  @file           veo16.c
  @author         JoergM
  @special area   Version and OS-ID
  @brief          Client RTE version and OS-ID functions
              (was veo02uc)
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

#include "hsp100.h"
#include "heo16.h"
#include "geo00_0.h"
#include "heo46.h"

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
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/
externC char *RTEBuildNumber(void);

externC void sqlversion ( tsp00_Version version )
{
    const char            *pszCompName;
    tsp00_Versionc         RTEVersionString;

#if _WIN32 || OS2 
    if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
    {
	    pszCompName = COMP_NAME_RTE_NT_SP100;
    }
    else
    {
	    pszCompName = COMP_NAME_RTE_WIN9X_SP100;
    }
#else
    pszCompName = COMP_NAME_RTE_SP100;
#endif

    sp100_GetVersionString((char *)pszCompName, RTEBuildNumber, RTEVersionString);

    eo46CtoP( version, (char *)RTEVersionString, sizeof(tsp00_Version) );
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
