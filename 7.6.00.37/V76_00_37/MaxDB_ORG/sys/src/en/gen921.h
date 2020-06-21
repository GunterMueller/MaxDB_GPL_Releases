/*!========================================================================

  @file         gen921.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        UNIX console cplusplus wrapper


  @since        2003-09-22  18:05

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


#ifndef GEN921_H
#define GEN921_H

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

/*!
  @brief retrieve task command timeout

  Needed to allow access to command timeout stored in C++ object from C code.
  @param pTaskObject [in] the task object
  @returns the timeout value
 */
externC SAPDB_ULong en921GetTaskCommandTimeOut( struct RTETask_Task* pTaskObject );

/*!
  @brief retrieve task type display name

  Needed to allow access to the display name stored in C++ object from C code.
  @param pTaskObject [in] the task object
  @returns display name
 */
externC SAPDB_Char const * const en921GetTaskDisplayName (struct RTETask_Task*  pTaskObject,
                                                          SAPDB_Bool            longName);


/*!
  @brief show content of communication queue

  Needed to allow access to command timeout stored in C++ object from C code.
  Called in consol application if shared memory was mapped at original position.

  @param pTaskSchedulerObject [in] pointer to task scheduler object
  @param needsRemapping [in] flag to indicate C++ object members are possibly not remapped
  @param rangeStartAddress [in] start address of range
  @param rangeEndAddress [in]   end address of range
  @param copyStartAddress [in]  start address of copy range
  @returns none
 */
externC void en921ShowCommqueueRemapped ( struct RTETask_TaskScheduler* pTaskSchedulerObject
                                        , SAPDB_Bool        needsRemapping
                                        , SAPDB_Byte const *rangeStartAddress
                                        , SAPDB_Byte const *rangeEndAddress
                                        , SAPDB_Byte       *copyStartAddress
                                        );

#endif  /* GEN921_H */
