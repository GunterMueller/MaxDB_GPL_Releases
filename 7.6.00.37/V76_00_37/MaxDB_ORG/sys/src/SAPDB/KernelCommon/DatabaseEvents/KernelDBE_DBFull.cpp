/*!
  @file           KernelDBE_DBFull.cpp
  @author         TorstenS
  @ingroup        KernelDBE_DBFull
  @brief          Database Events Management - DB Full eventing
*/

/*
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "vsp0031.h"  // tsp31_event_description
#include "heo60.h"    // vinsert_event
#include "hgg01_2.h"  // g01event_init

#include "KernelCommon/DatabaseEvents/KernelDBE_DBFull.hpp"

/*===========================================================================*
 *  BEGIN IF CODE                                                            *
 *===========================================================================*/

void KernelDBE_DBFull::SendEvent()
{
    tsp31_event_description eventDesc;

    g01event_init( eventDesc );

    eventDesc.sp31ed_ident.becomes( sp31ei_databasefull );
    eventDesc.sp31ed_priority.becomes( sp31ep_high );

    vinsert_event( eventDesc );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
