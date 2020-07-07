/*!
  @file           vos212.cpp
  @author         RaymondR
  @brief          MSCS
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

#include "gos202.h"
#include "geo60.h"
#include "gos212.h"


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

static tos202_CluManDll      CluManDll;


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


extern "C" DWORD os212_AddSERVERDBClusterResource( LPCSTR  SERVERDBName, 
                                                   LPCSTR  Remark )
{
  ROUTINE_DBG_MEO60 (_T("os212_AddSERVERDBClusterResource"));
  bool    Ok;

  if ( CluManDll.os203_DLLLoaded () == false )
    return ERROR_MOD_NOT_FOUND;

  Ok = CluManDll.os202_AddSERVERDBResource( SERVERDBName, Remark );

  if ( Ok == false )
    return ( GetLastError() == NO_ERROR ) ? !NO_ERROR : GetLastError();

  return NO_ERROR;
}

/*---------------------------------------------------------------------------*/

extern "C" DWORD os212_RemoveSERVERDBClusterResource( LPCSTR  SERVERDBName,
                                                      DWORD   OffLineTimeOut )
{
  ROUTINE_DBG_MEO60 (_T("os212_RemoveSERVERDBClusterResource"));
  bool    Ok;

  if ( CluManDll.os203_DLLLoaded () == false )
    return ERROR_MOD_NOT_FOUND;

  Ok = CluManDll.os202_RemoveSERVERDBResource( SERVERDBName, OffLineTimeOut );

  if ( Ok == false )
    return ( GetLastError() == NO_ERROR ) ? !NO_ERROR : GetLastError();

  return NO_ERROR;
}

/*---------------------------------------------------------------------------*/

extern "C" DWORD os212_OnlineXSERVERClusterResource( void )
{
  ROUTINE_DBG_MEO60 (_T("os212_OnlineXSERVERClusterResource"));
  bool    Ok;

  if ( CluManDll.os203_DLLLoaded () == false )
    return ERROR_MOD_NOT_FOUND;

  Ok = CluManDll.os202_OnlineXSERVERResource();

  if ( Ok == false )
    return ( GetLastError() == NO_ERROR ) ? !NO_ERROR : GetLastError();

  return NO_ERROR;
}

/*---------------------------------------------------------------------------*/

extern "C" DWORD os212_OfflineXSERVERClusterResource( void )
{
  ROUTINE_DBG_MEO60 (_T("os212_OfflineXSERVERClusterResource"));
  bool    Ok;

  if ( CluManDll.os203_DLLLoaded () == false )
    return ERROR_MOD_NOT_FOUND;

  Ok = CluManDll.os202_OfflineXSERVERResource();

  if ( Ok == false )
    return ( GetLastError() == NO_ERROR ) ? !NO_ERROR : GetLastError();

  return NO_ERROR;
}

/*---------------------------------------------------------------------------*/

extern "C" DWORD os212_OnlineSERVERDBClusterResource(  LPCSTR  ServerDB  )
{
  ROUTINE_DBG_MEO60 (_T("os212_OnlineSERVERDBClusterResource"));
  bool    Ok;

  if ( CluManDll.os203_DLLLoaded () == false )
    return ERROR_MOD_NOT_FOUND;

  Ok = CluManDll.os202_OnlineSERVERDBResource( ServerDB );

  if ( Ok == false )
    return ( GetLastError() == NO_ERROR ) ? !NO_ERROR : GetLastError();

  return NO_ERROR;
}

/*---------------------------------------------------------------------------*/

extern "C" DWORD os212_OfflineSERVERDBClusterResource(  LPCSTR  ServerDB  )
{
  ROUTINE_DBG_MEO60 (_T("os212_OfflineSERVERDBClusterResource"));
  bool    Ok;

  if ( CluManDll.os203_DLLLoaded () == false )
    return ERROR_MOD_NOT_FOUND;

  Ok = CluManDll.os202_OfflineSERVERDBResource( ServerDB );

  if ( Ok == false )
    return ( GetLastError() == NO_ERROR ) ? !NO_ERROR : GetLastError();

  return NO_ERROR;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/