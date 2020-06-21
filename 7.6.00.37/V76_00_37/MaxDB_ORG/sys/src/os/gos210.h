/*!
  @file           gos210.h
  @author         RaymondR
  @brief          Cluster Server DLL
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




#ifndef GOS210_H
#define GOS210_H

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

RESID WINAPI os210_XSERVER_Open        ( LPCWSTR                    ResourceName,
                                         HKEY                       ResourceKey,    
                                         RESOURCE_HANDLE            ResourceHandle );

RESID WINAPI os210_SERVERDB_Open       ( LPCWSTR                    ResourceName,
                                         HKEY                       ResourceKey,    
                                         RESOURCE_HANDLE            ResourceHandle );

void WINAPI  os210_XSERVER_Close       ( RESID                      ResourceId );
void WINAPI  os210_SERVERDB_Close      ( RESID                      ResourceId )
;
DWORD WINAPI os210_XSERVER_Online      ( RESID                      ResourceId, 
                                         PHANDLE                    EventHandle );
DWORD WINAPI os210_SERVERDB_Online     ( RESID                      ResourceId, 
                                         PHANDLE                    EventHandle );

DWORD WINAPI os210_XSERVER_Offline     ( RESID                      ResourceId );
DWORD WINAPI os210_SERVERDB_Offline    ( RESID                      ResourceId );

DWORD WINAPI os210_XSERVER_Terminate   ( RESID                      ResourceId );
DWORD WINAPI os210_SERVERDB_Terminate  ( RESID                      ResourceId );

BOOL WINAPI  os210_XSERVER_LooksAlive  ( RESID                      ResourceId );
BOOL WINAPI  os210_SERVERDB_LooksAlive ( RESID                      ResourceId );

BOOL WINAPI  os210_XSERVER_IsAlive     ( RESID                      ResourceId );
BOOL WINAPI  os210_SERVERDB_IsAlive    ( RESID                      ResourceId );

DWORD WINAPI os210_ResourceControl     ( RESID                      ResourceId,
                                         DWORD                      ControlCode,
                                         PVOID                      InBuffer,
                                         DWORD                      InBufferSize,
                                         PVOID                      OutBuffer,
                                         DWORD                      OutBufferSize,
                                         LPDWORD                    BytesReturned );

DWORD WINAPI os210_ResourceTypeControl ( LPCWSTR                    ResourceTypeName,
                                         DWORD                      ControlCode,
                                         PVOID                      InBuffer,
                                         DWORD                      InBufferSize,
                                         PVOID                      OutBuffer,
                                         DWORD                      OutBufferSize,
                                         LPDWORD                    BytesReturned );


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/


#endif  /* GOS210_H */
