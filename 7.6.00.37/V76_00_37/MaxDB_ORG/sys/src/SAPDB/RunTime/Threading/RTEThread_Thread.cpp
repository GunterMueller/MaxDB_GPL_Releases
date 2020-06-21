/*!========================================================================

  @file         RTEThread_Thread.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Thread base class for mutithreading apllication.

                The following class allows to create and manage threads.
                The own thread class must be derived from this base class 
                and has to implement the 'Main' methode.

  @since        2005-10-27  11:42
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "heo07.h"
#include    "RunTime/RTE_Message.hpp"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/RTE_Thread_Messages.hpp"
#include    "RunTime/Threading/RTEThread_Thread.hpp"

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
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

// true if the threading is aready initialized
SAPDB_Bool   RTEThread_Thread::initializeThreadingDone = false;


RTEThread_Thread::RTEThread_Thread( SAPDB_Char const * const  threadName,
                                    SAPDB_Bool                initializeThreading )
                 : m_Lock(0),
                   m_ThreadName( threadName ),
                   m_StartPriority(UNDEF_THREAD_PRIO),
                   m_pThread(0),
                   m_Detached(false),
                   m_Stopped(false),
                   m_InitializeThreading(initializeThreading)
{
}

/*---------------------------------------------------------------------------*/

RTEThread_Thread::~RTEThread_Thread()
{
}

/*---------------------------------------------------------------------------*/

RTEThread_Thread::ThrdRet 
    RTEThread_Thread::Create( CreationFlags         flags,
                              SAPDB_ULong           stackSize,
                              SAPDB_Int4            priority,
                              SAPDBErr_MessageList  &messageList )
{
    tsp00_ErrTextc  threadErrtext;
    teo07_ThreadErr threadErr;

    if ( m_pThread )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_ALREADY_RUNNING, 
                                            SAPDB_ToString (m_ThreadName) );
        return CommonError;
    }

    if (( true == m_InitializeThreading ) && ( false == initializeThreadingDone ))
    {
        initializeThreadingDone = true;
        teo07_Thread    thread;

        sqlinit_multithreading( &thread, threadErrtext, &threadErr );

        if ( threadErr != THR_OK_EO07 )
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_INIT, 
                                                threadErrtext );
            return static_cast<ThrdRet>(threadErr);
        }
    }

    sqlbeginthread( stackSize, (teo07_ThreadCallback)CThreadMain, (void*)this,
                    flags, &m_pThread, threadErrtext, &threadErr );

    if ( threadErr != THR_OK_EO07 )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_START, 
                                            SAPDB_ToString (m_ThreadName),
                                            threadErrtext );
        return static_cast<ThrdRet>(threadErr);
    }

    return NoError;
}

/*---------------------------------------------------------------------------*/

void RTEThread_Thread::EndThread( SAPDB_Int4         returnCode )
{
    m_Lock.Lock();

    if ( m_Detached )
    {
        m_Lock.Unlock(); // do a unlock before destroy object
        DestroyObject ();
    }
    else
    {
        m_Stopped = true;  // signal that the thread has stopped
        m_Lock.Unlock();   // do a unlock before end thread
    }

    sqlendthread( returnCode );
}

/*---------------------------------------------------------------------------*/

void* RTEThread_Thread::CThreadMain( void*  pThis )
{
    SAPDB_Int4          returnCode;
    RTEThread_Thread&   This = *reinterpret_cast<RTEThread_Thread*>(pThis);

#if defined (_WIN32)    
    __try 
    {
#       if defined(_FASTCAP)
         CAPNameThread (GetThreadName());
#       endif
#endif
         sqlsetthreadclasspointer(This.m_pThread, pThis);

         if ( This.m_StartPriority != UNDEF_THREAD_PRIO )
             sqlsetthreadpriority ( This.m_pThread, This.m_StartPriority ); 

         returnCode = This.ThreadMain();

#if defined (_WIN32)
    }
    __except( This.ExceptionFilter( GetExceptionInformation() ) ) 
    {
        returnCode = This.ExceptionHandler();
    }
#endif

    This.EndThread( returnCode ); // We will never reach the following return statement!

    return (char*)0 + returnCode; 
}


/*---------------------------------------------------------------------------*/

RTEThread_Thread::ThrdRet 
    RTEThread_Thread::Resume( SAPDBErr_MessageList  &messageList )
{
    tsp00_ErrTextc  threadErrtext;
    teo07_ThreadErr threadErr;

    sqlresumethread( m_pThread, threadErrtext, &threadErr );

    if ( threadErr != THR_OK_EO07 )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_RESUME,
                                            SAPDB_ToString (m_ThreadName),
                                            threadErrtext );
        return static_cast<ThrdRet>(threadErr);
    }

    return NoError;
}

/*---------------------------------------------------------------------------*/

RTEThread_Thread::ThrdRet 
    RTEThread_Thread::Suspend( SAPDBErr_MessageList  &messageList )
{
    tsp00_ErrTextc  threadErrtext;
    teo07_ThreadErr threadErr;

    sqlsuspendthread( m_pThread, threadErrtext, &threadErr );

    if ( threadErr != THR_OK_EO07 )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_SUSPEND,
                                            SAPDB_ToString (m_ThreadName),
                                            threadErrtext );
        return static_cast<ThrdRet>(threadErr);
    }

    return NoError;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEThread_Thread::SetPriority( SAPDB_Int4 priority )
{
    return 1 == sqlsetthreadpriority( m_pThread, priority ) == THR_OK_EO07;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEThread_Thread::ID()
{
    return static_cast<SAPDB_UInt4>(sqlthreadid(m_pThread));
}

/*---------------------------------------------------------------------------*/

void RTEThread_Thread::YieldThread()
{
    sqlyieldthread();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEThread_Thread::IsAlive()
{
    teo07_ThreadErr threadErr;
    tsp00_ErrTextc  threadErrtext;

    return 0 != sqlthreadalive( m_pThread, threadErrtext, &threadErr ) && false == m_Stopped;
}

/*---------------------------------------------------------------------------*/

RTEThread_Thread::ThrdRet 
    RTEThread_Thread::Cancel( SAPDB_Int4            returncode,
                              SAPDBErr_MessageList  &messageList )
{
    tsp00_ErrTextc  threadErrtext;
    teo07_ThreadErr threadErr;

    sqlcancelthread( m_pThread, returncode, threadErrtext, &threadErr );

    if ( threadErr != THR_OK_EO07 )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_CANCEL,
                                            SAPDB_ToString (m_ThreadName),
                                            threadErrtext );
        return static_cast<ThrdRet>(threadErr);
    }

    return NoError;
}

/*---------------------------------------------------------------------------*/

RTEThread_Thread::ThrdRet 
    RTEThread_Thread::Kill( SAPDBErr_MessageList  &messageList )
{
    tsp00_ErrTextc  threadErrtext;
    teo07_ThreadErr threadErr;

    sqlkillthread( m_pThread, threadErrtext, &threadErr );

    if ( threadErr != THR_OK_EO07 )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_KILL,
                                            SAPDB_ToString (m_ThreadName),
                                            threadErrtext );
        return static_cast<ThrdRet>(threadErr);
    }

    return NoError;
}

/*---------------------------------------------------------------------------*/

RTEThread_Thread::ThrdRet 
    RTEThread_Thread::Join( SAPDB_Int4            &returncode,
                            SAPDBErr_MessageList  &messageList )
{

    if ( m_Detached )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_NOT_JOINABLE,
                                            SAPDB_ToString (m_ThreadName) );
        return NotJoinable;
    }

    tsp00_ErrTextc  threadErrtext;
    teo07_ThreadErr threadErr;

    sqljointhread( m_pThread, &returncode, threadErrtext, &threadErr );

    if ( threadErr == THR_OK_EO07 )
    {
        DestroyObject ();
    }
    else
    {
        // the join operation failed, but we do not enforce retry!!!!

        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_THREAD_JOIN,
                                            SAPDB_ToString (m_ThreadName),
                                            threadErrtext );
        SAPDB_Bool stopped;

        // inform the thread, that we do no longer take responsibility for object destruction...
        { // this scope is locked
            RTESync_LockedScope Lock( m_Lock );
            m_Detached         = true;    // signal that we have detached this thread
            stopped            = m_Stopped;
        }

        // the join operation failed, but we take care about the object if itself already stopped
        if ( stopped )
        {
            DestroyObject ();
        }
    }

    return static_cast<ThrdRet>(threadErr);
}

/*---------------------------------------------------------------------------*/

RTEThread_Thread *RTEThread_Thread::Self()
{
    struct _teo07_Thread *pOwnThread = sqlgetthread();
    return ( pOwnThread ? (RTEThread_Thread *)sqlgetthreadclasspointer(pOwnThread) : (RTEThread_Thread *)0 );
}

/*---------------------------------------------------------------------------*/

void  RTEThread_Thread::Forget()
{
    tsp00_ErrTextc              threadErrtext;
    teo07_ThreadErr             threadErr;

    if ( m_Detached )
    {
        // ignore repeated Forget() calls...
        return;
    }

    SAPDB_Bool stopped;
    // we could check the alive state of the thread before entering scope, but we could not catch an 'unwanted'
    // exitthread ... So this is nasty, but it does not slow us down too much... And it works!
    {
        RTESync_LockedScope Lock( m_Lock );
        m_Detached         = true;    // signal that we have detached this thread
        stopped            = m_Stopped || (0 == sqlthreadalive( m_pThread, threadErrtext, &threadErr ));
    }

    sqlforgetthread( m_pThread );

    if ( stopped )
    {
        DestroyObject ();
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
