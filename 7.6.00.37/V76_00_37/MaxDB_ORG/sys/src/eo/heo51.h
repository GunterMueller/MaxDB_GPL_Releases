/*!
  @file           heo51.h
  @author         JoergM
  @special area   kernel_initializations
  @brief          Kernel Initialization Functions
  @see            example.html ...

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



#ifndef HEO51_H
#define HEO51_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "vsp001.h"
#include "vsp002.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void    vinitok      ( /* void */ );

externC void    vinit        ( tsp00_RteCommAddr VAR_VALUE_REF rte_comm_ptr,
						       tsp00_Int2                      len );

externC void    vdebug_break ( tsp00_Int4 i4DebugBreakPosition );

externC void    vgetuktid    ( tsp00_Int4 VAR_VALUE_REF id );

externC void    vgetpid      ( tsp00_TaskId VAR_VALUE_REF pid );

externC void    vgetfirstuserpid ( tsp00_TaskId VAR_VALUE_REF pid );

externC void    vptype       ( tsp00_TaskId                    pid ,
						       tsp2_process_type VAR_VALUE_REF proc_type );

externC void    vversion     ( tsp00_Version VAR_ARRAY_REF kernelversion, 
				               tsp00_Version VAR_ARRAY_REF rteversion );

externC void    vutil_startup ( tsp1_utility_startup VAR_VALUE_REF startupReason );

/*
  @brief initiate kernel state change to ONLINE mode

  This is the final call marking ONLINE state of kernel for runtime.

  @param pid [in] current task pid
  @param sysdevspace [in] unused (historical...)
 */
externC void    vrestart     ( tsp00_TaskId                pid , 
						       tsp00_DevName VAR_ARRAY_REF sysdevspace );

/*
  @brief initiate kernel state change to OFFLINE mode

  voffline with mode 'shtShutdKill_esp00' forces a backup of the diagnostic files (DIAGHISTORY) at next
  restart of the database.

  @param pid [in] current task pid
  @param mode [in] wanted shutdown mode (shtShutdNormal_esp00, shtShutdKill_esp00)
 */
externC void    voffline    ( tsp00_TaskId             pid ,
  						      tsp00_ShutdownMode_Param mode );

/*
  @brief final call of a no longer used task
 */
externC void    vfinish      ( /* void */ );

/*
  @brief get operation system information
  @param os [out] operation system type
 */
externC void    vos          ( tsp00_Os VAR_VALUE_REF os );

/*
  @brief lock against task movement
  Only the task itself is allowed to lock against task movement
  @param ownId [in] calling task id
 */
externC void    vmovelock ( tsp00_TaskId ownId );

/*
  @brief allow task movement for this task
  Only the task itself is allowed to unlock task movement
  @param ownId [in] calling task id
 */
externC void    vmoveunlock ( tsp00_TaskId ownId );

/*!
  @brief check if utility task is currently connected
  @return true if connected, false if not
 */
externC tsp00_Bool visutilitytaskconntected();

#endif  /* HEO51_H */
