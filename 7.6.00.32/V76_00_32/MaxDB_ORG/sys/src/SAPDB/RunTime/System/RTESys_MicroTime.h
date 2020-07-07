/*!---------------------------------------------------------------------
  @file           RTESys_MicroTime.h
  @author         JoergM
  @ingroup        RunTime
  @brief          System calls replace for micro second timer

\if EMIT_LICENCE

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



\endif
---------------------------------------------------------------------*/



#ifndef RTESYS_MICROTIME_H
#define RTESYS_MICROTIME_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_CompilerFeatures.h" /* for externC and VAR_ARRAY_REF ... */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!--------------------------------------------------------------------
   @brief    Initialize timer for microseconds since start of database

   You should call at the very beginning of your program. It is not not multithread save
   since RTESys_MicroSecTimer must be as short as possible!!!!
 */
externC void RTESys_InitMicroSecTimer();

/*!--------------------------------------------------------------------
   @brief  Return number of microseconds since last call to RTESys_InitMicroSecTimer()

   This call is optimized for performance, to allow constant time monitoring!

   @return Number of microseconds since last call to RTESys_InitMicroSecTimer()
 */
externC SAPDB_UInt8 RTESys_MicroSecTimer();

/*!--------------------------------------------------------------------
   @brief    Returns microsecond since since 1.1.1970 0:00 UTC

   Used as input for RTESys_BuildSQLTimeStamp to generate SQL time stamps. This
   call is not used to get timestamps for intenal timing measurements, so it is
   not called very frequently, but it should be as precise as possible and always
   monotonic ascendend. 

   @return   Microseconds since 1.1.1970 0:00 UTC
 */
externC SAPDB_UInt8 RTESys_MicroSecondTime();

#ifdef LINUX
/*!
  @brief check if cpu timer register is usable
  Since on multiprocessor system the CPU timer register content may leap 
  between different CPUs and the threads are not bound to specific CPUs it
  is impossible to use them if the CPU timer registers are not synchronized.
  This function tries to find that out.
  @param forceUseOfGettimeofday [in] flag used to overrule checking
  @return true if cpu counter is used, false if gettimeofday is used
 */
externC SAPDB_Bool RTESys_LinuxTimerLeapCheck(SAPDB_Bool forceUseOfGettimeofday);
#endif

#endif  /* RTESYS_MICROTIME_H */
