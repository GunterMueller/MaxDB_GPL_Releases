/*!---------------------------------------------------------------------
  @file           RTE_ActiveDatabase.hpp
  @author         JoergM, RobinW
  @brief          DBM Support: SAPDB Instance and Database Registration and Management Interfaces
  @see            

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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(WIN32)
# include "gos00.h" /* nocheck */
#endif

#include "RunTime/RTE_Types.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "Container/Container_List.hpp"
#include "RunTime/RTE_Installation.hpp"
#include "RunTime/RTE_ActiveInstallation.hpp"
#include "RunTime/RTE_DiagPipe.h"
#include "RunTime/RTE_KSS.h"
#include "RunTime/RTE_Database.hpp"
#include "KernelCommon/Kernel_SharedState.h"
class Msg_List;
#ifdef _WIN32
#include "RunTime/Synchronisation/RTESync_BinarySemaphore.hpp"
#include "RunTime/IPC/RTEIPC_NamedSharedMemory.hpp"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

 /*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef SAPDB_Char RTE_DBKey[256];

class RTE_DBRegister;

/*!
  @class RTE_ActiveDatabase
  @brief Active database information class

  It allows to get status information and the installed speeds.
  Addition the current active speed is requestable and start/stop
  of database is triggered.
 */
class RTE_ActiveDatabase: public RTE_Database
{
public:
    /*!
      @brief auto restart the database

      The difference to the Start method is the autorestart feature, which directly starts into 'ONLINE' mode
      if possible.

      @param    speed               [in]    the speed wanted
      @param    errList             [inout] possible error return
      @param    argc                [in]    optional argument array count (UNIX only)
      @param    argv                [in]    optional argument array (UNIX only)
      @param    forceAutorestart    [in]    if true autorestart is enforced, if false it starts only into 'ADMIM' mode
      @param    lowPriority         [in]    if true low priority is enforced, if false it starts with normal priority
      @return   true if auto restart done, else better inspect errList...
     */
    bool Start( RTE_SpeedInfo &speed,
                Msg_List &errList,
                int argc=0,
                char **argv=0,
                SAPDB_Bool forceAutorestart=false,
                SAPDB_Bool lowPriority=false );
    /*!
      @brief stop the database

      @param    errList             [inout] possible error return
      @param    offlineTimeOut      [in]    timeout in possible error return
      @param    dump                [in]    dump should be enforced if true
      @param    gracefully          [in]    automatic shutdown if true
      @return   true if stop done, else better inspect errList...
     */
    bool Stop(Msg_List &errList,
              SAPDB_UInt4 offlineTimeOut,
              bool dump=false,
              bool gracefully=false);

    /*!
      @brief information gathered on request
      @param    errList             [inout]     possible error return
      @return   pointer to database status info
     */
    const RTE_Database::DBStatusInfo* GetDBStatus(Msg_List &errList);

    /*!
      @brief information gathered on request
      @param    pExtendedState      [out]     filled with pointer to copy of additional kernel state information
      @param    errList             [inout]   possible error return
      @return   pointer to database status info or 0 if error
     */
    const RTE_Database::DBStatusInfo* GetExtendedDBStatus( Kernel_SharedState const *&pExtendedState
                                                         , Msg_List &errList);

    /*!
      @brief information gathered on request
      @return   if the database is running active speed (speedFast/speedQuick/speedSlow/speedTest) 
                if the database is not running 
     */
    const RTE_SpeedInfo &GetActiveSpeed();

    /*!
      @brief destruct the active database instance
     */
    ~RTE_ActiveDatabase()
    {
#ifdef _WIN32
        CloseKernel ();
#endif
    };
private:
    /*!
      @brief Update current database status
      @param errList [inout] possible error return
      @return true if call succeeded, false else (errList filled)
     */
    bool UpdateStatus(Msg_List &errList);

    friend class RTE_ActiveInstallation;
    friend class RTE_DBRegister;
#ifndef _WIN32
    RTE_ActiveDatabase( RTE_ActiveInstallation& installation,
                        const RTE_Path dbRoot,
                        RTE_DBName dbName)
                        : RTE_Database((RTE_Installation&)installation, dbRoot, dbName),
                          m_UpdateCounter(0),
                          m_SharedStateSize(0)
    {};

    SAPDB_Int  doStart (SAPDB_Int argc,SAPDB_Char *argv[],SAPDB_Char *kernelpgm,SAPDB_Char *rundir,RTE_ClientDiagPipe *diagPipe,SAPDB_Bool forceAutorestart, SAPDB_Bool lowPriority);
    SAPDB_Bool checkForDbStartable (RTE_Path &full_knl_path, RTE_Path &rundir, Msg_List &errList);
    SAPDB_Bool checkForRunningProcesses (SAPDB_Bool &dbRunning,Msg_List &errList); 
    SAPDB_Bool updateEnvironment(SAPDB_Char *errorText, SAPDB_Int4 maxErrorTextLengthInBytes);
    SAPDB_Bool getDbPidStateFromShm (pid_t *pParentPid,pid_t *pPid,SAPDB_UInt4 *pState,SAPDB_Bool *pWantDump,SAPDB_Bool *pGracefully,RTE_SpeedInfo *pActiveSpeed,Msg_List &errList);
    SAPDB_Bool doWaitForPid( pid_t kernelPid,SAPDB_UInt4 offlineTimeOut,Msg_List &errList );
    SAPDB_Bool checkFileDoesNotExist ( SAPDB_Char *path );
    SAPDB_Bool checkFileAccess ( SAPDB_Char *path, SAPDB_Int mode );
    SAPDB_Bool createPath ( SAPDB_Char * path_to_create, SAPDB_UInt2 mode );
    SAPDB_Bool redirectStandardDescriptors(SAPDB_Int &rc);
    void       saveErrorMessageToFile(SAPDB_Char *errorMessage);
#else
    RTE_ActiveDatabase( RTE_ActiveInstallation& installation,
                        const RTE_Path dbRoot,
                        RTE_DBName dbName)
                        : RTE_Database((RTE_Installation&)installation, dbRoot, dbName),
                          m_hKernel (RTE_UNDEF_HANDLE),
                          m_pKSS (NULL),
                          m_UpdateCounter(0),
                          m_SharedStateSize(0)
        {;}

    APIRET RTE_ActiveDatabase::stopService ( void );
    APIRET RTE_ActiveDatabase::startService ( RTE_SpeedInfo &speed 
                                            , SAPDB_Bool forceAutorestart
                                            , SAPDB_Bool lowPriority
                                            , SAPDB_Int  argc
                                            , SAPDB_Char **argv
                                            , Msg_List &errList);

    SAPDB_Bool StopServerDB ( SAPDB_Bool fDump, SAPDB_Bool gracefully, Msg_List &messageList );

    SAPDB_Bool OpenKernel   (SAPDB_UInt4 &              dbState, 
                             Msg_List &     messageList);
    void       CloseKernel  ();


    RTE_FileHandle          m_hKernel;
    KERNEL_SHARED_SEG *     m_pKSS;    
//    RTE_FileHandle          m_hKSS;
    RTEIPC_NamedSharedMemory m_kss;

    RTESync_BinarySemaphore m_startStopSemaphore;
#endif

    void UpdateSharedStateCopy( Kernel_SharedState &externalState
                              , SAPDB_ULong        &externalPendingUpdateCounter
                              , SAPDB_ULong        &externalCompletedUpdateCounter
                              , SAPDB_UInt4         externalStateSize);

    Kernel_SharedState      m_SharedStateCopy;
    SAPDB_ULong             m_UpdateCounter;
    SAPDB_UInt4             m_SharedStateSize;
};

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
