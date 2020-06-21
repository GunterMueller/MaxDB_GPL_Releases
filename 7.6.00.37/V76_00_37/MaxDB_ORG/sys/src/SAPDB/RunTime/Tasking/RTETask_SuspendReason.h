/*!========================================================================

  @file         RTETask_SuspendReason.h
  @ingroup      Runtime
  @author       StefanP

  @brief        Task Suspend Reasons

  @since        2005-01-04  15:36
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


#ifndef RTETASK_SUSPENDREASON_H
#define RTETASK_SUSPENDREASON_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define SUSPEND_REASON_INV_SEL_SERVER_REQUEST            70
#define SUSPEND_REASON_INV_SEL_SERVER_RESERVE_ROW_BUFFER 71
#define SUSPEND_REASON_INV_SEL_ITER_SUSPEND              72
#define TSK_VSUSPEND_REASON_WAIT_FOR_JOB_END            213
#define TSK_VSUSPEND_REASON_NO_WORK                     255

#endif  /* RTETASK_SUSPENDREASON_H */
