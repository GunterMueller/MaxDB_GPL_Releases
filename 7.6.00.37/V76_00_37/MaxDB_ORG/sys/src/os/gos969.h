/*!
  @file           gos969.h
  @author         RaymondR
  @brief          START/STOP/INSTALL/REMOVE ANY DATABASE_SERVER
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



#ifndef GOS969_H
#define GOS969_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gos00.h"
#include "gos41.h"

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

externC
DWORD os969StartAnyServerService    ( char *pszNodeName,
                                       char *pszServerName );

externC
DWORD os969StopAnyServerService     ( char *pszNodeName,
                                       char *pszServerName );

externC
DWORD os969UpdateAnyServerService   ( char      *pszNodeName, 
                                      char      *pszServerName, 
								      BOOL      fAutostart,
                                      BOOL      fNoDNSReverseLookup,
                                      DWORD     dwPortNo,
                                      DWORD     dwNIPortNo,
                                      DWORD     dwSSLPortNo );

externC
DWORD os969InstallAnyXserverService ( char      *pszNodeName, 
                                       char     *pszServerName, 
	                                   BOOL     fAutostart,
                                       BOOL     fNoDNSReverseLookup,
                                       DWORD    dwPortNo,
                                       DWORD    dwNIPortNo,
                                       DWORD    dwSSLPortNo );

externC
DWORD os969RemoveAnyXserverService  ( char *pszNodeName,
                                       char *pszServerName );

externC
DWORD os969StartAnyServer           ( char *pszCmdLine, 
                                       char *pszServerName, 
                                       char *pszServerSemaName, 
                                       BOOL fSuppressActiveMsg );

externC
DWORD os969StopAnyServer            ( char *pszServerName,
                                       char *pszServerSemName );

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS969_H */
