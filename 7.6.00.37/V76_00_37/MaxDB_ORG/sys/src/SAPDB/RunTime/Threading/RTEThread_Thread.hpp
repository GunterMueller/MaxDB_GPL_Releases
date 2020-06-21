/*!========================================================================

  @file         RTEThread_Thread.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Thread base class for mutithreading apllication.

                The following class allows to create and manage threads.
                The own thread class must be derived from this base class 
                and has to implement the 'Main' methode.

  @since        2005-01-21  9:05
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


#ifndef RTETHREAD_THREAD_HPP
#define RTETHREAD_THREAD_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "geo07.h"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/Synchronisation/RTESync_Spinlock.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define UNDEF_THREAD_PRIO    -1

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/




/*!---------------------------------------------------------------------------
  @class   RTEThread_Thread
  @brief   Thread base class for mutithreading apllication.

           The following class allows to create and manage threads.
           The own thread class must be derived from this base class 
           and has to implement the 'ThreadMain' and the 'DestroyObject' methode.
 ----------------------------------------------------------------------------*/
class RTEThread_Thread
{
public:
    /*!
    @brief  Thread creation flags
    */
    enum CreationFlags
    {
        Normal       = THR_CREATE_NORMAL_EO07,         // Create a thread which starts immediately
        Suspended    = THR_CREATE_SUSPENDED_EO07,      // Create a thread which is suspended after startup
        ProcessScope = THR_CREATE_PROCESS_SCOPE_EO07   // Set thread scope to process (UNIX only)
    };


    /*!
    @brief  Error return values
    */
    enum ThrdRet
    {
        NoError        = THR_OK_EO07,              // No error.
        NotImplemented = THR_NOT_IMPLEMENTED_EO07, // Functionality not implemented
        Timeout        = THR_TIMEOUT_EO07,         // Timeout occured
        ThreadNotFound = THR_NO_DATA_FOUND_EO07,   // The requested thread could not be found
        OutOfMemory,                               // Out of memory error
        NotJoinable,                               // Thread is not joinable 
        CommonError                                // common error  
    };

public:
    /*!
       @brief          Constructor
       @param          threadName [in]          - Name of thread
       @param          initializeThreading [in] - Initialize threading if not already done
     */
    RTEThread_Thread( SAPDB_Char const * const  threadName,
                      SAPDB_Bool                initializeThreading = true );

    /*!
       @brief          Constructor
     */
    virtual ~RTEThread_Thread();


    /*!
    @brief          Creates (starts) a thread

                    The created thread runs the code of the methode 'ThreadMain'
                    implemented by the derived class. If the creating thread instance
                    does not need any information of the started thread it MUST call
                    'Forget' to to reclaim resources. Another way to give up resource and
                    to get the created threads return value is to call the 'Join' methode.
                    This methode blocks until the thread has terminated by calling 
                    'EndThread' or has returned from 'ThreadMain'.

                    If this object instance has been created on a heap, an easy way to
                    free the entire object is to implement the 'DestroyObject' methode.
                    After a thread has terminated an the creator has called 'Forget'
                    or 'Join' the 'DestroyObject' methode will automatically executed
                    to free the object.  This methode should call the heap free
                    function with the this pointer like the COM Release mechanism.
                    Keep in mind, from now no object access is possible. 
            
    @param          flags        [in]     - Flags that control the creation of the thread. 
                                            If this value is Suspended, the thread is 
                                            created in a suspended state, and will not run
                                            until the 'Resume' methode is called. 
                                            If this value is Normal, the thread runs
                                            immediately after creation. 
    @param          stackSize    [in]     - Initial size of the stack, in bytes.
    @param          priority     [in]     - Set the threads priority ( Refer to 'SetPriority' )
    @param          messageList  [out]    - Error messages

    @return         NoError, if the thread has started successfully
    */
    ThrdRet                     Create ( CreationFlags         flags,
                                         SAPDB_ULong           stackSize,
                                         SAPDB_Int4            priority,
                                         SAPDBErr_MessageList  &messageList );

    /*!
    @brief          Creates (starts) a thread

                    The created thread runs the code of the methode 'ThreadMain'
                    implemented by the derived class. 
            
    @param          flags        [in]     - Flags that control the creation of the thread. 
                                            If this value is Suspended, the thread is 
                                            created in a suspended state, and will not run
                                            until the 'Resume' methode is called. 
                                            If this value is Normal, the thread runs
                                            immediately after creation. 
    @param          stackSize    [in]     - Initial size of the stack, in bytes.
    @param          messageList  [out]    - Error messages

    @return         NoError, if the thread has started successfully
    */
    ThrdRet                     Create ( CreationFlags         flags,
                                         SAPDB_ULong           stackSize,
                                         SAPDBErr_MessageList  &messageList )
    {
        return Create( flags, stackSize, UNDEF_THREAD_PRIO, messageList );
    }


    /*!
    @brief          Creates (starts) a thread

                    The created thread runs the code of the methode 'ThreadMain'
                    implemented by the derived class. 
            
    @param          stackSize    [in]     - Initial size of the stack, in bytes.
    @param          messageList  [out]    - Error messages

    @return         NoError, if the thread has started successfully
    */
    ThrdRet                     Create ( SAPDB_ULong           stackSize,
                                         SAPDBErr_MessageList  &messageList )
    {
        return Create( Normal, stackSize, UNDEF_THREAD_PRIO, messageList );
    }


    /*!
    @brief          Allows a thread to suspend itself.

                    NOTE: Only the thread itself can suspend itself.
                          In UNIX environment (POSIX Threads) threads are 
                          NOT allowed to suspend other threads but themself.

    @param          messageList  [out] - Error messages

    @return         NoError, if the thread has been supended successfully
    */
    ThrdRet                     Suspend( SAPDBErr_MessageList  &messageList );

    /*!
    @brief          Allows to resume a formerly suspended thread.
                    NOTE: If the given thread is not in suspended state
                          resume does not have any effect, but a subsequent
                          Suspend will return with an error.

    @param          messageList  [out] - Error messages

    @return         NoError, if the thread has resumed it's work
    */
    ThrdRet                     Resume ( SAPDBErr_MessageList  &messageList );


    /*!
    @brief          Modify the thread priority

                    Windows NT:    THREAD_PRIORITY_ABOVE_NORMAL  Indicates 1 point above normal priority
                                   THREAD_PRIORITY_BELOW_NORMAL  Indicates 1 point below normal priority
                                   THREAD_PRIORITY_HIGHEST       Indicates 2 points above normal priority
                                   THREAD_PRIORITY_IDLE          Indicates a base priority level of 1
                                   THREAD_PRIORITY_LOWEST        Indicates 2 points below normal priority
                                   THREAD_PRIORITY_NORMAL        Indicates normal priority for the priority class
                                   THREAD_PRIORITY_TIME_CRITICAL Indicates a base priority level of 15
                    UNIX/PTHREADS: absolute value   1..127 (1 least favoured, 127 most favoured)
                    UNIX/CTHREADS: absolute value   0..127 (0 least favoured, 127 most favoured)

    @param          priority     [in]     - The threads priority
    @return         true, if thread has suspended itself
    */
    SAPDB_Bool                  SetPriority( SAPDB_Int4       priority );


    /*!
    @brief          Returned TID uniquely identifies the given thread within process.
    @return         The unique thread id of the given thread or 0 if no such thread.
    */
    SAPDB_UInt4                 ID();


    /*!
    @brief          Returnes the name of the thread object
    @return         Thread object name.
    */
    SAPDB_Char const * const    GetThreadName() const 
    {
        return m_ThreadName;
    }

    /*!
    @brief          Allows a thread to reschedule itself.
    */
    void                        YieldThread();

    /*!
    @brief          Allows to check whether a thread is still running.
    @return         true, if thread is alive
    */
    SAPDB_Bool                  IsAlive();

    /*!
    @brief          Allows a thread to terminate itself.

    @param          returncode   [in]     - Returncode received by 'Join' 
    */
    void                        EndThread( SAPDB_Int4         returnCode );


    /*!
    @brief          Allows to cancel the thread.

                    Canceling has the same effect as an 'EndThread' call of 
                    the cancelled thread.

                    NOTE: Cancelling effectivly happens if the thread reaches a
                          cancelpoint, which is normally a call to 'YieldThread' 
                          or a system call.

    @param          returncode   [in]     - Returncode received by 'Join' 
                                           (Used in Windows only)
    @param          messageList  [out]    - Error messages

    @return         NoError, if the cancel request was send successfully
    */
    ThrdRet                     Cancel( SAPDB_Int4            returncode,
                                        SAPDBErr_MessageList  &messageList );

    /*!
    @brief          Allows to send a kill request to the thread.

                    Replaces kill(SIGTERM) function. In Windows environment
                    this kill terminates the thread.

    @param          messageList  [out]    - Error messages

    @return         NoError, if the kill request was send successfully
    */
    ThrdRet                     Kill( SAPDBErr_MessageList  &messageList );


    /*!
    @brief          Detach from thread.

                    If the creating thread instance does not need any information 
                    of the started thread it MUST call 'Forget' to reclaim resources. 
                    Another way to give up resource and to get the created threads 
                    return value is to call the 'Join' methode.

                    NOTE: After this call the use of thread object except of the 
                    running thread is a fatal mistake...
    */
    void                        Forget();

    /*!
    @brief          Allows to join a formerly created thread.

    @param          returncode   [out]    - Exit status of the thread
    @param          messageList  [out]    - Error messages

    @return         NoError, if the thread has joined successfully
    */
    ThrdRet                     Join( SAPDB_Int4            &returncode,
                                      SAPDBErr_MessageList  &messageList );

    /*!
    @brief          Return pointer to thread object
    @return         if called within code that is called from RTEThread derived class pointer to 'this', else 0
     */
    static RTEThread_Thread    *Self();

protected:
    /*!
    @brief          The created thread runs the code of the methode 'ThreadMain'
                    implemented by the derived class. 
    */
    virtual SAPDB_Int4  ThreadMain() = 0;

    /*!
    @brief          Destroy the entire object

                    If this object instance has been created on a heap, an easy way to
                    free the entire object is to implement the 'DestroyObject' methode.
                    After a thread has terminated an the creator has called 'Forget'
                    or 'Join' the 'DestroyObject' methode will automatically executed
                    to free the object.  This methode should call the heap free
                    function with the this pointer like the COM Release mechanism.
                    Keep in mind, from now no object access is possible. 
    */
    virtual void  DestroyObject() {};

#if defined (_WIN32)    
    /*!
    @brief          Exception Filter

                    If an exception occurs during execution of the thread main methode or 
                    in any routine the thread main methode calls, the the filter methode
                    is called to determine how the exception is handled. 

    @param          pExceptPtrs  [in]  - The machine-independent description of an exception, 
                                         and information about the machine state that existed
                                         for the thread when the exception occurred.

    @return         There are three  posible return values: 

                        EXCEPTION_CONTINUE_EXECUTION  Exception is dismissed. 
                                                      Continue execution at the 
                                                      point where the exception occurred. 

                        EXCEPTION_CONTINUE_SEARCH     Exception is not recognized. Continue 
                                                      to search up the stack for a handler, 
                                                      first for containing try-except 
                                                      statements, then for handlers with 
                                                      the next highest precedence. 

                        EXCEPTION_EXECUTE_HANDLER     Exception is recognized. 
                                                      Transfer control to the exception 
                                                      handler by executing the 
                                                      'ExceptionHandler' methode,
                                                      then leaving the thread main. 
    */
    virtual DWORD ExceptionFilter( LPEXCEPTION_POINTERS pExceptPtrs ) 
    {
        return EXCEPTION_CONTINUE_SEARCH;  // Find the next try/except-block. 
                                           // The handler methode is not called!!
    };

    /*!
    @brief          Exception Handler

                    Is execute if an exception occurs and the 'ExceptionFilter' methode
                    is leaving with 'EXCEPTION_EXECUTE_HANDLER'.

    @return         Return code of the thread. 
    */
    virtual SAPDB_Int4  ExceptionHandler() { return 0; };

#endif


private:
    static void* CThreadMain( void*  pThis );

    // don't allow copies
    inline   RTEThread_Thread( const RTEThread_Thread &c ) 
         : m_ThreadName(c.m_ThreadName) {;}

    static SAPDB_Bool             initializeThreadingDone;   // true if the threading is aready initialized

private:
    SAPDB_Bool                    m_InitializeThreading;     // initialize threading if not already done
    RTESync_Spinlock              m_Lock;                    // used for locked scopes
    SAPDB_Int4                    m_StartPriority;
    struct _teo07_Thread *        m_pThread;
    SAPDB_Char const * const      m_ThreadName;
    SAPDB_Bool                    m_Detached;         // creator has detached this thread 
    SAPDB_Bool                    m_Stopped;          // thread has stopped
};


/*!
  @brief convert RTEThread_Thread::ThrdRet into readable string
  @param rc [in] the return value to convert
 */
inline SAPDB_ToStringClass SAPDB_ToString(RTEThread_Thread::ThrdRet     rc)
{
    switch (rc)
    {
    case RTEThread_Thread::NoError: 
        return SAPDB_ToStringClass("No error");
    case RTEThread_Thread::NotImplemented: 
        return SAPDB_ToStringClass("Not implemented");
    case RTEThread_Thread::Timeout: 
        return SAPDB_ToStringClass("Timeout");
    case RTEThread_Thread::ThreadNotFound: 
        return SAPDB_ToStringClass("Thread not found");
    default:                          
        break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)rc); 
}

#endif  /* RTETHREAD_THREAD_HPP */
