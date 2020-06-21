/*!**********************************************************************

  module: vwd61dllmain.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  entry-point function for WebAgent DLL (Win NT)

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include <WINDOWS.H>

#include "hwd20control.h"

/*!**********************************************************************

  DllMain

************************************************************************/

BOOL APIENTRY DllMain( HANDLE	hModule,
                       DWORD    dwReason,
                       LPVOID   lpReserved)
{
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_PROCESS_DETACH:
        /* wd20ApiExit();
           makes problems because now its to late to cleanup memory in
           dbfsapi.dll */
        break;
    default:
        break;
    }; /* switch */

    return TRUE;
}; /* DllMain */
        
