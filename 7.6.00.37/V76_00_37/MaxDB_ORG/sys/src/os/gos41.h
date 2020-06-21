/*!
  @file           gos41.h
  @author         RaymondR
  @brief          Kernel Runtime Environment
              IPC resources
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
*/



#ifndef GOS41_H
#define GOS41_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#include  "gsp00.h"

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
 *  PROTOTYPES                                                               *
 *===========================================================================*/

externC LONG            sql41c_create_event_sem     ( PHANDLE    phevSysSem,
                                                      char *     pszSemPath,
                                                      char *     pszName,
                                                      ULONG      flAttr,
                                                      BOOL       fState,
                                                      PSECURITY_ATTRIBUTES pSA );
externC LONG            sql41c_wait_event_sem       ( HANDLE        hevSysSem,
                                                      ULONG         ulTimeout,
                                                      char  const  *pszSemDesc );
externC LONG            sql41c_post_event_sem       ( HANDLE        hevSysSem,
                                                      char  const  *pszSemDesc );
externC LONG            sql41c_close_event_sem      ( HANDLE     hevSysSem,
                                                      char  const  *pszSemDesc );
externC LONG            sql41c_open_event_sem       ( PHANDLE    phevSysSem,
                                                      char *     pszName,
                                                      char *     pszSemPath,
                                                      ULONG      ulNoMsgRc );
externC LONG            sql41c_create_shrd_mem      ( PVOID     *ppMemBase,
                                                      ULONG      ulMemSize,
                                                      char *     pszShrdMemPath,
                                                      char *     pszName,
                                                      PSECURITY_ATTRIBUTES pSA,
                                                      HANDLE     *phShrdMem );
externC LONG            sql41c_get_named_shrd_mem   ( PVOID     *ppMemBase,
                                                      char *     pszShrdMemPath,
                                                      char *     pszName,
                                                      HANDLE     *phShrdMem);
externC LONG            sql41c_give_shrd_mem        ( PVOID      pMemBase,
                                                      DWORD      pidDest,
                                                      HANDLE     hShrdMem,
                                                      HANDLE     *phDestShrdMem );
externC LONG            sql41c_free_shrd_mem        ( PVOID      pMemBase,
                                                      HANDLE     hShrdMem );
externC LONG            sql41c_create_queue_mailslot( PHANDLE    phHandle,
                                                      char *     pszPath,
                                                      char *     pszName,
                                                      ULONG      ulReqLen,
                                                      PSECURITY_ATTRIBUTES pSA);
externC LONG            sql41c_open_queue_mailslot  ( DWORD     *ppidOwner,
                                                      PHANDLE    phHandle,
                                                      char *     pszPath,
                                                      char *     pszName );
externC LONG            sql41c_write_queue_mailslot ( HANDLE     hHandle,
                                                      PVOID      pRequest,
                                                      ULONG      ulReqLen,
                                                      ULONG      ulDataLen,
                                                      PVOID      pData );
externC LONG            sql41c_read_queue_mailslot  ( HANDLE     hHandle,
                                                      PVOID      pRequest,
                                                      ULONG      ulReqLen,
                                                      PULONG     pulDataLen,
                                                      PVOID     *ppData );
externC LONG            sql41c_close_queue_mailslot ( HANDLE     hHandle );

externC VOID            sql41c_build_unique_obj_name( char      *pszClientObjName );


#endif  /* GOS41_H */
