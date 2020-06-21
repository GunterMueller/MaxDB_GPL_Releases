/*!
  @file           heo69.h
  @author         JoergM
  @special area   Accounting
  @brief          Accounting functions (Stack,Timing,IOs etc.)
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



#ifndef HEO69_H
#define HEO69_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
   @brief          Reset Millisecond counter
   @param          taskid [in] TaskId (only used in kernel RTE)
   @return         none


         This call gets the current number of ticks used in subsequent
         calls (vccheck) to get the number of milliseconds needed.
         User and System times are added to a total time.

 */

externC void vcinit ( RTE_TaskId  taskid );

/*!
   @brief          Get number of milliseconds since last call to vcinit()
   @param          taskid [in] TaskId (only used in kernel RTE)
   @param          time [out] Number of milliseconds since last vcinit
   @return         none


         This call gets the current number of ticks and returns the number
         of milliseconds used since last call to vcinit().
         User and System times are added to a total time.

 */

externC void vccheck ( RTE_TaskId  taskid,
					   SAPDB_Int4    VAR_VALUE_REF time );

/*!
   @brief          Reset task specific IO counter
   @param          taskid [in] TaskId (only used in kernel RTE)
   @return         none


         This call resets the global IO counter for the specific task

 */

externC void vioinit ( RTE_TaskId  taskid );

/*!
   @brief          Get direct IO counter since last call to vioinit()
   @param          taskid [in] TaskId (only used in kernel RTE)
   @param          no [out] Number of direct IO calls
   @return         none


         This call gets the global direct IO count for the specific task

 */

externC void viocheck ( RTE_TaskId  taskid ,
						SAPDB_Int4    VAR_VALUE_REF no );

/*!
   @brief          Get task specific counter
   @param          taskid [in] TaskId (only used in kernel RTE)
   @param          phys_ios [out] Sum of SelfIORead/Write and DevIORead/Write
   @param          suspends [out] Number of vsuspend() calls
   @param          waits [out] Number of vwait() calls
   @return         none


         This call gets the counter for the specific task.

 */

externC void vmonitor ( RTE_TaskId  taskid ,
						SAPDB_Int4    VAR_VALUE_REF phys_ios ,
						SAPDB_Int4    VAR_VALUE_REF suspends ,
						SAPDB_Int4    VAR_VALUE_REF waits );

/*!
   @brief          Initialization of stack checking code
   @return         none


   Initialization of stack checking code (only needed for Client RTE)
   This function is a dummy for kernel RTE.

 */

externC void vsinit ( /* void */ );

/*!
   @brief          Returns the number of bytes left on the stack
   @param          LeftStackSize [out] Number of bytes left on stack
   @return         none


   This call returns the current number of bytes left on the stack.
   In kernel RTE the current task stack is used.

 */

externC void vscheck ( SAPDB_Int4 VAR_VALUE_REF LeftStackSize );

/*!
   @brief          Fill stack with pattern
   @return         none


   Fill the remaning stack with a pattern.
   Leave the two-word chgstack()-test-pattern intact.
   Current function's additional stack requirements
   estimated as eight words.
   In kernel RTE the current task stack is used.

 */

externC void vsfill ( /* void */ );

/*!
   @brief          Return number of bytes used on stack since last call to vsfill()
   @param          MaxStackLeft [out] Number of bytes left on stack
   @return         none


   This call uses the first difference from the pattern setup with last
   vsfill() call. This difference is used to return the maximum number
   of bytes used on the stack.
   In kernel RTE the current task stack is used.

 */

externC void vsscan ( SAPDB_Int4 VAR_VALUE_REF MaxStackLeft );

/*!
   @brief          Calls vabort of stack overflow was detected
   @return         none


   This call uses the vsscan() function and compares the result with
   the RTE internal known task stack size. If overflow occured, a
   vabort() call is done (and a Stack Overflow message written to knldiag).

 */

#if COMPILEMODE_MSP00 >= SLOW_MSP00	
#if defined(_WIN32)
#define SECURITY_BUFFER_MXEO69 (kgs.ulSystemPageSize)
#else
extern long en81_MemPageSize;
#define SECURITY_BUFFER_MXEO69 en81_MemPageSize
#endif
#else
#define SECURITY_BUFFER_MXEO69 0
#endif
externC void vsoverflow ( /* void */ );

/*!
   @brief          Show maximum stack usage of givben task in knldiag
   @param          taskId [in] task id of task for which the value is requested
   @param          taskTypeName [in] readable type name of task

 */

externC void vsShowMaxStackUse(RTE_TaskId taskId, const char *taskTypeName);

/*!
   @brief          Reset the currently known max stack usage of given task.
   @param          taskId [in] task id of task for which the value is requested


   The maxstackusage is set by all vscheck() calls, if current stack usage is higher
   that the value stored in the task context.

 */

externC void vsResetCmdMax(RTE_TaskId taskId);

/*!
   @brief          Returns the currently known max stack usage of given task.
   @param          taskId [in] task id of task for which the value is requested
   @param          cmdMax [out] current maximum of stack usage for command
   @Return value   Stored maximum number of bytes used on stack of given task


   The maxstackusage is set by all vscheck() calls, if current stack usage is higher
   that the value stored in the task context.

 */

externC void vsGetCmdMax(RTE_TaskId taskId, SAPDB_Int4 VAR_VALUE_REF cmdMax);

/*!
   @brief          Reinitialize the current stack
   @param          taskId [in] task id for which to reinitialize stack
   @param          maxUsed [in] maximum amount of bytes used on stack of task


   This call does a vsfill() but beginning with current stack position up to a known
   max stack size or up to stack limit if not known. The maxUsed value can be retrieved
   with a call to vsscan().

 */

externC void vsReinit( RTE_TaskId taskId, SAPDB_Int4 maxUsed );

#endif  /* HEO69_H */
