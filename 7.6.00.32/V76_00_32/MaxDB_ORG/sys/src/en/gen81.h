/*!
  @file           gen81.h
  @author         JoergM
  @special area   RTE | Kernel Processes
  @brief          Coordinator Process
  @see            http://p26326.bea.sap-ag.de/Lzu/Specs/Processes.html

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


#ifndef GEN81_H
#define GEN81_H

#include "gsp00.h"

# if defined(__cplusplus)
extern "C" {
# endif

/*!
   @brief

 */


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*! @name Kernel Start and Termination */
 /*@(*/

/*!
   @brief          the coordinator main process prepares for all kernel processes/threads.
   @return         The code returned by any of each of these steps.
      &lt;UL>
              &lt;LI>0: Database orderly stopped
              &lt;LI>1: Wrong usage or bufwriter not resumed during KILL
              &lt;LI>2: Database died
              &lt;LI>3: Starting an child process/thread failed
              &lt;LI>other: Depend on subroutines called...
      &lt;/UL>


         &lt;UL>
                 &lt;LI>Buffering: Set all I/O channels to unbuffered, umask is reset
                 &lt;LI>SIGILL,SIGFPE,SIGBUS,SIGSEGV: Set to force core dump
                 &lt;LI>SIGTERM: Set to stop database
                 &lt;LI>SIGUSR2: Set to pull separate core dump via fork_and_exit
                 &lt;LI>All other signals: Set to be ignored
                 &lt;LI>Debuglevel: Global debug level is set
                 &lt;LI>Parse argument: Find out whether started as gateway or kernel
                 &lt;LI>Read XParam File: Find the configuration file and prepare the incore
                 XParam structure.
                 &lt;LI>Shared Memory: Prepare the shared memory segment using the values found.
                 &lt;LI>Timer: Initialize the timer process.
             &lt;LI>Semaphors: All kernel semaphores are created
                 &lt;LI>Communication: All communcation segments are allocated and the diag fifo
                 is opened
                 &lt;LI>Processes: All RTE processes are created
                 &lt;LI>User task Processes:All UserKernelProcesses(UKP)/Threads are created
                 &lt;LI>Wait: Waiting for any process/thread to terminate. If database is detected
                 to be in state SERVER_KILL call en81_kill_database().
                 &lt;LI>Cleanup:Kill all remnants. This final step is also executed if any step
                 before reported an error
         &lt;/UL>

 */

int	en81_kernel_main (int argc, char *argv[] , char *envp[]);

/*!
   @brief          If the server was in normal operation, try to activate the bufwriter to get
      dump and trace. When the bufwriter terminates, it's process exits.
   @return         &lt;UL>
              &lt;LI>0: Database orderly killed
              &lt;LI>1: Bufwriter not resumed during KILL
      &lt;/UL>


     Set state to SERVER_KILL. Set mode to MODE_KILL.
     Wake all user kernel processes to free their resources. The UKP with the sender
         task will distribute the dump request to all locally connected databases.
         Set bufwriter's trace and dump flag. If traceflag or dumpflag are not yet set,
         return an error code (rc = 1), which will break the coordinator loop.
     Resume the bufwriter.
         If traceflag or dumpflag could not be set, don't wait for the exit of the bufwriter.
     This can happen during the early start phase.

 */

int en81_kill_database ();

/*!
   @brief          Dump a core of the current process.
   @return         none


         This routine forks the kernel and let the son dump itself.
         Any allocated memory for caches is freed before the dump,
         to minimize the core size.

 */

void en81_dump_process_core( void );

/*!
   @brief          On normal termination each 'main' thread calls this
  function to terminate the database.
   @param          tw_gone [in] If set, tracewriter is gone
   @return         Current Control structure pointer

 */

void en81_NotifyCoordinator( int tw_gone );

/*!
   @brief          The 'real kernel' main function
   @param          pMain [inout] Pointer to KT main
   @return         exit code of 'real kernel'

 */

int en81RunKernel(struct ten50_KT_Control *pMain);

/*!
   @brief          Called in case kernel crashes
   @return         none

 */

void en81FinalAction();

/*!
   @brief          This call definitly forces a CoreDump by resetting signal
              handler and raising signals SIGIOT,SIGBUS and SIGSEGV...
   @return         none, will not return!

 */

void en81Abort();

/*!
   @brief          This call tries to call a debugger, if a debug environment is set.
   @Where Display must be set to your XDisplay identifier, i.e. p26216.0.
Make sure that Display is set to the correct value...
   @DebuggerCall depends on the Operating System<UL>
      <LI>AIX   - "dbx -a %d" No path to executable needed...
      <LI>HP    - "dde -ui line -attach %d path_to_exe"
      <LI>SUN   - "dbx path_to_exe %d"
      <LI>SOLARIS - "dbx path_to_exe %d"
      <LI>DEC   - "ladebug -pid %d path_to_exe"
               or "dbx path_to_exe %d"
      <LI>SNI   - "dbx %d" No path to executable needed...
      <LI>LINUX - "gdb path_to_exe %d"
      <LI>BSD44 - "gdb path_to_exe %d"
    </UL>

Except for AIX and SNI you have to specify the path to the executable.
   @Normally this is "$DBROOT/pgm/dbBName". But you cannot use $DBROOT,
but must specify the expanded path.
   @NOTE           This function is i.e. called inside of vabort() and vdebug_break()...

Environment variable SAPDB_DEBUG_RUN_KERNEL if found will enable starting
debugger at kernel start (usefull for LZU debugging).
   @return         Calling done
      &lt;UL>
              &lt;LI>0: Did not try to call debugger
      &lt;LI>1: Debugger called
  &lt;/UL>


   Environment is setup in a special text 'SAPDB_Debug_Env' file found in the
   RunDirectory. This file consists of individual lines containing variable
   assignments. Comment line are introduced by a Hashmark '#'. Variable assignment
   lines are formed as
   Variable = Value up til end of line
   Environment variable SAPDB_DEBUG_CMD will have the effect, that a call to this
   function leads in executing a command prepared using this environment value
   as format string for sprintf() with argument current pid (returned by getpid() ).
   General form of such an environment entry is
   SAPDB_DEBUG_CMD="xterm -display Display -e DebuggerCall &amp; sleep 5"

 */

int en81CallDebugger();

/*!
   @brief          This call tries to notify the starter process
   @return         none


   The kernel is started by an independent process. This process waits for
   signalling of successfull startup or SAP_DB kernel. This signal is send
   after the SAP_DB transition from STARTING to COLD.
   This function is called inside TimerThread only.

 */

void en81NotifyStarter();

/*!
   @brief          This call enables the timer thread to sleep
   @param          SleepPeriod [ in ] Time in seconds to sleep
   @return         none


   The Timer thread sleeps until the next job in its timer queue is due.
   This function is called inside TimerThread only.

 */

void en81TimerThreadSleep( tsp00_Int4 SleepPeriod );

/*!
   @brief          This call wakes up the timer thread
   @return         none


   The Timer thread sleeps until the next job in its timer queue is due.
   This call allows to wake it up prematuarly.
   This function is called inside TimerThread only.

 */

void en81TimerThreadWakeup( );

/*!
   @brief          the command line parsing of kernel

   @param argc [in] argumnent count passed to main
   @param argv [in] argument vector passed to main
   @return 0 command line ok, start kernel, != 0 do not start kernel
*/
int en81ParseCommandLine(int argc, char *argv[]);

/*@)*/

/*!
   @brief if enabled show max stack usage for all tasks

   If enabled by parameter SHOW_MAX_STACK_USE all task stack consumption is displayed
   @param none
   @return none
 */
void en81ShowMaxStackUse();

# if defined(__cplusplus)
}
# endif

/* KERNEL_GLOBAL_DBNAME */
extern tsp00_DbNamec	en81_dbname;
extern int				en81_usable_fds ;
extern long             en81_MemPageSize;
extern volatile int     en81_InCrashSignalHandler;

#endif  /* GEN81_H */
