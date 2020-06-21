/*!
 * \file    LVC_DispatcherHashEntry.cpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   OMS Stored Procedure dispatcher hash entry.
 */
/*

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



*/

#include "liveCache/LVC_DispatcherHashEntry.hpp"
#include <stdlib.h>

/*===========================================================================*
*  LVC_DispatcherHashEntry                                                   *
*===========================================================================*/

LVC_DispatcherHashEntry::LVC_DispatcherHashEntry (){
  m_lpnext            = NULL;
  m_session           = 0;
  m_iid               = GUID_NULL;
  m_IUnknown          = NULL;
  m_type              = ENTRY_Free;
  m_is_committed      = false;
  m_idebug_used_flag  = false;
  m_is_remote_debug   = false;
};

void LVC_DispatcherHashEntry::init(tsp00_Uint4 session, 
                                   const LPGUID lpGuid, 
                                   LPUNKNOWN    lpInterface, 
                                   bool         remoteDbgFlag,
                                   LVC_DispatcherHashEntry::HashEntryType hType )
{
  m_lpnext            = NULL;
  m_session           = session;
  m_iid               = *lpGuid;
  m_IUnknown          = lpInterface;
  m_type              = hType;
  m_is_committed      = false;
  m_idebug_used_flag  = false;
  m_is_remote_debug   = remoteDbgFlag;
}

/*---------------------------------------------------------------------------*/

void* LVC_DispatcherHashEntry::operator new( size_t sz )                    /* PTS 1110403 */
{                                                              /* PTS 1110403 */
  return malloc( sz );                                         /* PTS 1110403 */
}                                                              /* PTS 1110403 */

/*---------------------------------------------------------------------------*/

void LVC_DispatcherHashEntry::operator delete(void* p)                      /* PTS 1110403 */
{                                                              /* PTS 1110403 */
  free( p );                                                   /* PTS 1110403 */
}                                                              /* PTS 1110403 */

/*---------------------------------------------------------------------------*/
