/*!========================================================================

  @file         gen53.h
  @ingroup      
  @author       RaymondR

  @brief        old UNIX communication

  @since        2003-09-22  15:53
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/


#ifndef GEN53_H
#define GEN53_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo50_0.h"

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

/*
 @brief  Insert task into communication queue, where it waits for a packet
 */ 
externC void e53_wait_for_communication( struct TASK_TYPE*       tcb,
                                         connection_info*        cip,
                                         SAPDB_ULong             CommandTimeOut );

#endif  /* GEN53_H */
