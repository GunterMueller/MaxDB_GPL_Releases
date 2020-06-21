/*!
  @file           RTE_ActiveDatabase.cpp
  @author         JoergM, RobinW
  @brief          DBM: SAPDB Instance and Database Registration and Management Interfaces
  @see

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
---------------------------------------------------------------------*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hsp100.h"

#ifdef _WIN32            /*&if $OSSPEC = WIN32*/
#  include "gos00k.h"    /* nocheck but_imkdep */
#  if defined CLUSTER_SERVER_VERSION
#    include "gos212.h"  /* nocheck but_imkdep */
#  endif           

#else                   /*&else*/
/*! define */
#  define USE_KGS
#  include "gen34.h"                                /* nocheck but_imkdep */
#  include "geo002.h"                               /* nocheck but_imkdep */
#  include "RunTime/RTE_PidListForExecutable.hpp"   /* nocheck but_imkdep */
#endif                  /*&endif*/

#include "RunTime/System/RTESys_SecurityAttributes.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Messages.hpp"
#include "RunTime/RTE_Database.hpp"
#include "RunTime/RTE_ActiveDatabase.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Communication/RTEComm_PacketHeader.h"
#include "RunTime/RTE_DBRegister.hpp"
#include "Messages/Msg_List.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "heo01.h"

#ifdef _WIN32                             /*&if $OSSPEC = WIN32*/
#  include "RunTime/RTE_NTService.hpp"    /* nocheck */
#endif                                    /*&endif*/

extern "C"  // this is just for the sqlx*() wrappers
{
#include "heo05.h"
}

// this for writing the state of the kernel process to the dbm protocol if the kernel process cannot be stopped
// happens on HPUX only and is very platform specific
#ifdef HPUX             /*&if $MACH in [ PA11, PA20W, HP_IA64 ]*/
#include <sys/pstat.h>
#include "hin100.h"     /* nocheck but_imkdep */
#endif                  /*&endif*/
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*! global ERROR state */
static RTE_Database::DBStatusInfo dbstateError(           0,                     (const SAPDB_UTF8 *)"ERROR");
/*! global OFFLINE state */
static RTE_Database::DBStatusInfo dbstateOffline(         SERVER_OFFLINE,        (const SAPDB_UTF8 *)"OFFLINE");
/*! global STARTING state */
static RTE_Database::DBStatusInfo dbstateStarting(        SERVER_STARTING,       (const SAPDB_UTF8 *)"STARTING");
/*! global ADMIN state */
static RTE_Database::DBStatusInfo dbstateAdmin(           SERVER_COLD,           (const SAPDB_UTF8 *)"ADMIN");
/*! global STANDBY state */
static RTE_Database::DBStatusInfo dbstateStandby(         SERVER_STANDBY_EO00,   (const SAPDB_UTF8 *)"STANDBY");
/*! global ONLINE state */
static RTE_Database::DBStatusInfo dbstateOnline(          SERVER_WARM,           (const SAPDB_UTF8 *)"ONLINE");
/*! global SHUTDOWN state */
static RTE_Database::DBStatusInfo dbstateShutdown(        SERVER_SHUTDOWN,       (const SAPDB_UTF8 *)"SHUTDOWN");
/*! global SHUTDOWNREINIT state */
static RTE_Database::DBStatusInfo dbstateShutdownReinit(  SERVER_SHUTDOWNREINIT, (const SAPDB_UTF8 *)"SHUTDOWNREINIT");
/*! global SHUTDOWNKILL state */
static RTE_Database::DBStatusInfo dbstateShutdownKill(    SERVER_SHUTDOWNKILL,   (const SAPDB_UTF8 *)"SHUTDOWNKILL");
/*! global STOP state */
static RTE_Database::DBStatusInfo dbstateStop(            SERVER_STOP,           (const SAPDB_UTF8 *)"STOP");
/*! global KILL state */
static RTE_Database::DBStatusInfo dbstateKill(            SERVER_KILL,           (const SAPDB_UTF8 *)"KILL");
#ifdef _WIN32
/*! global ABORT state */
static RTE_Database::DBStatusInfo dbstateAbort(           SERVER_ABORT,          (const SAPDB_UTF8 *)"ABORT");
/*! global STOPPED state */
static RTE_Database::DBStatusInfo dbstateStopped(         SERVER_STOPPED,        (const SAPDB_UTF8 *)"STOPPED");
/*! global UNKNOWN state */
static RTE_Database::DBStatusInfo dbstateUnknown(         SERVER_UNKOWN,         (const SAPDB_UTF8 *)"UNKNOWN");
#else
/*! global UNKNOWN state */
static RTE_Database::DBStatusInfo dbstateUnknown(         0,                     (const SAPDB_UTF8 *)"UNKNOWN");
#endif

/*! start/stop service polling timeout */
#define TIMEOUT_                   240
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

static SAPDB_Char *oneCharacterPointer; ///< not thread save...ask RobinW...
/*! ipc elements are identified by names like \<ipc-ident\>\<dbName\>
   this macros builds a name like this from a given ipc-ident and given dbName */
#define CONCATENATED_STRING(first,second) (SAPDB_Char *)(oneCharacterPointer=(SAPDB_Char *)alloca(strlen(first)+strlen(second)+1), SAPDB_sprintf(oneCharacterPointer,(int)(strlen(first)+strlen(second)+1),"%s%s",first,second), oneCharacterPointer)


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

const RTE_Database::DBStatusInfo* RTE_ActiveDatabase::GetDBStatus(Msg_List &errList)
{
    return ( UpdateStatus(errList) ? &m_currentState : (const DBStatusInfo *)0 );
}

//----

const RTE_Database::DBStatusInfo* RTE_ActiveDatabase::GetExtendedDBStatus( Kernel_SharedState const *&pExtendedState
                                                                         , Msg_List &errList )
{
    if ( UpdateStatus(errList) )
    {
        switch( m_currentState.statusId )
        {
        case 0:
        case SERVER_STARTING:
        case SERVER_OFFLINE:
        case SERVER_STOP:
        case SERVER_KILL:
#ifdef _WIN32
        case SERVER_STOPPED:
        case SERVER_ABORT:
#endif
            pExtendedState = 0;
            break;
        default:
            pExtendedState = &m_SharedStateCopy;
            break;
        }
        return &m_currentState;
    }
    else
    {
        pExtendedState = 0;
        return (const DBStatusInfo *)0;
    }
}

//----

/* PTS 1129710 */
void RTE_ActiveDatabase::UpdateSharedStateCopy( Kernel_SharedState &externalState
                                              , SAPDB_ULong        &externalPendingUpdateCounter
                                              , SAPDB_ULong        &externalCompletedUpdateCounter
                                              , SAPDB_UInt4         externalStateSize)
{
    SAPDB_Bool needUpdate;

    if ( externalStateSize == 0 )
    {
        return;
    }

    RTESys_ReadMemoryBarrier();
    needUpdate = ( 0 == m_SharedStateSize
                || m_UpdateCounter != externalCompletedUpdateCounter
                || m_UpdateCounter != externalPendingUpdateCounter );

    if ( 0 == m_SharedStateSize )
    {
        if ( externalStateSize > sizeof(Kernel_SharedState) )
        {
            m_SharedStateSize = sizeof(Kernel_SharedState);
        }
        else
        {
            m_SharedStateSize = externalStateSize;
        }
    }

    if ( needUpdate )
    {
        // this code tries not only to do the copy, but verify that a second copy still gets the same result...
        do
        {
            RTESys_ReadMemoryBarrier();

            m_UpdateCounter = externalCompletedUpdateCounter;

            // first local copy
            SAPDB_memcpy( &m_SharedStateCopy, &externalState, m_SharedStateSize);

            RTESys_ReadMemoryBarrier();

        } while ( m_UpdateCounter != externalCompletedUpdateCounter 
               || m_UpdateCounter != externalPendingUpdateCounter );
    }
}

//----

#ifdef _WIN32



APIRET RTE_ActiveDatabase::startService ( RTE_SpeedInfo &speed
                                        , SAPDB_Bool forceAutorestart
                                        , SAPDB_Bool lowPriority
                                        , SAPDB_Int  argc // currently not used
                                        , SAPDB_Char **argv // currently not used
                                        , Msg_List &errList)
{
	APIRET rc = NO_ERROR;

    CHAR   szOptions[20];
    SAPDB_sprintf ( szOptions,
                    sizeof(szOptions),"%d",
                    (forceAutorestart ? FORCE_AUTORESTART : IGNORE_AUTORESTART)
                  | (lowPriority ? LOW_PROCESS_PRIORITY : 0) );

    RTE_ClientDiagPipe diagPipe;
    diagPipe.create(m_dbName);

    RTE_NTService service(m_dbName,m_dbRoot,NULL,NULL,speed);
    rc = service.Start(szOptions);

    if(NO_ERROR == rc)
    {
        Msg_List newErrList;
        ULONG                ulTimeout    = TIMEOUT_;
        do
        {
            if(diagPipe.readMessage(newErrList))
            {
                errList = errList + newErrList;
            }
            else
            {
                SLEEP( 1000 );
                ulTimeout--;

                DWORD dwCurrentState;
                rc = service.GetStatus(dwCurrentState);
                if( dwCurrentState == SERVICE_RUNNING )
                {
                    break;
                }
                else if ( dwCurrentState == SERVICE_STOPPED )
                {
                    rc = ( (APIRET) -2 );
                }
                if ( ulTimeout == 0 )
                {
                    rc = ERROR_TIMEOUT;
                }
            }
        }
        while ( rc == NO_ERROR );

        diagPipe.closeClientSide();
    }

#  if defined CLUSTER_SERVER_VERSION
    if (( rc    == NO_ERROR ) &&
        ( speed == speedFast ))
    {
        APIRET newrc;
        // --- try start SERVERDB via cluster server
        newrc = os212_OnlineSERVERDBClusterResource ( m_dbName );

        if (( newrc != ERROR_RESOURCE_NOT_FOUND ) &&
            ( newrc != ERROR_MOD_NOT_FOUND ))
        {
            RTESys_ErrorMessage spaceToFill;
            errList = SAPDBErr_MessageList( RTE_CONTEXT
                                          , RTEERR_CLUSTER_DB_ONLINE
                                          , SAPDB_ToString(newrc)
                                          , RTESys_StrError(newrc, spaceToFill)
                                          );
            rc = newrc;
        }
    }
#  endif

    return rc;
}

//-----------------------------------------------------------

bool
RTE_ActiveDatabase::Start( RTE_SpeedInfo &speed
                         , Msg_List &errList
                         , int argc
                         , char **argv
                         , SAPDB_Bool forceAutorestart
                         , SAPDB_Bool lowPriority)
{
    APIRET         rc;

    rc = startService(speed,forceAutorestart,lowPriority,argc,argv,errList);

    if( ERROR_SERVICE_DOES_NOT_EXIST == rc )
    {
        SAPDB_UInt sizeOfCmdLine = strlen(m_dbRoot)+2+strlen(speed.ExecutableName())+2+strlen(m_dbName)+1+10;   // 10 is the maximum number of digits of an UInt

        tsp00_Pathc u3Root;
        tsp01_RteError  rteError; 
        if(sqlGetPortableInstallationRootPath( u3Root,
                                            TERM_WITH_DELIMITER_EO01,
                                            &rteError ))
        {
            sizeOfCmdLine += strlen(u3Root);
        }
        else
        {
            u3Root[0] = 0;
        }
        SAPDB_Char *cmdline = (SAPDB_Char *)alloca( sizeOfCmdLine );

        SAPDB_sprintf(cmdline,sizeOfCmdLine,"%s%s\\""%s"" %s %d"
            ,u3Root.asCharp()
            ,m_dbRoot
            ,speed.ExecutableName()
            ,m_dbName
            ,(SAPDB_UInt4)(forceAutorestart ? FORCE_AUTORESTART : IGNORE_AUTORESTART)
            | (lowPriority ? LOW_PROCESS_PRIORITY : 0) 
            );
    
        STARTUPINFO         startupInfo;
        PROCESS_INFORMATION procInfo;
      
        GetStartupInfo (&startupInfo);

        startupInfo.dwFlags = STARTF_USESHOWWINDOW;
        startupInfo.wShowWindow = SW_HIDE;

        if(CreateProcess (0, (LPTSTR)cmdline, 0, 0, TRUE, 0, 0, 0, &startupInfo, &procInfo))
        {
            RTE_ClientDiagPipe diagPipe;
            diagPipe.create(m_dbName);

            rc = NO_ERROR;
            Msg_List newErrList;
            ULONG                ulTimeout    = TIMEOUT_;
            CloseHandle (procInfo.hThread);//nötich?
            do
            {
                if(diagPipe.readMessage(newErrList))
                {
                    errList = errList + newErrList;
                }
                else
                {
                    break;
                }
            }
            while ( rc == NO_ERROR );

            diagPipe.closeClientSide();
        }
        else
        {
            rc = GetLastError();
        }                        
    }

    switch (rc)
    {
    case NO_ERROR:
        break;
    case -2:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_STOPPED_EV_LOG, m_dbName );
        break;
    case ERROR_SERVICE_DATABASE_LOCKED:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVICE_DB_LOCKED);
        break;
    case ERROR_FILE_NOT_FOUND:
        {
            RTE_NTService service(m_dbName,m_dbRoot,NULL,NULL,speed);
            RTE_Path executableName = {0};
            service.GetExecutableName(executableName);
            if( executableName[0] )
            {
                errList = SAPDBErr_MessageList ( RTE_CONTEXT, RTEERR_DBREG_KERNEL_EXE_MISSING,executableName );
            }
            else
            {
                errList = SAPDBErr_MessageList ( RTE_CONTEXT, RTEERR_DBREG_KERNEL_EXE_MISSING,speed.ExecutableName());
            }
        }
        break;
    case ERROR_PATH_NOT_FOUND:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVICE_NOT_INSTALLED_CORRECTLY );
        break;
    case ERROR_SERVICE_DOES_NOT_EXIST:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVICE_NOT_INSTALLED, SAPDB_ToString(SERVICE_DISPLAY_STR), m_dbName, SAPDB_ToString((SAPDB_Char *)speed.Name()) );
        break;
    case ERROR_SERVICE_ALREADY_RUNNING:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_ALREADY_STARTED, m_dbName );
        break;
    case ERROR_ACCESS_DENIED:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_ACCESS_DENIED );
        break;
    case ERROR_TIMEOUT:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVERDB_NOT_RESPONDING );
        break;
    case ERROR_SERVICE_LOGON_FAILED:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVICE_LOGON_FAILURE, SERVICE_DISPLAY_STR, m_dbName, SAPDB_ToString((SAPDB_Char *)speed.Name()) );
        break;
    default:
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_START , m_dbName, SAPDB_ToString((SAPDB_ULong)rc) );
        break;
    }

    return (NO_ERROR == rc);
}

//-----------------------------------------------------------

APIRET RTE_ActiveDatabase::stopService ( void )
{
    APIRET                                rc           = NO_ERROR;
    APIRET                                rcSav        = NO_ERROR;
    SAPDB_ULong                           ulTimeout    = TIMEOUT_;
    SAPDB_Bool                            fFinish      = FALSE;
    RTE_DBName dbName;
    strcpy(dbName,m_dbName);
    // --- convert to upper case
    CharUpper(dbName);

    Container_List<RTE_SpeedInfo>::Iterator speedIterator = GetDBSpeeds().Begin();
    while(!fFinish && speedIterator != 0)
    {
        RTE_NTService service(m_dbName,m_dbRoot,NULL,NULL,*speedIterator);
        rc = service.Stop();

        if(NO_ERROR == rc)
        {
            DWORD dwCurrentState;
            do
            {
                SLEEP( 1000 );
                ulTimeout--;
                service.GetStatus(dwCurrentState);
                if( SERVICE_STOPPED == dwCurrentState )
                {
                    rcSav   = NO_ERROR;
                    fFinish = TRUE;
                    break;
                }

                if ( ulTimeout == 0 )
                {
                    rc = ERROR_TIMEOUT;
                }
            }
            while ( rc == NO_ERROR );
        }
        if (( rc == ERROR_SERVICE_DOES_NOT_EXIST ) ||
            ( rc == ERROR_SERVICE_NOT_ACTIVE     ))
        {
            if ( rcSav != ERROR_SERVICE_NOT_ACTIVE )
                rcSav = rc;

            rc = NO_ERROR;
        }
        ++speedIterator;
    }
    if ( rc == NO_ERROR )
    {
        rc = rcSav;
    }

    return( rc );
}

//-----------------------------------------------------------

SAPDB_Bool RTE_ActiveDatabase::StopServerDB ( SAPDB_Bool fDump, SAPDB_Bool gracefully, Msg_List &messageList )
{
    Msg_List dummyMessageList;  // a place for error messages that can be ignored
    KERNEL_SHARED_SEG               *pKSS         = NULL;
    tsp00_Versionc                  RTEVersionString;
    tsp100_VersionResult            Result;

    // --- convert to upper case
    RTE_DBName dbName;
    strcpy(dbName,m_dbName);
    CharUpper(dbName);

    if (0 != m_startStopSemaphore.Create(CONCATENATED_STRING(SEM_XSTOP,dbName),
                                         (RTESys_SecurityAttributes::Instance()).GetWorldSA(),
                                         messageList))
    {
        m_startStopSemaphore.Close(dummyMessageList);
        return false;
    }

    RTEIPC_NamedSharedMemory kss;
    if(0!=kss.Open(CONCATENATED_STRING (SHM_KSS,dbName),
                   NULL,
                   RTE_OpenExisting,
                   RTESys_SecurityAttributes::Instance().GetNoSA (),
                   sizeof(KERNEL_SHARED_SEG),
                   (void **)&pKSS,
                   messageList))
    {
        m_startStopSemaphore.Close(dummyMessageList);
        messageList = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_DBREG_DB_NOT_STARTED,dbName);
        return false;
    }

    Result = sp100_CompareVersionIDs (&pKSS->RTEVersionID);

    if ( Result != IsEqual_esp100)
    {
        // --- set the RTE version string
        if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
        {
            sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, RTEVersionString);
        }
        else
        {
            sp100_GetVersionString(COMP_NAME_RTE_WIN9X_SP100, s100buildnumber, RTEVersionString);
        }

        m_startStopSemaphore.Close(dummyMessageList);
        messageList = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_DBREG_WRONG_DB_VERSION);
        return false;
    }
    pKSS->fDump = fDump;
    pKSS->gracefully = gracefully;

#  if defined CLUSTER_SERVER_VERSION
   // --- try stop SERVERDB via cluster server
   APIRET rc = os212_OfflineSERVERDBClusterResource ( dbName );

   if ( NO_ERROR == rc )
   {
       return true;
   }

   if (( rc != ERROR_RESOURCE_NOT_FOUND ) &&
       ( rc != ERROR_MOD_NOT_FOUND ))
   {
       RTESys_ErrorMessage spaceToFill;
       messageList = SAPDBErr_MessageList( RTE_CONTEXT
                                         , RTEERR_CLUSTER_DB_OFFLINE
                                         , SAPDB_ToString(rc)
                                         , RTESys_StrError(rc, spaceToFill)
                                         );
       m_startStopSemaphore.Close(dummyMessageList);
       return false;
   }
#  endif

    RTESync_BinarySemaphore coordinationSemaphore;
    if(0!=coordinationSemaphore.Open(CONCATENATED_STRING(SEM_COORD,dbName),messageList))
    {
        m_startStopSemaphore.Close(dummyMessageList);
        return false;
    }
    else
    {
        if(0!=coordinationSemaphore.Post(messageList))
        {
            coordinationSemaphore.Close(dummyMessageList);
            m_startStopSemaphore.Close(dummyMessageList);
            return false;
        }
        coordinationSemaphore.Close(messageList);
    }

    if(NOERROR != m_startStopSemaphore.Wait(TIMEOUT_ * 1000,messageList))
    {
// if waiting for the semaphore failed, this is the return value, not the one from the Close() call
        m_startStopSemaphore.Close(dummyMessageList);
        return false;
    }
    else
    {
// if waiting for the semaphore succeeded, the return value is the one from the Close() call
        return 0==m_startStopSemaphore.Close(messageList);
    }
}

//-----------------------------------------------------------

bool
RTE_ActiveDatabase::Stop(Msg_List &errList,SAPDB_UInt4 offlineTimeOut, bool dump, bool gracefully)
{
    if(! StopServerDB ( dump, gracefully, errList ))
    {
        return false;
    }
    if (0 != offlineTimeOut)
    {
        Msg_List err;
        UpdateStatus(err);
        while(!(m_currentState == dbstateOffline) && offlineTimeOut>0)
        {
            SLEEP(1000);
            UpdateStatus(err);
            offlineTimeOut--;
        }
        if( 0 == offlineTimeOut )
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SERVERDB_NOT_RESPONDING );
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------

const RTE_SpeedInfo &RTE_ActiveDatabase::GetActiveSpeed()
{
    Msg_List    err;
    SAPDB_UInt4          dbState = SERVER_UNDEFINED;
    RTE_SystemRc         rc = RTE_SYSTEMRC_NO_ERROR;

    if (!m_pKSS)
    {
        if (!OpenKernel (dbState, err) || SERVER_OFFLINE == dbState)
        {
            return speedNone;
        }
    }
    if(0 == strncmp(m_pKSS->KernelVersion,COMP_NAME_KERNEL_SP100,strlen(COMP_NAME_KERNEL_SP100)))
    {
        return speedFast;
    }
    else if(0 == strncmp(m_pKSS->KernelVersion,COMP_NAME_QUICKKNL_SP100,strlen(COMP_NAME_QUICKKNL_SP100)))
    {
        return speedQuick;
    }
    else if(0 == strncmp(m_pKSS->KernelVersion,COMP_NAME_SLOWKNL_SP100,strlen(COMP_NAME_SLOWKNL_SP100)))
    {
        return speedSlow;
    }
    else if(0 == strncmp(m_pKSS->KernelVersion,COMP_NAME_OMSTSTKNL_SP100,strlen(COMP_NAME_OMSTSTKNL_SP100)))
    {
        return speedTest;
    }
    else
    {
        return speedNone;
    }
}

//-----------------------------------------------------------

SAPDB_Bool RTE_ActiveDatabase::OpenKernel
(
    SAPDB_UInt4 &           dbState,
    Msg_List    &messageList
)
{
    //Open KSS
    RTEIPC_NamedSharedMemory::SHMRet rc;

    dbState = SERVER_UNDEFINED;

    rc = m_kss.Open( CONCATENATED_STRING (SHM_KSS,m_dbName),
                    NULL,
                    RTE_OpenExistingReadOnly,
                    RTESys_SecurityAttributes::Instance().GetNoSA (),
                    sizeof(KERNEL_SHARED_SEG),
                    (void **)&m_pKSS,
                    messageList);
    if(RTEIPC_NamedSharedMemory::ResourceMissing == rc)
    {
        // check if the service is running
        Msg_List    err;
        Container_List<RTE_SpeedInfo>::Iterator speedIterator = GetDBSpeeds().Begin();
        DWORD dwCurrentState;
        while(speedIterator != 0)
        {
            RTE_NTService service(m_dbName,m_dbRoot,NULL,NULL,*speedIterator);
            APIRET rc;
            SAPDB_Bool firstCall = true;
            do
            {
                // Check the state of the service. To minimize the bad busy-waiting-effects,
                // wait a second between the calls to the Windows service manager - but NOT
                // before the first call!
                if(!firstCall)
                {
                    SLEEP ( 1000 );
                }
                else
                {
                    firstCall = false;
                }
                rc = service.GetStatus(dwCurrentState);
            }
            // when the state cannot be determined, regard the kernel as offline - may have stopped in the meantime
            // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126561
            // This was only half-right: the kernel may be starting. So wait until it is completely stopped OR
            // completely started.
            // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127480
            while( (rc == NO_ERROR) && (dwCurrentState != SERVICE_STOPPED) && (dwCurrentState != SERVICE_RUNNING));
            if(SERVICE_RUNNING == dwCurrentState)
            {
                // okay, the service has reached state RUNNING -> try to open the shm again
                RTEIPC_NamedSharedMemory::SHMRet rc = m_kss.Open( CONCATENATED_STRING (SHM_KSS,m_dbName),
                                NULL,
                                RTE_OpenExistingReadOnly,
                                RTESys_SecurityAttributes::Instance().GetNoSA (),
                                sizeof(KERNEL_SHARED_SEG),
                                (void **)&m_pKSS,
                                messageList);
                if(NO_ERROR != rc)
                {   // failed -> give up
                    return false;
                }
                break;
            }
            ++speedIterator;
        }
        if(SERVICE_RUNNING != dwCurrentState)
        {
            dbState = SERVER_OFFLINE;
            return true;
        }
    }
    else if(RTEIPC_NamedSharedMemory::NoError != rc)
    {
        return false;
    }

    m_hKernel = OpenProcess (SYNCHRONIZE, false, m_pKSS->KernelPid);
    if (NULL == m_hKernel)
    {
        RTE_SystemRc            rc;
        rc = GetLastError ();
        m_hKernel = RTE_UNDEF_HANDLE;
        if (ERROR_ACCESS_DENIED == rc)
        {
            return true;
        }

        if (ERROR_INVALID_PARAMETER == rc)
        {
            dbState = SERVER_OFFLINE;
            return true;
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_OPEN_KERNEL_PROCESS, SAPDB_ToString (rc));
        }

        CloseKernel ();
        return false;
    }

    DWORD procState = WaitForSingleObject (m_hKernel, 0);

    //WAIT_TIMEOUT == rc: Process is still alive 
    if (WAIT_TIMEOUT != procState)
    {
        dbState = SERVER_OFFLINE;
        CloseKernel ();
    }

    return true;
}

void    RTE_ActiveDatabase::CloseKernel ()
{

    //Close KSS
    Msg_List messageList;
    if(m_pKSS)
    {
        m_kss.Close(messageList);
    }

    //Close kernel handle
    if (RTE_UNDEF_HANDLE != m_hKernel)
    {
        CloseHandle (m_hKernel);
    }

    m_pKSS = NULL;
    m_hKernel = RTE_UNDEF_HANDLE;
}


bool RTE_ActiveDatabase::UpdateStatus
(
    Msg_List &errList
)
{
    SAPDB_UInt4          dbState = SERVER_UNDEFINED;
    RTE_SystemRc         rc = RTE_SYSTEMRC_NO_ERROR;


    if (!m_pKSS/*RTE_UNDEF_HANDLE == m_hKSS*/ || RTE_UNDEF_HANDLE == m_hKernel)
    {
        if (!OpenKernel (dbState, errList))
        {
            return false;
        }
    }

    if (SERVER_UNDEFINED == dbState)
    {
        if (RTE_UNDEF_HANDLE != m_hKernel)
        {
            rc = WaitForSingleObject (m_hKernel, 0);
            //WAIT_TIMEOUT == rc: Process is still alive
            if (WAIT_TIMEOUT != rc)
            {
                if (WAIT_FAILED == rc)
                {   //An error has occurred by calling WaitForSingleObject
                    CloseKernel ();
                    errList = errList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_WAIT_FOR_KERNEL,
                                                              SAPDB_ToString (GetLastError()));
                    return false;
                }
                else if (WAIT_OBJECT_0 != rc)
                {
                    CloseKernel ();
                    errList = errList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_WAIT_FOR_KERNEL,
                                                              SAPDB_ToString (rc));

                }

                //rc == WAIT_OBJECT_0 : Kernel has terminated!!!
                CloseKernel ();
                dbState = SERVER_OFFLINE;
            }
        }

        if (SERVER_UNDEFINED == dbState)
        {
            dbState = m_pKSS->dBState;
            /* PTS 1129710 */
            UpdateSharedStateCopy( m_pKSS->kernelState
                                 , m_pKSS->pendingUpdateCounter
                                 , m_pKSS->completedUpdateCounter
                                 , m_pKSS->kernelStateSize );
        }
    }


    DBStatusInfo statusInfo = dbstateUnknown;

    switch( dbState )
    {
    case SERVER_OFFLINE:
        statusInfo = dbstateOffline;
        break;
    case SERVER_STARTING:
        statusInfo = dbstateStarting;
        break;
    case SERVER_COLD:
        statusInfo = dbstateAdmin;
        break;
    case SERVER_STANDBY_EO00:
        statusInfo = dbstateStandby;
        break;
    case SERVER_WARM:
        statusInfo = dbstateOnline;
        break;
    case SERVER_SHUTDOWN:
        statusInfo = dbstateShutdown;
        break;
    case SERVER_SHUTDOWNREINIT:
        statusInfo = dbstateShutdownReinit;
        break;
    case SERVER_SHUTDOWNKILL:
        statusInfo = dbstateShutdownKill;
        break;
    case SERVER_STOP:
        statusInfo = dbstateStop;
        break;
    case SERVER_KILL:
        statusInfo = dbstateKill;
        break;
    case SERVER_ABORT:
        statusInfo = dbstateAbort;
        break;
    case SERVER_STOPPED:
        statusInfo = dbstateStopped;
        break;
    }
    m_currentState = statusInfo;


    if (RTE_UNDEF_HANDLE == m_hKernel)
    {
        CloseKernel ();
    }

    return true;
}
#else   // UNIX

#include <sys/wait.h>   // for WHNOHANG
#include <unistd.h>

#include "gsp09.h"
extern "C"
{
#include "heo44.h"
#include "gen41.h"  /* nocheck */ // for sql41_get_ipc_dir()
}
#include "RunTime/RTE_IniFileHandling.h"  /* nocheck */
#include "hen40.h"  /* nocheck */ // for sqlerrs()

#include "RunTime/RTE_saveUNIXcalls.h"  /* nocheck */

/*! define */
#define DO_START_SUCCEEDED              0
/*! define */
#define DO_START_CANT_REDIRECT_STDOUT   1
/*! define */
#define DO_START_CANT_REDIRECT_STDERR   2
/*! define */
#define DO_START_CANT_OPEN_DEV_NULL     3
/*! define */
#define DO_START_CANNOT_FORK            4
/*! define */
#define DO_START_CANT_CHANGE_TO_RUN_DIR 5
/*! define */
#define DO_START_DIED_BEFORE_COLD_STATE 6
/*! define */
#define DO_START_CANT_SET_ENVIRONMENT   7
/*! define */
#define DO_START_ERROR_IN_POLL          8
/* DO_START_KERNEL_EXIT_CODE_OFFSET must be highest number, since exit_code is added */
/*! define */
#define DO_START_KERNEL_EXIT_CODE_OFFSET 9


//-----------------------------------------------------------
/*! define */
#define maxArgsC 10


/*===========================================================================*
 *  STATIC FUNCTIONS                                                         *
 *===========================================================================*/

void RTE_ActiveDatabase::saveErrorMessageToFile(SAPDB_Char *errorMessage)
{
    SAPDB_Int fd = open("/tmp/SaveCloseError.txt", O_RDWR);
    if ( fd >= 0 )
    {
        lseek(fd, 0, SEEK_END);
        write(fd, errorMessage, strlen(errorMessage));
    }
    close(fd);
}

// this for writing the state of the kernel process to the dbm protocol if the kernel process cannot be stopped
// happens on HPUX only and is very platform specific
#ifdef HPUX
static void outputProcessState(pid_t kernelPid,Msg_List &err)
{
    struct pst_status psa;
    if ( pstat_getproc(&psa, sizeof(struct pst_status), 0, kernelPid) == -1 )
    {
        return; //  error in pstat_getproc();
    }
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_idx", SAPDB_ToString(psa.pst_idx));        /* Index for further pstat() requests int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_uid", SAPDB_ToString(psa.pst_uid));        /* Real UID         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_pid", SAPDB_ToString(psa.pst_pid));        /* Process ID         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_ppid", SAPDB_ToString(psa.pst_ppid));       /* Parent process ID         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_dsize", SAPDB_ToString(psa.pst_dsize));      /* # real pages used for data         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_tsize", SAPDB_ToString(psa.pst_tsize));      /* # real pages used for text         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_ssize", SAPDB_ToString(psa.pst_ssize));      /* # real pages used for stack         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_nice", SAPDB_ToString(psa.pst_nice));       /* Nice value         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_term.psd_minor", SAPDB_ToString(psa.pst_term.psd_minor));       /* TTY of this process; -1/-1 if there isn't one         struct psdev    */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_term.psd_major", SAPDB_ToString(psa.pst_term.psd_major));       /* TTY of this process; -1/-1 if there isn't one         struct psdev    */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_pgrp", SAPDB_ToString(psa.pst_pgrp));       /* Process group of this process         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_pri", SAPDB_ToString(psa.pst_pri));        /* priority of process         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_addr", SAPDB_ToString(psa.pst_addr));       /* address of process (in memory)         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_cpu", SAPDB_ToString(psa.pst_cpu));        /* processor utilization for scheduling         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_utime", SAPDB_ToString(psa.pst_utime));      /* user time spent executing (in seconds)         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_stime", SAPDB_ToString(psa.pst_stime));      /* system time spent executing (in seconds)         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_start", SAPDB_ToString(psa.pst_start));      /* time process started (seconds since epoch)         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_flag", SAPDB_ToString(psa.pst_flag));       /* flags associated with process         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_stat", SAPDB_ToString(psa.pst_stat));       /* Current status         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_wchan", SAPDB_ToString(psa.pst_wchan));      /* If state PS_SLEEP, value sleeping on         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_procnum", SAPDB_ToString(psa.pst_procnum));    /* processor this proc last run on         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_cmd", SAPDB_ToString(psa.pst_cmd));        /* Command line for the process, if available         char[PST_CLEN]            */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_time", SAPDB_ToString(psa.pst_time));       /* resident time for scheduling         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_cpticks", SAPDB_ToString(psa.pst_cpticks));    /* ticks of cpu time */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_cptickstotal", SAPDB_ToString(psa.pst_cptickstotal)); /* total ticks for life of process         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_fss", SAPDB_ToString(psa.pst_fss));        /* fair share scheduler group id         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_pctcpu", SAPDB_ToString(psa.pst_pctcpu));     /* %cpu for this process during p_time         float */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_rssize", SAPDB_ToString(psa.pst_rssize));     /* resident set size for process (private pages)         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_suid", SAPDB_ToString(psa.pst_suid));       /* saved UID         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_ucomm", SAPDB_ToString(psa.pst_ucomm));      /* executable basename the process is running         char[PST_UCOMMLEN]                 */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_shmsize", SAPDB_ToString(psa.pst_shmsize));    /* # real pages used for shared memory         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_mmsize", SAPDB_ToString(psa.pst_mmsize));     /* # real pages used for memory mapped files         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_usize", SAPDB_ToString(psa.pst_usize));      /* # real pages used for U-Area & K-Stack         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_iosize", SAPDB_ToString(psa.pst_iosize));     /* # real pages used for I/O device mapping         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_vtsize", SAPDB_ToString(psa.pst_vtsize));     /* # virtual pages used for text         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_vdsize", SAPDB_ToString(psa.pst_vdsize));     /* # virtual pages used for data         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_vssize", SAPDB_ToString(psa.pst_vssize));     /* # virtual pages used for stack         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_vshmsize", SAPDB_ToString(psa.pst_vshmsize));   /* # virtual pages used for shared memory         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_vmmsize", SAPDB_ToString(psa.pst_vmmsize));    /* # virtual pages used for mem-mapped files         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_vusize", SAPDB_ToString(psa.pst_vusize));     /* # virtual pages used for U-Area & K-Stack         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_viosize", SAPDB_ToString(psa.pst_viosize));    /* # virtual pages used for I/O dev mapping         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_minorfaults", SAPDB_ToString(psa.pst_minorfaults));/* # page reclaims for the process         uint64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_majorfaults", SAPDB_ToString(psa.pst_majorfaults));/* # page faults needing disk access         uint64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_nswap", SAPDB_ToString(psa.pst_nswap));      /* # of swaps for the process         uint64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_nsignals", SAPDB_ToString(psa.pst_nsignals));   /* # signals received by the process         uint64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_msgrcv", SAPDB_ToString(psa.pst_msgrcv));     /* # socket msgs received by the proc        uint64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_msgsnd", SAPDB_ToString(psa.pst_msgsnd));     /* # of socket msgs sent by the proc         uint64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_maxrss", SAPDB_ToString(psa.pst_maxrss));     /* highwater mark for proc resident set size         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_sid", SAPDB_ToString(psa.pst_sid));        /* session ID         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_schedpolicy", SAPDB_ToString(psa.pst_schedpolicy));/* scheduling policy for the process         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_ticksleft", SAPDB_ToString(psa.pst_ticksleft));  /* clock ticks left in process' RR timeslice         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_rdir.psf_fsid.psfs_id", SAPDB_ToString(psa.pst_rdir.psf_fsid.psfs_id));       /* File ID of the process' root directory         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_rdir.psf_fsid.psfs_type", SAPDB_ToString(psa.pst_rdir.psf_fsid.psfs_type));       /* File ID of the process' root directory         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_rdir.psf_fileid", SAPDB_ToString(psa.pst_rdir.psf_fileid));       /* File ID of the process' root directory         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_rdir.psf_spare", SAPDB_ToString(psa.pst_rdir.psf_spare));       /* File ID of the process' root directory         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_cdir.psf_fsid.psfs_id", SAPDB_ToString(psa.pst_cdir.psf_fsid.psfs_id));       /* File ID of the process' current directory         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_cdir.psf_fsid.psfs_type", SAPDB_ToString(psa.pst_cdir.psf_fsid.psfs_type));       /* File ID of the process' current directory         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_cdir.psf_fileid", SAPDB_ToString(psa.pst_cdir.psf_fileid));       /* File ID of the process' current directory         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_cdir.psf_spare", SAPDB_ToString(psa.pst_cdir.psf_spare));       /* File ID of the process' current directory         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_text.psf_fsid.psfs_id", SAPDB_ToString(psa.pst_text.psf_fsid.psfs_id));       /* File ID of the process' executable         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_text.psf_fsid.psfs_type", SAPDB_ToString(psa.pst_text.psf_fsid.psfs_type));       /* File ID of the process' executable         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_text.psf_fileid", SAPDB_ToString(psa.pst_text.psf_fileid));       /* File ID of the process' executable         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_text.psf_spare", SAPDB_ToString(psa.pst_text.psf_spare));       /* File ID of the process' executable         struct psfileid*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_highestfd", SAPDB_ToString(psa.pst_highestfd));  /* highest file descriptor currently opened         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_euid", SAPDB_ToString(psa.pst_euid));       /* Effective UID         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_egid", SAPDB_ToString(psa.pst_egid));       /* Effective GID         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_ioch", SAPDB_ToString(psa.pst_ioch));       /* # of characters read/written         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_usercycles.psc_hi", SAPDB_ToString(psa.pst_usercycles.psc_hi)); /* 64-bit user mode execution cycle count         struct pstcycles*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_usercycles.psc_lo", SAPDB_ToString(psa.pst_usercycles.psc_lo)); /* 64-bit user mode execution cycle count         struct pstcycles*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_systemcycles.psc_hi", SAPDB_ToString(psa.pst_systemcycles.psc_hi));/* 64-bit system mode execution cycle count         struct pstcycles*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_systemcycles.psc_lo", SAPDB_ToString(psa.pst_systemcycles.psc_lo));/* 64-bit system mode execution cycle count         struct pstcycles*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_interruptcycles.psc_hi", SAPDB_ToString(psa.pst_interruptcycles.psc_hi));/* 64-bit interrupt for proc cycle count         struct pstcycles*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_interruptcycles.psc_lo", SAPDB_ToString(psa.pst_interruptcycles.psc_lo));/* 64-bit interrupt for proc cycle count         struct pstcycles*/
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_gid", SAPDB_ToString(psa.pst_gid));        /* Real GID         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_sgid", SAPDB_ToString(psa.pst_sgid));       /* saved effective gid         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_nlwps", SAPDB_ToString(psa.pst_nlwps));      /* # lwps within this process         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_lwpid", SAPDB_ToString(psa.pst_lwpid));      /* LWP identifier. NOTE: If this process is multi-threaded, this is an lwpid of one of the LWPs in the process at this exact moment, which LWP is undefined (random) int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_valid", SAPDB_ToString(psa.pst_valid));      /* This is a vector that will indicate if a certain field in the structure is valid or not. int32_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_text_size", SAPDB_ToString(psa.pst_text_size)); /* Page size used for text objects.         int64_t */
    err = err + SAPDBErr_MessageList (RTE_CONTEXT, RTEINFO_PROCESSSTATE, "psa.pst_data_size", SAPDB_ToString(psa.pst_data_size)); /* Page size used for data objects.         int64_t */

    tin100_GlobalLog::writeEntry (err);
}
#endif
/*-----------------------------------------------------------*/


/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

SAPDB_Bool RTE_ActiveDatabase::checkForRunningProcesses (SAPDB_Bool &dbRunning,Msg_List &errList)
{
    SAPDB_Int      rc ;
    tsp00_TaskId   dbWatchdogPid;
    tsp00_TaskId   dbKernelPid;

    if ( (en41GetPPIDFromFile(m_dbName, &dbWatchdogPid) != 0) || (dbWatchdogPid == 0) )
	{   /* the watchdog process did not write its pid file. This is the normal case when */
        /* the database is simply not running at all. */
        dbRunning = false;
        return (true);
    }

    rc = kill ( (pid_t) dbWatchdogPid , 0 ) ;
    if ( rc == 0 || errno != ESRCH )
    {   /* check explicit for ESRCH since watchdog process is running even
           if current user is not authorized to send signal... */
        dbRunning = true;
        return (true);
    }

    /* now we know watchdog process does not run */

    if ( (en41GetPIDFromFile(m_dbName, &dbKernelPid) != 0) || (dbKernelPid == 0) )
	{   /* while kernel process did not write its pid file, state is OFFLINE */
        dbRunning = false;
        return (true);
    }

    rc = kill ( (pid_t) dbKernelPid , 0 ) ;
    if(rc == 0 || errno != ESRCH)
    {   /* The watchdog does not exist (Terminated by unfriendly user?) but the
           kernel process does exist... Be friendly and call it 'running...' since
           we only loose the cleanup and flush of knltrace after kernel stopped */
        dbRunning = true;
    }
    else
    {   /* both processes do not exist */
        dbRunning = false;
    }

    return (true);
}



/*----------------------------------------*/

SAPDB_Bool RTE_ActiveDatabase::checkFileAccess (
    SAPDB_Char *path,
    SAPDB_Int mode )
{
    struct stat buf;
    SAPDB_Bool access_ok ;

    access_ok = ( stat ( path, &buf )  == 0 && (buf.st_mode & mode) == mode );
    return ( access_ok ) ;
}

/*-----------------------------------------------------------*/

SAPDB_Bool RTE_ActiveDatabase::createPath (
    SAPDB_Char * path_to_create,
    SAPDB_UInt2 mode )
{
    SAPDB_Bool create_ok = TRUE ;
    SAPDB_Char *act_pos = path_to_create , *end_pos;

    if ( *act_pos == '/' )
        act_pos++ ;

    do
    {
        if ( (end_pos = (SAPDB_Char *)strchr ( act_pos, '/' )) != '\0' )
            *end_pos = '\0' ;
        if ( !checkFileAccess ( path_to_create , S_IFDIR ) )
        {
            create_ok = (mkdir ( path_to_create, mode ) == 0);
        }
        if ( end_pos != '\0' )
        {
            *end_pos = '/';
            act_pos = end_pos+1 ;
        }
    }
    while ( create_ok && end_pos != '\0' ) ;
    return ( create_ok ) ;
}

/* PTS 1107044 */
/*
 * function: updateEnvironment
 * description:
 * Update environment of starting kernel. It updates the sharedc library paths, DBROOT
 * and any extra environment needed platform dependend.
 * arguments: dbroot [in] the dbroot of the starting kernel
 *            xerror [out] the error return
 * return value: 0 if update failed (message is written), 1 if successfull
 */

SAPDB_Bool RTE_ActiveDatabase::updateEnvironment(
    SAPDB_Char *errorText,
    SAPDB_Int4 maxErrorTextLengthInBytes)
{
    static SAPDB_Char *envDBROOT = NULL;
    SAPDB_Char *oldenvDBROOT = NULL;
    tsp01_RteError RteError;

    if ( !sqlUpdateLibPathEnvironment(&RteError) )
    {
        SAPDB_sprintf(errorText,maxErrorTextLengthInBytes,"start: %s", RteError.RteErrText );
        return false;
    }

    /* Always set DBROOT    */
    /* PTS 1105164          */
    oldenvDBROOT = envDBROOT;
    envDBROOT = (SAPDB_Char *)malloc(strlen("DBROOT=") + strlen(m_dbRoot) + 1);

    if ( envDBROOT != NULL )
    {
        strcpy(envDBROOT, "DBROOT=");
        strcat(envDBROOT, m_dbRoot);


        if ( putenv(envDBROOT) < 0 )
        {
            SAPDB_sprintf(errorText,maxErrorTextLengthInBytes,"start:   putenv DBROOT failed" );
            free(envDBROOT);
            return false;
        }
        else if ( oldenvDBROOT != NULL )
        {
            free(oldenvDBROOT); /* prevent some memory leak, accept that last malloc is not freed... */
        }
    }
    else
    {
        SAPDB_sprintf(errorText,maxErrorTextLengthInBytes,"start:   DBROOT too long" );
        return false;
    }

    /* ------------------------------ */
    /* Platform dependend environment */
    /* ------------------------------ */

#if defined(AIX)
#if   defined(BIT64)
    if ( putenv("EXTSHM=OFF") != 0 )
    {
        SAPDB_sprintf(errorText,maxErrorTextLengthInBytes,"start:   Cannot putenv EXTSHM=OFF" );
        return false;
    }
#  else
    /* set EXTSHM on 32 Bit AIX to avoid shm attach problems    */
    /* PTS 1106620                                              */
    if ( putenv("EXTSHM=ON") != 0 )
    {
        SAPDB_sprintf(errorText,maxErrorTextLengthInBytes,"start:   Cannot putenv EXTSHM=ON" );
        return false;
    }
#  endif
#endif

#if defined(LINUX) && defined(SDBonPPC64)
    if ( putenv("LD_BIND_NOW=1") != 0 )
    {
        SAPDB_sprintf(errorText,maxErrorTextLengthInBytes,"start:   Cannot putenv LD_BIND_NOW=1" );
        return false;
    }
#endif

#if defined(LINUX)
    /* enforce linuxthread compatibility mode for non NPTL supporting database kernel */
    /* for detailed description see http://people.redhat.com/drepper/assumekernel.html */
    if ( !getenv("SDB_DONT_MODIFY_KERNEL_ENVIRONMENT_DURING_START") )
    {
#  if defined(HAS_NPTL)
        unsetenv("LD_ASSUME_KERNEL");
#  else
        if ( putenv("LD_ASSUME_KERNEL=2.4.1") != 0 )
        {
            SAPDB_sprintf(errorText,maxErrorTextLengthInBytes,"start:   Cannot putenv LD_ASSUME_KERNEL=2.4.1" );
            return false;
        }
#  endif
    }
#endif

    return true;
}

/*-----------------------------------------------------------*/
/* PTS 1109708 */
/*!--------------------------------------------------------------------
   @description    EINTR save version of waitpid
 Difference from waitpid call. This call will only return, if
 the pid was really returned (collecting other waited for processes
 without notion)!
   @param          pidToWaitFor [in]
   @param          pStatus [out] filled if != 0
   @param          flags [in] if WNOHANG is set, 0 is a possible return value
   @return value   (pid_t)-1 on error other than EINTR
              pidToWaitFor if process exited
              0 if WNOHANG was specified and process did not already exited

 --------------------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*
 * Wait for either a child or a process started by some other process...
 *
 * Return value: 0 if Waiting successfull
 *               1 if Waiting aborted
 */

SAPDB_Bool RTE_ActiveDatabase::doWaitForPid(
    pid_t kernelPid,
    SAPDB_UInt4 offlineTimeOut,
    Msg_List &errList )
{
    pid_t childPid;
    SAPDB_Bool ret = true;

    do
    {
        childPid = waitpid(kernelPid, 0, 0);
    }
    while ( ( (childPid > 0)            && (childPid != kernelPid) )
         || ( (childPid == ((pid_t)-1)) && (errno == EINTR)      ) );


    if ( childPid == (pid_t)-1 )
    {
        if ( errno == ECHILD )
        {
            /* some other process has started it...  */

            /* PTS 1106476 This produced an endless loop if kernel became zombie... */
            /* Scenario: DBMGUI or dbmcli started kernel and was left open. */
            /* Second dbmcli stopped kernel and waited forever */
            /* but it now possible, since no more zombis are created (see DoStart()) */

            if( 0 != offlineTimeOut )
            {
                SAPDB_UInt4 countDown = offlineTimeOut;
                while ( kill( kernelPid, 0 ) == 0 && countDown > 0)
                {
                    // sleep for one second to reduce busy waiting effects..
                    if ( sleep(1) != 0 )
                    {
                        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_SLEEP_FAILED ,sqlerrs());
                        ret = false;
                        break;
                    }
                    countDown--;
                }
                if( 0 == countDown )
                {
// this for writing the state of the kernel process to the dbm protocol if the kernel process cannot be stopped
// happens on HPUX only and is very platform specific
#ifdef HPUX
                    outputProcessState(kernelPid,errList);
#endif
                    ret = false;
                }

            }
        }
        else
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_WAITPID_FAILED ,sqlerrs());
            ret = false;
        }
    }
    return ret;
}

/*-----------------------------------------------------------*/

/*PTS 1113454
  It happened, that sqlxstart was called with stdout closed. To prevent this from
  producing a severe problem, channel 0/1/2 are opened with /dev/null if not already
  open and stdout/stderr are redirected to /dev/null
 */

SAPDB_Bool RTE_ActiveDatabase::redirectStandardDescriptors(SAPDB_Int &rc)
{
    SAPDB_Int fd;
    do
    {
        fd = open ( "/dev/null", O_WRONLY|O_CREAT|O_NDELAY , 0664 );
    }
    while ( fd >= 0 && fd <= 2 );

    if ( fd >= 0 )
    {
        SAPDB_Int DummyFd ;

        while ( -1 == close(0) )
        {
            SAPDB_Char errmsg[80];
            SAPDB_sprintf(errmsg, sizeof(errmsg), "close(0) reports error %d\n",errno );
            saveErrorMessageToFile(errmsg);
            if ( errno != EINTR )
            {
                rc = DO_START_CANT_REDIRECT_STDOUT;
                return false;
            }
        }
        DummyFd = dup ( fd );
        if ( DummyFd != 0 )
        {
            SAPDB_Char errmsg[80];
            SAPDB_sprintf(errmsg, sizeof(errmsg), "dup(0) != %d reports error %d\n",
            DummyFd, errno );
            saveErrorMessageToFile(errmsg);
            rc = DO_START_CANT_REDIRECT_STDOUT;
            return false;
        }

        while ( -1 == close(1) )
        {
            SAPDB_Char errmsg[80];
            SAPDB_sprintf(errmsg, sizeof(errmsg), "close(1) reports error %d\n",errno );
            saveErrorMessageToFile(errmsg);
            if ( errno != EINTR )
            {
                rc = DO_START_CANT_REDIRECT_STDOUT;
                return false;
            }
        }
        DummyFd = dup ( fd );
        if ( DummyFd != 1 )
        {
            SAPDB_Char errmsg[80];
            SAPDB_sprintf(errmsg, sizeof(errmsg), "dup(1) != %d reports error %d\n",
            DummyFd, errno );
            saveErrorMessageToFile(errmsg);
            rc = DO_START_CANT_REDIRECT_STDOUT;
            return false;
        }

        while ( -1 == close(2) )
        {
            SAPDB_Char errmsg[80];
            SAPDB_sprintf(errmsg, sizeof(errmsg), "close(2) reports error %d\n", errno );
            saveErrorMessageToFile(errmsg);
            if ( errno != EINTR )
            {
                rc = DO_START_CANT_REDIRECT_STDERR;
                return false;
            }
        }
        DummyFd = dup ( fd );
        if ( DummyFd != 2 )
        {
            SAPDB_Char errmsg[80];
            SAPDB_sprintf(errmsg, sizeof(errmsg), "dup(2) != %d reports error %d\n",
            DummyFd, errno );
            saveErrorMessageToFile(errmsg);
            rc = DO_START_CANT_REDIRECT_STDERR;
            return false;
        }

        (void) close (fd);
    }
    else
    {
        rc = DO_START_CANT_OPEN_DEV_NULL;
        return false;
    }
    return true;
}

/*-----------------------------------------------------------*/

SAPDB_Bool RTE_ActiveDatabase::checkFileDoesNotExist (
    SAPDB_Char *path )
{
    struct stat buf;
    return ( stat ( path, &buf ) != 0 && errno == ENOENT );
}

/*-----------------------------------------------------------*/

SAPDB_Bool RTE_ActiveDatabase::checkForDbStartable ( RTE_Path &full_knl_path, RTE_Path &rundir, Msg_List &errList)
{
    int rc ;

    SAPDB_Char errorMessage[256];

    SAPDB_UInt4 dbState;    
    if( !getDbPidStateFromShm(NULL,NULL,&dbState,0,0,NULL,errList) )
    {
        tsp00_Pathc testDir;
        sql41_get_spool_dir(testDir);
        rc = access ( testDir, R_OK|W_OK|X_OK );
        if (rc)
        {
            errList = errList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_NO_RWX_PERMISSION, testDir );
        }

        en41GetPPIDDirectory(testDir);
        rc = access ( testDir, R_OK|W_OK|X_OK );
        if (rc)
        {
            errList = errList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_NO_RWX_PERMISSION, testDir );
        }

        en41GetPIDDirectory(testDir);
        rc = access ( testDir, R_OK|W_OK|X_OK );
        if (rc)
        {
            errList = errList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_NO_RWX_PERMISSION, testDir );
        }
        return false;
    }
    if ( SERVER_OFFLINE != dbState )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_ALREADY_STARTED, m_dbName );
        return false;
    }
    /* PTS 1110606 */
    if ( !checkFileAccess ( full_knl_path, S_IXUSR ) )
    {
        if ( checkFileDoesNotExist( full_knl_path ) )
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_KERNEL_EXE_MISSING,full_knl_path);
        }
        else
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_KERNEL_NOT_EXECUTABLE,full_knl_path);
        }
        return false;
    }

    RTE_Path ipcdb;
    RTE_Path ipcus;
    sql41_get_ipc_dir( *((tsp00_Pathc *)ipcdb) );
    strcat( ipcdb, "/db:" );
    strcat( ipcdb, m_dbName );
    sql41_get_ipc_dir( *((tsp00_Pathc *)ipcus) );
    strcat( ipcus, "/us:" );
    strcat( ipcus, m_dbName );

    rc = sql41_remove_ipc_resources ( m_dbName );
    if ( 0 != rc )
    {
        if ( checkFileAccess ( ipcdb, S_IFDIR ) )
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_IPC_DIR_STILL_EXISTS,ipcdb);
            return false;
        }

        if ( checkFileAccess ( ipcus, S_IFDIR ) )
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_IPC_DIR_STILL_EXISTS,ipcus);
            return false;
        }
    }
    /*
     *  create IPC directories
     */

    int saved_umask;

    /*
     *  File permission mode is always specified absolutely
     */
    saved_umask = umask ( 0 );

    if ( !createPath ( ipcdb, 0777 ) )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CANNOT_CREATE_IPC_DB_DIR, m_dbName );
        umask(saved_umask);
        return false;
    }
    if ( !createPath ( ipcus, 0777 ) )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CANNOT_CREATE_IPC_US_DIR, m_dbName );
        umask(saved_umask);
        return false;
    }

    /*
     * PTS 1105151
     * All resources created, umask can be reset
     */
    umask(saved_umask);

    if(0 != rundir[0])
    {
        rc = access ( rundir, R_OK|W_OK|X_OK );
        if (rc)
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_NO_RWX_PERMISSION, rundir );
            return false;
        }
    }

    /* allowed to start... */
    return true;
}


        /* PTS 1106476 */
SAPDB_Int RTE_ActiveDatabase::doStart (
    SAPDB_Int   argc,
    SAPDB_Char *argv[],
    SAPDB_Char *kernelpgm,
    SAPDB_Char *rundir,
    RTE_ClientDiagPipe *diagPipe,
    SAPDB_Bool  forceAutorestart,
    SAPDB_Bool  lowPriority
    )
{
    /* child process */
    pid_t kernelPid;
    pid_t returnedPid;
    int   kernelExitStatus = 0;
    char* args[maxArgsC + 1];
    int   argStart;
    int   i;
    SAPDB_Int  rc;
    RTE_DBName currentDbname;

    SAPDB_Bool kernelIndicatedColdState = true;

    strcpy((char *)&currentDbname[0], m_dbName); /* Length is checked by caller */

    if(!redirectStandardDescriptors(rc))
    {
        return (rc);
    }
    /* now close all handles that are not needed anymore. Keeping handles open caused   */
    /* problems when the dbmcli was called by SAPINST. stdin and stdout were            */
    /* redirected to pipes and a command was detected as executed successfully          */
    /* when the pipes were closed. For simple commands this happened when the           */
    /* dbmcli exit()ed. Unfortunately, for the db_start command, the kernel is          */
    /* fork()ed and all open handles (including the ones for the stdin and stdout       */
    /* pipes) were inherited to it. So, when the dbmcli exit()s, the kernel still       */
    /* had handles to the pipes, the pipes were not closed and SAPINST waited forever.  */
    for(i=3;i<sysconf(_SC_OPEN_MAX);i++)    // starting with 3 because stdin/stdout/stderr must not be closed
    {
        if(!diagPipe->HandleBelongsToDiagPipe(i))
            RTE_save_close(i);
    }

    if ( chdir(&rundir[0]) != 0 )
    {
        return DO_START_CANT_CHANGE_TO_RUN_DIR;
    }
    /* Put dbname in environment */
    tsp01_RteError RteError;
    if(!sqlPutDbNameToEnv (*(tsp00_DbNamec *)m_dbName))
    {
        return DO_START_CANT_SET_ENVIRONMENT;
    }

    /* Prepare arguments for starting kernel */
    args [0] = kernelpgm;
    args [1] = (char *)&currentDbname[0];
    argStart = 2;
    if ( forceAutorestart )
    {
        args[argStart] = (SAPDB_Char *)"-online";
        ++argStart;
    }
    if ( lowPriority )
    {
        args[argStart] = (SAPDB_Char *)"-lowprio";
        ++argStart;
    }
    argc = min (argc, maxArgsC - argStart);
    for (i = 0; i < argc; ++i)
    {
        args [i + argStart] = argv [i];
    }
    args [i + argStart] = NULL;

    /* Reset flag set by signal handler to indicate kernel is in ADMIN state */

    /* PTS 1106476
    * Why a second fork ?
    * Not so easy to explain... The kernel expect to have a parent process.
    * This forces at least one fork. The dbmsrv wants to
    * continue its work, so he cannot wait until the kernel is finished.
    * If he does not terminate after the call, the terminating kernel will become a zombie.
    * The second fork prevents this. Its only task is to start the kernel and wait until
    * it is in ADMIN state (indicated by a signal). Then the 'process in between' can
    * terminate. When the kernel finally terminates its parent is already gone so he cannot
    * become a zombie!
    */

    kernelPid = fork();
    if ( kernelPid == -1 )
    {
        return(DO_START_CANNOT_FORK);
    }

    if ( kernelPid == 0 )
    {
        diagPipe->closeClientSide();
        execv ( kernelpgm, args);
        /* after execv failed _exit() is better that exit() ... */
        _exit(1);
        /* end of child process */
    }

    /* parent, fork ok */
    // close the pipe. Our parent wants to be noticed when the kernel (our child) closes the pipe. For this to work, no one else may have the pipe open.
    // anyway, we don't need it
    diagPipe->closeKernelSide();
    // wait for our child (the kernel) to signalize admin state (by closing the pipe) or terminate...
    // Only our parent process is interested in what the kernel writes to the pipe and
    // reads that itself. So we must not read from the pipe as this would steal messages from our parent
    if(!diagPipe->waitForClose())
    {
        return DO_START_ERROR_IN_POLL;
    }

    for(int waitCounter = 0 ; waitCounter <= 5 ; waitCounter++ )
    {
        do
        {
            returnedPid = RTE_save_waitpid(kernelPid, &kernelExitStatus, WNOHANG);
        }
        while((returnedPid > 0) && (returnedPid != kernelPid));
        if(0 != returnedPid)
            break;
        // a little sleep() is necessary: If the child process terminates, read() returns 0
        // but a waitpid() immediately after returns 0. After a little sleep(),waitpid()
        // returns the pid of the child process...
        sleep(1);
    }

    if(0 != returnedPid)
    {
        if ( !WIFEXITED(kernelExitStatus) )
        {
            return(DO_START_DIED_BEFORE_COLD_STATE);
        }
        return(DO_START_KERNEL_EXIT_CODE_OFFSET + WEXITSTATUS(kernelExitStatus));
    }
    return(DO_START_SUCCEEDED);
}

SAPDB_Bool RTE_ActiveDatabase::getDbPidStateFromShm (
    pid_t *pParentPid,
    pid_t *pPid,
    SAPDB_UInt4 *pState,
    SAPDB_Bool *pWantDump,
    SAPDB_Bool *pGracefully,
    RTE_SpeedInfo *pActiveSpeed,
    Msg_List &errList)
{
    key_t commonRTEKey;
    SAPDB_Bool dbRunning;

    errList.ClearMessageList();
    // first check if the kernel is running at all - if it wasn't, looking in the shared memory would not make any sense
    if(!checkForRunningProcesses( dbRunning,errList ))
    {
        // errList has been filled by checkForRunningProcesses(), so we do not have to generate it by ourselves
        return false;
    }
    if(!dbRunning)
    {
        if ( pState )
        {
            *pState = SERVER_OFFLINE;
        }
        errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_DBREG_DB_NOT_STARTED, m_dbName);
        return true;
    }

    commonRTEKey = sql41_get_server_key ( *((tsp00_DbNamec *)m_dbName) );
    if ( commonRTEKey == (key_t)-1 )
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_CREATE_SERVER_KEY );
        if ( pState )
        {
            *pState = SERVER_OFFLINE;
        }
        return true;
    }
    /*
     *  Get shmid
     */
    SAPDB_Int                       shmid;
    shmid = shmget ( commonRTEKey , 0 , 0 );
    if ( shmid < 0 )
    {
        if ( errno == ENOENT )
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_DBREG_DB_NOT_STARTED, m_dbName);
            if ( pState )
            {
                *pState = SERVER_OFFLINE;
            }
            return true;
        }
        return false;
    }

    struct  KERNEL_GLOBAL_SECTION  *KgsNotRemapped;

    KgsNotRemapped = sql34MapKgsNotRemapped ( shmid, pWantDump != 0);

    if ( !KgsNotRemapped )
    {
        if ( pState )
        {
            *pState = SERVER_OFFLINE;
        }
        return true;
    }
    RTE_SpeedInfo speedFromKSS = speedNone;
    if( KgsNotRemapped->state ) // if there is no valid state in the kss, any other information may not be usable
    {
        if(0 == strncmp(KgsNotRemapped->KernelVersion,COMP_NAME_KERNEL_SP100,strlen(COMP_NAME_KERNEL_SP100)))
        {
            speedFromKSS = speedFast;
        }
        else if(0 == strncmp(KgsNotRemapped->KernelVersion,COMP_NAME_QUICKKNL_SP100,strlen(COMP_NAME_QUICKKNL_SP100)))
        {
            speedFromKSS = speedQuick;
        }
        else if(0 == strncmp(KgsNotRemapped->KernelVersion,COMP_NAME_SLOWKNL_SP100,strlen(COMP_NAME_SLOWKNL_SP100)))
        {
            speedFromKSS = speedSlow;
        }
        else if(0 == strncmp(KgsNotRemapped->KernelVersion,COMP_NAME_OMSTSTKNL_SP100,strlen(COMP_NAME_OMSTSTKNL_SP100)))
        {
            speedFromKSS = speedTest;
        }
        if(pActiveSpeed)
        {
            *pActiveSpeed = speedFromKSS;
        }

        if ( IsEqual_esp100 != sp100_CompareVersionIDs ( &KgsNotRemapped->RTEVersionID ) )
        {
		    tsp00_Versionc currentVersion;

		    sp100_GetVersionString( COMP_NAME_RTE_SP100,
								    s100buildnumber,
								    currentVersion);
            errList = SAPDBErr_MessageList(RTE_CONTEXT, 
                RTEERR_DBREG_WRONG_DB_VERSION, KgsNotRemapped->RTEVersion, currentVersion );
            Msg_List dummyList;
            sql34UnmapKgsNotRemapped(KgsNotRemapped);
            return false;
        }

        if ( pParentPid )
        {
            *pParentPid = KgsNotRemapped->parentPid;
        }
        if ( pPid )
        {
            *pPid = KgsNotRemapped->pid;
        }
    }
    if ( pState )
    {
        *pState = KgsNotRemapped->state;
    }
    RTE_Path kernelExecutablePath;
    SAPDB_sprintf(kernelExecutablePath,sizeof(RTE_Path),"%s/pgm/%s",m_dbRoot,speedFromKSS.ExecutableName());
    RTE_PidListForExecutable pidList(kernelExecutablePath);
    if ( !pidList.IsPidForExecutableAndArgument(KgsNotRemapped->parentPid, m_dbName) )
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_DBREG_DB_NOT_STARTED, m_dbName);
        Msg_List dummyList;
        sql34UnmapKgsNotRemapped(KgsNotRemapped);
        if ( pState )
        {
            *pState = SERVER_OFFLINE;
        }
        return true;
    }
    if ( pWantDump )
    {
        KgsNotRemapped->wantDump = *pWantDump;
    }
    if ( pGracefully )
    {
        KgsNotRemapped->gracefully = *pGracefully;
    } 

    UpdateSharedStateCopy( KgsNotRemapped->kernelState
                         , KgsNotRemapped->pendingUpdateCounter
                         , KgsNotRemapped->completedUpdateCounter
                         , KgsNotRemapped->kernelStateSize );

    Msg_List dummyList;
    sql34UnmapKgsNotRemapped(KgsNotRemapped);
    return true;
}

//-----------------------------------------------------------

const RTE_SpeedInfo &RTE_ActiveDatabase::GetActiveSpeed()
{
    RTE_SpeedInfo &activeSpeed = speedNone;
    Msg_List errList;
    getDbPidStateFromShm(NULL,NULL,NULL,0,0,&activeSpeed,errList);
    return activeSpeed;
}

//---------------------

bool
RTE_ActiveDatabase::Start( RTE_SpeedInfo &speed
                          ,Msg_List &errList
                          ,int argc
                          ,char **argv
                          ,SAPDB_Bool forceAutorestart
                          ,SAPDB_Bool lowPriority
                         )
{
    int                         rc;
    pid_t                       serverpid;
    pid_t                       returnedPid;
    RTE_Path                    fullKernelPath;  /* PTS 1105631 */
    RTE_Path                    rundir;     /* PTS 1117575 */
    int                         startProcessStatus;
    RTE_ClientDiagPipe          diagPipe;

    errList.ClearMessageList();
    if ( strlen(m_dbName) > sizeof(RTE_DBName) )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_NAME_TOO_LONG, m_dbName );
        return false;
    }

    if ( !m_dbRoot || !m_dbRoot[0] )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_NO_DBROOT, m_dbName );

        return false;
    }

    tsp00_Pathc     PgmPath;
    tsp01_RteError  RteError;
    if(!sqlGetDbrootPgmPath ( PgmPath,TERM_WITH_DELIMITER_EO01 , &RteError))
    {
		errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_GET_DBROOT_PGM_PATH,
                  SAPDB_ToString (RteError.OsErrCode), SAPDB_ToString (RteError.RteErrCode));
        return false;
    }
    SAPDB_sprintf ( fullKernelPath, sizeof(RTE_Path), "%s%s", PgmPath.asCharp(), speed.ExecutableName() ) ;

    RTEConf_Parameter config(m_dbName,0,RTECONF_MAXNAMELENGTH,RTECONF_MAXSTRINGLENGTH);
    if(!config.GetRundirectoryAbsolutePath( (RTEConf_Parameter::String)&rundir[0],errList ))
    {
        rundir[0]=0;
    }

    if(!checkForDbStartable ( fullKernelPath, rundir , errList))
    {
        return false;
    }

    /*
     *  Create server process
     */
    /*
    *  SIGCLD is required for the wait system call below.
    */
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;
    if ( sigaction( SIGCLD, &sa, NULL) != 0 )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT,  RTEERR_DBREG_CANNOT_CATCH_SIGCLD, sqlerrs() );
        return false;
    }

#ifdef SIGWINCH
    sa.sa_handler = SIG_IGN;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;
    if ( sigaction( SIGWINCH, &sa, NULL) != 0 )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CANNOT_IGNORE_SIGWINCH, sqlerrs() );
        return false;
    }
#endif

    /* PTS 1107044 */
    SAPDB_Char errorMessage[256];
    if ( !updateEnvironment( errorMessage, sizeof(errorMessage) ) )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_GENERAL, errorMessage );
        return false;
    }
    /*
     *  Fork and exec database kernel.
     */
    diagPipe.create(m_dbName);
    serverpid = fork ();
    if ( serverpid == 0 )
    {
        /* child process */
        int returnValue;

        returnValue =  doStart(argc, argv, fullKernelPath, rundir, &diagPipe, forceAutorestart, lowPriority);
        exit ( returnValue );
    }

    diagPipe.closeKernelSide();
    Msg_List newErrList;
    while(diagPipe.readMessage(newErrList))
    {
        errList = errList + newErrList;
    }
    do
    {
        returnedPid = RTE_save_waitpid(serverpid, &startProcessStatus, 0);
    }
    while(returnedPid != serverpid);

    /*
        Now check if the kernel state is really admin or already online -
        To handle the case that the kernel crashed after closing the diag pipe correctly
        http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122382
    */
    SAPDB_UInt4 dbState;
    getDbPidStateFromShm(NULL,NULL,&dbState,0,0,NULL,errList);
    if(SERVER_COLD != dbState && SERVER_WARM != dbState)
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_KERNEL_DIED_BEFORE_ADMIN);
        return false;
    }

    if ( returnedPid == ((pid_t)-1) )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CANNOT_WAIT_FOR_STARTER, sqlerrs() );
    }
    else if ( !WIFEXITED(startProcessStatus) )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_STARTER_DID_NOT_EXIT_NORMALLY, SAPDB_ToString(startProcessStatus) );
    }
    else
    {
        switch( WEXITSTATUS(startProcessStatus) )
        {
        case DO_START_SUCCEEDED:
            return true;
            break;

        case DO_START_CANT_REDIRECT_STDOUT:
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CANNOT_REDIRECT_STDOUT);
            break;

        case DO_START_CANT_REDIRECT_STDERR:
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CANNOT_REDIRECT_STDERR);
            break;
        case DO_START_CANT_OPEN_DEV_NULL:
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CANNOT_OPEN_DEV_NULL);
            break;
        case DO_START_CANNOT_FORK:
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CANNOT_FORK);
            break;
        case DO_START_DIED_BEFORE_COLD_STATE:
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_KERNEL_DIED_BEFORE_ADMIN);
            break;
        default:
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_KERNEL_EXITED_BEFORE_ADMIN,SAPDB_ToString(WEXITSTATUS(startProcessStatus) - DO_START_KERNEL_EXIT_CODE_OFFSET ));
            break;
        }
    }

    return false;
}

//-----------------------------------------------------------

bool
RTE_ActiveDatabase::Stop(Msg_List &errList,SAPDB_UInt4 offlineTimeOut, bool dump, bool gracefully)
{
    int             rc;
    pid_t           kernelParentPid ;
    pid_t           kernelPid ;

    if(!getDbPidStateFromShm(&kernelParentPid, &kernelPid, NULL, &dump, &gracefully, NULL,errList) )
    {
        return false;
    }
// if the kernel is not running at all, getDbPidStateFromShm() returns true, but fills errList. In this case, nothing has to be done...
    if(!errList.IsEmpty())
    {
        return true;
    }
    if ( kill ( kernelPid, SIGTERM ) != 0 )
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_KILL_FAILED, sqlerrs() );
        return false;
    }

    if ( !doWaitForPid(kernelParentPid,offlineTimeOut,errList) != 0 )
    {
        if ( kill( kernelPid, SIGKILL ) != 0 )
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_KILL_FAILED, sqlerrs() );
            return false;
        }
        if ( !doWaitForPid(kernelParentPid,offlineTimeOut,errList) != 0 )
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_KILL_KERNEL,SAPDB_ToString(offlineTimeOut*2));
            return false;
        }
    }

    rc = sql41_remove_ipc_resources ( m_dbName );
    if ( rc == 0 )
    {
        return true;
    }
    else
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_CANNOT_REMOVE_IPC_RESOURCES, sqlerrs() );
        return false;
    }
}


bool RTE_ActiveDatabase::UpdateStatus(Msg_List &errList)
{
    SAPDB_UInt4 dbState;
    if(!getDbPidStateFromShm( NULL, NULL, &dbState, 0, 0,NULL, errList ))
    {
        errList = errList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_GET_KERNEL_STATE,sqlerrs());
        dbState = 0;
        return false;
    }
    // Map the kernel status to return DbStatusInfo
    DBStatusInfo statusInfo = dbstateOffline;
    switch( dbState )
    {
    case SERVER_OFFLINE:
        statusInfo = dbstateOffline;
        break;
    case SERVER_STARTING:
        statusInfo = dbstateStarting;
        break;
    case SERVER_COLD:
        statusInfo = dbstateAdmin;
        break;
    case SERVER_STANDBY_EO00:
        statusInfo = dbstateStandby;
        break;
    case SERVER_WARM:
        statusInfo = dbstateOnline;
        break;
    case SERVER_SHUTDOWN:
        statusInfo = dbstateShutdown;
        break;
    case SERVER_SHUTDOWNREINIT:
        statusInfo = dbstateShutdownReinit;
        break;
    case SERVER_SHUTDOWNKILL:
        statusInfo = dbstateShutdownKill;
        break;
    case SERVER_STOP:
        statusInfo = dbstateStop;
        break;
    case SERVER_KILL:
        statusInfo = dbstateKill;
        break;
    default:
        statusInfo = dbstateUnknown;
        break;
    }
    m_currentState = statusInfo;
    return true;
}

#endif

/*!
  @brief C wrapper for sqlxstart
  @param dbname [in] database name
  @param pgmKind [in] fast or slow mode
  @param dbroot [in] database root
  @param is_gateway [in] obsolete gateway flag
  @param argc [in] argument count
  @param argv [in] argument vector
  @param xerror [out] error structure
  @return none
 */
void sqlxstart (
    tsp9_cstr dbname,
    tsp9_pgm_kind pgmKind,
    tsp9_cstr dbroot,
    tsp00_Bool is_gateway,
    int argc,
    char* argv[],
    tsp9_rte_xerror* xerror)
{
    Msg_List errList;
    RTE_ActiveDatabase * dbInstance;

    if ( is_gateway )
    {
        MSG_9 ((xerror, -11987, (char *)"Gateway %s no longer supported", dbname ));
        return;
    }

    dbInstance = RTE_DBRegister::Instance().GetActiveDatabase(dbname, errList);
    if ( !dbInstance )
    {
       RTE_FillXErrorFromErrList(xerror, errList);
       return;
    }


    // speedFast is set by default, so the active speed must be set only if quick or slow is desired

    RTE_SpeedInfo speed(speedFast);

    switch(pgmKind)
    {
    case csp9_quick_pgm:
        speed = speedQuick;
        break;
    case csp9_slow_pgm:
        speed = speedSlow;
        break;
    case csp9_test_pgm:
        speed = speedTest;
        break;
    }

    if ( !dbInstance->Start(speed,errList, argc, &argv[0]) )
    {
       RTE_FillXErrorFromErrList(xerror, errList);
    }
    else
    {
        eo44initError(xerror);
#if DIAGPIPE_TESTING_
        if(!errList.IsEmpty())
        {
            Msg_List *nextMessage = &errList;
            while(NULL != nextMessage)
            {
                printf("%s",nextMessage->Message());
                nextMessage = nextMessage->NextMessage();
            }
        }
#endif
    }
    delete dbInstance;
}


/*!
  @brief Obsolete C wrapper for sqlxclear ---- dummy
  @param dbname [in] database name
  @param dbroot [in] database root
  @param xerror [out] error structure
  @return none
 */
void sqlxclear (
    tsp9_cstr dbname,
    tsp9_cstr dbroot,
    tsp9_rte_xerror* xerror)
{
    eo44initError(xerror);
}

/*!
  @brief Obsolete C wrapper for sqlxcanstart ---- dummy
  @param xerror [out] error structure
  @return none
 */
void sqlxcanstart (
    tsp9_rte_xerror* xerror)
{
    eo44initError(xerror);
}

#if defined (HPUX) && !defined (HP_IA64)
/*! HP sometimes needs veeeery looooong to get rid of the watchdog process */
#define STOP_TIMEOUT_ 900
#else
/*! define */
#define STOP_TIMEOUT_ 120
#endif

/*!
  @brief C wrapper for sqlxstop
  @param dbname [in] database name
  @param dbroot [in] database root
  @param dumpflag [in] flag to force dump
  @param xerror [out] error structure
  @return none
 */
void sqlxstop (
    tsp9_cstr dbname,
    tsp9_cstr dbroot,
    tsp00_Bool dumpflag,
    tsp9_rte_xerror* xerror)
{
    Msg_List errList;
    RTE_ActiveDatabase * dbInstance;

    dbInstance = RTE_DBRegister::Instance().GetActiveDatabase(dbname, errList);
    if ( !dbInstance )
    {
       RTE_FillXErrorFromErrList(xerror, errList);
       return;
    }

    if ( !dbInstance->Stop(errList, STOP_TIMEOUT_, dumpflag != 0) )
    {
       RTE_FillXErrorFromErrList(xerror, errList);
    }
    else
    {
       eo44initError(xerror);
    }
    delete dbInstance;
}

char * RTEACT_lastpDbname = 0; ///< last database name used
RTE_ActiveDatabase * RTEACT_dbInstance = 0; ///< active database instance

/*!
  @brief C wrapper for sqlxfinish 
  @return none
 */
externC void sqlxfinish (void)
{
}

/*!
  @brief C wrapper for sqlxinit 
  Only used on windows to disable output
  @param dbname [in] database name
  @param dbroot [in] database root
  @return none
 */
externC void sqlxinit (
    tsp9_cstr dbname,
    tsp9_cstr dbroot)
{
#ifdef _WIN32
    /* disable message output */
    sql60_disable_GUI_console();
    sql60_disable_message_box ();
    sql60_disable_console ();
#endif
}

