/*!**************************************************************************

  module      : RTETask_IContext.hpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Task Context
  description : Class defining a kernel task context

                Defining the Task context means to define all task context
                depending functions. If you look for other RTE funktion, that
                are not task depending or depending on the task pool look
                into description of RTE_IGlobal class. This defines i.e. the
                GetCurrentTaskContext() function to retrieve the context of
                the currently running task, which then can be used to access
                the task context depending functions.

  last changed: 2000-07-24  18:15
  see also    : example.html ...
  first created:2000-03-17  20:20

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


#ifndef RTETASK_ICONTEXT_HPP
#define RTETASK_ICONTEXT_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

class SAPDBErr_MessageList;
class AK_TransContext; // Name to be defined...

/*! Class: RTETask_IContext

    Description: Class defining the internals of a kernel task

    Each kernel task should get this pointer as its first argument
 */

class RTETask_IContext {
public:
  // --------------------------------------------------------------------------------------------------------------------
  /*! Function:	GetTaskId

      Description: Get Id of calling task

      Replaces vgetpid()

      With this function the calling task can identify itself. To do this it inspects the thread
      local storage to find the hook to the running UKT. The UKT contains the id its current
      running task.

      Internal error occurs if not called inside a UKT.

      NOTE: This is the only function, that does not has to give its CallingTaskId as first
      parameter.

      Arguments:	none

      Return value:	Task id of calling task
   */
  virtual const RTE_TaskId GetTaskId() const = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:	    Suspend

      Description:  Called by a task to suspend itself for the given reason

      Replaces vsuspend(TaskId, Reason, LEVEL_TASK)

      Counterpart to the Resume () function

      Calling this function will force a task dispatching, if the reason for suspend is not
      already posted by another task (state 0). The dispatcher sets the new internal 
      state (-1) and searches for the next . If the reason was already posted ( state 1 ) the
      internal task state is change to running (state 0) and the task will continue execution
      immediatly without a task switch.

      Level is no longer needed, since the function is splitted from SuspendForTrace(). The
      level was needed since after informing a resource about a outstanding Suspend() a
      trace was written, which could also Suspend() the task until trace writer allowed the
      task to write its trace. Now with two outstanding Suspend() it was necessary to
      introduce a level to handle two possible Resume() reasons. 

      Internal error is reported if a Resume() was already posted for this task, but with a
      different reason than the Suspend() reason given.

      Arguments:  Reason [in] The reason for the task to suspend itself

      Return value:	none
  */
  virtual void Suspend(	const RTE_Reason Reason	) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     Resume

      Description:  Resume the given task context since reason is posted

      Replaces vresume(tsp00_TaskId, 0)

      Counterpart to the Suspend() function

      If the given task has called Suspend() before with the same reason, it is inserted in the
      run queue according to its priority. Therefor it is possible that this call forces a
      rescheduling of the task context unless prevented by a call to LockDispatcher().

      Internal error is to call this to resume itself.

      Internal error is to call this to resume a task that has already an outstanding
      Resume() call.

      Internal error is to call this to resume a task that called Suspend() with a different
      reason.

      Discussion showed that is is better to resume the task context instead of
      calling a member function of a task context to resume another task context.

      Arguments:	
      TaskResuming [in] The context of the task that is resuming
            Reason [in] The reason why to resume the task

      Return value:	none
  */
  virtual void Resume(	RTETask_IContext &TaskResuming,
                const RTE_Reason Reason	) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     IsResumed

      Description:  Test whether given task context is already resumed
      
      Replaces visresume(tsp00_TaskId, 0)

      Check if the given task has already been target to a resume.

      Arguments:	none

      Return value:	true if resumed, false else
  */
  virtual bool IsResumed( ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     SuspendWhileTraceBusy

      Description:  Suspend the current task until trace writer resumes it

      Replaces vsuspend(TaskId, WAIT_FOR_TRACEWRITER, 1)

      Counterpart to the ResumeByTracewriter () function

      Separated from Suspend() since it is possible to write trace while having already
      informed the resource about an outstanding waiting for a resource availibility.
      No suspend reason is needed, since trace writer has only one reason to suspend a
      task: Trace writer busy. Therefore shrinks the number of parameters from TaskId,
      Reason and Level  to the TaskId. 

      Arguments:    none

      Return value: none
   */
  virtual void SuspendWhileTraceBusy( ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     ResumeByTracewriter

      Description:  Resume the given task for given reason

      Replaces vresume(tsp00_TaskId, 1)

      Counterpart to the SuspendWhileTraceBusy() function

      CallingTask is used to find the internal task without inspecting thread local storage.

      NOTE: TaskResuming is not redundant, if no longer only a single tracewriter task exists!

      Arguments:  TaskResuming [in] The context of the tracewriter task that is resuming

      Return value:	none
   */
  virtual void ResumeByTracewriter(  RTETask_IContext & TaskResuming ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     IsResumedByTracewriter

      Description:  Test whether given task is already resumed by tracewriter
      
      Replaces visresume(tsp00_TaskId, 1)

      Check if the given task has already been target to a resume.

      Arguments:	none

      Return value:	true if already resumed by tracewriter, false else
  */
  virtual bool IsResumedByTracewriter( ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     Wait

      Description:  Wait for being signalled

      Replaces vwait (tsp00_TaskId, boolean *ok)

      Counterpart to the Signal () function.

      Stop a task until a Signal () was called. If the task is already marked for continuation
      it modifies its corresponding state variable only and returns. Otherwise it modifies its
      state variable and calls the dispatcher to allow another task to use the UKT. The 
      Signal() call sets the boolean value that is returned by Wait.

      Arguments:  Ok [out] The value set by the corresponding Signal() call

      Return value: none
   */
  virtual void Wait ( bool &Ok  ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     Signal

      Description:  Signal the given task

      Replaces vcontinue (tsp00_TaskId)

      Counterpart to the Wait() function.

      Continue a task that stopped itself using Wait () function. If task is waiting it is
      resumed and put into the running queue. Otherwise it is just marked for being
      signalled.

      Internal error if called for task that is already marked for signaling.

      Arguments:  TaskSignalling [in] The context of the task that is signalling
                  Ok [in] The value to be signaled to the task calling Wait()

      Return value: none
   */
  virtual void Signal ( RTETask_IContext &TaskSignalling,
                const bool Ok ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     Sleep

      Description:  Let the calling task sleep for some seconds

      Replaces vsleep (tsp00_TaskId, SAPDB_Int2 limit)

      But the task in the working queue of the time thread. If 0 seconds are given, this forces
      a rescheduling only.

      Arguments:  SleepLimitInSeconds [in] Number of seconds to sleep

      Return value: none
   */
  virtual void Sleep (  const SAPDB_Int2 SleepLimitInSeconds  ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     RescheduleMsec

      Description:  Reschedule task and return the number of milliseconds spent in the function
      
      Replaces vreschedulemsec (tsp00_TaskId, SAPDB_Int4 & MsecLimit, SAPDB_Int4 & MsecRemaining )

      Arguments:  MsecStart [in] Number of Milliseconds since kernel start
                  MsecRemaining [in] Number of Milliseconds running inside this routine
                  
      Return value: none
   */
  virtual void RescheduleMsec ( SAPDB_Int4 & MsecStart, SAPDB_Int4 & MsecRemaining ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:     Stop

      Description:  Stop the calling task

      Replaces vstop (tsp00_TaskId)

      Counterpart to the Continue() function.

      Stop a task until a Continue() was called. If the task is already marked for continuation
      it modifies its corresponding state variable only and returns. Otherwise it modifies its
      state variable and calls the dispatcher to allow another task to use the UKT.

      Arguments:    none
  
      Return value: none
   */
  virtual void Stop ( ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:	    Continue

      Description:  Continue the given task

      Replaces vcontinue (tsp00_TaskId)

      Counterpart to the Stop() function.

      Continue a task that stopped itself using Stop() function. This call can be called
      from any thread within the system. The calling thread is not necessarily a UKT.
      If the task to continue is already stopped, this call inserts the task in the run queue.

      Internal error if called for task that is already marked for continue.

      Arguments:  none

      Return value:	none
   */
  virtual void Continue( ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /* Function: LockUKT

      Description:  Prevent dispatcher from rescheduling any other task on the current UKT

      Replaces vLockDispatcher()

      Counterpart to the UnlockUKT () function

      This call is extremely dangerous if misused. It locks the UKT to the current task. So
      even if the task is blocked due to waiting for a resource the UKT is not allowed to
      switch over to the next runnable task. The reason for this is twofolded. On one hand it
      is possible for a task like the tracewriter to make sure the task is alone in its UKT. But
      the real reason for this is that some C++ Compilers have problems with exception catch
      handling routines that do internal task switching using coroutines.

      If the calling task is suspending itself afterwards, a new UKT is created, so that the
      number of UKTs runnable is kept constant to MAXCPU! If such a former locked and suspended
      UKT is resumed, the next UnlockUKT() call will prepare the UKT to terminate. When the next
      task switch in this UKT takes place,  the UKT will terminate. For the time between UnlockUKT()
      and the task switch it is possible that there are more UKTs than MAXCPU running. This is necessary
      since the resuming task must have time to leave the catch handler. Freeing the UKT with the call
      to UnlockUKT() would introduce another restriction on the usage of C++ exception handlers, since
      than UnlockUKT COULD NEVER be used inside a catch handler.

      Internal error if current UKT is already locked

      Arguments:    none

      Return value: none
   */
//  virtual void LockUKT( ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /* Function: UnlockUKT

      Description:	Allow dispatcher again to switch to other task on the current UKT

      Replaces vUnlockDispatcher()

      Counterpart to the LockUKT () function

      This call is the counterpart to LockUKT. It will prepare the UKT to terminate itself if 
      it was suspended while locked. This is necessary to keep the number of runnable UKTs to MAXCPU. 
      If the UKT was not suspended while locked, this call will simply reset a internal variable.

      Internal error if current UKT is not locked by the calling task

      Arguments:	none

      Return value:	none
   */
//  virtual void UnlockUKT() = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:	    SetTransContext

      Description:  Set the trans context hook

      Counterpart to the GetTransContext() function.

      This call allows to establish a hook to a trans context, that should be 
      valid as long as the task lives. It can therefore be a pointer to a stack
      instance of a trans context. This class does not know anything about the
      content of this class (forward decleration above...).

      Arguments:  pTaskTransContext [in] Trans context to set

      Return value:	former trans context
   */
  virtual AK_TransContext *SetTransContext( AK_TransContext *pTaskTransContext ) = 0;

  // --------------------------------------------------------------------------------------------------------------------

  /*! Function:	    GetTransContext

      Description:  Get the current trans context hook

      Counterpart to the SetTransContext() function.

      This call allows to get the current hook to a trans context, that should 
      be valid as long as the task lives. It can therefore be a pointer to a 
      stack instance of a trans context. This class does not know anything about 
      the content of this class (forward decleration above...).

      Arguments:  none

      Return value:	current trans context
   */
  virtual AK_TransContext *GetTransContext( ) = 0;
  /*!
    Function: GetLastErrorMessage
    Description: Get copy of last error message list

    The internal stored message list is only meaningfull, if retrieved after an error
    was reported. You will always retrieve the complete last error message list, including all
    appended messages.
    
    Return value: Last Error Message list set by any error reporting call via SetLastErrorMessage()
   */
  virtual const SAPDBErr_MessageList & GetLastErrorMessage() = 0;

  /*!
    Function: SetLastErrorMessage
    Description: Get copy of last error message list

    The internal stored message list is only meaningfull, if retrieved after an error
    was reported. You will always retrieve the last error occured!

    Arguments: LastErrorMessage [in] Message to set or append
               DoAppend [in] Flag to indicate replacement or appending of LastErrorMessage
   */
  virtual void SetLastErrorMessage(const SAPDBErr_MessageList & LastErrorMessage, SAPDB_Bool DoAppend) = 0;

};

/*!
  EndClass: RTETask_IContext
 */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
#endif  /* RTETASK_ICONTEXT_HPP */
