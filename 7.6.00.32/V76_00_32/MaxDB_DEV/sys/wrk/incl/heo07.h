/*!
  @file           heo07.h
  @author         JoergM
  @special area   Multithreading Runtime API
  @brief          Routines for Creating and managing Threads,Mutex and 
              Semaphores.
  @Original AuthorBurkhard Diesing
  @see            


                Routines are system independend. Including
                heo07.h or geo07.h does not force to include
                system specific header files.

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



#ifndef HEO07_H
#define HEO07_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "geo07.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*! @name Thread */
 /*@(*/

/*!
   @brief          This function initialize global data needed.
   @param          thread [out] pointer to thread structure for calling thread
   @param          errtext [out] verbose error message
   @param          err [out] see below
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     It must to called before
     any thread is created or any other function of this API is called.

 */

void sqlinit_multithreading(teo07_Thread *thread,
                            tsp00_ErrTextc VAR_ARRAY_REF errtext,
                            teo07_ThreadErr *err);

/*!
   @brief          Get the minimum thread stack size.
   @return         minimum stack size in bytes


     If the given thread size for sqlbeginthread is smaller than this amount,
     it is silently increased (as the OS does too...).

 */

tsp00_Int4 sqlget_thread_min_stack();

/*!
   @brief          Creates a thread.
   @param          stack_size_in_bytes [in] stack size wanted for thread
   @param          proc [in] thread main function
   @param          arg [in] argument passed to thread main function
   @param          flags [in] flags specifying thread
   @param          pThread [out] pointer storing returned thread
   @param          errtext [out] error message if error occured
   @param          err [out] error return value
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_IMPLEMENTED_EO07: If linked with no thread library
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     NOTE: If the given thread is started SUSPENDED, it will suspend itself.
     In UNIX pthread implementation only the thread itself can suspend itself.
     So be carefull not to misuse SUSPENDING...
     Another thread should NOT allowed be suspended from outside.
     Replaces fork() function in a process environment, but SHARES all data.

 */

void sqlbeginthread(tsp00_Int4 stack_size_in_bytes, 
                    teo07_ThreadCallback proc, 
                    void *arg, 
                    tsp00_Int4 flags, 
                    teo07_Thread *pThread,
                    tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                    teo07_ThreadErr *err);

/*!
   @brief          Creates a thread, but using thread stack from outside.
   @param          stack_size_in_bytes [in] stack size wanted for thread
   @param          stack_start_address [in] stack start address
   @param          proc [in] thread main function
   @param          arg [in] argument passed to thread main function
   @param          flags [in] flags specifying thread
   @param          pThread [out] pointer storing returned thread
   @param          errtext [out] error message if error occured
   @param          err [out] error return value
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_IMPLEMENTED_EO07: If linked with no thread library
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     NOTE: If the given thread is started SUSPENDED, it will suspend itself.
     In UNIX pthread implementation only the thread itself can suspend itself.
     So be carefull not to misuse SUSPENDING...
     Another thread should NOT allowed be suspended from outside.
     Replaces fork() function in a process environment, but SHARES all data.
     The stack start address is always given as the lowest address of the stack area even
     if the stack is unusually growing to higher addresses...
     Additional guardpages are not added by this call, so they have to be added outside
     if wanted. WIN32 does not support own thread stack, so that parameter is ignored.

 */

void sqlbeginthread_on_own_stack(tsp00_Int4 stack_size_in_bytes, 
                     void *stack_start_address,
                     teo07_ThreadCallback proc, 
                     void *arg, 
                     tsp00_Int4 flags, 
                     teo07_Thread *pThread,
                     tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                     teo07_ThreadErr *err);

/*!
   @brief          Modify the thread priority
   @param          thread [in] Thread to modify
   @param          priority [in] Wanted priority                  Windows NT:                                    THREAD_PRIORITY_ABOVE_NORMAL                   Indicates 1 point above normal priority                                    THREAD_PRIORITY_BELOW_NORMAL                   Indicates 1 point below normal priority                                    THREAD_PRIORITY_HIGHEST                   Indicates 2 points above normal priority                                    THREAD_PRIORITY_IDLE                   Indicates a base priority level of 1                                    THREAD_PRIORITY_LOWEST                   Indicates 2 points below normal priority                                    THREAD_PRIORITY_NORMAL                   Indicates normal priority for the priority class                  THREAD_PRIORITY_TIME_CRITICAL                   Indicates a base priority level of 15                                    UNIX/PTHREADS:                                    absolute value                  1..127 (1 least favoured, 127 most favoured)                  UNIX/CTHREADS:                  absolute value                  0..127 (0 least favoured, 127 most favoured)
   @return         &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_IMPLEMENTED_EO07: If priority cannot be set
      &lt;LI>THR_NOT_OK_EO07: 
  &lt;/UL>

 */

teo07_ThreadErr sqlsetthreadpriority( teo07_Thread thread, 
                                      tsp00_Int4 priority );

/*!
   @brief          Modify the current thread priority
   @param          thread [in] Thread to modify
   @param          priority [in] Wanted priority                  Windows NT:                                    THREAD_PRIORITY_ABOVE_NORMAL                   Indicates 1 point above normal priority                                    THREAD_PRIORITY_BELOW_NORMAL                   Indicates 1 point below normal priority                                    THREAD_PRIORITY_HIGHEST                   Indicates 2 points above normal priority                                    THREAD_PRIORITY_IDLE                   Indicates a base priority level of 1                                    THREAD_PRIORITY_LOWEST                   Indicates 2 points below normal priority                                    THREAD_PRIORITY_NORMAL                   Indicates normal priority for the priority class                  THREAD_PRIORITY_TIME_CRITICAL                   Indicates a base priority level of 15                                    UNIX/PTHREADS:                                    absolute value                  1..127 (1 least favoured, 127 most favoured)                  UNIX/CTHREADS:                  absolute value                  0..127 (0 least favoured, 127 most favoured)
   @return         &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_IMPLEMENTED_EO07: If priority cannot be set
      &lt;LI>THR_NOT_OK_EO07: 
  &lt;/UL>

 */

teo07_ThreadErr sqlsetmythreadpriority( tsp00_Int4 priority );

/*!
   @brief          Return the thread priority
   @param          thread [in] Thread
   @param          pPriority [out] Priority return value (see sqlsetthreadpriority for values)
   @return         &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_IMPLEMENTED_EO07: If priority cannot be set
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>

 */

teo07_ThreadErr sqlgetthreadpriority( teo07_Thread thread, 
                                      tsp00_Int4 VAR_VALUE_REF pPriority );

/*!
   @brief          Return the current thread priority
   @param          pPriority [out] Priority return value (see sqlsetthreadpriority for values)
   @return         &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_IMPLEMENTED_EO07: If priority cannot be set
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>

 */

teo07_ThreadErr sqlgetmythreadpriority( tsp00_Int4 VAR_VALUE_REF pPriority );

/*!
   @brief          Allows a thread to terminate itself.
   @param          returncode [in] Returncode passed to sqljointhread
   @return         none


     Equivalent to exit() function in a process environment.

 */

void sqlendthread(tsp00_Int4 returncode);

/*!
   @brief          Allows to send a SIGTERM signal to a specific thread.
   @param          thread [in] Pointer to the thread structure
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     Replaces kill(SIGTERM) function. In WINDOWS environment this kill
     terminates the thread.

 */

void sqlkillthread(teo07_Thread thread, 
                   tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                   teo07_ThreadErr *err);

/*!
   @brief          Allows to send a given signal to a specific thread.
   @param          thread [in] Pointer to the thread structure
   @param          signal [in] The signal to be send
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     Added for KERNEL interface. Replaces kill() function.

 */

void sqlsigthread( teo07_Thread thread, 
                   tsp00_Int4 signal,
                   tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                   teo07_ThreadErr *err);

/*!
   @brief          Allows to cancel a specific thread.
   @param          thread [in] Pointer to the thread structure
   @param          returncode [in] Returncode received by sqljointhread call (Used in WIN32 only)
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned NOTE: errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     Canceling has the
     same effect as an sqlendthread call of the cancelled thread.
     NOTE: Cancelling effectivly happens if the thread reaches a
     cancelpoint, which is normally a call to sqlyield() or a system
     call.
     Added for KERNEL interface.

 */

void sqlcancelthread(teo07_Thread thread, 
                     tsp00_Int4 returncode,
                     tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                     teo07_ThreadErr *err);

/*!
   @brief          Detaches the given thread.
   @param          thread [in] Pointer to the thread structure
   @return         none


     No sqljointhread is needed to reclaim storage allocated for the thread.
     NOTE: After this call use of 'thread' is a fatal mistake...

 */

void sqlforgetthread (teo07_Thread thread);

/*!
   @brief          Allows a thread to reschedule itself.
   @return         none


     Needed if explicit blocking (i.e. via sqllockmutex) would lead to block all
     messages (i.e. window messages would not arrive at client,
     so no update or repaint is possible...).

 */

void sqlyieldthread(void);

/*!
   @brief          Returns the current thread structure, usable in subsequent calls
   @return         The current thread of the calling procedure

 */

teo07_Thread   sqlgetthread();

/*!
   @brief          Returned TID uniquely identifies this thread within process.
   @return         The unique thread id of the current thread

 */

teo07_ThreadId sqlgetthreadid();

/*!
   @brief          Returned TID uniquely identifies the given thread within process.
   @param          thread [in] Pointer to the thread structure
   @return         The unique thread id of the given thread or 0 if no such thread.


     Added for KERNEL interface.

 */

teo07_ThreadId sqlthreadid(teo07_Thread thread);

/*!
   @brief          Allows a thread to suspend itself.
   @param          thread [in] Pointer to the thread structure
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_OK_EO07: If thread cannot suspend
  &lt;/UL>


     NOTE: Only the thread itself can suspend itself.
     In UNIX environment (POSIX Threads) threads are 
     NOT allowed to suspend other threads but themself.

 */

void sqlsuspendthread(teo07_Thread thread, 
                      tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                      teo07_ThreadErr *err);

/*!
   @brief          Allows to resume a formerly suspended thread.
   @param          thread [in] Pointer to the thread structure
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     NOTE: If the given thread is not in suspended state
     resume does not have any effect, but a subsequent
     sqlsuspendthread will return THR_NOT_OK_EO07.

 */

void sqlresumethread(teo07_Thread thread, 
                     tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                     teo07_ThreadErr *err);

/*!
   @brief          Check if given thread has suspended itself
   @param          thread [in] Pointer to the thread structure
   @return         &lt;UL>
      &lt;LI>true: Thread has suspended itself
              &lt;LI>false: Thread has not suspended itself
  &lt;/UL>


     For 'thread grouping' there is a little time period where two
         threads are running. The one that gave up its slice has to wake
         up and suspend itself afterwards. The second thread can use this
         call, to check that the first has entered its suspend state.
     If alone, it is i.e. possible to protect its task stack etc...
         NT: No API function that allows this...

 */

tsp00_Bool sqlIsThreadSuspended(teo07_Thread thread);

/*!
   @brief          Allows to join a formerly created thread.
   @param          thread [in] Pointer to the thread structure
   @param          status [out] Exit status of the thread
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NO_DATA_FOUND_EO07: If thread was not found
      &lt;LI>THR_NOT_OK_EO07: If thread cannot be joined
  &lt;/UL>


     Joining means
     that it will wait until the given thread exits, or is 
     terminated. Joining includes reclaiming all memory that
     was allocated for the thread.
     NOTE: If the given thread is detached, joining cannot
     be done.

 */

void sqljointhread(teo07_Thread thread, 
                   tsp00_Int4 *status, 
                   tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                   teo07_ThreadErr *err);

/*!
   @brief          Allows to check whether a thread is still running.
   @param          thread [in] Pointer to the thread structure
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         TRUE if thread is alive
              FALSE if thread is dead

 */

int sqlthreadalive( teo07_Thread thread, 
                     tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                     teo07_ThreadErr *err);

/*!
   @brief          Allows to execute a function exactly once.
   @param          thread [in] Pointer to the thread structure
   @param          callback [in] The function to execute
   @param          arg [out] The argument for that function
   @return         none


     The first call
     for a given initialized flag will execute the callback
     with the given argument. All subsequent calls will not
     get executed.

 */

void sqlonce (tsp00_Int4* initialized,
              teo07_ThreadCallback callback,
              void* arg);

/*@)*/

/*! @name Mutex */
 /*@(*/
/*!
   @brief          Creates a mutual exlusive (mutex) structure.
   @param          hMutex [in] The mutex to destroy
   @return         none


     Such a structure is used to protect a code region from being entered
     by several threads at once.
     NOTE: Must be destroyed by a subsequent call to 
     sqldestroymutex    if no longer needed.

 */

void sqlcreatemutex(teo07_Mutex *hMutex);
/*!
   @brief          Destroyes a mutex structure.
   @param          hMutex [in] The mutex to destroy
   @return         none

 */

void sqldestroymutex(teo07_Mutex *hMutex);

/*!
   @brief          Blocking version of entering a critical section.
   @param          hMutex [in] The mutex
   @return         none


     NOTE: Some systems do not allow messages to be passed
     while waiting to enter a critical section (NT/Windows95).

 */

void sqlbeginmutex(teo07_Mutex *hMutex);

/*!
   @brief          Test whether critical section is entered
   @param          hMutex [in] The mutex
   @return         none

 */

int sqlissetmutex(teo07_Mutex *hMutex);

/*!
   @brief          Non blocking version of entering a critical section.
   @param          hMutex [in] The mutex
   @return         1 (true) on success, 0 (false) on error


     Used in combination with sqlyieldthread() to allow
     messages to be passed while waiting to enter a critical
     section.
     A special macro SYNCHRONIZED_EO07 can be used to form a block
     of statements protected by a pair of sqlbeginmutex()/sqlendmutex()
     calls.

 */

tsp00_Int4 sqltrybeginmutex(teo07_Mutex *hMutex);
/*!
   @brief          Leave a critical section
   @param          hMutex [in] The mutex
   @return         none

 */

void sqlendmutex(teo07_Mutex *hMutex);
/*@)*/

/*! @name Local Storage */
 /*@(*/
/*!
   @brief          Allocates a Thread Local Storage key.
   @param          hKey [in] The Key to be allocated
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_IMPLEMENTED_EO07: If linked with no thread library
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     NOTE: Must be destroyed by a subsequent call to 
     sqldestroytls if no longer needed.

 */

void sqlcreatetls(tsp00_Int4 *hKey, 
                  tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                  teo07_ThreadErr *err);
/*!
   @brief          Frees a Thread Local Storage key.
   @param          hKey [in] The Key to be deallocated
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>

 */

void sqldestroytls(tsp00_Int4 hKey, 
                   tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                   teo07_ThreadErr *err);
/*!
   @brief          Set the value for a given Thread Local Storage key.
   @param          hKey [in] The Key to be allocated
   @param          pObjPtr [in] The pointer to store.
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     Each thread will see always only its on TLS Key value.

 */

void sqlsettls(tsp00_Int4 hKey, 
               void*  pObjPtr, 
               tsp00_ErrTextc VAR_ARRAY_REF errtext, 
               teo07_ThreadErr *err);
/*!
   @brief          Get the value for a given Thread Local Storage key.
   @param          hKey [in] The Key to be allocated
   @NOTE           Since NULL can be a valid value, no error check can be done...
   @return         the value set by a previous sqlsettls call or NULL.


     Each thread will see always only its on TLS Key value.

 */

void *sqlgettls(tsp00_Int4 hKey);
/*@)*/

/*! @name Semaphore */
 /*@(*/

/*!
   @brief          Create a thread global semaphore.
   @param          sem [out] The pointer to the semaphore to create
   @param          initval [in] Initial value
   @param          errtext [out] Place for error message to be returned
   @param          err [out] Place for error code to be returned
   @NOTE           errtext is set only if returned *err is != THR_OK_EO07
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>THR_OK_EO07: On success
      &lt;LI>THR_NOT_OK_EO07: See errtext for details
  &lt;/UL>


     NOTE: These semaphore are NOT used to synchronize between processes. 
     Only usefull for interthread synchronization. You cannot use them 
     i.e. for shared memory synchronization between processes!

 */

void sqlcreatesem (teo07_ThreadSemaphore* sem,
                   tsp00_Int4 initval,
                   tsp00_ErrTextc VAR_ARRAY_REF errtext, 
                   teo07_ThreadErr* err);
/*!
   @brief          Delete a semaphore if no longer used
   @param          sem [in] The semaphore to destroy
   @return         none

 */

void sqldestroysem (teo07_ThreadSemaphore sem);

/*!
   @brief          Blocking wait on semaphore.
   @param          sem [in] The semaphore to wait for
   @return         none


     Decreasing counter by 1. If already 0 then wait 
     blocking on the semaphore to get signalled.

 */

void sqlwaitsem (teo07_ThreadSemaphore sem);
/*!
   @brief          Nonblocking wait on semaphore.
   @param          sem [in] The semaphore to wait for
   @param          seconds [in] The maximum number of seconds to wait
   @param          err [out] Set to THR_TIMEOUT_EO07 or THR_OK_EO07 respectivly.
   @return         none, but err is modified according to result.


     Decreasing counter by 1. If already 0 then wait on 
     the semaphore to get signalled within a specified
     timeout.

 */

void sqltimedwaitsem (teo07_ThreadSemaphore sem, 
                      tsp00_Int4 seconds, 
                      teo07_ThreadErr *err);
/*!
   @brief          Signals the given semaphore, inceasing counter by 1
   @param          sem [in] The semaphore to trigger
   @return         none

 */

void sqlsignalsem (teo07_ThreadSemaphore sem);
/*@)*/

/* _m is of type (teo07_Mutex *) */
#define SYNCHRONIZED_EO07(_m) for ( sqlbeginmutex(_m); \
                                    sqlissetmutex(_m); \
                                    sqlendmutex(_m) )

#if defined(__cplusplus)

/*!
   @brief          Setup pointer to thread class handling this thread
   @param          thread [in] Pointer to the thread structure
   @param          threadclasspointer [in] Pointer to 'this' pointer of thread class object
   @return         none
 */
void sqlsetthreadclasspointer(teo07_Thread thread, void *threadclasspointer);

/*!
   @brief          Setup pointer to thread class handling this thread
   @param          thread [in] Pointer to the thread structure
   @param          threadclasspointer [in] Pointer to 'this' pointer of thread class object
   @return         none
 */
void *sqlgetthreadclasspointer(teo07_Thread thread);

} /* extern "C" */

class Synchronized_eo07
{
private:
    teo07_Mutex *m_mutex;
public:
    Synchronized_eo07(teo07_Mutex *m):m_mutex(m) { sqlbeginmutex(m); }
    ~Synchronized_eo07()                         { sqlendmutex(m_mutex); }
};

#endif /* __cplusplus */

#endif  /* HEO07_H */
