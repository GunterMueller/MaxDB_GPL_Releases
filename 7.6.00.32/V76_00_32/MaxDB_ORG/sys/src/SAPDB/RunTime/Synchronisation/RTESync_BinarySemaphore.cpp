/*!========================================================================
  @ingroup      Sync
  @file         RTESync_BinarySemaphore.cpp
  @author       StefanP

  @brief        Semaphore class for non-counting semaphores (also called binary semaphores or Events
                                                             respectively)

  @since        2003-11-14  12:11
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

#if !defined (_WIN32)										
#include    "RunTime/RTE_saveUNIXcalls.h"					/*nocheck*/
#include    "RunTime/RTE_UNIXAuthenticate.hpp"				/*nocheck*/
//#if defined (_REENTRANT)
#include    "RunTime/MemoryManagement/RTEMem_Allocator.hpp" /*nocheck*/
//#endif														
#endif														

#include    "RunTime/System/RTESys_SecurityAttributes.hpp"
#include    "RunTime/Synchronisation/RTESync_BinarySemaphore.hpp"
#include    "RunTime/RTE_GlobalNameSpaceNT.h"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Sync_Messages.hpp"
#include    "RunTime/RTE_Messages.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "SAPDBCommon/SAPDB_sprintf.h"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"




extern SAPDBTrace_Topic Sync_Trace; ///<Synchronisation trace
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

RTESync_BinarySemaphore::RTESync_BinarySemaphore ()   
    : m_Handle (RTE_UNDEF_HANDLE),
      m_SemID (RTE_SYNC_UNDEF_SEM_ID)
#if !defined (_WIN32)
	  ,m_EventType (UndefEvent)
//#if defined (_REENTRANT)
	  ,m_pEventUNIX (NULL),
	  m_OwnSemaBuffer (false)
//#endif
#endif
{
    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::RTESync_BinarySemaphore", Sync_Trace, 9);
}

/*---------------------------------------------------------------------------*/

RTESync_BinarySemaphore::~RTESync_BinarySemaphore ()
{
    SAPDBErr_MessageList       messageList;

    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::~RTESync_BinarySemaphore", Sync_Trace, 9);

    if (NoError != Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_CLOSE);
        RTE_Message (messageList);
    }
}

/*---------------------------------------------------------------------------*/

#if defined (_WIN32)
RTESync_BinarySemaphore::SemRC    RTESync_BinarySemaphore::Create   
(
    RTESync_SemaphoreID const       pSemID,
    RTE_SECURITY_ATTRIBUTES  &      secAttribute,   
    SAPDBErr_MessageList &          messageList
)
{
    RTE_SystemRc                    rc = RTE_SYSTEMRC_NO_ERROR;
    SAPDB_UInt4                     nameBufferLength = 0; 
    SemRC                           retCode = NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::Create", Sync_Trace, 9);

    if (NULL != pSemID)
    {
        RTE_BuildGlobalNameSpaceName (pSemID, NULL, &nameBufferLength); //Determine the length needed 
                                                                                   //for the semaphore name
        m_SemID = (SAPDB_Char *)(RTEMem_RteAllocator::Instance().Allocate (nameBufferLength));
        if (NULL == m_SemID)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_ALLOCATE_SEMID,
                                                              SAPDB_ToString (nameBufferLength),
                                                              pSemID);
            return Error;
        }

        RTE_BuildGlobalNameSpaceName (pSemID, m_SemID, &nameBufferLength);
    }


    /* Automatic Reset; Initial state: nonsignaled */
    m_Handle = CreateEvent (&secAttribute, FALSE, FALSE, m_SemID);
  
    if (NULL != m_Handle)
    {
        rc  = GetLastError ();
        if (rc == RTE_SYSTEMRC_ALREADY_EXISTS)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_EXIST,
                NULL == m_SemID ? "UNNAMED" : m_SemID);
            retCode = AlreadyExist;
        }
    }
    else
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_CREATE,
            NULL == m_SemID ? "UNNAMED" : m_SemID, 
            SAPDB_ToString (GetLastError ()));
        retCode = Error;
    }
    
    return retCode;
}

/*---------------------------------------------------------------------------*/

RTESync_BinarySemaphore::SemRC    RTESync_BinarySemaphore::Open   
(
    RTESync_SemaphoreID const       semID,
    SAPDBErr_MessageList &          messageList
)
{
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;
    SAPDB_UInt4             nameBufferLength = 0; 
    SemRC                   retCode = NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::Open", Sync_Trace, 9);

    if (RTE_UNDEF_HANDLE == m_Handle)
    {
        if (NULL != semID)
        {
            RTE_BuildGlobalNameSpaceName (semID, NULL, &nameBufferLength); //Determine the length needed 
            //for the semaphore name
            m_SemID = (SAPDB_Char *)(RTEMem_RteAllocator::Instance().Allocate (nameBufferLength));
            if (NULL == m_SemID)
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_ALLOCATE_SEMID,
                    SAPDB_ToString (nameBufferLength),
                    semID);
                return Error;
            }

            RTE_BuildGlobalNameSpaceName (semID, m_SemID, &nameBufferLength);
        }
        else
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_OPEN_UNNAMED);
            return Error;
        }


        /* Automatic Reset; Initial state: nonsignaled */
        m_Handle = OpenEvent (EVENT_ALL_ACCESS, FALSE, m_SemID);

        if (NULL == m_Handle)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_OPEN,
                m_SemID, 
                SAPDB_ToString (GetLastError ()));
            m_Handle = RTE_UNDEF_HANDLE;
            retCode = Error;
        }
    }
    return retCode;
}

/*---------------------------------------------------------------------------*/

#else

/*
 * _REENTRANT is defined for all UNIX platforms if compiled multithreaded
*/


//***************************************************************************************
//PThread Mutex mit Condition Variable:
//Macht auch für LINUX Sinn, aber bislang nur für scope == PTHREAD_PROCESS_PRIVATE
//***************************************************************************************

RTESync_BinarySemaphore::SemRC    RTESync_BinarySemaphore::Create   
(
    EventUNIX *	const			pEventUNIX,
    SAPDB_Int					scope, 
    SAPDBErr_MessageList &      messageList
)
{
    RTE_SystemRc            rc;

    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::Create", Sync_Trace, 9);

#if defined (_REENTRANT) 
	m_EventType = PThreadMutex;

    //the attribute objects have not to be stored in shared memory. They were only used
    //to initialize the mutex and condition respectively. 
    pthread_mutexattr_t     mutexAttribute;
    pthread_condattr_t      conditionAttribute;

	if (pEventUNIX != NULL)
	{
		m_pEventUNIX = pEventUNIX;
	}
	else
	{
		if (PTHREAD_PROCESS_PRIVATE != scope)
		{
			messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYNC_PMUTEX_MISSING_STORAGE);       
			return Error;
		}

		m_pEventUNIX = new (RTEMem_Allocator::Instance()) EventUNIX (); 
		if (NULL == m_pEventUNIX)
		{
			messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, 
											   "EventUNIX");       
			return Error;
		}

		m_OwnSemaBuffer = true;
	}

	m_pEventUNIX->state = Unposted;
//	m_pEventUNIX->mutex = PTHREAD_MUTEX_INITIALIZER;
//	m_pEventUNIX->condition = PTHREAD_COND_INITIALIZER;

    /* MUTEX */

    /*
    Setting scope of mutex to system (interprocess) or private 
    */

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutexattr_init (&mutexAttribute)))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PMUTEX_ATTR_INIT, 
            RTE_save_strerror (errno));
        return Error;
    }

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutexattr_setpshared (&mutexAttribute,
        scope)))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PMUTEX_ATTR_SET, 
            RTE_save_strerror (errno));
        return Error;
    }

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutex_init (&m_pEventUNIX->mutex, 
															&mutexAttribute)))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PMUTEX_INIT, 
            RTE_save_strerror (errno));

        return Error;
    }


    /* CONDITION */

    /*
    Setting scope of mutex to system (interprocess) or private
    */
    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_condattr_init (&conditionAttribute)))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PCONDITION_ATTR_INIT, 
            RTE_save_strerror (errno));
        return Error;
    }

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_condattr_setpshared (&conditionAttribute, 
        scope)))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PCONDITION_ATTR_SET, 
            RTE_save_strerror (errno));
        return Error;
    }

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_cond_init (&m_pEventUNIX->condition, 
														   &conditionAttribute)))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PCONDITION_INIT, 
            RTE_save_strerror (errno));
        return Error;
    }


	return NoError;
#else
	messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYNC_PMUTEX_MISSING_STORAGE);       
	return Error;
#endif
}

/*---------------------------------------------------------------------------*/

RTESync_BinarySemaphore::SemRC    RTESync_BinarySemaphore::Open   
(
    EventUNIX *	const			pEventUNIX,
    SAPDBErr_MessageList &      messageList
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::Open", Sync_Trace, 9);

	m_EventType = PThreadMutex;

	if (NULL == pEventUNIX)
	{
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PMUTEX_OPEN); 
		return Error;
	}

	m_pEventUNIX = pEventUNIX;

	return NoError;
}

//#endif _REENTRANT

/*---------------------------------------------------------------------------*/

//IPC Semaphoren arbeiten derzeit nur ohne Timeout

//Evtl. Timeout Parameter mit aufnehmen, falls hier Alarm-Handler (!_REENTRANT) oder Timeout-Thread
//aufgesetzt werden muss. Diese sollte aber wahrscheinlich eher an zentraler Stelle geschehen.

RTESync_BinarySemaphore::SemRC    RTESync_BinarySemaphore::Create   
(
    RTE_UserType const              userType,
    RTE_DBName const                serverDB,
    RTE_SECURITY_ATTRIBUTES  &      secAttribute,   
    SAPDBErr_MessageList &          messageList
)
{
    RTE_SystemRc                    rc = RTE_SYSTEMRC_NO_ERROR;
    union semun                     arg ;
    struct semid_ds                 sbuf ;
    RTE_Path                        handleAsString;
    SAPDB_Int4                      sdbOwnerUserId;
    SAPDB_Int4                      dummy;


    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::Create", Sync_Trace, 9);

	m_EventType = IPCSemaphore;

    m_Handle = semget (IPC_PRIVATE, 1, secAttribute);
#if defined(SAPDB_DEBUG_SEMAPHORES) /* normally not defined !!! See RTE_saveUNIXcalls.h */
        if ( m_Handle >= 0 )
        {
            char outputLine[1024];
            int fd = open("/SAP_DB/data/wrk/remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
            if ( fd < 0 )
            {
                fd = open("remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
            }
            if ( fd >= 0 )
            {
                time_t now = time(0);
                ctime_r(&now, outputLine);
                sprintf(outputLine + strlen(outputLine), "Bins: PID %8ld Create Semaphore %12d\n", getpid(), m_Handle);
                write(fd, outputLine, strlen(outputLine));
                close(fd);
            }
        }
#endif

    /* won't use a zero id */
    if (0 == m_Handle)
    {
        SAPDBErr_MessageList                       tmpMessageList;

        //Ignoring any errors while closing
        Close (tmpMessageList);
        /* That we gain a zero again is nearly impossible because */
        /* the ID is not used again as long we have no full turn in the range of usable ID's */
        m_Handle = semget (IPC_PRIVATE, 1, secAttribute); 
#if defined(SAPDB_DEBUG_SEMAPHORES) /* normally not defined !!! See RTE_saveUNIXcalls.h */
        if ( m_Handle >= 0 )
        {
            char outputLine[1024];
            int fd = open("/SAP_DB/data/wrk/remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
            if ( fd < 0 )
            {
                fd = open("remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
            }
            if ( fd >= 0 )
            {
                time_t now = time(0);
                ctime_r(&now, outputLine);
                sprintf(outputLine + strlen(outputLine), "Bins: PID %8ld Create Semaphore %12d\n", getpid(), m_Handle );
                write(fd, outputLine, strlen(outputLine));
                close(fd);
            }
        }
#endif
    }

    if (RTE_UNDEF_HANDLE == m_Handle)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_CREATE, 
            RTE_save_strerror (errno));
        return Error;
    }

    m_SemID = (RTESync_SemaphoreID) m_Handle;

    /* transfer ownership */
    arg.buf = &sbuf ;
    rc = RTE_save_semctl (m_Handle, 0, IPC_STAT, SEMCTL_IPC_STAT(arg));
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_SEMCTL_STAT, 
            RTE_save_strerror(errno));
        return Error;
    }

    if (!RTE_GetDefaultSapdbUserAndGroupIds (sdbOwnerUserId, dummy, messageList))
    {
        return Error;
    }

    if ( sbuf.sem_perm.uid != (uid_t)sdbOwnerUserId )
    {
        sbuf.sem_perm.uid = (uid_t)sdbOwnerUserId;
        arg.buf = &sbuf ;
        rc = RTE_save_semctl (m_Handle , 0 , IPC_SET , SEMCTL_IPC_SET(arg));
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_SEMCTL_SET, 
                RTE_save_strerror(errno));
            return Error;
        }
    }

    SAPDB_sprintf (handleAsString, sizeof(handleAsString), "%d", m_Handle);
    if (!RTE_GetIPCFileName (m_TagFileName, userType, serverDB, Semaphore, handleAsString, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_GET_FILENAME,
                                                          SAPDB_ToString (userType),
                                                          SAPDB_ToString (Semaphore),
                                                          SAPDB_ToString (serverDB),
                                                          handleAsString);
        return Error;
    }
    else
    {
        RTE_FileHandle      handle;

        //No permissions necessary for the tag file because it is not written or read.
        //This file is located in a directory which has world access. Therefore it is
        //no problem to unlink this file from everyone!
        handle = RTE_save_openCreate (m_TagFileName, 
                                      O_CREAT, 
                                      RTESys_SecurityAttributes::Instance().GetLockSA ());
        if (RTE_UNDEF_HANDLE == handle)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_CREATE_ID_FILE,
                m_TagFileName,
                RTE_save_strerror (errno));
            return Error;
        }

        RTE_save_close (handle);
    }

	return NoError;
}

/*---------------------------------------------------------------------------*/

RTESync_BinarySemaphore::SemRC    RTESync_BinarySemaphore::Open   
(
    RTESync_SemaphoreID const       semID,
    RTE_UserType const              userType,
    RTE_DBName const                serverDB,
    SAPDBErr_MessageList &          messageList
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::Open", Sync_Trace, 9);

	m_EventType = IPCSemaphore;

    if (RTE_UNDEF_HANDLE == m_Handle)
    {
        RTE_Path    handleAsString;

        m_SemID = semID;
        m_Handle = (RTE_SemaphoreHandle) semID;
        SAPDB_sprintf (handleAsString, sizeof(handleAsString), "%d", m_Handle);
        if (!RTE_GetIPCFileName (m_TagFileName, userType, serverDB, Semaphore, handleAsString, messageList))
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_GET_FILENAME,
                                                              SAPDB_ToString (userType),
                                                              SAPDB_ToString (Semaphore),
                                                              SAPDB_ToString (serverDB),
                                                              handleAsString);
            return Error;
        }

    }

	return NoError;
}

#endif //_WIN32

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTESync_BinarySemaphore::IsOpen     () 
const
{
#if !defined (_WIN32)
	if (IPCSemaphore == m_EventType)
	{
		return (m_Handle != RTE_UNDEF_HANDLE);
	}
//#if defined (_REENTRANT)
	else if (PThreadMutex == m_EventType)
	{
		return (m_pEventUNIX != NULL);
	}
//#endif
	else
	{
		return false;
	}
#else
	return (m_Handle != RTE_UNDEF_HANDLE);
#endif
}


/*---------------------------------------------------------------------------*/

RTESync_BinarySemaphore::SemRC  RTESync_BinarySemaphore::Post    
(
    SAPDBErr_MessageList &      messageList
)
const
{

    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::Post", Sync_Trace, 9);

#if defined (_WIN32)
    if (!SetEvent (m_Handle))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_POST,
                                                          m_SemID,
                                                          SAPDB_ToString (GetLastError()));
        return Error;
    }
#else

#if defined (_REENTRANT)
if (PThreadMutex == m_EventType)
{
	RTE_SystemRc            rc;
    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutex_lock (&m_pEventUNIX->mutex)))
    {
			messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PMUTEX_LOCK, 
				                                             RTE_save_strerror (errno));
			return Error;
    }

	if (Waiting == m_pEventUNIX->state 
        && RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_cond_signal (&m_pEventUNIX->condition)))
	{
		messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PCONDITION_SIGNAL, 
			                                              RTE_save_strerror (errno));
		return Error;
	}
 
    m_pEventUNIX->state = Posted;

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutex_unlock (&m_pEventUNIX->mutex)))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PMUTEX_UNLOCK, 
                                                          RTE_save_strerror (errno));
        return Error;
    }
}
#endif

if (IPCSemaphore == m_EventType)
{
    union semun   arg;

	arg.val = 1;

    if (RTE_SYSTEMRC_NO_ERROR != RTE_save_semctl (m_Handle, 0 , SETVAL , SEMCTL_SETVAL(arg)))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_POST, 
                                                          SAPDB_ToString (m_SemID),
                                                          RTE_save_strerror (errno));
        return Error;
    }
}

#endif	//_WIN32

    return NoError;
}

/*---------------------------------------------------------------------------*/

RTESync_BinarySemaphore::SemRC    RTESync_BinarySemaphore::Wait
(
    SAPDB_ULong const           semaphoreTimeout,
    SAPDBErr_MessageList &      messageList
)
const
{
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;
    SemRC                   retCode = NoError;

    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::Wait", Sync_Trace, 9);

#if defined (_WIN32)
    switch (WaitForSingleObject (m_Handle, semaphoreTimeout))
    {
    case WAIT_OBJECT_0:
        break;

    case WAIT_TIMEOUT:  
        retCode = Timeout;
        break;

    default: // WAIT_FAILED:    
        rc = GetLastError();       
        break;
    }

    if ( rc != RTE_SYSTEMRC_NO_ERROR )
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_WAIT,
                                                          m_SemID,
                                                          SAPDB_ToString (rc));
        retCode = Error;
    }

#else

#if defined (_REENTRANT)
if (PThreadMutex == m_EventType)
{
//errno abfragen (EINVAL,  

    if (RTE_SYSTEMRC_NO_ERROR != (rc = pthread_mutex_lock (&m_pEventUNIX->mutex)))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PMUTEX_LOCK, 
                                                          RTE_save_strerror (errno));
        return Error;
    }

    if (Posted != m_pEventUNIX->state)
    {
		m_pEventUNIX->state = Waiting;
		if (RTE_INFINITE_WAIT != semaphoreTimeout)
		{
			struct timespec timer;

			timer.tv_sec = time (NULL) + semaphoreTimeout/1000; // semaphoreTimeout is given in msec. Therefore
			timer.tv_nsec = 0;                         // we have to transform it to seconds first
        
			rc =  pthread_cond_timedwait (&m_pEventUNIX->condition, 
		                                  &m_pEventUNIX->mutex, 
		                                  &timer);
		}
		else //Wait time infinite
		{
			rc =  pthread_cond_wait (&m_pEventUNIX->condition, 
			                         &m_pEventUNIX->mutex);
		}

		if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            //The semaphore may be posted even if a timeout has occurred
            if (ETIMEDOUT == rc && Posted != m_pEventUNIX->state)
            {
                retCode = Timeout;
            }
            else if (RTE_SYSTEMRC_NO_ERROR != rc)
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PCONDITION_WAIT_SEM, 
                                                                  RTE_save_strerror (errno));
                retCode = Error;
            }
        }
    }

    m_pEventUNIX->state = Unposted;

    if (RTE_SYSTEMRC_NO_ERROR != (rc =  pthread_mutex_unlock (&m_pEventUNIX->mutex)))
    {
		if (Error == retCode)
		{
			messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PMUTEX_UNLOCK, 
                                                          RTE_save_strerror (errno));
		}
		else
		{
  			messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_PMUTEX_UNLOCK, 
                                                          RTE_save_strerror (errno));
		}

        retCode = Error;
    }
}
else // IPCSemaphore == m_EventType
#endif
if ( IPCSemaphore == m_EventType )
{
	//Timeout with IPC Semaphores not yet implemented
	if (RTE_INFINITE_WAIT != semaphoreTimeout)
	{
		messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_NO_TIMEOUT);
		retCode = Error; 
	}
	else
	{
	    struct sembuf       semWait;

		semWait.sem_num = 0;
		semWait.sem_op  = -1;
		semWait.sem_flg = SEM_UNDO;

		if ( RTE_SYSTEMRC_NO_ERROR != RTE_save_semop (m_Handle, &semWait, 1))
		{
			if (EIDRM == errno)
			{
				retCode = Removed;
			}
			else
			{
				messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_WAIT,
					                                             SAPDB_ToString (m_SemID),
						                                         RTE_save_strerror (errno));
				retCode = Error;
			}
		}
	}
}

#endif //_WIN32

    return retCode;
}

/*---------------------------------------------------------------------------*/

RTESync_BinarySemaphore::SemRC    RTESync_BinarySemaphore::Close   
(
     SAPDBErr_MessageList &      messageList
)
{
    SAPDBErr_MessageList            tmpMessageList;
    SemRC                           retCode = NoError;


    SAPDBTRACE_METHOD_DEBUG ("RTESync_BinarySemaphore::Close", Sync_Trace, 9);

#if !defined (_WIN32)
#if defined (_REENTRANT)
	if (NULL != m_pEventUNIX)
	{
//Es ist noch unklar, ob mutex und cond auf beiden Seiten destroiert werden dürfen. (->Testprogramm)
//Auf jeden Fall sind Returncodes zu beachten: Ist die Mutex gerade im Zugriff, so ist errno == EBUSY
//Ist die Mutex von der anderen Seite bereits destruiert, so erhalten wir wahrscheinlich den Fehler
//errno == EINVAL. Dieser kann dann eigentlich ignoriert werden.
//Das selbe gilt für die Condition.

		pthread_mutex_destroy (&m_pEventUNIX->mutex);
		pthread_cond_destroy  (&m_pEventUNIX->condition);

		if (m_OwnSemaBuffer)
		{
			destroy (m_pEventUNIX, RTEMem_Allocator::Instance());
		}
	}
	m_pEventUNIX = NULL;
	m_OwnSemaBuffer = false;
#endif

	m_EventType = UndefEvent;

#else
    if (RTE_SYNC_UNDEF_SEM_ID != m_SemID)
    {
        RTEMem_RteAllocator::Instance().Deallocate (m_SemID);
    }
#endif

	if (RTE_UNDEF_HANDLE != m_Handle) 
    {
#if defined (_WIN32)
        if ( !CloseHandle (m_Handle))
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_CLOSE_HANDLE, 
                                                              m_SemID,
                                                              SAPDB_ToString (GetLastError()));
            retCode = Error;
        }

#else
        union semun arg;
        arg.val = 0;


        if (RTE_SYSTEMRC_NO_ERROR != RTE_save_semctl (m_Handle, 0 , IPC_RMID, SEMCTL_SETVAL(arg)))
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_CLOSE_HANDLE, 
                                                              SAPDB_ToString (m_SemID),
                                                              RTE_save_strerror (errno));
//            retCode = Error;
        }

        if (RTE_SYSTEMRC_NO_ERROR != RTE_save_unlink (m_TagFileName))
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_SYNC_SEM_UNLINK, 
                                                                    m_TagFileName,
                                                                    RTE_save_strerror (errno));
//            retCode = Error;
        }

#endif
    }

    m_Handle = RTE_UNDEF_HANDLE;
    m_SemID = RTE_SYNC_UNDEF_SEM_ID;


    if (NoError != retCode)
    {
        messageList = tmpMessageList;
    }

    return retCode;
}






