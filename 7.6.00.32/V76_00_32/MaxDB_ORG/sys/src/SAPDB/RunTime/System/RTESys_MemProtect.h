/*!---------------------------------------------------------------------
  @file           RTESys_MemProtect.h
  @author         RaymondR
  @brief          Memory Protection
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
---------------------------------------------------------------------*/



#ifndef RTESYS_MEMPROTECT_H
#define RTESYS_MEMPROTECT_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MEMORYPROTECTION_READWRITE 0
#define MEMORYPROTECTION_NOACCESS  1
#define MEMORYPROTECTION_READONLY  2

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTIONS                                                                *
 *===========================================================================*/

#if defined(__cplusplus)
 extern "C" {
#endif

SAPDB_ULong RTESys_MemProtect         ( void *pvAdr, SAPDB_ULong ulSize, SAPDB_Int type );

SAPDB_ULong RTESys_MemProtectReadWrite( void *pvAdr, SAPDB_ULong ulSize );
SAPDB_ULong RTESys_MemProtectNoAccess ( void *pvAdr, SAPDB_ULong ulSize );
SAPDB_ULong RTESys_MemProtectReadOnly ( void *pvAdr, SAPDB_ULong ulSize );


#if defined(__cplusplus)
 }
#endif

#endif  /* RTESYS_MEMPROTECT_H */
