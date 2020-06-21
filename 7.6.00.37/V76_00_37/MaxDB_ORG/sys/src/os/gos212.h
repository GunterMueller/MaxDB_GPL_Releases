/*!
  @file           gos212.h
  @author         RaymondR
  @brief          MS Cluster Sever
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



#ifndef GOS212_H
#define GOS212_H



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

#if defined(__cplusplus)
extern "C" {
#endif

DWORD os212_AddSERVERDBClusterResource    ( LPCSTR   SERVERDBName, 
                                            LPCSTR   Remark );
DWORD os212_RemoveSERVERDBClusterResource ( LPCSTR   SERVERDBName,
                                            DWORD    OffLineTimeOut );

DWORD os212_OnlineXSERVERClusterResource  ( void );
DWORD os212_OfflineXSERVERClusterResource ( void );
DWORD os212_OnlineSERVERDBClusterResource (  LPCSTR  ServerDB  );
DWORD os212_OfflineSERVERDBClusterResource(  LPCSTR  ServerDB  );

#if defined(__cplusplus)
}
#endif


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS212_H */
